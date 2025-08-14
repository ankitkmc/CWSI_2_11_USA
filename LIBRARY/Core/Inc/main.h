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
#include "stm32l4xx_hal.h"

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
#define RE7_Pin GPIO_PIN_3
#define RE7_GPIO_Port GPIOE
#define DE7_Pin GPIO_PIN_1
#define DE7_GPIO_Port GPIOE
#define UL1_Pin GPIO_PIN_8
#define UL1_GPIO_Port GPIOB
#define UB1_Pin GPIO_PIN_3
#define UB1_GPIO_Port GPIOH
#define EN_12V_Pin GPIO_PIN_7
#define EN_12V_GPIO_Port GPIOD
#define RE5_Pin GPIO_PIN_4
#define RE5_GPIO_Port GPIOE
#define DE5_Pin GPIO_PIN_2
#define DE5_GPIO_Port GPIOE
#define RE1_Pin GPIO_PIN_13
#define RE1_GPIO_Port GPIOC
#define DE1_Pin GPIO_PIN_5
#define DE1_GPIO_Port GPIOE
#define RG_M_INT_Pin GPIO_PIN_9
#define RG_M_INT_GPIO_Port GPIOA
#define RG_M_INT_EXTI_IRQn EXTI9_5_IRQn
#define NEOWAY_PWR_Pin GPIO_PIN_7
#define NEOWAY_PWR_GPIO_Port GPIOC
#define V3_8_Pin GPIO_PIN_6
#define V3_8_GPIO_Port GPIOC
#define SOLAR_IN_Pin GPIO_PIN_0
#define SOLAR_IN_GPIO_Port GPIOC
#define LUX_IN_Pin GPIO_PIN_1
#define LUX_IN_GPIO_Port GPIOC
#define BATTERY_IN_Pin GPIO_PIN_2
#define BATTERY_IN_GPIO_Port GPIOC
#define WD_M_IN_Pin GPIO_PIN_3
#define WD_M_IN_GPIO_Port GPIOC
#define IRRO1_IN_Pin GPIO_PIN_5
#define IRRO1_IN_GPIO_Port GPIOA
#define MUX2_Pin GPIO_PIN_4
#define MUX2_GPIO_Port GPIOC
#define WS_M_IN_Pin GPIO_PIN_15
#define WS_M_IN_GPIO_Port GPIOB
#define IRRO2_IN_Pin GPIO_PIN_6
#define IRRO2_IN_GPIO_Port GPIOA
#define IRRO1_1_Pin GPIO_PIN_2
#define IRRO1_1_GPIO_Port GPIOB
#define IRRO2_1_Pin GPIO_PIN_8
#define IRRO2_1_GPIO_Port GPIOE
#define DE2_Pin GPIO_PIN_10
#define DE2_GPIO_Port GPIOE
#define DE3_Pin GPIO_PIN_12
#define DE3_GPIO_Port GPIOE
#define MUX0_Pin GPIO_PIN_10
#define MUX0_GPIO_Port GPIOB
#define MUX1_Pin GPIO_PIN_11
#define MUX1_GPIO_Port GPIOB
#define DE4_Pin GPIO_PIN_12
#define DE4_GPIO_Port GPIOB
#define IRRO1_2_Pin GPIO_PIN_1
#define IRRO1_2_GPIO_Port GPIOB
#define IRRO2_2_Pin GPIO_PIN_7
#define IRRO2_2_GPIO_Port GPIOE
#define RE2_Pin GPIO_PIN_9
#define RE2_GPIO_Port GPIOE
#define RE3_Pin GPIO_PIN_11
#define RE3_GPIO_Port GPIOE
#define RE6_Pin GPIO_PIN_13
#define RE6_GPIO_Port GPIOE
#define DE6_Pin GPIO_PIN_14
#define DE6_GPIO_Port GPIOE
#define RE4_Pin GPIO_PIN_15
#define RE4_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
