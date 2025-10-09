/**
 ******************************************************************************
 * @file    adxl375.hxx
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
#ifndef __SENSORS_ADXL375_HXX__
#define __SENSORS_ADXL375_HXX__

#include "main.hxx"
#include "adxl375_regs.hxx"
#include "math/vec3.hxx"

namespace sensors {
  
  class ADXL375 {
  private: 
    I2C_TypeDef *m_i2c;
    uint8_t m_address;

    inline bool write_reg(uint8_t reg_addr, const uint8_t* data, uint16_t size); 
    inline bool read_reg(uint8_t reg_addr, uint8_t* data, uint16_t size);
  public: 
    ADXL375(I2C_TypeDef *i2c, uint8_t address = 0x53)
      : m_i2c(i2c), m_address(address) {}
    
    bool init();
    bool set_standby_mode(bool enabled);
    bool set_odr(adxl::ODR odr);
    bool get_accel(math::Vec3& accel);
  };

} 

#endif /* __SENSORS_ADXL375_HXX__ */
