/**
 ******************************************************************************
 * @file    icm-45686.hh
 * @brief   This file contains the driver for the ICM-45686 IMU.
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
#ifndef __ICM_45686_HH__
#define __ICM_45686_HH__

#include "main.hh"
#include "spi.hh"
#include "icm-45686_regs.hh"
#include "math/vec3.hh"

#include <optional>
#include <utility>

namespace sensors {

  /**
   * @brief Class to interface with the ICM-45686 IMU via SPI.
   */
  class ICM_45686 {
  public: 
    ICM_45686(SPI_TypeDef* spi, GPIO_TypeDef* cs_port, uint32_t cs_pin)
      : _spi{spi}, _cs_pin{cs_port, cs_pin} {}

    /**
     * @brief Initialize the ICM-45686 sensor.
     * @return `true` if initialization was successful, `false` otherwise.
     */
    bool init();

    /**
     * @brief Perform a soft reset of the ICM-45686 sensor.
     * @return `true` if the soft reset was successful, `false` otherwise.
     */
    bool softReset();

    /**
     * @brief Verify the identity of the ICM-45686 sensor.
     * @return `true` if the sensor identity is correct, `false` otherwise.
     */
    bool enableAccelerometer(icm::AccelODR odr, icm::AccelFS fs);

    /**
     * @brief Enable the gyroscope.
     * @param odr The output data rate to set.
     * @param fs The full scale range to set.
     * @return `true` if the gyroscope was enabled successfully, `false` otherwise.
     */
    bool enableGyroscope(icm::GyroODR odr, icm::GyroFS fs);
    
    /**
     * @brief Calibrate the gyroscope by computing the bias over a number of samples.
     * @param samples The number of samples to use for calibration (default is 100).
     * @return `true` if calibration was successful, `false` otherwise.
     */
    bool calibrateGyroscope(size_t samples = 100); 

    /**
     * @brief Get the current accelerometer readings.
     * @param accel Reference to a Vec3 object to store the accelerometer readings.
     * @return `true` if the readings were successfully retrieved, `false` otherwise.
     */
    std::optional<math::Vec3> getAcceleration();

    /**
     * @brief Get the current gyroscope readings.
     * @param gyro Reference to a Vec3 object to store the gyroscope readings.
     * @return `true` if the readings were successfully retrieved, `false` otherwise.
     */
    std::optional<math::Vec3> getGyroscope();

    /**
     * @brief Enable the FIFO buffer for accelerometer and gyroscope data.
     * @return `true` if the FIFO was enabled successfully, `false` otherwise.
     */
    bool enableFIFO();

    /**
     * @brief Get the current number of samples in the FIFO buffer.
     * @return An optional uint16_t containing the FIFO sample count, or std::nullopt if the read failed.
     */
    std::optional<uint16_t> getFIFOCount();

    /**
     * @brief Retrieve the latest accelerometer and gyroscope data from the FIFO buffer.
     * @return An optional pair of Vec3 objects containing the accelerometer and gyroscope data,
     *         or std::nullopt if the read operation fails.
     */
    std::optional<std::pair<math::Vec3, math::Vec3>> getLatestFIFOData();

  private: 
    SPI_TypeDef* _spi;
    spi::ChipSelectPin _cs_pin;

    float _accel_scale = 0.0f;
    float _gyro_scale = 0.0f;

    math::Vec3 _accel_bias = math::Vec3::zero();
    math::Vec3 _gyro_bias = math::Vec3::zero();

    inline bool _writeReg(uint16_t reg, const uint8_t* data, uint16_t size);
    inline bool _readReg(uint16_t reg, uint8_t* data, uint16_t size);
  };

}

#endif /* __ICM_45686_HH__ */
