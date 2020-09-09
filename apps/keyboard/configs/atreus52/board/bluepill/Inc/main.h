/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
#define IDLE_LED_Pin GPIO_PIN_13
#define IDLE_LED_GPIO_Port GPIOC
#define COL1_OUT_Pin GPIO_PIN_0
#define COL1_OUT_GPIO_Port GPIOA
#define COL2_OUT_Pin GPIO_PIN_1
#define COL2_OUT_GPIO_Port GPIOA
#define COL3_OUT_Pin GPIO_PIN_2
#define COL3_OUT_GPIO_Port GPIOA
#define COL4_OUT_Pin GPIO_PIN_3
#define COL4_OUT_GPIO_Port GPIOA
#define COL5_OUT_Pin GPIO_PIN_4
#define COL5_OUT_GPIO_Port GPIOA
#define COL6_OUT_Pin GPIO_PIN_5
#define COL6_OUT_GPIO_Port GPIOA
#define ROW1_IN_Pin GPIO_PIN_0
#define ROW1_IN_GPIO_Port GPIOB
#define ROW2_IN_Pin GPIO_PIN_10
#define ROW2_IN_GPIO_Port GPIOB
#define COL9_OUT_Pin GPIO_PIN_8
#define COL9_OUT_GPIO_Port GPIOA
#define COL11_OUT_Pin GPIO_PIN_9
#define COL11_OUT_GPIO_Port GPIOA
#define COL10_OUT_Pin GPIO_PIN_10
#define COL10_OUT_GPIO_Port GPIOA
#define COL12_OUT_Pin GPIO_PIN_15
#define COL12_OUT_GPIO_Port GPIOA
#define COL13_OUT_Pin GPIO_PIN_3
#define COL13_OUT_GPIO_Port GPIOB
#define COL14_OUT_Pin GPIO_PIN_4
#define COL14_OUT_GPIO_Port GPIOB
#define ROW3_IN_Pin GPIO_PIN_5
#define ROW3_IN_GPIO_Port GPIOB
#define COL7_OUT_Pin GPIO_PIN_6
#define COL7_OUT_GPIO_Port GPIOB
#define COL8_OUT_Pin GPIO_PIN_7
#define COL8_OUT_GPIO_Port GPIOB
#define ROW4_IN_Pin GPIO_PIN_8
#define ROW4_IN_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
