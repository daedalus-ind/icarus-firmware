/**
 ******************************************************************************
 * @file    spi.hxx
 * @brief   This file contains all the function prototypes for
 *          the spi.cxx file.
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
#ifndef __SPI_HXX__
#define __SPI_HXX__

#include "main.hxx"

namespace spi {

  /**
   * @brief Class to manage a chip select (CS) pin for SPI communication.
   */
  class ChipSelectPin {
  public:
    /* Constructor to initialize the CS pin */
    inline explicit ChipSelectPin(GPIO_TypeDef *port, uint32_t pin) 
      : _port{port}, 
        _pin{pin} {
      deselect();
    }

    /**
     * @brief Select the SPI device by setting the CS pin low.
     */
    inline void select() const {
      LL_GPIO_ResetOutputPin(_port, _pin);
    }

    /**
     * @brief Deselect the SPI device by setting the CS pin high.
     */
    inline void deselect() const {
      LL_GPIO_SetOutputPin(_port, _pin);
    }

  private:
    GPIO_TypeDef *_port;
    uint32_t _pin;
  };

  /**
   * @brief Write data to an SPI device.
   * @param spi_x Pointer to the SPI peripheral (e.g., SPI1, SPI2, etc.)
   * @param reg Register address to write to
   * @param data Pointer to the data buffer
   * @param len Length of the data buffer
   * @param timeout Timeout duration in milliseconds
   * @return `true` if the write operation was successful, `false` otherwise
   */
  bool write(SPI_TypeDef *spi_x, uint16_t reg, uint8_t const *data, uint16_t len, uint32_t timeout);

  /**
   * @brief Read data from an SPI device.
   * @param spi_x Pointer to the SPI peripheral (e.g., SPI1, SPI2, etc.)
   * @param reg Register address to read from
   * @param data Pointer to the data buffer
   * @param len Length of the data buffer
   * @param timeout Timeout duration in milliseconds
   * @return `true` if the read operation was successful, `false` otherwise
   */
  bool read(SPI_TypeDef *spi_x, uint16_t reg, uint8_t *data, uint16_t len, uint32_t timeout);

  /**
   * @brief Read dummy data from an SPI device (used for devices that require dummy reads).
   * @param spi_x Pointer to the SPI peripheral (e.g., SPI1, SPI2, etc.)
   * @param reg Register address to read from
   * @param len Length of the dummy data to read
   * @param timeout Timeout duration in milliseconds
   * @return `true` if the dummy read operation was successful, `false` otherwise
   */
  bool read_dummy(SPI_TypeDef *spi_x, uint16_t reg, uint16_t len, uint32_t timeout);

}

void MX_SPI2_Init();

#endif /* __SPI_HXX__ */
