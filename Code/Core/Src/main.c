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
#include "lwip.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include<stdio.h>
#include "gesture_utility.h"
#include "paj7660.h"
#include "paj7660_thumb.h"
#include "tcp_echo_server.h"
#include "debug_macros.h"
#include "modbus_rtu.h"
#include "MQTTPacket.h"
#include "lwip/sockets.h"
#include "lwip/netif.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

UART_HandleTypeDef huart3;
UART_HandleTypeDef huart6;

PCD_HandleTypeDef hpcd_USB_OTG_FS;

/* External variables */
extern struct netif gnetif;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for GestureTask */
osThreadId_t GestureTaskHandle;
const osThreadAttr_t GestureTask_attributes = {
  .name = "GestureTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for FeedbackTask */
osThreadId_t FeedbackTaskHandle;
const osThreadAttr_t FeedbackTask_attributes = {
  .name = "FeedbackTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for ModbusTask */
osThreadId_t ModbusTaskHandle;
const osThreadAttr_t ModbusTask_attributes = {
  .name = "ModbusTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for MQTTTask */
osThreadId_t MQTTTaskHandle;
const osThreadAttr_t MQTTTask_attributes = {
  .name = "MQTTTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for gestureQueue */
osMessageQueueId_t gestureQueueHandle;
const osMessageQueueAttr_t gestureQueue_attributes = {
  .name = "gestureQueue"
};
/* USER CODE BEGIN PV */
osMessageQueueId_t gestureQueueHandle;
osThreadId_t FeedbackTaskHandle;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USB_OTG_FS_PCD_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C2_Init(void);
static void MX_USART6_UART_Init(void);
void StartDefaultTask(void *argument);
void StartGestureTask(void *argument);
void StartFeedbackTask(void *argument);
void StartModbusTask(void *argument);
void StartMQTTTask(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int _write(int file, char *ptr, int len){
	HAL_UART_Transmit(&huart3, (uint8_t *)ptr, len, 100);
	return len;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
//	char tx_buffer[] = "GestLink UART Debug Test\r\n";
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
  MX_GPIO_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */
  TCP_Echo_Init();
  HAL_Delay(2000);
  if (PAJ7660_Init(&hi2c2)) {
	  DEBUG_LOG(LOG_LEVEL_USER, "Gesture Sensor Status: Initialized successfully.");
  }
  else{
	  DEBUG_LOG(LOG_LEVEL_ERROR, "Gesture Sensor Status: Initialization FAILED.");
  }
  printf("System Initialized. Starting RTOS Scheduler...\r\n");
 // volatile uint8_t gesture;
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of gestureQueue */
  gestureQueueHandle = osMessageQueueNew (10, sizeof(uint8_t), &gestureQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  //defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of GestureTask */
  GestureTaskHandle = osThreadNew(StartGestureTask, NULL, &GestureTask_attributes);

  /* creation of FeedbackTask */
  FeedbackTaskHandle = osThreadNew(StartFeedbackTask, NULL, &FeedbackTask_attributes);

  /* creation of ModbusTask */
  //ModbusTaskHandle = osThreadNew(StartModbusTask, NULL, &ModbusTask_attributes);

<<<<<<< Updated upstream
  /* creation of MQTTTask */
  MQTTTaskHandle = osThreadNew(StartMQTTTask, NULL, &MQTTTask_attributes);
=======
  /* Creation of mqttTask */
  //mqttTaskHandle = osThreadNew(StartMQTTTask, NULL, &mqttTask_attributes);
>>>>>>> Stashed changes

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	 // MX_LWIP_Process();
//	  gesture = PAJ7660_PollGesture();
//	  if(gesture != GESTURE_NONE){
//			  printf("Gesture code: %d\r\n", gesture);
//		  switch(gesture){
//			  case GESTURE_1_FINGER:
//				  gesture_feedback(GESTURE_1_FINGER);
//				  break;
//			  case GESTURE_2_FINGER:
//				  gesture_feedback(GESTURE_2_FINGER);
//				  break;
//			  case GESTURE_3_FINGER:
//				  gesture_feedback(GESTURE_3_FINGER);
//				  break;
//			  case GESTURE_4_FINGER:
//				  gesture_feedback(GESTURE_4_FINGER);
//				  break;
//			  case GESTURE_5_FINGER:
//				  gesture_feedback(GESTURE_5_FINGER);
//				  break;
//			  case GESTURE_LEFT:
//				  gesture_feedback(GESTURE_LEFT);
//				  break;
//			  case GESTURE_RIGHT:
//				  gesture_feedback(GESTURE_RIGHT);
//				  break;
//			  case GESTURE_CW:
//				  gesture_feedback(GESTURE_CW);
//				  break;
//			  case GESTURE_CCW:
//				  gesture_feedback(GESTURE_CCW);
//				  break;
////				  case FINGER_1_PUSH:
////					  PAJ7660_SetGestureMode(MODE_THUMB);
////					  break;
//			  default:
//				  gesture_feedback(31);
//				  break;
//		  }
//	  }
////	  else if(gesture_mode == MODE_THUMB){
////		  ThumbState_t thumb = PAJ7660_ReadThumbState(&hi2c1);
////		  switch (thumb) {
////		          case THUMB_UP:
////		              printf("Thumb Status: UP\r\n");
////		              gesture_feedback(GESTURE_THUMB_UP); // Your custom feedback function
////		              break;
////
////		          case THUMB_DOWN:
////		              printf("Thumb Status: DOWN\r\n");
////		              gesture_feedback(GESTURE_THUMB_DOWN);
////		              break;
////
////		          case THUMB_NONE:
////		              // No thumb detected, keep polling
////		              break;
////
////		          case THUMB_ERROR:
////		              // Handle I2C error (e.g., reset sensor or blink LED red)
////		              printf("Sensor Error!\r\n");
////		              break;
////		  }
////	  }
//	  HAL_Delay(150);
//
//
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//	  uint8_t current_gesture_id = read_gesture_id(); // You will implement this later
//
//	      if (current_gesture_id != 0) // Only update feedback if a new gesture is detected
//	      {
//	          gesture_feedback(current_gesture_id);
//	      }
//
//	      HAL_Delay(500);
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 240;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 5;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */
//
  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */
//
  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */
//
  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

}

/**
  * @brief USB_OTG_FS Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_OTG_FS_PCD_Init(void)
{

  /* USER CODE BEGIN USB_OTG_FS_Init 0 */
//
  /* USER CODE END USB_OTG_FS_Init 0 */

  /* USER CODE BEGIN USB_OTG_FS_Init 1 */
//
  /* USER CODE END USB_OTG_FS_Init 1 */
  hpcd_USB_OTG_FS.Instance = USB_OTG_FS;
  hpcd_USB_OTG_FS.Init.dev_endpoints = 4;
  hpcd_USB_OTG_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_OTG_FS.Init.dma_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd_USB_OTG_FS.Init.Sof_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.vbus_sensing_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.use_dedicated_ep1 = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_OTG_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_OTG_FS_Init 2 */
//
  /* USER CODE END USB_OTG_FS_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_2|GPIO_PIN_3|USB_PowerSwitchOn_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4
                          |GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin : USER_Btn_Pin */
  GPIO_InitStruct.Pin = USER_Btn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_Btn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PC0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB14 LD2_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_14|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PG2 PG3 USB_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|USB_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_OverCurrent_Pin */
  GPIO_InitStruct.Pin = USB_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_OverCurrent_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PD1 PD2 PD3 PD4
                           PD5 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4
                          |GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : PE0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void check_eth_link_status(void){
	uint32_t phy_reg_val;
	HAL_ETH_ReadPHYRegister(&heth, LAN8742A_PHY_ADDRESS, &phy_reg_val);

	if (phy_reg_val & 0x0004){
		DEBUG_LOG(LOG_LEVEL_INFO, "ETH link Status: UP");
	}
	else{
		DEBUG_LOG(LOG_LEVEL_WARN, "ETH link Status: DOWN");
	}
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* init code for LWIP */
  MX_LWIP_Init();
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartGestureTask */
/**
* @brief Function implementing the GestureTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartGestureTask */
void StartGestureTask(void *argument)
{
  /* USER CODE BEGIN StartGestureTask */
  uint8_t current_gesture;
  for(;;) {
	osDelay(150);
    current_gesture = PAJ7660_PollGesture();

    // Only send to queue if it's a valid gesture (not 0 and not 31)
    if (current_gesture != GESTURE_NONE && current_gesture != 31) {
        printf("Gesture Detected: %d\r\n", current_gesture);
        osMessageQueuePut(gestureQueueHandle, &current_gesture, 0, 0);
    }
    osDelay(150);
  }
  /* USER CODE END StartGestureTask */
}

/* USER CODE BEGIN Header_StartFeedbackTask */
/**
* @brief Function implementing the FeedbackTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartFeedbackTask */
void StartFeedbackTask(void *argument)
{
  /* USER CODE BEGIN StartFeedbackTask */
  uint8_t receivedGesture;
  for(;;)
  {
    // This task will block (wait) until a gesture ID is available in the queue
    if (osMessageQueueGet(gestureQueueHandle, &receivedGesture, NULL, osWaitForever) == osOK)
    {
       printf("Feedback Task: Processing gesture %d\r\n", receivedGesture);
       gesture_feedback(receivedGesture);
       // The task then loops back and waits for the next one
    }
  }
  /* USER CODE END StartFeedbackTask */
}

/* USER CODE BEGIN Header_StartModbusTask */
/**
* @brief Function implementing the ModbusTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartModbusTask */
void StartModbusTask(void *argument)
{
  /* USER CODE BEGIN StartModbusTask */
  uint8_t rx_data[8];
  uint8_t tx_data[256];

  for(;;)
  {
	  __HAL_UART_CLEAR_OREFLAG(&huart6);
	  __HAL_UART_CLEAR_NEFLAG(&huart6);
	  __HAL_UART_CLEAR_FEFLAG(&huart6);
    // Receive 8 bytes with a 50ms timeout
    if (HAL_UART_Receive(&huart6, rx_data, 8, 100) == HAL_OK)
    {
    	HAL_GPIO_TogglePin(GPIOB, LD2_Pin);
      // The function now handles slave_id check and CRC internally
    	uint16_t response_len = Modbus_ProcessRequest(rx_data, tx_data, 8);

      if (response_len > 0)
      {
        HAL_UART_Transmit(&huart6, tx_data, response_len, 100);
      }
    }
    osDelay(1); // Small yield for RTOS
  }
  /* USER CODE END StartModbusTask */
}

<<<<<<< Updated upstream
/* USER CODE BEGIN Header_StartMQTTTask */
/**
* @brief Function implementing the MQTTTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartMQTTTask */
void StartMQTTTask(void *argument)
{
=======
void StartMQTTTask(void *argument) {
>>>>>>> Stashed changes
  /* USER CODE BEGIN StartMQTTTask */
  unsigned char buf[200];
  int buflen = sizeof(buf);
  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
  int mysock = -1;

  printf("MQTT Task Started...\r\n");

  for(;;) {
    // 1. Wait for DHCP (Network must be ready)
    if (gnetif.ip_addr.addr != 0 && mysock < 0) {

      printf("Network Ready. Connecting to HiveMQ...\r\n");

      // 2. Open Socket
      mysock = socket(AF_INET, SOCK_STREAM, 0);

      struct sockaddr_in servaddr;
      servaddr.sin_family = AF_INET;
      servaddr.sin_port = htons(1883);
<<<<<<< Updated upstream
      servaddr.sin_addr.s_addr = inet_addr("3.125.105.15"); // HiveMQ Public IP
=======
      servaddr.sin_addr.s_addr = inet_addr("35.172.255.228"); // HiveMQ Public IP
>>>>>>> Stashed changes

      // 3. Connect TCP
      if (connect(mysock, (struct sockaddr*)&servaddr, sizeof(servaddr)) == 0) {
        printf("TCP Connected!\r\n");

        // 4. Paho: Serialize the Connect Packet
        data.MQTTVersion = 3;
        data.clientID.cstring = "STM32_F207_Node_Test";
        int len = MQTTSerialize_connect(buf, buflen, &data);

        // 5. Send Connect Packet to Broker
        send(mysock, buf, len, 0);

        // Brief delay to let the broker acknowledge
        osDelay(500);

        // 6. NEW: Test Publish Packet
        char *test_payload = "Gesture System Online";
        MQTTString topicString = MQTTString_initializer;
        topicString.cstring = "stm32/gesture/data/gestlink";

        int pub_len = MQTTSerialize_publish(buf, buflen, 0, 0, 0, 0, topicString, (unsigned char*)test_payload, strlen(test_payload));

        if (send(mysock, buf, pub_len, 0) > 0) {
            printf("MQTT Message Published to HiveMQ!\r\n");
        }

        // For now, we stay in this loop or close to avoid spamming the public broker
        // In a real app, you would stay connected. For this test, let's wait.
        osDelay(10000);

      } else {
        printf("TCP Connection Failed. Retrying...\r\n");
        close(mysock);
        mysock = -1;
      }
    }

    osDelay(2000); // Check/Retry every 2 seconds
  }
  /* USER CODE END StartMQTTTask */
}
/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
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
//  /* User can add his own implementation to report the file name and line number,
//     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
