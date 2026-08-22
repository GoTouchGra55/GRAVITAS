/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "stm32h7xx_hal.h"

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BMP_CS_Pin GPIO_PIN_0
#define BMP_CS_GPIO_Port GPIOC
#define BMP_MOSI_Pin GPIO_PIN_1
#define BMP_MOSI_GPIO_Port GPIOC
#define BMP_MISO_Pin GPIO_PIN_2
#define BMP_MISO_GPIO_Port GPIOC
#define BMP_INT_Pin GPIO_PIN_3
#define BMP_INT_GPIO_Port GPIOC
#define GRID_CH2_Pin GPIO_PIN_1
#define GRID_CH2_GPIO_Port GPIOA
#define GRID_CH4_Pin GPIO_PIN_3
#define GRID_CH4_GPIO_Port GPIOA
#define IMU_CS_Pin GPIO_PIN_4
#define IMU_CS_GPIO_Port GPIOA
#define IMU_SCK_Pin GPIO_PIN_5
#define IMU_SCK_GPIO_Port GPIOA
#define IMU_MISO_Pin GPIO_PIN_6
#define IMU_MISO_GPIO_Port GPIOA
#define IMU_MOSI_Pin GPIO_PIN_7
#define IMU_MOSI_GPIO_Port GPIOA
#define IMU_INT1_Pin GPIO_PIN_4
#define IMU_INT1_GPIO_Port GPIOC
#define IMU_INT2_Pin GPIO_PIN_5
#define IMU_INT2_GPIO_Port GPIOC
#define SX_MOSI_Pin GPIO_PIN_2
#define SX_MOSI_GPIO_Port GPIOB
#define TVC_CH1_Pin GPIO_PIN_9
#define TVC_CH1_GPIO_Port GPIOE
#define TVC_CH2_Pin GPIO_PIN_11
#define TVC_CH2_GPIO_Port GPIOE
#define GRID_CH3_Pin GPIO_PIN_10
#define GRID_CH3_GPIO_Port GPIOB
#define BMP_SCK_Pin GPIO_PIN_13
#define BMP_SCK_GPIO_Port GPIOB
#define PYRO_CH1_Pin GPIO_PIN_6
#define PYRO_CH1_GPIO_Port GPIOC
#define PYRO_CH2_Pin GPIO_PIN_7
#define PYRO_CH2_GPIO_Port GPIOC
#define PYRO_CH3_Pin GPIO_PIN_8
#define PYRO_CH3_GPIO_Port GPIOC
#define PYRO_CH4_Pin GPIO_PIN_9
#define PYRO_CH4_GPIO_Port GPIOC
#define GRID_CH1_Pin GPIO_PIN_15
#define GRID_CH1_GPIO_Port GPIOA
#define SX_SCK_Pin GPIO_PIN_10
#define SX_SCK_GPIO_Port GPIOC
#define SX_MISO_Pin GPIO_PIN_11
#define SX_MISO_GPIO_Port GPIOC
#define SX_CS_Pin GPIO_PIN_12
#define SX_CS_GPIO_Port GPIOC
#define SX_NRST_Pin GPIO_PIN_0
#define SX_NRST_GPIO_Port GPIOD
#define SX_BUSY_Pin GPIO_PIN_1
#define SX_BUSY_GPIO_Port GPIOD
#define SD_CD_Pin GPIO_PIN_7
#define SD_CD_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */
typedef struct{
  float gyro_x;
  float gyro_y;
  float gyro_z;
} GYRO_DATA;

typedef struct{
  float accel_x;
  float accel_y;
  float accel_z;
} ACC_DATA;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
