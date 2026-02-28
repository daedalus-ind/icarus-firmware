/**
 ******************************************************************************
 * @file    spi.cc
 * @brief   This file provides code for the configuration
 *          of the SPI instances.
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
#include "spi.hh"
#include "time.hh"

/* SPI2 init function */
void MX_SPI2_Init()
{

  LL_SPI_InitTypeDef SPI_InitStruct = {};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
  /**
   * SPI2 GPIO Configuration
   * PB13   ------> SPI2_SCK
   * PB14   ------> SPI2_MISO
   * PB15   ------> SPI2_MOSI
   */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_13|LL_GPIO_PIN_14|LL_GPIO_PIN_15;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
  SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
  SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV2;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 10;
  LL_SPI_Init(SPI2, &SPI_InitStruct);
  LL_SPI_SetStandard(SPI2, LL_SPI_PROTOCOL_MOTOROLA);

}

bool spi::write(SPI_TypeDef *spi_x, uint16_t reg, const uint8_t* data, uint16_t len, uint32_t timeout) {
  time::ScopedTimer timer{timeout};

  if (reg > 0xFF) {
    // 1.a. Wait for TXE flag to be set
    while (!(spi_x->SR & SPI_SR_TXE)) {
      if (timer.isExpired())
        return false;
    }

    // 1.b. Send high byte of register address
    *reinterpret_cast<volatile uint8_t *>(&spi_x->DR) = (reg >> 8) & 0xFF;

    // 1.c. Wait until RXNE flag is set
    while (!(spi_x->SR & SPI_SR_RXNE)) {
      if (timer.isExpired())
        return false;
    }

    // 1.d. Read dummy data to clear RXNE flag
    (void)spi_x->DR;
  }

  // 1. Wait for TXE flag to be set
  while (!(spi_x->SR & SPI_SR_TXE)) {
    if (timer.isExpired())
      return false;
  }

  // 2. Send low byte of register address
  *reinterpret_cast<volatile uint8_t *>(&spi_x->DR) = reg & 0xFF;

  // 3. Wait until RXNE flag is set
  while (!(spi_x->SR & SPI_SR_RXNE)) {
    if (timer.isExpired())
      return false;
  }

  // 4. Read dummy data to clear RXNE flag
  (void)spi_x->DR;

  // 5. Send data
  for (uint16_t i = 0; i < len; i++) {
    // 5.a. Wait for TXE flag to be set
    while (!(spi_x->SR & SPI_SR_TXE)) {
      if (timer.isExpired())
        return false;
    }

    // 5.b. Send data byte
    *reinterpret_cast<volatile uint8_t *>(&spi_x->DR) = data[i];

    // 5.c. Wait until RXNE flag is set
    while (!(spi_x->SR & SPI_SR_RXNE)) {
      if (timer.isExpired())
        return false;
    }

    // 5.d. Read dummy data to clear RXNE flag
    (void)spi_x->DR;
  }

  // 6. Wait for BSY flag to be reset
  while (spi_x->SR & SPI_SR_BSY) {
    if (timer.isExpired())
      return false;
  }

  return true;
}

bool spi::read(SPI_TypeDef *spi_x, uint16_t reg, uint8_t* data, uint16_t len, uint32_t timeout) 
{
  time::ScopedTimer timer{timeout};

  if (reg > 0xFF) {
    // 1.a. Wait for TXE flag to be set
    while (!(spi_x->SR & SPI_SR_TXE)) {
      if (timer.isExpired())
        return false;
    }

    // 1.b. Send high byte of register address
    *reinterpret_cast<volatile uint8_t *>(&spi_x->DR) = (reg >> 8) & 0xFF;

    // 1.c. Wait until RXNE flag is set
    while (!(spi_x->SR & SPI_SR_RXNE)) {
      if (timer.isExpired())
        return false;
    }

    // 1.d. Read dummy data to clear RXNE flag
    (void)spi_x->DR;
  }

  // 1. Wait for TXE flag to be set
  while (!(spi_x->SR & SPI_SR_TXE)) {
    if (timer.isExpired())
      return false;
  }

  // 2. Send low byte of register address
  *reinterpret_cast<volatile uint8_t *>(&spi_x->DR) = reg & 0xFF;

  // 3. Wait until RXNE flag is set
  while (!(spi_x->SR & SPI_SR_RXNE)) {
    if (timer.isExpired())
      return false;
  }

  // 4. Read dummy data to clear RXNE flag
  (void)spi_x->DR;

  // 5. Read data
  for (uint16_t i = 0; i < len; i++) {
    // 5.a. Wait for TXE flag to be set
    while (!(spi_x->SR & SPI_SR_TXE)) {
      if (timer.isExpired())
        return false;
    }

    // 5.b. Send dummy byte to generate clock for reading
    *reinterpret_cast<volatile uint8_t *>(&spi_x->DR) = 0x00;

    // 5.c. Wait until RXNE flag is set
    while (!(spi_x->SR & SPI_SR_RXNE)) {
      if (timer.isExpired())
        return false;
    }

    // 5.d. Read data byte
    data[i] = *reinterpret_cast<volatile uint8_t *>(&spi_x->DR);
  }

  // 6. Wait for BSY flag to be reset
  while (spi_x->SR & SPI_SR_BSY) {
    if (timer.isExpired())
      return false;
  }

  return true;
}

bool spi::read_dummy(SPI_TypeDef *spi_x, uint16_t reg, uint16_t len, uint32_t timeout) 
{
  time::ScopedTimer timer{timeout};

  if (reg > 0xFF) {
    // 1.a. Wait for TXE flag to be set
    while (!(spi_x->SR & SPI_SR_TXE)) {
      if (timer.isExpired())
        return false;
    }

    // 1.b. Send high byte of register address
    *reinterpret_cast<volatile uint8_t *>(&spi_x->DR) = (reg >> 8) & 0xFF;

    // 1.c. Wait until RXNE flag is set
    while (!(spi_x->SR & SPI_SR_RXNE)) {
      if (timer.isExpired())
        return false;
    }

    // 1.d. Read dummy data to clear RXNE flag
    (void)spi_x->DR;
  }

  // 1. Wait for TXE flag to be set
  while (!(spi_x->SR & SPI_SR_TXE)) {
    if (timer.isExpired())
      return false;
  }

  // 2. Send low byte of register address with read bit (0x80)
  *reinterpret_cast<volatile uint8_t *>(&spi_x->DR) = reg & 0xFF;

  // 3. Wait until RXNE flag is set
  while (!(spi_x->SR & SPI_SR_RXNE)) {
    if (timer.isExpired())
      return false;
  }

  // 4. Read dummy data to clear RXNE flag
  (void)spi_x->DR;

  // 5. Read dummy data
  for (uint16_t i = 0; i < len; i++) {
    // 5.a. Wait for TXE flag to be set
    while (!(spi_x->SR & SPI_SR_TXE)) {
      if (timer.isExpired())
        return false;
    }

    // 5.b. Send dummy byte to generate clock for reading
    *reinterpret_cast<volatile uint8_t *>(&spi_x->DR) = 0x00;

    // 5.c. Wait until RXNE flag is set
    while (!(spi_x->SR & SPI_SR_RXNE)) {
      if (timer.isExpired())
        return false;
    }

    // 5.d. Read dummy data to clear RXNE flag
    (void)spi_x->DR;
  }

  // 6. Wait for BSY flag to be reset
  while (spi_x->SR & SPI_SR_BSY) {
    if (timer.isExpired())
      return false;
  }

  return true;
}
