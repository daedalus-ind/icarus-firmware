/**
 ******************************************************************************
 * @file    usart.hxx
 * @brief   This file contains all the function prototypes for
 *          the usart.cxx file.
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
#ifndef __USART_HXX__
#define __USART_HXX__

#include "main.hxx"

namespace serial {

  /**
   * @brief Write data to a USART peripheral.
   * @param usart_x Pointer to the USART peripheral (e.g., USART1, USART2, etc.)
   * @param data Pointer to the data buffer
   * @param len Length of the data buffer
   * @param timeout Timeout duration in milliseconds
   * @return `true` if the write operation was successful, `false` otherwise
   */
  bool write(USART_TypeDef* usart_x, uint8_t const* data, uint16_t len, uint32_t timeout);

  /**
   * @brief Read data from a USART peripheral.
   * @param usart_x Pointer to the USART peripheral (e.g., USART1, USART2, etc.)
   * @param data Pointer to the data buffer
   * @param len Length of the data buffer
   * @param timeout Timeout duration in milliseconds
   * @return `true` if the read operation was successful, `false` otherwise
   */
  bool read(USART_TypeDef* usart_x, uint8_t* data, uint16_t len, uint32_t timeout);

}

void MX_USART2_UART_Init();
void MX_USART3_UART_Init();

#endif /* __USART_HXX__ */
