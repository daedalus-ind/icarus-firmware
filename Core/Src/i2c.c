/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.c
  * @brief   This file provides code for the configuration
  *          of the I2C instances.
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
/* Includes ------------------------------------------------------------------*/
#include "i2c.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* I2C1 init function */
void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  LL_I2C_InitTypeDef I2C_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
  /**I2C1 GPIO Configuration
  PB8   ------> I2C1_SCL
  PB9   ------> I2C1_SDA
  */
  GPIO_InitStruct.Pin = GPS_I2C_SCL_Pin|GPS_I2C_SDA_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */

  /** I2C Initialization
  */
  LL_I2C_DisableOwnAddress2(I2C1);
  LL_I2C_DisableGeneralCall(I2C1);
  LL_I2C_EnableClockStretching(I2C1);
  I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
  I2C_InitStruct.ClockSpeed = 400000;
  I2C_InitStruct.DutyCycle = LL_I2C_DUTYCYCLE_2;
  I2C_InitStruct.OwnAddress1 = 0;
  I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
  I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
  LL_I2C_Init(I2C1, &I2C_InitStruct);
  LL_I2C_SetOwnAddress2(I2C1, 0);
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}
/* I2C2 init function */
void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  LL_I2C_InitTypeDef I2C_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
  /**I2C2 GPIO Configuration
  PB10   ------> I2C2_SCL
  PB11   ------> I2C2_SDA
  */
  GPIO_InitStruct.Pin = ACC_I2C_SCL_Pin|ACC_I2C_SCLB11_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */

  /** I2C Initialization
  */
  LL_I2C_DisableOwnAddress2(I2C2);
  LL_I2C_DisableGeneralCall(I2C2);
  LL_I2C_EnableClockStretching(I2C2);
  I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
  I2C_InitStruct.ClockSpeed = 400000;
  I2C_InitStruct.DutyCycle = LL_I2C_DUTYCYCLE_2;
  I2C_InitStruct.OwnAddress1 = 0;
  I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
  I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
  LL_I2C_Init(I2C2, &I2C_InitStruct);
  LL_I2C_SetOwnAddress2(I2C2, 0);
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/* USER CODE BEGIN 1 */

bool I2Cx_Transmit8(I2C_TypeDef *I2Cx, uint8_t slave_addr, uint8_t reg, const uint8_t *data, uint16_t size, uint32_t timeout
) {
  uint32_t tickstart = Tick_GetTick();

  // 0. Wait until the I2C bus is not busy 
  while (I2Cx->SR2 & I2C_SR2_BUSY) {
    if ((Tick_GetTick() - tickstart) > timeout) {
      return false; // Timeout
    }
  }

  // 1. Generate start condition 
  I2Cx->CR1 |= I2C_CR1_START;
  while (!(I2Cx->SR1 & I2C_SR1_SB)) { // Wait for SB (Start Bit) flag
    if ((Tick_GetTick() - tickstart) > timeout) {
      I2Cx->CR1 |= I2C_CR1_STOP; // Try to release the bus
      return false; // Timeout
    }
  }

  // 2. Send slave address with write bit (0) 
  I2Cx->DR = (slave_addr << 1) & ~I2C_OAR1_ADD0;
  while (!(I2Cx->SR1 & I2C_SR1_ADDR)) { // Wait for ADDR flag
    if (I2Cx->SR1 & I2C_SR1_AF) { // Check of acknowledge failure
      I2Cx->SR1 &= ~I2C_SR1_AF; // Clear AF flag
      I2Cx->CR1 |= I2C_CR1_STOP;
      return false; // NACK
    }
    if ((Tick_GetTick() - tickstart) > timeout) {
      I2Cx->CR1 |= I2C_CR1_STOP;
      return false; // Timeout
    }
  }
  (void)I2Cx->SR2; // Clear ADDR flag

  // 3. Send register address
  while (!(I2Cx->SR1 & I2C_SR1_TXE)) { // Wait for TXE
    if ((Tick_GetTick() - tickstart) > timeout) {
      I2Cx->CR1 |= I2C_CR1_STOP;
      return false; // Timeout
    }
  }
  // Send the register address
  I2Cx->DR = reg;

  // Wait until register transfer is finished
  while (!(I2Cx->SR1 & I2C_SR1_BTF)) {
    if (I2Cx->SR1 & I2C_SR1_AF) {
      I2Cx->SR1 &= ~I2C_SR1_AF;
      I2Cx->CR1 |= I2C_CR1_STOP;
      return false; // NACK
    }
    if ((Tick_GetTick() - tickstart) > timeout) {
      I2Cx->CR1 |= I2C_CR1_STOP;
      return false; // Timeout
    }
  }

  // 4. Send the data
  while (size > 0) {
    while (!(I2Cx->SR1 & I2C_SR1_TXE)) { // Wait for TXE
      if (I2Cx->SR1 & I2C_SR1_AF) {
        I2Cx->SR1 &= ~I2C_SR1_AF;
        I2Cx->CR1 |= I2C_CR1_STOP;
        return false; // NACK
      }
      if ((Tick_GetTick() - tickstart) > timeout) {
        I2Cx->CR1 |= I2C_CR1_STOP;
        return false; // Timeout
      }
    }
    I2Cx->DR = *data; // Transmit data

    data++;
    size--;
  }

  while (!(I2Cx->SR1 & I2C_SR1_BTF)) { // Wait for BTF
    if (I2Cx->SR1 & I2C_SR1_AF) {
      I2Cx->SR1 &= ~I2C_SR1_AF;
      I2Cx->CR1 |= I2C_CR1_STOP;
      return false; // NACK
    }
    if ((Tick_GetTick() - tickstart) > timeout) {
      I2Cx->CR1 |= I2C_CR1_STOP;
      return false; // Timeout
    }
  }

  // 5. Generate stop condition
  I2Cx->CR1 |= I2C_CR1_STOP;

  return true;
}

