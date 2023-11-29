/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "tof_lib.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define FLEX_SPI_I2C_N_Pin GPIO_PIN_13
#define FLEX_SPI_I2C_N_GPIO_Port GPIOC
#define LPn1_Pin GPIO_PIN_0
#define LPn1_GPIO_Port GPIOC
#define LPn2_Pin GPIO_PIN_1
#define LPn2_GPIO_Port GPIOC
#define LPn3_Pin GPIO_PIN_2
#define LPn3_GPIO_Port GPIOC
#define LPn4_Pin GPIO_PIN_3
#define LPn4_GPIO_Port GPIOC
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define INT_C_Pin GPIO_PIN_4
#define INT_C_GPIO_Port GPIOA
#define INT_C_EXTI_IRQn EXTI4_IRQn
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define INT1_Pin GPIO_PIN_6
#define INT1_GPIO_Port GPIOA
#define INT2_Pin GPIO_PIN_7
#define INT2_GPIO_Port GPIOA
#define PWR_EN_C_Pin GPIO_PIN_0
#define PWR_EN_C_GPIO_Port GPIOB
#define INT3_Pin GPIO_PIN_8
#define INT3_GPIO_Port GPIOA
#define INT4_Pin GPIO_PIN_9
#define INT4_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define I2C_RST_C_Pin GPIO_PIN_3
#define I2C_RST_C_GPIO_Port GPIOB
#define LPn_C_Pin GPIO_PIN_4
#define LPn_C_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
