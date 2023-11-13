/* USER CODE BEGIN Header */
/********************************************************************************
*
* Copyright (c) 2018 STMicroelectronics - All Rights Reserved
*
* License terms: STMicroelectronics Proprietary in accordance with licensing
* terms at www.st.com/sla0081
*
* STMicroelectronics confidential
* Reproduction and Communication of this document is strictly prohibited unless
* specifically authorized in writing by STMicroelectronics.
*
*******************************************************************************/

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "apps_layer_utils.h"
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

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

extern int LMZ_gesture_app_main(int start_ranging, uint8_t *xtalk_calib_buf);
extern int SC_HandleCmd(const char *Buffer);
int handle_cmd(void);

// XTALK CALIBRATION //
// copy the code generated in your UART terminal after the command "calibrate xtalk"
// uint8_t 				Xtalk_cal_buf[VL53LMZ_XTALK_BUFFER_SIZE] = {00,01,02,03,
//																	04,05,...};
uint8_t Xtalk_cal_buf[VL53LMZ_XTALK_BUFFER_SIZE] = {
0x9f, 0xd8, 0x00, 0xc0,
0x03, 0x20, 0x09, 0x60,
0x0b, 0x08, 0x08, 0x17,
0x08, 0x08, 0x08, 0x03,
0x9f, 0xe4, 0x01, 0x40,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x01, 0xe0, 0x00, 0x20,
0x00, 0x00, 0x00, 0x20,
0x9f, 0xf8, 0x00, 0x40,
0x17, 0x17, 0x17, 0x17,
0x9f, 0xfc, 0x04, 0x04,
0x00, 0x00, 0x46, 0xa4,
0x00, 0x00, 0x37, 0x66,
0x00, 0x00, 0x26, 0x60,
0x00, 0x00, 0x1c, 0xbc,
0x00, 0x00, 0x17, 0x73,
0x00, 0x00, 0x11, 0x25,
0x00, 0x00, 0x11, 0x07,
0x00, 0x00, 0x0e, 0x63,
0x00, 0x00, 0x8b, 0x4c,
0x00, 0x00, 0x60, 0xa2,
0x00, 0x00, 0x3d, 0xc0,
0x00, 0x00, 0x26, 0xaa,
0x00, 0x00, 0x1b, 0xc2,
0x00, 0x00, 0x18, 0x04,
0x00, 0x00, 0x14, 0x97,
0x00, 0x00, 0x10, 0xed,
0x00, 0x01, 0x28, 0x1b,
0x00, 0x00, 0x93, 0xf0,
0x00, 0x00, 0x57, 0x61,
0x00, 0x00, 0x30, 0x2b,
0x00, 0x00, 0x20, 0xaa,
0x00, 0x00, 0x1a, 0xb6,
0x00, 0x00, 0x15, 0xc3,
0x00, 0x00, 0x16, 0x0e,
0x00, 0x01, 0x7f, 0xbb,
0x00, 0x00, 0xad, 0x58,
0x00, 0x00, 0x71, 0xaf,
0x00, 0x00, 0x36, 0xd9,
0x00, 0x00, 0x22, 0xfb,
0x00, 0x00, 0x1c, 0x96,
0x00, 0x00, 0x18, 0x83,
0x00, 0x00, 0x17, 0x96,
0x00, 0x01, 0x90, 0x00,
0x00, 0x00, 0x97, 0xd6,
0x00, 0x00, 0x66, 0x3b,
0x00, 0x00, 0x33, 0x0a,
0x00, 0x00, 0x20, 0xcd,
0x00, 0x00, 0x19, 0x38,
0x00, 0x00, 0x16, 0xa5,
0x00, 0x00, 0x14, 0xbb,
0x00, 0x00, 0xaf, 0xcf,
0x00, 0x00, 0x65, 0x7d,
0x00, 0x00, 0x3d, 0x93,
0x00, 0x00, 0x29, 0xd1,
0x00, 0x00, 0x19, 0x4e,
0x00, 0x00, 0x15, 0xba,
0x00, 0x00, 0x11, 0xc6,
0x00, 0x00, 0x12, 0x7f,
0x00, 0x00, 0x73, 0x1d,
0x00, 0x00, 0x42, 0x2c,
0x00, 0x00, 0x2e, 0x82,
0x00, 0x00, 0x1e, 0x80,
0x00, 0x00, 0x18, 0x1c,
0x00, 0x00, 0x13, 0x2d,
0x00, 0x00, 0x0f, 0xc6,
0x00, 0x00, 0x0f, 0x85,
0x00, 0x00, 0x4f, 0x04,
0x00, 0x00, 0x33, 0xe9,
0x00, 0x00, 0x1f, 0x06,
0x00, 0x00, 0x18, 0x40,
0x00, 0x00, 0x13, 0x2c,
0x00, 0x00, 0x12, 0x97,
0x00, 0x00, 0x0e, 0x01,
0x00, 0x00, 0x0d, 0xac,
0xa0, 0xfc, 0x01, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x03,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0xa1, 0x0c, 0x01, 0x00,
0x00, 0x00, 0x00, 0x80,
0x00, 0x00, 0x00, 0x03,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0xa1, 0x1c, 0x00, 0xc0,
0x00, 0x00, 0x70, 0xeb,
0x0c, 0x80, 0x01, 0xe0,
0x00, 0x00, 0x00, 0x26,
0xa1, 0x28, 0x09, 0x02,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x01, 0x00, 0x00,
0x00, 0x36, 0x00, 0x03,
0x01, 0xd9, 0x01, 0x43,
0x02, 0x33, 0x02, 0x17,
0x02, 0x4b, 0x02, 0x41,
0x01, 0x17, 0x02, 0x22,
0x00, 0x27, 0x00, 0x5d,
0x00, 0x05, 0x00, 0x11,
0x00, 0x00, 0x00, 0x01,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0xa2, 0x48, 0x00, 0x40,
0x00, 0x00, 0x00, 0x00,
0xa2, 0x4c, 0x00, 0x81,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0xa2, 0x54, 0x00, 0x81,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0xa2, 0x5c, 0x00, 0x81,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0xa2, 0x64, 0x00, 0x81,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0xa2, 0x6c, 0x00, 0x84,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0xa2, 0x8c, 0x00, 0x82,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x0f,
0x00, 0x01, 0x03, 0x04,
};

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
extern volatile int IntrCount;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin==INT_C_Pin)
	{
		IntrCount++;
	}
}

