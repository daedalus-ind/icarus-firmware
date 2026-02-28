/**
 ******************************************************************************
 * @file    lis2mdl.hh
 * @brief   This file contains the driver for the LIS2MDL magnetometer.
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
#ifndef __LIS2MDL_HH__
#define __LIS2MDL_HH__

#include "main.hh"
#include "lis2mdl_regs.hh"
#include "math/vec3.hh"
#include "math/mat3x3.hh"

#include <optional>

namespace sensors {

  /**
   * @brief Class to interface with the LIS2MDL magnetometer via I2C.
   */
  class LIS2MDL {
  public: 
    LIS2MDL(I2C_TypeDef* i2c, uint8_t address = 0x1E) 
      : _i2c{i2c}, _address{address} {}

    /**
     * @brief Initialize the LIS2MDL sensor.
     * @return `true` if initialization was successful, `false` otherwise.
     */
    bool init();

    /**
     * @brief Reset the LIS2MDL sensor to default settings.
     * @return `true` if reset was successful, `false` otherwise.
     */
    bool reset();

    /**
     * @brief Set the operating mode of the LIS2MDL sensor.
     * @param mode The desired operating mode.
     * @return `true` if the operation was successful, `false` otherwise.
     */
    bool setMode(lismdl::Mode mode);

    /**
     * @brief Set the output data rate (ODR) of the LIS2MDL sensor.
     * @param odr The desired output data rate.
     * @return `true` if the operation was successful, `false` otherwise.
     */
    bool setODR(lismdl::ODR odr);

    /**
     * @brief Enable or disable the low-pass filter (LPF) on the LIS2MDL sensor.
     * @param enabled `true` to enable the LPF, `false` to disable it.
     * @return `true` if the operation was successful, `false` otherwise.
     */
    bool setLPF(bool enabled = true);

    /**
     * @brief Set the soft-iron correction matrix and hard-iron offset vector
     *        for magnetometer calibration.
     * @param soft_iron_matrix The 3x3 soft-iron correction matrix.
     * @param hard_iron_offset The 3D hard-iron offset vector.
     */
    void setCorrectionParameters(const math::Mat3x3& soft_iron_matrix,
                                   const math::Vec3& hard_iron_offset);

    /**
     * @brief Retrieve the current magnetic field data from the LIS2MDL sensor.
     * @return An optional `math::Vec3` containing the magnetic field data in
     *         microteslas (µT), or `std::nullopt` if the read operation fails.
     */
    std::optional<math::Vec3> getMagneticField();

    /**
     * @brief Retrieve the current temperature from the LIS2MDL sensor.
     * @return An optional `float` containing the temperature in degrees Celsius,
     *         or `std::nullopt` if the read operation fails.
     */
    std::optional<float> getTemperature();

  private: 
    I2C_TypeDef* _i2c;
    uint8_t _address; 

    math::Mat3x3 _soft_iron_matrix = math::Mat3x3::identity();
    math::Vec3 _hard_iron_offset = math::Vec3::zero();

    inline bool _softReset();
    inline bool _reboot();
    inline bool _enableTemperatureCompensation();
    inline bool _enableBDU();

    inline bool _writeReg(uint8_t reg_addr, const uint8_t* data, uint16_t size); 
    inline bool _readReg(uint8_t reg_addr, uint8_t* data, uint16_t size);
  };

}

#endif /* __SENSORS_LIS2MDL_HH__ */
