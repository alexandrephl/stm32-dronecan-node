/******************************************************************************
* @file    main.c
* @brief   Main application entry point
*
*          This file contains system initialization, RTOS startup,
*          peripheral configuration, and task creation.
*
*          Application responsibilities:
*            - Sensor acquisition (BMP280)
*            - CAN bus transmission (raw CAN + DroneCAN)
*            - System scheduling using CMSIS-RTOS
*
* @board   Custom STM32F446RE board
* @author  Alexandre Panhaleux
******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
#include "bsp.h"
#include "bmp280.h"
#include <stdio.h>
#include <string.h>
#include "dronecan.h"


/* DroneCAN node configuration------------------------------------------------*/
#define DRONECAN_NODE_ID        42
#define DRONECAN_NODE_NAME     "com.alexandrepanhaleux.bmp280"
#define DRONECAN_CAN_BITRATE   500000



/* RTOS tasks handles --------------------------------------------------------*/
osThreadId SensorTaskHandle;
osThreadId CanTaskHandle;

/* Application devices / driver instances ------------------------------------*/
BMP280 dev;

/* CAN peripheral handle -----------------------------------------------------*/
CAN_HandleTypeDef hcan1;

/* I2C peripheral handle ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

/* UART peripheral handle ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* Shared sensor data structure : written by the SensorTask and read by the CanTask */
typedef struct
{
    float temperature_c;
    float pressure_hpa;
    uint32_t timestamp_ms;
    uint8_t valid;			/* Indicates that at least one valid sample exists */
    osMutexId mutex;		/* Protects concurrent access */
} SensorData_t;

static SensorData_t g_sensorData;


/* RTOS tasks prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_CAN1_Init(void);
void StartSensorTask(void const * argument);
void StartCanTask(void const * argument);


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{


  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();


  /* Configure the system clock */
  SystemClock_Config();


  /* ------------------------------------------------------------------------
   * Peripheral initialization
   * ------------------------------------------------------------------------ */
  MX_CAN1_Init();		/* Initialize CAN peripheral */
  BSP_Init();			/* Board-level peripherals */
  BMP280_Init(&dev);	/* Sensor initialization */
  DroneCAN_Init(DRONECAN_NODE_ID);

  /* ------------------------------------------------------------------------
   * RTOS object creation
   * ------------------------------------------------------------------------ */
  osMutexDef(SENSOR_DATA_MUTEX);
  g_sensorData.mutex = osMutexCreate(osMutex(SENSOR_DATA_MUTEX));

  /* ------------------------------------------------------------------------
   * Task creation
   * ------------------------------------------------------------------------ */

  /* definition and creation of SensorTask */
  osThreadDef(SensorTask, StartSensorTask, osPriorityNormal, 0, 512);
  SensorTaskHandle = osThreadCreate(osThread(SensorTask), &g_sensorData);

  /* definition and creation of CanTask */
  osThreadDef(CanTask, StartCanTask, osPriorityBelowNormal, 0, 256);
  CanTaskHandle = osThreadCreate(osThread(CanTask), &g_sensorData);

  /* Start RTOS scheduler */
  osKernelStart();

  /* Infinite loop */
  while (1)
  {

  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN1_Init(void)
{

  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 6;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_11TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;

  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
	  Error_Handler();
  }

  CAN_FilterTypeDef filter = {0};
  filter.FilterBank = 0;
  filter.FilterMode = CAN_FILTERMODE_IDMASK;
  filter.FilterScale = CAN_FILTERSCALE_32BIT;
  filter.FilterIdHigh = 0x0000;
  filter.FilterIdLow = 0x0000;
  filter.FilterMaskIdHigh = 0x0000;   // Accept all IDs
  filter.FilterMaskIdLow = 0x0000;
  filter.FilterFIFOAssignment = CAN_RX_FIFO0;
  filter.FilterActivation = ENABLE;
  filter.SlaveStartFilterBank = 14;


  if (HAL_CAN_ConfigFilter(&hcan1, &filter) != HAL_OK)
  {
      Error_Handler();
  }
  if (HAL_CAN_Start(&hcan1) != HAL_OK)
      Error_Handler();
}



/**
* @brief Function implementing the SensorTask thread.
* This task is responsible for:
*            - Acquiring temperature and pressure from the BMP280
*            - Updating the shared sensor structure
*           - Timestamp measurements
* @param argument: argument Pointer to shared SensorData_t structure
* @retval None
*/
void StartSensorTask(void const * argument)
{
	DroneCAN_ProcessRx();
	char msg[64];
    /*sensor data struct*/
    SensorData_t *data = (SensorData_t *)argument;

    for(;;)
    {
        float temperature = 0.0f;
        float pressure    = 0.0f;

        if (BMP280_ReadValues(&dev, &temperature, &pressure) == HAL_OK)
        {
        	osMutexWait(data->mutex, osWaitForever);

            data->temperature_c = temperature;
            data->pressure_hpa  = pressure;
            data->timestamp_ms  = HAL_GetTick();
            data->valid         = 1;

            osMutexRelease(data->mutex);

        }
        else {
            snprintf(msg, sizeof(msg), "BMP280 ERROR\n");
        }

        /* Sensor sampling period */
        osDelay(2000);
    }
}

/**
 * @brief  CAN / DroneCAN transmission task
 *
 *         This task:
 *           - Reads the latest sensor values (thread-safe)
 *           - Publishes pressure and temperature using DroneCAN
 *           - Sends periodic heartbeat messages
 *
 * @param  argument Pointer to shared SensorData_t structure
 */
void StartCanTask(void const * argument)
{
    SensorData_t *shared = (SensorData_t *)argument;
    SensorData_t local;   /* Local copy to minimize mutex hold time */


    for (;;)
    {
    	/* Copy shared data atomically */
        osMutexWait(shared->mutex, osWaitForever);
        local = *shared;
        osMutexRelease(shared->mutex);

        if (local.valid)
        {

            DroneCAN_SendPressure(local.pressure_hpa);

            DroneCAN_SendTemperature(local.temperature_c);

        }

        /* Publish node heartbeat */
        DroneCAN_SendHeartbeat();

        /* Listen to bus for Latency ping */
        DroneCAN_ProcessRx();

        /* Transmission period */
        osDelay(10);
    }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
