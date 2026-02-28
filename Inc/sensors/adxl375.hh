/**
 ******************************************************************************
 * @file    adxl375.hh
 * @brief   This file contains the class definitions for the ADXL375
 *          3-axis digital accelerometer.
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
#ifndef __ADXL375_HH__
#define __ADXL375_HH__

#include "main.hh"
#include "adxl375_regs.hh"
#include "math/vec3.hh"

#include <optional>

namespace sensors {
  
  /**
   * @brief Class to interface with the ADXL375 3-axis digital accelerometer via I2C.
   */
  class ADXL375 {
  public: 
    ADXL375(I2C_TypeDef* i2c, uint8_t address = adxl::I2C_ADDRESS)
      : _i2c{i2c}, _address{address} {}
    
    /**
     * @brief Initializes the ADXL375 sensor.
     * @return `true` if initialization is successful, `false` otherwise.
     */
    bool init();

    /**
     * @brief Sets the standby mode of the ADXL375 sensor.
     * @param enabled `true` to enable standby mode, `false` to disable it.
     * @return `true` if the operation is successful, `false` otherwise.
     */
    bool setStandbyMode(bool enabled);

    /**
     * @brief Sets the output data rate (ODR) of the ADXL375 sensor.
     * @param odr The desired output data rate.
     * @return `true` if the operation is successful, `false` otherwise.
     */
    bool setODR(adxl::ODR odr);

    /**
     * @brief Retrieves the current acceleration data from the ADXL375 sensor.
     * @return An optional `math::Vec3` containing the acceleration data in g's,
     *         or `std::nullopt` if the read operation fails.
     */
    std::optional<math::Vec3> getAcceleration();

  private: 
    I2C_TypeDef* _i2c;
    uint8_t _address;

    inline bool _writeReg(uint8_t reg_addr, const uint8_t* data, uint16_t size); 
    inline bool _readReg(uint8_t reg_addr, uint8_t* data, uint16_t size);
  };

} 

#endif /* __ADXL375_HH__ */
