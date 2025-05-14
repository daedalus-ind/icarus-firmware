/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    spi.h
  * @brief   This file contains all the function prototypes for
  *          the spi.c file
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
#ifndef __SPI_H__
#define __SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include <stddef.h>
#include <stdbool.h>
#include "tick.h"
/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define SPI_OK 0 
#define SPI_TIMEOUT 1
/* USER CODE END Private defines */

void MX_SPI1_Init(void);
void MX_SPI2_Init(void);

/* USER CODE BEGIN Prototypes */

/**
 * @brief Transmit and receive bytes of data over SPI.
 * @param SPIx Pointer to the SPI peripheral.
 * @param tx_data The data to be transmitted.
 * @param rx_data The buffer to store the received data (can be `NULL`).
 * @param size The number of bytes to transmit/receive.
 * @param timeout Timeout value (in milliseconds).
 * @return The status of the operation (true: success, false: timeout).
 */
bool SPIx_TransmitReceive8(SPI_TypeDef *SPIx, const uint8_t *tx_data, uint8_t *rx_data, uint16_t size, uint32_t timeout);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __SPI_H__ */

