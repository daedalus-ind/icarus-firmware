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
#include "sensors/adxl375.hxx"
#include "i2c.hxx"

using namespace sensors::adxl;

bool sensors::ADXL375::init() {
  bool rslt = true; 

  /* Enable I2C line, if not already enabled */
  if (!LL_I2C_IsEnabled(m_i2c))
    LL_I2C_Enable(m_i2c);

  /* Check device ID */
  uint8_t dev_id;
  rslt &= read_reg(REG_DEVID, &dev_id, sizeof(dev_id));
  rslt &= (dev_id == adxl::DEVID_CONTENT);

  /* Errata corrige: write right DATA_FORMAT reset value */
  rslt &= write_reg(REG_DATA_FORMAT, &DATA_FORMAT_RESET_VAL, sizeof(DATA_FORMAT_RESET_VAL));

  /* Disable interrupts */
  uint8_t int_enable = 0x00;
  rslt &= write_reg(REG_INT_ENABLE, &int_enable, sizeof(int_enable));

  return rslt;
}

bool sensors::ADXL375::set_standby_mode(bool enable) {
  bool rslt = true; 

  /* Read current power control register */
  Power_Ctl power_ctl;
  rslt &= read_reg(REG_POWER_CTL, reinterpret_cast<uint8_t*>(&power_ctl), sizeof(power_ctl));

  /* Change the measure bit accordingly */
  power_ctl.measure = enable ? 0 : 1; 

  /* Write back the updated power control register */
  rslt &= write_reg(REG_POWER_CTL, reinterpret_cast<uint8_t*>(&power_ctl), sizeof(power_ctl));

  return rslt;
}

bool sensors::ADXL375::set_odr(ODR odr) {
  bool rslt = true; 

  /* Read current BW_RATE register */
  BW_Rate bw_rate;
  rslt &= read_reg(REG_BW_RATE, reinterpret_cast<uint8_t*>(&bw_rate), sizeof(bw_rate));

  /* Change the rate bits accordingly */
  bw_rate.rate = odr; 

  /* Write back the updated BW_RATE register */
  rslt &= write_reg(REG_BW_RATE, reinterpret_cast<uint8_t*>(&bw_rate), sizeof(bw_rate));

  return rslt;
}

bool sensors::ADXL375::get_accel(math::Vec3& accel) {
  bool rslt = true; 

  /* Read raw data from data registers */
  int16_t raw_data[3];
  rslt &= read_reg(REG_DATAX0, reinterpret_cast<uint8_t*>(raw_data), sizeof(raw_data));

  /* Convert raw data to g */
  accel.x = static_cast<float>(raw_data[0]) * DATA_SENSITIVITY;
  accel.y = static_cast<float>(raw_data[1]) * DATA_SENSITIVITY;
  accel.z = static_cast<float>(raw_data[2]) * DATA_SENSITIVITY;

  return rslt;
}

inline bool sensors::ADXL375::write_reg(uint8_t reg_addr, const uint8_t* data, uint16_t size) {
  return i2c::write(m_i2c, m_address, reg_addr, data, size, 2);
}

inline bool sensors::ADXL375::read_reg(uint8_t reg_addr, uint8_t* data, uint16_t size) {
  return i2c::read(m_i2c, m_address, reg_addr, data, size, 2);
}
