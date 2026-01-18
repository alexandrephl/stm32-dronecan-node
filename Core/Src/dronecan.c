/******************************************************************************
* @file    dronecan.c
* @brief   DroneCAN (UAVCAN v0) communication layer.
*          This module initializes and interfaces with libcanard to publish
*          DroneCAN messages over Classic CAN (CAN 2.0).
*
*          Implemented messages:
*            - uavcan.node.Heartbeat
*            - Static pressure (Pa)
*            - Temperature (K)
*
* @board   Custom STM32F446RE board
* @author  Alexandre Panhaleux
******************************************************************************/
#include "dronecan.h"
#include "main.h"
#include "cmsis_os.h"
#include <string.h>

#include "canard.h"		/* libcanard core API */
#include "bsp.h"		/* Board-specific CAN send function */

/* Configuration ----------------------------------------------------------*/
#define CANARD_MEM_SIZE 1024				/* Size of the memory pool used by libcanard */

/* uavcan.node.Heartbeat */
#define UAVCAN_NODE_HEARTBEAT_ID 7509
#define UAVCAN_NODE_HEARTBEAT_SIGNATURE 0x0F0868D0C1A7C6F1ULL

/* uavcan.equipment.air_data.StaticPressure */
#define UAVCAN_PRESSURE_ID        1027
#define UAVCAN_PRESSURE_SIGNATURE 0xCDDE07BB89A56356ULL

/* uavcan.equipment.air_data.StaticTemperature */
#define UAVCAN_TEMPERATURE_ID        1028
#define UAVCAN_TEMPERATURE_SIGNATURE 0x4A2F9D4F0C1F5B6EULL

#define UAVCAN_LATENCY_PING_ID        20001
#define UAVCAN_LATENCY_PING_SIGNATURE 0xABCDEF0123456789ULL  // Fake, but needs to be unique

#define UAVCAN_RTT_PING_ID        20002
#define UAVCAN_RTT_PING_SIGNATURE 0xCAFEBABE12345678ULL

#define LATENCY_PING_DATA_TYPE_ID   78   // example unused ID
#define LATENCY_PING_SIGNATURE     0x123456789ABCDEF0ULL  // any random 64-bit

/* Static module state ----------------------------------------------------*/
static uint8_t heartbeat_tid = 0;				/* Transfer-ID counters */
static uint8_t canard_memory[CANARD_MEM_SIZE];	/* libcanard memory arena */
static CanardInstance canard;					/* libcanard instance */
static uint8_t node_id;							/* Local node ID*/

/* Libcanard callbacks ----------------------------------------------------*/

/**
 * @brief Decide whether an incoming transfer should be accepted
 *
 * @note  For now, all broadcast transfers are accepted, but none are processed.
 */
static bool should_accept_transfer(
    const CanardInstance* ins,
    uint64_t* out_data_type_signature,
    uint16_t data_type_id,
    CanardTransferType transfer_type,
    uint8_t source_node_id)
{
	if (transfer_type == CanardTransferTypeBroadcast)
	    {
	        if (data_type_id == LATENCY_PING_DATA_TYPE_ID)
	        {
	            *out_data_type_signature = LATENCY_PING_SIGNATURE;
	            return true;
	        }
	    }
    return false;
}

/**
 * @brief Return current system time in microseconds
 *
 * @note  Used by libcanard for transfer timing and deadlines.
 */
static uint64_t get_time_usec(void)
{
    return ((uint64_t)HAL_GetTick()) * 1000ULL;
}

/**
 * @brief Callback invoked when a transfer is received
 *
 * @note  RX handling is intentionally not implemented yet.
 * @retval None
 */
static void on_transfer_received(
    CanardInstance* ins,
    CanardRxTransfer* transfer)
{
    (void)ins;

    /* Example: latency ping echo */
    if (transfer->data_type_id == LATENCY_PING_DATA_TYPE_ID)
    {
        if (transfer->payload_len >= sizeof(uint32_t))
        {
            uint32_t sent_ms;
            memcpy(&sent_ms, transfer->payload_head, sizeof(sent_ms));

            // Get current STM32 time
            uint32_t recv_ms = HAL_GetTick();

            uint8_t payload[8];
            memcpy(payload, &sent_ms, 4);
            memcpy(payload + 4, &recv_ms, 4);
            /* Echo back same timestamp */
            DroneCAN_SendLatencyPing(payload, 8);
        }
    }

    /* Add other message handlers here later */
}

void DroneCAN_ProcessRx(void)
{
    uint32_t can_id;
    uint8_t data[8];
    uint8_t len;

    while (BSP_CAN_ReceiveFrame(&can_id, data, &len))
    {

        CanardCANFrame frame;

        frame.id = can_id;
        frame.data_len = len;
        memcpy(frame.data, data, len);

        osDelay(1);

        /* Feed frame into libcanard */
        canardHandleRxFrame(
            &canard,
            &frame,
            get_time_usec()
        );

    }
}




/* Public API ------------------------------------------------------------*/

