/**
 ******************************************************************************
 * @file    usart.cc
 * @brief   This file provides code for the configuration
 *          of the USART instances.
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
#include "usart.hh"
#include "stm32f4xx_ll_usart.h"
#include "time.hh"

/* USART2 init function */
void MX_USART2_UART_Init()
{

  LL_USART_InitTypeDef USART_InitStruct = {};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {};

  // Peripheral clock enable
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  /**
   * USART2 GPIO Configuration
   * PA2   ------> USART2_TX
   * PA3   ------> USART2_RX
   */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_2|LL_GPIO_PIN_3;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // USART2 interrupt Init
  NVIC_SetPriority(USART2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(USART2_IRQn);

  USART_InitStruct.BaudRate = 9600;
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
  LL_USART_Init(USART2, &USART_InitStruct);
  LL_USART_ConfigAsyncMode(USART2);
  LL_USART_Enable(USART2);

}

/* USART3 init function */
void MX_USART3_UART_Init()
{

  LL_USART_InitTypeDef USART_InitStruct = {};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {};

  // Peripheral clock enable
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
  /**
   * USART3 GPIO Configuration
   * PC10   ------> USART3_TX
   * PC11   ------> USART3_RX
   */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_10|LL_GPIO_PIN_11;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  // USART3 interrupt Init
  NVIC_SetPriority(USART3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(USART3_IRQn);

  USART_InitStruct.BaudRate = 38400;
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
  LL_USART_Init(USART3, &USART_InitStruct);
  LL_USART_ConfigAsyncMode(USART3);
  LL_USART_Enable(USART3);

}

bool serial::write(USART_TypeDef* usart_x, const uint8_t* data, uint16_t len, uint32_t timeout) 
{
  time::ScopedTimer timer{timeout};

  for (uint16_t i = 0; i < len; i++) {
    // Wait for TXE flag is set
    while (!(usart_x->SR & USART_SR_TXE)) {
      if (timer.isExpired())
        return false;
    }
    
    // Send data
    *reinterpret_cast<volatile uint8_t *>(&usart_x->DR) = data[i];
  }

  return true;
}

bool serial::read(USART_TypeDef* usart_x, uint8_t* data, uint16_t len, uint32_t timeout) 
{
  time::ScopedTimer timer{timeout};

  for (uint16_t i = 0; i < len; i++) {
    // Wait for RXNE flag is set
    while (!(usart_x->SR & USART_SR_RXNE)) {
      if (timer.isExpired())
        return false;
    }

    // Read data
    data[i] = *reinterpret_cast<volatile uint8_t *>(&usart_x->DR);
  }

  return true;
}

void serial::setBaudRate(USART_TypeDef* usart_x, uint32_t baudrate)
{
  LL_RCC_ClocksTypeDef rcc_clocks;
  LL_RCC_GetSystemClocksFreq(&rcc_clocks);
  while (!LL_USART_IsActiveFlag_TC(usart_x)) {}
  LL_USART_Disable(usart_x);

  // Handle different peripheral clocks
  uint32_t periph_clk = 0;
  switch (reinterpret_cast<uintptr_t>(usart_x)) {
    case USART1_BASE:
    case USART6_BASE:
      periph_clk = rcc_clocks.PCLK2_Frequency;
      break;
    case USART2_BASE:
    case USART3_BASE:
    case UART4_BASE:
    case UART5_BASE:
      periph_clk = rcc_clocks.PCLK1_Frequency;
      break;
    default:
      LL_USART_Enable(usart_x);
      return; // Invalid USART instance
  }

  LL_USART_SetBaudRate(usart_x, periph_clk, LL_USART_OVERSAMPLING_16, baudrate);
  LL_USART_Enable(usart_x);
}
