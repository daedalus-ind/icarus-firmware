/**
 ******************************************************************************
 * @file    i2c.hxx
 * @brief   This file contains all the function prototypes for
 *          the i2c.cxx file.
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
#ifndef __I2C_HXX__
#define __I2C_HXX__

#include "main.hxx"

namespace i2c {

  /**
   * @brief Write data to an I2C device. 
   * @param i2c_x Pointer to the I2C peripheral (e.g., I2C1, I2C2, etc.)
   * @param dev_addr 7-bit I2C device address
   * @param reg_addr Register address to write to
   * @param data Pointer to the data buffer
   * @param len Length of the data buffer
   * @param timeout Timeout duration in milliseconds
   * @return `true` if the write operation was successful, `false` otherwise
   */
  bool write(I2C_TypeDef *i2c_x, uint8_t dev_addr, uint8_t reg_addr, uint8_t const* data, uint16_t len, uint32_t timeout);

  /**
   * @brief Read data from an I2C device.
   * @param i2c_x Pointer to the I2C peripheral (e.g., I2C1, I2C2, etc.)
   * @param dev_addr 7-bit I2C device address
   * @param reg_addr Register address to read from
   * @param data Pointer to the data buffer
   * @param len Length of the data buffer
   * @param timeout Timeout duration in milliseconds
   * @return `true` if the read operation was successful, `false` otherwise
   */
  bool read(I2C_TypeDef *i2c_x, uint8_t dev_addr, uint8_t reg_addr, uint8_t* data, uint16_t len, uint32_t timeout);

}

void MX_I2C3_Init();

#endif /* __I2C_HXX__ */
