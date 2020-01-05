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
#define ENC1_CHAN_A_Pin GPIO_PIN_1
#define ENC1_CHAN_A_GPIO_Port GPIOA
#define ENC1_CHAN_A_EXTI_IRQn EXTI1_IRQn
#define ENC1_CHAN_B_Pin GPIO_PIN_2
#define ENC1_CHAN_B_GPIO_Port GPIOA
#define ENC1_CHAN_B_EXTI_IRQn EXTI2_IRQn
#define ROW1_IN_Pin GPIO_PIN_6
#define ROW1_IN_GPIO_Port GPIOA
#define ROW2_IN_Pin GPIO_PIN_7
#define ROW2_IN_GPIO_Port GPIOA
#define ROW3_IN_Pin GPIO_PIN_0
#define ROW3_IN_GPIO_Port GPIOB
#define COL3_OUT_Pin GPIO_PIN_1
#define COL3_OUT_GPIO_Port GPIOB
#define COL2_OUT_Pin GPIO_PIN_10
#define COL2_OUT_GPIO_Port GPIOB
#define COL1_OUT_Pin GPIO_PIN_11
#define COL1_OUT_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
