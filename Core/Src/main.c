/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bsp.h"
#include "bmp280.h"
#include <stdio.h>
#include <string.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define MSG_SIZE 64
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

//osThreadId LedTaskHandle;
osThreadId SensorTaskHandle;
//osThreadId LoggerTaskHandle;


/* USER CODE BEGIN PV */
BMP280 dev;

osThreadId CanTaskHandle;

CAN_TxHeaderTypeDef TxHeader;
uint32_t txMailbox;
CAN_HandleTypeDef hcan1;
uint8_t txData[8] = {0};

//data struct to collect sensor samples
typedef struct
{
    float temperature_c;
    float pressure_hpa;
    uint32_t timestamp_ms;
    uint8_t valid;
    osMutexId mutex;
} SensorData_t;

static SensorData_t g_sensorData;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_CAN1_Init(void);
//void StartLedTask(void const * argument);
void StartSensorTask(void const * argument);

//void StartLoggerTask(void const * argument);

/* USER CODE BEGIN PFP */
void StartCanTask(void const * argument);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_CAN1_Init();
  /* USER CODE BEGIN 2 */
  TxHeader.StdId = 0x123;
  TxHeader.IDE = CAN_ID_STD;
  TxHeader.RTR = CAN_RTR_DATA;
  TxHeader.DLC = 1;
  /* board support package code initialization */
  BSP_Init();

  BMP280_Init(&dev);


  BSP_I2C_TestBMP280();


  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  osMutexDef(SENSOR_DATA_MUTEX);
  g_sensorData.mutex = osMutexCreate(osMutex(SENSOR_DATA_MUTEX));
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of LedTask */
  /*osThreadDef(LedTask, StartLedTask, osPriorityLow, 0, 128);
  LedTaskHandle = osThreadCreate(osThread(LedTask), NULL);*/

  /* definition and creation of SensorTask */
  osThreadDef(SensorTask, StartSensorTask, osPriorityNormal, 0, 256);
  SensorTaskHandle = osThreadCreate(osThread(SensorTask), &g_sensorData);


  /* definition and creation of LoggerTask */
  /*osThreadDef(LoggerTask, StartLoggerTask, osPriorityBelowNormal, 0, 256);
  LoggerTaskHandle = osThreadCreate(osThread(LoggerTask), NULL);*/

  /* USER CODE BEGIN RTOS_THREADS */
  /* definition and creation of CanTask */
  osThreadDef(CanTask, StartCanTask, osPriorityBelowNormal, 0, 256);
  SensorTaskHandle = osThreadCreate(osThread(CanTask), &g_sensorData);
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
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

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
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
  /* USER CODE BEGIN CAN1_Init 2 */
  HAL_CAN_Start(&hcan1);
  /* USER CODE END CAN1_Init 2 */

}




/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartLedTask */
/**
  * @brief  Function implementing the LedTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartLedTask */
//void StartLedTask(void const * argument)
//{
//  /* USER CODE BEGIN 5 */
//  /* Infinite loop */
//  for(;;)
//  {
//      BSP_LED_Toggle();
//      osDelay(500);
//  }
//  /* USER CODE END 5 */
//}

/* USER CODE BEGIN Header_StartSensorTask */
/**
* @brief Function implementing the SensorTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartSensorTask */
void StartSensorTask(void const * argument)
{
  /* USER CODE BEGIN StartSensorTask */
	char msg[64];
    //sensor data struct
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

        osDelay(2000);
    }
  /* USER CODE END StartSensorTask */
}


void StartCanTask(void const * argument)
{
    SensorData_t *shared = (SensorData_t *)argument;
    SensorData_t local;   // local copy (stack)

    for (;;)
    {
        osMutexWait(shared->mutex, osWaitForever);
        local = *shared;              // STRUCT COPY
        osMutexRelease(shared->mutex);

        if (local.valid)
        {
            char msg[64];
            snprintf(msg, sizeof(msg),
                     "[CAN] T=%.2f P=%.2f ts=%lu\r\n",
                     local.temperature_c,
                     local.pressure_hpa,
                     local.timestamp_ms);

            BSP_UART_Send((uint8_t*)msg, strlen(msg));
        }

        osDelay(2000);
    }
}


/* USER CODE BEGIN Header_StartLoggerTask */
/**
* @brief Function implementing the LoggerTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartLoggerTask */
//void StartLoggerTask(void const * argument)
//{
//  /* USER CODE BEGIN StartLoggerTask */
//    char msg[64];
//
//    for(;;)
//    {
//        if (xQueueReceive(sensorQueue, msg, portMAX_DELAY) == pdTRUE)
//        {
//            BSP_UART_Send((uint8_t*)msg, strlen(msg));
//        }
//
//        txData[0]++;
//        if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, txData, &txMailbox) == HAL_OK) {
//            BSP_UART_Send((uint8_t*)"CAN TX OK\r\n", 11);
//        } else {
//            BSP_UART_Send((uint8_t*)"CAN TX FAIL\r\n", 13);
//        }
//        osDelay(1000);
//    }
//  /* USER CODE END StartLoggerTask */
//}

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
