/******************************************************************************
* @file    dronecan.h
* @brief   DroneCAN (UAVCAN v0) public interface
*
*          Declares initialization and publishing functions for the
*          DroneCAN communication stack.
*
*          This header exposes a minimal API to the application layer,
*          hiding all libcanard internals.
*
* @board   Custom STM32F446RE board
* @author  Alexandre Panhaleux
******************************************************************************/

#ifndef INC_DRONECAN_H_
#define INC_DRONECAN_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported functions --------------------------------------------------------*/
/* Initialization */
void DroneCAN_Init(uint8_t node_id);
void DroneCAN_ProcessRx(void);

/* Publishers */
void DroneCAN_SendHeartbeat(void);
void DroneCAN_SendPressure(float pressure_hpa);
void DroneCAN_SendTemperature(float temperature_c);
void DroneCAN_SendLatencyPing(const uint8_t* payload, uint8_t len);

#endif /* INC_DRONECAN_H_ */
