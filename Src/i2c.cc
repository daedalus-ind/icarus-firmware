/**
 ******************************************************************************
 * @file    i2c.cc
 * @brief   This file provides code for the configuration
 *          of the I2C instances.
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
#include "i2c.hh"
#include "time.hh"

/* I2C3 init function */
void MX_I2C3_Init()
{

  LL_I2C_InitTypeDef I2C_InitStruct {};

  LL_GPIO_InitTypeDef GPIO_InitStruct {};

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  /** 
   * I2C3 GPIO Configuration
   * PC9   ------> I2C3_SDA
   * PA8   ------> I2C3_SCL
   */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_8;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Peripheral clock enable
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C3);

  // I2C Initialization
  LL_I2C_DisableOwnAddress2(I2C3);
  LL_I2C_DisableGeneralCall(I2C3);
  LL_I2C_EnableClockStretching(I2C3);
  I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
  I2C_InitStruct.ClockSpeed = 400000;
  I2C_InitStruct.DutyCycle = LL_I2C_DUTYCYCLE_2;
  I2C_InitStruct.OwnAddress1 = 0;
  I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
  I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
  LL_I2C_Init(I2C3, &I2C_InitStruct);
  LL_I2C_SetOwnAddress2(I2C3, 0);

}

bool i2c::write(I2C_TypeDef* i2c_x, uint8_t dev_addr, uint8_t reg_addr, const uint8_t* data, uint16_t len, uint32_t timeout) 
{
  /**
   * This procedure is based on paragraph "Master transmitter", section 27.3.3, page 851, of reference manual RM0090
   * for the STM32F405 series of microcontrollers.
   * 
   * @see https://www.st.com/resource/en/reference_manual/rm0090-stm32f405415-stm32f407417-stm32f427437-and-stm32f429439-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
   */
  time::ScopedTimer timer{timeout};

  // 1. Wait until I2C is not busy
  while (i2c_x->SR2 & I2C_SR2_BUSY) {
    if (timer.isExpired())
      goto STOP;
  }

  // 2. Disable POS flag
  i2c_x->CR1 &= ~I2C_CR1_POS;

  // 3. Enable acknowledge
  i2c_x->CR1 |= I2C_CR1_ACK;

  // 4. Generate START condition
  i2c_x->CR1 |= I2C_CR1_START;

  // 5. Wait for SB flag to be set (EV5)
  while (!(i2c_x->SR1 & I2C_SR1_SB)) {
    if (timer.isExpired())
      goto STOP;
  }

  // 6. Send slave address with write bit
  *reinterpret_cast<volatile uint8_t *>(&i2c_x->DR) = (dev_addr << 1) & ~0x01;

  // 7. Wait for ADDR flag to be set (EV6)
  while (!(i2c_x->SR1 & I2C_SR1_ADDR)) {
    if (timer.isExpired())
      goto STOP;
    else if (i2c_x->SR1 & I2C_SR1_AF)
      goto CLEAR_NACK;
  }

  // 8. Clear ADDR flag
  (void)i2c_x->SR1;
  (void)i2c_x->SR2;

  // 9. Wait for TXE flag to be set (EV8_1)
  while (!(i2c_x->SR1 & I2C_SR1_TXE)) {
    if (timer.isExpired())
      goto STOP;
  }

  // 10. Send register address
  *reinterpret_cast<volatile uint8_t *>(&i2c_x->DR) = reg_addr;

  // 11. Send data
  for (uint16_t i = 0; i < len; i++) {
    while (!(i2c_x->SR1 & I2C_SR1_TXE)) {
      if (timer.isExpired())
        goto STOP;
      else if (i2c_x->SR1 & I2C_SR1_AF)
        goto CLEAR_NACK;
    }
    *reinterpret_cast<volatile uint8_t *>(&i2c_x->DR) = data[i];
  }

  // 12. Wait until TXE and BTF flags are set (EV8_2)
  while (!(i2c_x->SR1 & I2C_SR1_TXE) || !(i2c_x->SR1 & I2C_SR1_BTF)) {
    if (timer.isExpired())
      goto STOP;
    else if (i2c_x->SR1 & I2C_SR1_AF)
      goto CLEAR_NACK;
  }

  // 13. Generate STOP condition
  i2c_x->CR1 |= I2C_CR1_STOP;

  return true;

CLEAR_NACK:
  i2c_x->SR1 &= ~I2C_SR1_AF;
STOP:
  i2c_x->CR1 |= I2C_CR1_STOP;
  return false;
}