/**
 * @brief Initialize the DroneCAN stack
 *
 * @param id  UAVCAN node ID (1–127)
 * @retval None
 */
void DroneCAN_Init(uint8_t id)
{
    node_id = id;

    canardInit(
        &canard,
        canard_memory,
        sizeof(canard_memory),
        on_transfer_received,
        should_accept_transfer,
        NULL
    );

    canard.node_id = node_id;
}


/**
 * @brief Publish a UAVCAN heartbeat message
 *
 * 		Message: uavcan.node.Heartbeat
 *
 * @retval None
 */
void DroneCAN_SendHeartbeat(void)
{
    uint8_t payload[7];

    uint32_t uptime = HAL_GetTick() / 1000;

    payload[0] = (uint8_t)(uptime >> 0);
    payload[1] = (uint8_t)(uptime >> 8);
    payload[2] = (uint8_t)(uptime >> 16);
    payload[3] = (uint8_t)(uptime >> 24);
    payload[4] = 0; // health = OK
    payload[5] = 0; // mode = OPERATIONAL
    payload[6] = 0; // vendor-specific

    const int16_t res = canardBroadcast(
        &canard,
        UAVCAN_NODE_HEARTBEAT_SIGNATURE,
        UAVCAN_NODE_HEARTBEAT_ID,
        &heartbeat_tid,
        CANARD_TRANSFER_PRIORITY_LOW,
        payload,
        sizeof(payload)
    );

    if (res < 0)
    {
        return; // out of memory or error
    }

    // Flush TX queue
    const CanardCANFrame* frame;
    while ((frame = canardPeekTxQueue(&canard)) != NULL)
    {
        BSP_CAN_SendFrame(
            frame->id,
            frame->data,
            frame->data_len
        );

        canardPopTxQueue(&canard);
    }
}

/**
 * @brief Publish static air pressure
 *
 * Message: uavcan.equipment.air_data.StaticPressure
 *
 * @param pressure_hpa  Pressure in hectopascals (hPa)
 * @retval None
 */
void DroneCAN_SendPressure(float pressure_hpa)
{
    static uint8_t pressure_tid = 0;

    /* UAVCAN requires pressure in Pascals */
    float pressure_pa = pressure_hpa * 100.0f;

    const int16_t res = canardBroadcast(
        &canard,
        UAVCAN_PRESSURE_SIGNATURE,
        UAVCAN_PRESSURE_ID,
        &pressure_tid,
        CANARD_TRANSFER_PRIORITY_LOW,
		(const uint8_t*)&pressure_pa,
        sizeof(pressure_pa)
    );

    if (res < 0)
    {
        return; // out of memory or error
    }

    const CanardCANFrame* frame;
    while ((frame = canardPeekTxQueue(&canard)) != NULL)
    {
        BSP_CAN_SendFrame(frame->id, frame->data, frame->data_len);
        canardPopTxQueue(&canard);
    }
}

/**
 * @brief Publish static air temperature
 *
 * Message: uavcan.equipment.air_data.StaticTemperature
 *
 * @param temperature_c  Temperature in degrees Celsius
 * @retval None
 */
void DroneCAN_SendTemperature(float temperature_c)
{
    static uint8_t temp_tid = 0;

    /* UAVCAN requires temperature in Kelvin */
    float temperature_k = temperature_c + 273.15f;

    const int16_t res = canardBroadcast(
        &canard,
        UAVCAN_TEMPERATURE_SIGNATURE,
        UAVCAN_TEMPERATURE_ID,
        &temp_tid,
        CANARD_TRANSFER_PRIORITY_LOW,
        &temperature_k,
        sizeof(temperature_k)
    );

    if (res < 0)
    {
        return; // out of memory or error
    }

    // Flush TX queue
    const CanardCANFrame* frame;
    while ((frame = canardPeekTxQueue(&canard)) != NULL)
    {
        BSP_CAN_SendFrame(
            frame->id,
            frame->data,
            frame->data_len
        );

        canardPopTxQueue(&canard);
    }
}


/**
 * @brief Publish latency ping (timestamp)
 *
 * Message: com.alexandrepanhaleux.ping
 *
 * @param timestamp_ms  Timestamp in milliseconds (from HAL_GetTick)
 * @retval None
 */
void DroneCAN_SendLatencyPing(const uint8_t* payload, uint8_t len)
{
    static uint8_t ping_tid = 0;

    const int16_t res = canardBroadcast(
        &canard,
        UAVCAN_LATENCY_PING_SIGNATURE,
        UAVCAN_LATENCY_PING_ID,
        &ping_tid,
        CANARD_TRANSFER_PRIORITY_LOW,
        payload,
        len
    );

    if (res < 0)
    {
        return;
    }

    const CanardCANFrame* frame;
    while ((frame = canardPeekTxQueue(&canard)) != NULL)
    {
        BSP_CAN_SendFrame(frame->id, frame->data, frame->data_len);
        canardPopTxQueue(&canard);
    }
}
