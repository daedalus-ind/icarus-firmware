/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.h
  * @brief   This file contains all the function prototypes for
  *          the i2c.c file
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
#ifndef __I2C_H__
#define __I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include "tick.h"
/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_I2C1_Init(void);
void MX_I2C2_Init(void);

/* USER CODE BEGIN Prototypes */

/**
 * @brief Transmit data over I2C.
 * @param I2Cx Pointer to the I2C peripheral.
 * @param address The I2C address of the device.
 * @param reg The register address to write to.
 * @param data Pointer to the data to be transmitted.
 * @param size The number of bytes to transmit.
 * @param timeout Timeout value (in milliseconds).
 * @return The status of the operation (true: success, false: timeout).
 */
bool I2Cx_Transmit8(I2C_TypeDef *I2Cx, uint8_t address, uint8_t reg, const uint8_t *data, uint16_t size, uint32_t timeout);

/**
 * @brief Receive data over I2C.
 * @param I2Cx Pointer to the I2C peripheral.
 * @param address The I2C address of the device.
 * @param reg The register address to read from.
 * @param data Pointer to the buffer to store the received data.
 * @param size The number of bytes to receive.
 * @param timeout Timeout value (in milliseconds).
 * @return The status of the operation (true: success, false: timeout).
 */
bool I2Cx_Receive8(I2C_TypeDef *I2Cx, uint8_t address, uint8_t reg, uint8_t *data, uint16_t size, uint32_t timeout);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __I2C_H__ */