bool I2Cx_Receive8(I2C_TypeDef *I2Cx, uint8_t address, uint8_t reg, uint8_t *data, uint16_t size, uint32_t timeout) {
  uint32_t tickstart = Tick_GetTick();

  // 1. Generate START condition 
  I2Cx->CR1 |= I2C_CR1_START;
  while (!(I2Cx->SR1 & I2C_SR1_SB)) { // Wait for SB flag
    if ((Tick_GetTick() - tickstart) > timeout) {
      I2Cx->CR1 |= I2C_CR1_STOP; // Try to release the bus
      return false; // Timeout
    }
  }

  // 2. Send slave address with write bit (0)
  I2Cx->DR = (address << 1) & ~I2C_OAR1_ADD0;
  while (!(I2Cx->SR1 & I2C_SR1_ADDR)) { // Wait for ADDR flag
    if (I2Cx->SR1 & I2C_SR1_AF) { 
      I2Cx->SR1 &= ~I2C_SR1_AF; 
      I2Cx->CR1 |= I2C_CR1_STOP;
      return false; // NACK
    }
    if ((Tick_GetTick() - tickstart) > timeout) {
      I2Cx->CR1 |= I2C_CR1_STOP;
      return false; // Timeout
    }
  }
  (void)I2Cx->SR2; // Clear ADDR flag

  // 3. Send register address
  while (!(I2Cx->SR1 & I2C_SR1_TXE)) { // Wait for TXE
    if ((Tick_GetTick() - tickstart) > timeout) {
      I2Cx->CR1 |= I2C_CR1_STOP;
      return false; // Timeout
    }
  }
  // Send the register address
  I2Cx->DR = reg;

  // Wait until register transfer is finished
  while (!(I2Cx->SR1 & I2C_SR1_BTF)) {
    if (I2Cx->SR1 & I2C_SR1_AF) {
      I2Cx->SR1 &= ~I2C_SR1_AF;
      I2Cx->CR1 |= I2C_CR1_STOP;
      return false; // NACK
    }
    if ((Tick_GetTick() - tickstart) > timeout) {
      I2Cx->CR1 |= I2C_CR1_STOP;
      return false; // Timeout
    }
  }

  // 5. Generate REPEATED START condition
  I2Cx->CR1 |= I2C_CR1_START;
  while (!(I2Cx->SR1 & I2C_SR1_SB)) { // Wait for SB flag
    if ((Tick_GetTick() - tickstart) > timeout) {
      // Do not release the bus here
      return false; // Timeout
    }
  }

  // 6. Set ACK and POS accordling
  if (size == 1) {
    I2Cx->CR1 &= ~I2C_CR1_ACK; // NACK for the only byte
  } else if (size == 2) {
    I2Cx->CR1 |= I2C_CR1_ACK; // ACK for the first byte
    I2Cx->CR1 |= I2C_CR1_POS; // Set POS to 1 for the second byte
  } else {
    I2Cx->CR1 |= I2C_CR1_ACK; // Enable ACK
  }

  // 7. Send slave address with read bit (1)
  I2Cx->DR = (address << 1) | 0x01; 
  while (!(I2Cx->SR1 & I2C_SR1_ADDR)) {
    if (I2Cx->SR1 & I2C_SR1_AF) {
      I2Cx->SR1 &= ~I2C_SR1_AF;
      I2Cx->CR1 |= I2C_CR1_STOP;
      return false; // NACK
    }
    if ((Tick_GetTick() - tickstart) > timeout) {
      I2Cx->CR1 |= I2C_CR1_STOP;
      return false; // Timeout
    }
  }
  (void)I2Cx->SR1; 
  (void)I2Cx->SR2;

  if (size == 1) {
    I2Cx->CR1 |= I2C_CR1_STOP;
      while (!(I2Cx->SR1 & I2C_SR1_RXNE)) {
      if ((Tick_GetTick() - tickstart) > timeout) {
        return false; // Timeout
      }
      *data = I2Cx->DR;
    }
  } else if (size == 2) {
    I2Cx->CR1 &= ~I2C_CR1_POS;

    while (!(I2Cx->SR1 & I2C_SR1_BTF)) {
      if ((Tick_GetTick() - tickstart) > timeout) { 
        I2Cx->CR1 |= I2C_CR1_STOP;
        return false; // Timeout
      }
    }

    I2Cx->CR1 |= I2C_CR1_STOP;
    *data++ = I2Cx->DR; // Read first byte
    *data = I2Cx->DR; // Read second byte
  } else {
    // Read N-2 bytes 
    while (size > 2) {
      while (!(I2Cx->SR1 & I2C_SR1_RXNE)) { // Wait for RXNE for each byte
        if ((Tick_GetTick() - tickstart) > timeout) { 
          I2Cx->CR1 |= I2C_CR1_STOP;
          return false; }
      }
      *data++ = I2Cx->DR;
      size--;
    }

    while (!(I2Cx->SR1 & I2C_SR1_BTF)) { // Wait for BTF
      if ((Tick_GetTick() - tickstart) > timeout) { 
        I2Cx->CR1 |= I2C_CR1_STOP; 
        return false; 
      }
    }

    // Set NACK for the last byte
    I2Cx->CR1 &= ~I2C_CR1_ACK;
    // Read N-1 byte
    *data++ = I2Cx->DR;

    // Generate STOP condition
    I2Cx->CR1 |= I2C_CR1_STOP;
    // Read last byte
    *data = I2Cx->DR;        
  }

  return true;
}
/* USER CODE END 1 */
