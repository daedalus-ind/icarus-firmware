/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "stm32f4xx_ll_adc.h"
#include "stm32f4xx_ll_i2c.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_spi.h"
#include "stm32f4xx_ll_tim.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_gpio.h"

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

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
#define USER_LED_Pin LL_GPIO_PIN_13
#define USER_LED_GPIO_Port GPIOC
#define PYRO_A_FIRE_Pin LL_GPIO_PIN_14
#define PYRO_A_FIRE_GPIO_Port GPIOC
#define PYRO_B_FIRE_Pin LL_GPIO_PIN_15
#define PYRO_B_FIRE_GPIO_Port GPIOC
#define PYRO_A_SENSE_Pin LL_GPIO_PIN_0
#define PYRO_A_SENSE_GPIO_Port GPIOC
#define PYRO_B_SENSE_Pin LL_GPIO_PIN_1
#define PYRO_B_SENSE_GPIO_Port GPIOC
#define RF_M1_Pin LL_GPIO_PIN_3
#define RF_M1_GPIO_Port GPIOC
#define RF_M0_Pin LL_GPIO_PIN_0
#define RF_M0_GPIO_Port GPIOA
#define RF_AUX_Pin LL_GPIO_PIN_1
#define RF_AUX_GPIO_Port GPIOA
#define RF_UART_RX_Pin LL_GPIO_PIN_2
#define RF_UART_RX_GPIO_Port GPIOA
#define RX_UART_TX_Pin LL_GPIO_PIN_3
#define RX_UART_TX_GPIO_Port GPIOA
#define FLASH_CS_Pin LL_GPIO_PIN_4
#define FLASH_CS_GPIO_Port GPIOA
#define FLASH_SPI_SCK_Pin LL_GPIO_PIN_5
#define FLASH_SPI_SCK_GPIO_Port GPIOA
#define FLASH_SPI_MISO_Pin LL_GPIO_PIN_6
#define FLASH_SPI_MISO_GPIO_Port GPIOA
#define FLASH_SPI_MOSI_Pin LL_GPIO_PIN_7
#define FLASH_SPI_MOSI_GPIO_Port GPIOA
#define ACC_INT_Pin LL_GPIO_PIN_2
#define ACC_INT_GPIO_Port GPIOB
#define ACC_I2C_SCL_Pin LL_GPIO_PIN_10
#define ACC_I2C_SCL_GPIO_Port GPIOB
#define ACC_I2C_SCLB11_Pin LL_GPIO_PIN_11
#define ACC_I2C_SCLB11_GPIO_Port GPIOB
#define BAR_CS_Pin LL_GPIO_PIN_12
#define BAR_CS_GPIO_Port GPIOB
#define MAG_CS_Pin LL_GPIO_PIN_6
#define MAG_CS_GPIO_Port GPIOC
#define IMU_CS_Pin LL_GPIO_PIN_7
#define IMU_CS_GPIO_Port GPIOC
#define IMU_INT_Pin LL_GPIO_PIN_8
#define IMU_INT_GPIO_Port GPIOC
#define GPS_UART_RX_Pin LL_GPIO_PIN_10
#define GPS_UART_RX_GPIO_Port GPIOC
#define GPS_UART_TX_Pin LL_GPIO_PIN_11
#define GPS_UART_TX_GPIO_Port GPIOC
#define SERVO1_PWM_Pin LL_GPIO_PIN_4
#define SERVO1_PWM_GPIO_Port GPIOB
#define SERVO2_PWM_Pin LL_GPIO_PIN_5
#define SERVO2_PWM_GPIO_Port GPIOB
#define SERVO3_PWM_Pin LL_GPIO_PIN_6
#define SERVO3_PWM_GPIO_Port GPIOB
#define SERVO4_PWM_Pin LL_GPIO_PIN_7
#define SERVO4_PWM_GPIO_Port GPIOB
#define GPS_I2C_SCL_Pin LL_GPIO_PIN_8
#define GPS_I2C_SCL_GPIO_Port GPIOB
#define GPS_I2C_SDA_Pin LL_GPIO_PIN_9
#define GPS_I2C_SDA_GPIO_Port GPIOB
#ifndef NVIC_PRIORITYGROUP_0
#define NVIC_PRIORITYGROUP_0         ((uint32_t)0x00000007) /*!< 0 bit  for pre-emption priority,
                                                                 4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1         ((uint32_t)0x00000006) /*!< 1 bit  for pre-emption priority,
                                                                 3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2         ((uint32_t)0x00000005) /*!< 2 bits for pre-emption priority,
                                                                 2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3         ((uint32_t)0x00000004) /*!< 3 bits for pre-emption priority,
                                                                 1 bit  for subpriority */
#define NVIC_PRIORITYGROUP_4         ((uint32_t)0x00000003) /*!< 4 bits for pre-emption priority,
                                                                 0 bit  for subpriority */
#endif

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