//https://electronics.stackexchange.com/questions/206113/how-do-i-use-the-printf-function-on-stm32
__attribute__((weak)) int __io_putchar(int ch)
{
    HAL_StatusTypeDef status = HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
    return (status == HAL_OK ? ch : 0);
}

__attribute__((weak)) int __io_getchar(void)
{
	int ch = 0;
    HAL_StatusTypeDef status = HAL_UART_Receive(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
    return (status == HAL_OK ? ch : 0);
}

// Code to enable sending data through the UART: Host->STM32
#define UART_BUFFER_SIZE    2048
volatile char Uart_RXBuffer[UART_BUFFER_SIZE];
char UartComm_RXBuffer[UART_BUFFER_SIZE];
int UartComm_RXSize = 0;
volatile size_t Uart_RxRcvIndex = 0;
volatile uint32_t Uart_nOverrun = 0;
volatile int UartComm_CmdReady = 0;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (Uart_RXBuffer[Uart_RxRcvIndex] == '\r'
				|| Uart_RXBuffer[Uart_RxRcvIndex] == '\n'
				|| Uart_RXBuffer[Uart_RxRcvIndex] == '\x03') //Ctrl-C)
	{

		if (Uart_RXBuffer[Uart_RxRcvIndex] != '\x03') // \x0x3 is Ctrl-c
    		Uart_RXBuffer[Uart_RxRcvIndex] = 0;

		memcpy(UartComm_RXBuffer ,
				(char*)Uart_RXBuffer,
				Uart_RxRcvIndex+1 ); //copy ending 0
        UartComm_RXSize = Uart_RxRcvIndex;
		UartComm_CmdReady = 1;

		Uart_RxRcvIndex = 0;
	}
	else
	{
		if (Uart_RxRcvIndex < UART_BUFFER_SIZE)
		{
			Uart_RxRcvIndex++;
		}
		else
		{
			Uart_RxRcvIndex = 0;
			Uart_nOverrun++;
		}
	}

	HAL_UART_Receive_IT(huart,
					(uint8_t *) &Uart_RXBuffer[Uart_RxRcvIndex],
					1);
}

void UartComm_Start() {
	HAL_UART_StateTypeDef State;
	UartComm_CmdReady = 0;
    Uart_RxRcvIndex = 0;

    State = HAL_UART_GetState(&huart2);
	if (State != HAL_UART_STATE_BUSY_TX_RX && State != HAL_UART_STATE_BUSY_RX) {
        // arm it
        HAL_UART_Receive_IT(&huart2, (uint8_t *) Uart_RXBuffer, 1);
    }

}

void HAL_UART_ErrorCallback(UART_HandleTypeDef * huart) {
    // clear error and  kick of next
    huart->ErrorCode = 0;
    HAL_UART_Receive_IT(huart, (uint8_t *) &Uart_RXBuffer[Uart_RxRcvIndex], Uart_RxRcvIndex);

}


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
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  UartComm_Start();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  LMZ_gesture_app_main(1, NULL);
	  // XTALK CALIBRATION //
	  // To load the Xtalk Calibration buffer in the sensor, use the command below, using Xtalk_cal_buf
	  // LMZ_gesture_app_main(1, Xtalk_cal_buf);
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

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
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
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

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 921600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(FLEX_SPI_I2C_N_GPIO_Port, FLEX_SPI_I2C_N_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, PWR_EN_C_Pin|LPn_C_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(I2C_RST_C_GPIO_Port, I2C_RST_C_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : FLEX_SPI_I2C_N_Pin */
  GPIO_InitStruct.Pin = FLEX_SPI_I2C_N_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(FLEX_SPI_I2C_N_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : INT_C_Pin */
  GPIO_InitStruct.Pin = INT_C_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(INT_C_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PWR_EN_C_Pin LPn_C_Pin */
  GPIO_InitStruct.Pin = PWR_EN_C_Pin|LPn_C_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : I2C_RST_C_Pin */
  GPIO_InitStruct.Pin = I2C_RST_C_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(I2C_RST_C_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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

#ifdef  USE_FULL_ASSERT
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
