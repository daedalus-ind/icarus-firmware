/**
 ******************************************************************************
 * @file    main.hxx
 * @brief   Header for main.cxx file.
 *          This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 Daedalus Industries.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
#ifndef __MAIN_HXX__
#define __MAIN_HXX__

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
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_gpio.h"

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

extern "C" void Error_Handler();

#define CCMRAM __attribute__((section(".ccmram")))

#define RADIO_USART USART2
#define GPS_UART USART3 

#define MAG_I2C I2C3

#define LED_Pin LL_GPIO_PIN_13
#define LED_GPIO_Port GPIOC
#define RADIO_M0_Pin LL_GPIO_PIN_3
#define RADIO_M0_GPIO_Port GPIOC
#define RADIO_M1_Pin LL_GPIO_PIN_0
#define RADIO_M1_GPIO_Port GPIOA
#define RADIO_AUX_Pin LL_GPIO_PIN_1
#define RADIO_AUX_GPIO_Port GPIOA
#define BARO_CS_Pin LL_GPIO_PIN_12
#define BARO_CS_GPIO_Port GPIOB
#define IMU_CS_Pin LL_GPIO_PIN_6
#define IMU_CS_GPIO_Port GPIOC
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

#endif /* __MAIN_HXX__ */
