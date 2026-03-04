/**
 ******************************************************************************
 * @file    lis2mdl.cc
 * @brief   This file contains the implementation of the LIS2MDL driver.
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

#include "sensors/lis2mdl.hh"
#include "time.hh"
#include "i2c.hh"

using namespace sensors::lismdl;

bool sensors::LIS2MDL::init() 
{
  bool result = true;

  // Enable I2C line, if not already enabled
  if (!(_i2c->CR1 & I2C_CR1_PE))
    _i2c->CR1 |= I2C_CR1_PE;

  // Check device ID
  uint8_t who_am_i = 0;
  result &= _readReg(REG_WHO_AM_I, &who_am_i, sizeof(who_am_i));
  result &= (who_am_i == WHO_AM_I_CONTENT);

  // Reset the device
  result &= reset();

  return result;
}

bool sensors::LIS2MDL::reset() 
{
  bool result = true;

  // Perform a soft reset
  result &= _softReset();

  /**
   * @note AN5069
   * The LIS2MDL requires a delay of 5us after the soft reset before
   * rebooting the device.
   */
  time::delay(1);

  // Perform reboot
  result &= _reboot();

  /**
   * @note AN5069
   * The LIS2MDL requires a delay of 20ms after the reboot.
   */
  time::delay(20);

  // Enable temperature compensation
  result &= _enableTemperatureCompensation();

  // Enable Block Data Update (BDU)
  result &= _enableBDU();

  return result;
}

bool sensors::LIS2MDL::setMode(Mode mode) 
{
  bool result = true; 

  // Read CGC_REG_A
  CfgA cfg_a;
  result &= _readReg(REG_CFG_A, reinterpret_cast<uint8_t *>(&cfg_a), sizeof(cfg_a));

  cfg_a.mode = mode; // Set mode
  
  // Write back CGC_REG_A
  result &= _writeReg(REG_CFG_A, reinterpret_cast<uint8_t *>(&cfg_a), sizeof(cfg_a));

  return result;
}

bool sensors::LIS2MDL::setODR(ODR odr) 
{
  bool result = true; 

  // Read CGC_REG_A
  CfgA cfg_a;
  result &= _readReg(REG_CFG_A, reinterpret_cast<uint8_t *>(&cfg_a), sizeof(cfg_a));

  cfg_a.odr = odr; // Set ODR
  
  // Write back CGC_REG_A
  result &= _writeReg(REG_CFG_A, reinterpret_cast<uint8_t *>(&cfg_a), sizeof(cfg_a));

  return result;
}

void sensors::LIS2MDL::setCorrectionParameters(const math::Mat3x3& soft_iron_matrix,
                                               const math::Vec3& hard_iron_offset) 
{
  _soft_iron_matrix = soft_iron_matrix;
  _hard_iron_offset = hard_iron_offset;
}

bool sensors::LIS2MDL::setLPF(bool enabled) 
{
  bool result = true; 

  // Read CGC_REG_B
  CfgB cfg_b;
  result &= _readReg(REG_CFG_B, reinterpret_cast<uint8_t *>(&cfg_b), sizeof(cfg_b));

  // Change the low-pass filter bit accordingly
  cfg_b.lpf = enabled;

  // Write back CGC_REG_B
  result &= _writeReg(REG_CFG_B, reinterpret_cast<uint8_t *>(&cfg_b), sizeof(cfg_b));

  return result;
}

std::optional<math::Vec3> sensors::LIS2MDL::readMagneticField() 
{
  bool result = true;

  // Get raw registers data
  int16_t mag_data[3]; 
  result &= _readReg(REG_OUTX_L, reinterpret_cast<uint8_t *>(mag_data), sizeof(mag_data));

  if (!result) 
    return std::nullopt;

  // Convert to Gauss
  math::Vec3 mag_raw {
    -static_cast<float>(mag_data[0]) * MAG_SENSITIVITY, // X axis is inverted to account for sensor orientation
     static_cast<float>(mag_data[1]) * MAG_SENSITIVITY,
     static_cast<float>(mag_data[2]) * MAG_SENSITIVITY
  };

  // Apply calibration
  return _soft_iron_matrix * (mag_raw - _hard_iron_offset);
}

std::optional<float> sensors::LIS2MDL::readTemperature() 
{
  bool result = true;

  // Get raw temperature data
  int16_t temp_data;
  result &= _readReg(REG_TEMP_OUT_L, reinterpret_cast<uint8_t *>(&temp_data), sizeof(temp_data));

  if (!result) 
    return std::nullopt;

  // Convert to degrees Celsius
  return static_cast<float>(temp_data) * TEMP_SENSITIVITY + TEMP_OFFSET;
}

inline bool sensors::LIS2MDL::_softReset() 
{
  bool result = true; 

  // Read CGC_REG_A
  CfgA cfg_a;
  result &= _readReg(REG_CFG_A, reinterpret_cast<uint8_t *>(&cfg_a), sizeof(cfg_a));

  cfg_a.soft_reset = 1; // Set soft reset bit

  // Write back CGC_REG_A
  result &= _writeReg(REG_CFG_A, reinterpret_cast<uint8_t *>(&cfg_a), sizeof(cfg_a));

  return result;
}

inline bool sensors::LIS2MDL::_reboot() 
{
  bool result = true; 

  // Read CGC_REG_A
  CfgA cfg_a;
  result &= _readReg(REG_CFG_A, reinterpret_cast<uint8_t *>(&cfg_a), sizeof(cfg_a));

  cfg_a.reboot = 1; // Set reboot bit
  
  // Write back CGC_REG_A
  result &= _writeReg(REG_CFG_A, reinterpret_cast<uint8_t *>(&cfg_a), sizeof(cfg_a));

  return result;
}

inline bool sensors::LIS2MDL::_enableTemperatureCompensation() 
{
  bool result = true; 

  // Read CGC_REG_A
  CfgA cfg_a;
  result &= _readReg(REG_CFG_A, reinterpret_cast<uint8_t *>(&cfg_a), sizeof(cfg_a));

  cfg_a.comp_temp_en = 1; // Enable temperature compensation
  
  // Write back CGC_REG_A
  result &= _writeReg(REG_CFG_A, reinterpret_cast<uint8_t *>(&cfg_a), sizeof(cfg_a));

  return result;
}

inline bool sensors::LIS2MDL::_enableBDU() 
{
  bool result = true; 

  // Read CGC_REG_C
  CfgC cfg_c;
  result &= _readReg(REG_CFG_C, reinterpret_cast<uint8_t *>(&cfg_c), sizeof(cfg_c));

  cfg_c.bdu = 1; // Enable Block Data Update

  // Write back CGC_REG_C
  result &= _writeReg(REG_CFG_C, reinterpret_cast<uint8_t *>(&cfg_c), sizeof(cfg_c));

  return result;
}

inline bool sensors::LIS2MDL::_writeReg(uint8_t reg_addr, const uint8_t* data, uint16_t size) 
{
  return i2c::write(_i2c, _address, reg_addr, data, size, _timeout);
}

inline bool sensors::LIS2MDL::_readReg(uint8_t reg_addr, uint8_t* data, uint16_t size) 
{
  return i2c::read(_i2c, _address, reg_addr, data, size, _timeout);
}