bool i2c::read(I2C_TypeDef* i2c_x, uint8_t dev_addr, uint8_t reg_addr, uint8_t* data, uint16_t len, uint32_t timeout) 
{
  /**
   * This procedure is based on paragraph "Master receiver", section 27.3.3, page 852, of reference manual RM0090
   * for the STM32F405 series of microcontrollers.
   * @see https://www.st.com/resource/en/reference_manual/rm0090-stm32f405415-stm32f407417-stm32f427437-and-stm32f429439-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
   */
  time::ScopedTimer timer{timeout};

  // 1. Wait until I2C is not busy
  while (i2c_x->SR2 & I2C_SR2_BUSY) {
    if (timer.isExpired())
      goto STOP;
  }

  // 2. Disable POS flag
  i2c_x->CR1 &= ~I2C_CR1_POS;

  // 3. Enable acknowledge
  i2c_x->CR1 |= I2C_CR1_ACK;

  // 4. Generate START condition
  i2c_x->CR1 |= I2C_CR1_START;

  // 5. Wait for SB flag to be set (EV5)
  while (!(i2c_x->SR1 & I2C_SR1_SB)) {
    if (timer.isExpired())
      goto STOP;
  }

  // 6. Send slave address with the write bit (0)
  *reinterpret_cast<volatile uint8_t *>(&i2c_x->DR) = (dev_addr << 1) & ~0x01;

  // 7. Wait for ADDR flag to be set (EV6)
  while (!(i2c_x->SR1 & I2C_SR1_ADDR)) {
    if (timer.isExpired())
      goto STOP;
    else if (i2c_x->SR1 & I2C_SR1_AF)
      goto CLEAR_NACK;
  }

  // 8. Clear ADDR flag
  (void)i2c_x->SR1;
  (void)i2c_x->SR2;

  // 9. Wait for TXE flag to be set
  while (!(i2c_x->SR1 & I2C_SR1_TXE)) {
    if (timer.isExpired())
      goto STOP;
  }

  // 10. Send register address
  *reinterpret_cast<volatile uint8_t *>(&i2c_x->DR) = reg_addr;

  // 11. Wait until TXE flag is set
  while (!(i2c_x->SR1 & I2C_SR1_TXE)) {
    if (timer.isExpired())
      goto STOP;
    else if (i2c_x->SR1 & I2C_SR1_AF)
      goto CLEAR_NACK;
  }
 
  // 12. Generate RE-START condition
  i2c_x->CR1 |= I2C_CR1_START;

  // 13. Wait for SB flag to be set (EV5)
  while (!(i2c_x->SR1 & I2C_SR1_SB)) {
    if (timer.isExpired())
      goto STOP;
  }

  // 14. Send slave address with the read bit (1)
  *reinterpret_cast<volatile uint8_t *>(&i2c_x->DR) = (dev_addr << 1) | 0x01;

  // 15. Wait for ADDR flag to be set (EV6)
  while (!(i2c_x->SR1 & I2C_SR1_ADDR)) {
    if (timer.isExpired())
      goto STOP;
    else if (i2c_x->SR1 & I2C_SR1_AF)
      goto CLEAR_NACK;
  }

  // 16. The implementation is different depending on the number of bytes to read
  if (len == 0) {
    // Clear ADDR flag
    (void)i2c_x->SR1;
    (void)i2c_x->SR2;

    // Generate STOP condition
    i2c_x->CR1 |= I2C_CR1_STOP;
  }
  else if (len == 1) {
    // Disable ACK
    i2c_x->CR1 &= ~I2C_CR1_ACK;

    // Clear ADDR flag
    (void)i2c_x->SR1;
    (void)i2c_x->SR2;

    // Wait until RXNE flag is set
    while (!(i2c_x->SR1 & I2C_SR1_RXNE)) {
      if (timer.isExpired())
        goto STOP;
    }

    // Generate STOP condition
    i2c_x->CR1 |= I2C_CR1_STOP;

    // Read data
    *data = *reinterpret_cast<volatile uint8_t *>(&i2c_x->DR);
  } else if (len == 2) {
    // Disable ACK
    i2c_x->CR1 &= ~I2C_CR1_ACK;

    // Set POS flag
    i2c_x->CR1 |= I2C_CR1_POS;

    // Clear ADDR flag
    (void)i2c_x->SR1;
    (void)i2c_x->SR2;

    // Wait until BTF flag is set
    while (!(i2c_x->SR1 & I2C_SR1_BTF)) {
      if (timer.isExpired())
        goto STOP;
    }

    // Generate STOP condition
    i2c_x->CR1 |= I2C_CR1_STOP;

    // Read data
    data[0] = *reinterpret_cast<volatile uint8_t *>(&i2c_x->DR);
    data[1] = *reinterpret_cast<volatile uint8_t *>(&i2c_x->DR);
  } else {
    // Clear ADDR flag
    (void)i2c_x->SR1;
    (void)i2c_x->SR2;

    // Read data until len-3
    for (uint16_t i = 0; i < (len - 3); i++) {
      // Wait until RXNE flag is set
      while (!(i2c_x->SR1 & I2C_SR1_RXNE)) {
        if (timer.isExpired())
          goto STOP;
      }
      data[i] = *reinterpret_cast<volatile uint8_t *>(&i2c_x->DR);
    }

    // Wait until BTF flag is set (data len-3 in DR, data len-2 in SR)
    while (!(i2c_x->SR1 & I2C_SR1_BTF)) {
      if (timer.isExpired())
        goto STOP;
    }

    // Disable ACK
    i2c_x->CR1 &= ~I2C_CR1_ACK;

    // Read data len-3
    data[len - 3] = *reinterpret_cast<volatile uint8_t *>(&i2c_x->DR);

    // Wait until BTF flag is set (data len-2 in DR, data len-1 in SR)
    while (!(i2c_x->SR1 & I2C_SR1_BTF)) {
      if (timer.isExpired())
        goto STOP;
    }

    // Generate STOP condition
    i2c_x->CR1 |= I2C_CR1_STOP;

    // Read data len-2 and len-1
    data[len - 2] = *reinterpret_cast<volatile uint8_t *>(&i2c_x->DR);
    data[len - 1] = *reinterpret_cast<volatile uint8_t *>(&i2c_x->DR);
  }

  return true;

CLEAR_NACK:
  i2c_x->SR1 &= ~I2C_SR1_AF;
STOP:
  i2c_x->CR1 |= I2C_CR1_STOP;
  return false;
}
