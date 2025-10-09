/**
 ******************************************************************************
 * @file    lis2mdl.hxx
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
#ifndef __SENSORS_LIS2MDL_HXX__
#define __SENSORS_LIS2MDL_HXX__

#include "main.hxx"
#include "lis2mdl_regs.hxx"

#include "math/vec3.hxx"
#include "math/mat3x3.hxx"

namespace sensors {

  class LIS2MDL {
  private: 
    I2C_TypeDef* m_i2c;
    uint8_t m_address; 

    math::Mat3x3 m_soft_iron_matrix;
    math::Vec3 m_hard_iron_offset;

    inline bool soft_reset();
    inline bool reboot();
    inline bool enable_temperature_compensation();
    inline bool enable_bdu();

    inline bool write_reg(uint8_t reg_addr, const uint8_t* data, uint16_t size); 
    inline bool read_reg(uint8_t reg_addr, uint8_t* data, uint16_t size);

  public: 
    LIS2MDL(I2C_TypeDef* i2c, uint8_t address = 0x1E) 
      : m_i2c(i2c), 
        m_address(address),
        m_soft_iron_matrix(math::Mat3x3::eye()),
        m_hard_iron_offset(math::Vec3::zero()) {}

    bool init();
    bool reset();

    bool set_mode(lismdl::Mode mode);
    bool set_odr(lismdl::ODR odr);
    bool set_lpf(bool enabled = true);

    void set_calibration_matrix(math::Mat3x3 const& soft_iron_matrix);
    void set_calibration_offset(math::Vec3 const& hard_iron_offset);

    bool get_mag(math::Vec3& mag);
    bool get_temp(float& temperature);
  };

}

#endif /* __SENSORS_LIS2MDL_HXX__ */
