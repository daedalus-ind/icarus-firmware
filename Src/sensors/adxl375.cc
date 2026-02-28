/**
 ******************************************************************************
 * @file    adxl375.cc
 * @brief   This file contains the implementation of the ADXL375
 *          3-axis digital accelerometer driver.
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
#include "sensors/adxl375.hh"
#include "i2c.hh"

using namespace sensors::adxl;

bool sensors::ADXL375::init() 
{
  bool result = true; 

  // Enable I2C line, if not already enabled
  if (!(_i2c->CR1 & I2C_CR1_PE))
    _i2c->CR1 |= I2C_CR1_PE;

  // Check device ID
  uint8_t dev_id;
  result &= _readReg(REG_DEVID, &dev_id, sizeof(dev_id));
  result &= (dev_id == adxl::DEVID_CONTENT);

  // Errata corrige: write right DATA_FORMAT reset value
  result &= _writeReg(REG_DATA_FORMAT, &DATA_FORMAT_RESET_VAL, sizeof(DATA_FORMAT_RESET_VAL));

  // Disable interrupts
  uint8_t int_enable = 0x00;
  result &= _writeReg(REG_INT_ENABLE, &int_enable, sizeof(int_enable));

  return result;
}

bool sensors::ADXL375::setStandbyMode(bool enable) 
{
  bool result = true; 

  // Read current power control register
  PowerCtl power_ctl;
  result &= _readReg(REG_POWER_CTL, reinterpret_cast<uint8_t *>(&power_ctl), sizeof(power_ctl));

  // Change the measure bit accordingly
  power_ctl.measure = enable ? 0 : 1; 

  // Write back the updated power control register
  result &= _writeReg(REG_POWER_CTL, reinterpret_cast<uint8_t *>(&power_ctl), sizeof(power_ctl));

  return result;
}

bool sensors::ADXL375::setODR(ODR odr) 
{
  bool result = true; 

  // Read current BW_RATE register
  BWRate bw_rate;
  result &= _readReg(REG_BW_RATE, reinterpret_cast<uint8_t *>(&bw_rate), sizeof(bw_rate));

  // Change the rate bits accordingly
  bw_rate.rate = odr; 

  // Write back the updated BW_RATE register
  result &= _writeReg(REG_BW_RATE, reinterpret_cast<uint8_t *>(&bw_rate), sizeof(bw_rate));

  return result;
}

std::optional<math::Vec3> sensors::ADXL375::getAcceleration() 
{
  bool result = true; 

  // Read raw data from data registers
  int16_t raw_data[3];
  result &= _readReg(REG_DATAX0, reinterpret_cast<uint8_t*>(raw_data), sizeof(raw_data));

  if (!result) 
    return std::nullopt;

  math::Vec3 accel;
  // Convert raw data to g's
  accel.x = static_cast<float>(raw_data[0]) * DATA_SENSITIVITY;
  accel.y = static_cast<float>(raw_data[1]) * DATA_SENSITIVITY;
  accel.z = static_cast<float>(raw_data[2]) * DATA_SENSITIVITY;
  return accel;
}

inline bool sensors::ADXL375::_writeReg(uint8_t reg_addr, const uint8_t* data, uint16_t size)
{
  return i2c::write(_i2c, _address, reg_addr, data, size, 2);
}

inline bool sensors::ADXL375::_readReg(uint8_t reg_addr, uint8_t* data, uint16_t size)
{
  return i2c::read(_i2c, _address, reg_addr, data, size, 2);
}
