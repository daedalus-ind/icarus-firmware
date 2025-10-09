/**
 ******************************************************************************
 * @file    lis2mdl.cxx
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

#include "sensors/lis2mdl.hxx"
#include "i2c.hxx"

using namespace sensors::lismdl;

bool sensors::LIS2MDL::init() {
  bool rslt = true;

  /* Enable I2C line, if not already enabled */
  if (!LL_I2C_IsEnabled(m_i2c))
    LL_I2C_Enable(m_i2c);

  /* Check device ID */
  uint8_t who_am_i = 0;
  rslt &= read_reg(REG_WHO_AM_I, &who_am_i, sizeof(who_am_i));
  rslt &= (who_am_i == WHO_AM_I_CONTENT);

  /* Reset the device */
  rslt &= reset();

  return rslt;
}

bool sensors::LIS2MDL::reset() {
  bool rslt = true;

  /* Perform a soft reset */
  rslt &= soft_reset();

  /**
  * @note AN5069
  * The LIS2MDL requires a delay of 5us after the soft reset before
  * rebooting the device.
  */
  LL_mDelay(1);

  /* Perform reboot */
  rslt &= reboot();

  /**
  * @note AN5069
  * The LIS2MDL requires a delay of 20ms after the reboot.
  */
  LL_mDelay(20);

  /* Enable temperature compensation */
  rslt &= enable_temperature_compensation();

  /* Enable Block Data Update (BDU) */
  rslt &= enable_bdu();

  return rslt;
}

bool sensors::LIS2MDL::set_mode(Mode mode) {
  bool rslt = true; 

  /* Read CGC_REG_A */
  Cfg_A cfg_a;
  rslt &= read_reg(REG_CFG_A, reinterpret_cast<uint8_t*>(&cfg_a), sizeof(cfg_a));

  cfg_a.mode = mode; // Set mode
  
  /* Write back CGC_REG_A */
  rslt &= write_reg(REG_CFG_A, reinterpret_cast<uint8_t*>(&cfg_a), sizeof(cfg_a));

  return rslt;
}

bool sensors::LIS2MDL::set_odr(ODR odr) {
  bool rslt = true; 

  /* Read CGC_REG_A */
  Cfg_A cfg_a;
  rslt &= read_reg(REG_CFG_A, reinterpret_cast<uint8_t*>(&cfg_a), sizeof(cfg_a));

  cfg_a.odr = odr; // Set ODR
  
  /* Write back CGC_REG_A */
  rslt &= write_reg(REG_CFG_A, reinterpret_cast<uint8_t*>(&cfg_a), sizeof(cfg_a));

  return rslt;
}

void sensors::LIS2MDL::set_calibration_matrix(math::Mat3x3 const& soft_iron_matrix) {
  m_soft_iron_matrix = soft_iron_matrix;
}

void sensors::LIS2MDL::set_calibration_offset(math::Vec3 const& hard_iron_offset) {
  m_hard_iron_offset = hard_iron_offset;
}

bool sensors::LIS2MDL::set_lpf(bool enabled) {
  bool rslt = true; 

  /* Read CGC_REG_B */
  Cfg_B cfg_b;
  rslt &= read_reg(REG_CFG_B, reinterpret_cast<uint8_t*>(&cfg_b), sizeof(cfg_b));

  /* Change the low-pass filter bit accordingly */
  cfg_b.lpf = enabled;
  
  /* Write back CGC_REG_B */
  rslt &= write_reg(REG_CFG_B, reinterpret_cast<uint8_t*>(&cfg_b), sizeof(cfg_b));

  return rslt;
}

bool sensors::LIS2MDL::get_mag(math::Vec3& mag) {
  bool rslt = true;

  /* Get raw registers data */
  int16_t mag_data[3]; 
  rslt &= read_reg(REG_OUTX_L, reinterpret_cast<uint8_t*>(mag_data), sizeof(mag_data));

  /* Convert to Gauss */
  math::Vec3 mag_raw;
  mag_raw.x = -static_cast<float>(mag_data[0]) * MAG_SENSITIVITY; // X axis is inverted
  mag_raw.y =  static_cast<float>(mag_data[1]) * MAG_SENSITIVITY;
  mag_raw.z =  static_cast<float>(mag_data[2]) * MAG_SENSITIVITY;

  /* Apply calibration */
  mag = m_soft_iron_matrix * (mag_raw - m_hard_iron_offset);

  return rslt;
}

bool sensors::LIS2MDL::get_temp(float& temperature) {
  bool rslt = true;

  /* Get raw temperature data */
  int16_t temp_data;
  rslt &= read_reg(REG_TEMP_OUT_L, reinterpret_cast<uint8_t*>(&temp_data), sizeof(temp_data));

  /* Convert to degrees Celsius */
  temperature = static_cast<float>(temp_data) * TEMP_SENSITIVITY;

  return rslt;
}

inline bool sensors::LIS2MDL::soft_reset() {
  bool rslt = true; 

  /* Read CGC_REG_A */
  Cfg_A cfg_a;
  rslt &= read_reg(REG_CFG_A, reinterpret_cast<uint8_t*>(&cfg_a), sizeof(cfg_a));

  cfg_a.soft_reset = 1; // Set soft reset bit
  
  /* Write back CGC_REG_A */
  rslt &= write_reg(REG_CFG_A, reinterpret_cast<uint8_t*>(&cfg_a), sizeof(cfg_a));

  return rslt;
}

inline bool sensors::LIS2MDL::reboot() {
  bool rslt = true; 

  /* Read CGC_REG_A */
  Cfg_A cfg_a;
  rslt &= read_reg(REG_CFG_A, reinterpret_cast<uint8_t*>(&cfg_a), sizeof(cfg_a));

  cfg_a.reboot = 1; // Set reboot bit
  
  /* Write back CGC_REG_A */
  rslt &= write_reg(REG_CFG_A, reinterpret_cast<uint8_t*>(&cfg_a), sizeof(cfg_a));

  return rslt;
}

inline bool sensors::LIS2MDL::enable_temperature_compensation() {
  bool rslt = true; 

  /* Read CGC_REG_A */
  Cfg_A cfg_a;
  rslt &= read_reg(REG_CFG_A, reinterpret_cast<uint8_t*>(&cfg_a), sizeof(cfg_a));

  cfg_a.comp_temp_en = 1; // Enable temperature compensation
  
  /* Write back CGC_REG_A */
  rslt &= write_reg(REG_CFG_A, reinterpret_cast<uint8_t*>(&cfg_a), sizeof(cfg_a));

  return rslt;
}

inline bool sensors::LIS2MDL::enable_bdu() {
  bool rslt = true; 

  /* Read CGC_REG_C */
  Cfg_C cfg_c;
  rslt &= read_reg(REG_CFG_C, reinterpret_cast<uint8_t*>(&cfg_c), sizeof(cfg_c));

  cfg_c.bdu = 1; // Enable Block Data Update
  
  /* Write back CGC_REG_C */
  rslt &= write_reg(REG_CFG_C, reinterpret_cast<uint8_t*>(&cfg_c), sizeof(cfg_c));

  return rslt;
}

inline bool sensors::LIS2MDL::write_reg(uint8_t reg_addr, const uint8_t* data, uint16_t size) {
  return i2c::write(m_i2c, m_address, reg_addr, data, size, 2);
}

inline bool sensors::LIS2MDL::read_reg(uint8_t reg_addr, uint8_t* data, uint16_t size) {
  return i2c::read(m_i2c, m_address, reg_addr, data, size, 2);
}
