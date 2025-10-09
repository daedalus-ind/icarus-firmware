/**
 ******************************************************************************
 * @file    icm-45686.cxx
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

#include "sensors/icm-45686.hxx"
#include "spi.hxx"

using namespace sensors::icm;


bool sensors::ICM_45686::init() {
  bool rslt = true;

  /* Enable SPI line, if not already enabled */
  if (!LL_SPI_IsEnabled(_spi))
    LL_SPI_Enable(_spi);

  /* Configure 4 wire SPI interface */
  INTFConfig1Ovrd intf_config1_ovrd = {};
  intf_config1_ovrd.ap_spi_34_mode_ovrd = 1;
  intf_config1_ovrd.ap_spi_34_mode_ovrd_val = SPI34Mode::MODE_4_WIRE;
  rslt &= write_reg(REG_INTF_CONFIG1_OVRD, reinterpret_cast<uint8_t*>(&intf_config1_ovrd), sizeof(intf_config1_ovrd));

  /* Read WHO_AM_I register to verify communication */
  uint8_t who_am_i = 0;
  rslt &= read_reg(REG_WHO_AM_I, &who_am_i, sizeof(who_am_i));

  /* Check WHO_AM_I register content */
  rslt &= (who_am_i == WHO_AM_I_CONTENT);

  /* Perform a soft reset */
  rslt &= soft_reset();

  return rslt;
}

bool sensors::ICM_45686::soft_reset() {
  bool rslt = true; 

  /* Save previous INTF_CONFIG1_OVRD register */
  INTFConfig1Ovrd intf_config1_ovrd;
  rslt &= read_reg(REG_INTF_CONFIG1_OVRD, reinterpret_cast<uint8_t*>(&intf_config1_ovrd), sizeof(intf_config1_ovrd));

  /* Save previous INTF_CONFIG0 register */
  INTFConfig_0 intf_config0;
  rslt &= read_reg(REG_INTF_CONFIG0, reinterpret_cast<uint8_t*>(&intf_config0), sizeof(intf_config0));

  /* Trigger soft reset */
  Misc2 misc2 = {};
  misc2.soft_rst = 1;
  rslt &= write_reg(REG_MISC2, reinterpret_cast<uint8_t*>(&misc2), sizeof(misc2));

  /* Wait for reset to complete */
  LL_mDelay(1);
  
  /* Restore DRIVE_CONFIG0 register */
  rslt &= write_reg(REG_INTF_CONFIG0, reinterpret_cast<uint8_t*>(&intf_config0), sizeof(intf_config0));

  /* Restore INTF_CONFIG1_OVRD register */
  rslt &= write_reg(REG_INTF_CONFIG1_OVRD, reinterpret_cast<uint8_t*>(&intf_config1_ovrd), sizeof(intf_config1_ovrd));

  /* Clear the RESET_DONE interrupt */ 
  INT1Status_0 int1_status0 = {};
  rslt &= read_reg(REG_INT1_STATUS0, reinterpret_cast<uint8_t*>(&int1_status0), sizeof(int1_status0));
  rslt &= (int1_status0.reset_done == 1); 

  return rslt; 
}

bool sensors::ICM_45686::enable_accel(AccelODR odr, AccelFS fs) {
  bool rslt = true;

  /* Set ODR and scale */
  AccelConfig_0 accel_config0 = {};
  accel_config0.odr = odr;
  accel_config0.fs_sel = fs;

  rslt &= write_reg(REG_ACCEL_CONFIG0, reinterpret_cast<uint8_t*>(&accel_config0), sizeof(accel_config0));

  switch (fs) {
  case AccelFS::G_32:
    _accel_scale = 32.0f / 32768.0f;
    break;
  case AccelFS::G_16:
    _accel_scale = 16.0f / 32768.0f;
    break;
  case AccelFS::G_8:
    _accel_scale = 8.0f / 32768.0f;
    break;
  case AccelFS::G_4:
    _accel_scale = 4.0f / 32768.0f;
    break;
  case AccelFS::G_2:
    _accel_scale = 2.0f / 32768.0f;
    break;
  default:
    return false;
  }

  /* Set accelerometer to Low Noise mode */
  PwrMgmt_0 pwr_mgmt_0 = {};
  rslt &= read_reg(REG_PWR_MGMT_0, reinterpret_cast<uint8_t*>(&pwr_mgmt_0), sizeof(pwr_mgmt_0));

  pwr_mgmt_0.accel_mode = AccelMode::LOW_NOISE;

  rslt &= write_reg(REG_PWR_MGMT_0, reinterpret_cast<uint8_t*>(&pwr_mgmt_0), sizeof(pwr_mgmt_0));

  return rslt;
}

bool sensors::ICM_45686::enable_gyro(GyroODR odr, GyroFS fs) {
  bool rslt = true;

  /* Set ODR and scale */
  GyroConfig_0 gyro_config0 = {};
  gyro_config0.odr = odr;
  gyro_config0.fs_sel = fs;

  rslt &= write_reg(REG_GYRO_CONFIG0, reinterpret_cast<uint8_t*>(&gyro_config0), sizeof(gyro_config0));

  switch (fs) {
  case GyroFS::DPS_4000:
    _gyro_scale = 4000.0f / 32768.0f;
    break;
  case GyroFS::DPS_2000:
    _gyro_scale = 2000.0f / 32768.0f;
    break;
  case GyroFS::DPS_1000:
    _gyro_scale = 1000.0f / 32768.0f;
    break;
  case GyroFS::DPS_500:
    _gyro_scale = 500.0f / 32768.0f;
    break;
  case GyroFS::DPS_250:
    _gyro_scale = 250.0f / 32768.0f;
    break;
  case GyroFS::DPS_125:
    _gyro_scale = 125.0f / 32768.0f;
    break;
  case GyroFS::DPS_62_5:
    _gyro_scale = 62.5f / 32768.0f;
    break;
  case GyroFS::DPS_31_25:
    _gyro_scale = 31.25f / 32768.0f;
    break;
  case GyroFS::DPS_15_625:
    _gyro_scale = 15.625f / 32768.0f;
    break;
  default:
    return false;
  }

  /* Set gyroscope to Low Noise mode */
  PwrMgmt_0 pwr_mgmt_0 = {};
  rslt &= read_reg(REG_PWR_MGMT_0, reinterpret_cast<uint8_t*>(&pwr_mgmt_0), sizeof(pwr_mgmt_0));

  pwr_mgmt_0.gyro_mode = GyroMode::LOW_NOISE;

  rslt &= write_reg(REG_PWR_MGMT_0, reinterpret_cast<uint8_t*>(&pwr_mgmt_0), sizeof(pwr_mgmt_0));

  return rslt;
}

bool sensors::ICM_45686::calibrate_gyro(size_t samples) {
  math::Vec3 sum = math::Vec3::zero();

  for (size_t i = 0; i < samples; ++i) {
    math::Vec3 gyro;
    if (!get_gyro(gyro)) {
      return false;
    }
    sum += gyro;
    LL_mDelay(1); // Wait 1 ms between samples
  }

  _gyro_bias = sum / static_cast<float>(samples);

  return true;
}

bool sensors::ICM_45686::get_accel(math::Vec3& accel) {
  bool rslt = true;

  int16_t accel_data[3];
  rslt &= read_reg(REG_ACCEL_DATA_X1, reinterpret_cast<uint8_t*>(accel_data), sizeof(accel_data));

  accel.x = accel_data[0] * _accel_scale - _accel_bias.x;
  accel.y = accel_data[1] * _accel_scale - _accel_bias.y;
  accel.z = accel_data[2] * _accel_scale - _accel_bias.z;

  return rslt;
}

bool sensors::ICM_45686::get_gyro(math::Vec3& gyro) {
  bool rslt = true;

  int16_t gyro_data[3];
  rslt &= read_reg(REG_GYRO_DATA_X1, reinterpret_cast<uint8_t*>(gyro_data), sizeof(gyro_data));

  gyro.x = gyro_data[0] * _gyro_scale - _gyro_bias.x;
  gyro.y = gyro_data[1] * _gyro_scale - _gyro_bias.y;
  gyro.z = gyro_data[2] * _gyro_scale - _gyro_bias.z;

  return rslt;
}

bool sensors::ICM_45686::enable_fifo() {
  bool rslt = true;

  /* Set FIFO depth to 2K bytes */
  FIFOConfig_0 fifo_config0 = {};
  fifo_config0.depth = FIFODepth::DEPTH_2K;
  rslt &= write_reg(REG_FIFO_CONFIG0, reinterpret_cast<uint8_t*>(&fifo_config0), sizeof(fifo_config0));

  /* Enable high resolution FIFO */
  FIFOConfig_3 fifo_config3 = {};
  fifo_config3.hires_en = 1;
  rslt &= write_reg(REG_FIFO_CONFIG3, reinterpret_cast<uint8_t*>(&fifo_config3), sizeof(fifo_config3));

  /* Enable timestamp in FIFO */
  FIFOConfig_4 fifo_config4 = {};
  fifo_config4.fifo_tmst_fsync_en = 1;
  rslt &= write_reg(REG_FIFO_CONFIG4, reinterpret_cast<uint8_t*>(&fifo_config4), sizeof(fifo_config4));

  /* Enable timestamp */
  SMCControl_0 smc_control_0 = {};
  rslt &= read_reg(REG_SMC_CONTROL_0, reinterpret_cast<uint8_t*>(&smc_control_0), sizeof(smc_control_0));
  LL_mDelay(1); // Wait at least 4 us after accessing indirect registers

  smc_control_0.tmst_fsync_en = 1;

  rslt &= write_reg(REG_SMC_CONTROL_0, reinterpret_cast<uint8_t*>(&smc_control_0), sizeof(smc_control_0));

  /* Enable FIFO */
  fifo_config0.mode = FIFOMode::STREAM;
  rslt &= write_reg(REG_FIFO_CONFIG0, reinterpret_cast<uint8_t*>(&fifo_config0), sizeof(fifo_config0));

  fifo_config3.if_en = 1;
  rslt &= write_reg(REG_FIFO_CONFIG3, reinterpret_cast<uint8_t*>(&fifo_config3), sizeof(fifo_config3));

  return rslt;
}

bool sensors::ICM_45686::get_fifo_count(uint16_t& count) {
  bool rslt = true;

  rslt &= read_reg(REG_FIFO_COUNT_0, reinterpret_cast<uint8_t*>(&count), sizeof(count));

  /**
  * @note Errata AN-000364 (2.2)
  * Read FIFO_COUNT value twice, and use the second value.
  */
  rslt &= read_reg(REG_FIFO_COUNT_0, reinterpret_cast<uint8_t*>(&count), sizeof(count));

  return rslt;
}

bool sensors::ICM_45686::get_latest_fifo_data(math::Vec3& accel, math::Vec3& gyro) {
  bool rslt = true;

  /* Get current FIFO count */
  uint16_t fifo_count;
  rslt &= get_fifo_count(fifo_count);

  if (!rslt || fifo_count < 1) return false;

  /* Discard all but the latest FIFO frame */
  _cs_pin.select();
  rslt &= spi::read_dummy(_spi, REG_FIFO_DATA, (fifo_count - 1) * 20, 10);
  _cs_pin.deselect();

  /* Read latest FIFO frame */
  uint8_t fifo_frame[20]; 
  rslt &= read_reg(REG_FIFO_DATA, fifo_frame, sizeof(fifo_frame));

  /* Format FIFO frame data */
  int32_t accel_x = (fifo_frame[2] << 12) + (fifo_frame[1] << 4) + (fifo_frame[17] >> 4);
  int32_t accel_y = (fifo_frame[4] << 12) + (fifo_frame[3] << 4) + (fifo_frame[18] >> 4);
  int32_t accel_z = (fifo_frame[6] << 12) + (fifo_frame[5] << 4) + (fifo_frame[19] >> 4);

  int32_t gyro_x = (fifo_frame[8]  << 12) + (fifo_frame[7]  << 4) + (fifo_frame[17] & 0x0F);
  int32_t gyro_y = (fifo_frame[10] << 12) + (fifo_frame[9]  << 4) + (fifo_frame[18] & 0x0F);
  int32_t gyro_z = (fifo_frame[12] << 12) + (fifo_frame[11] << 4) + (fifo_frame[19] & 0x0F);

  /* Convert accelerometer and gyroscope data to signed 32-bit integers */
  if (accel_x & 0x080000) 
    accel_x |= 0xFFF00000; 

  if (accel_y & 0x080000) 
    accel_y |= 0xFFF00000; 

  if (accel_z & 0x080000) 
    accel_z |= 0xFFF00000; 

  if (gyro_x & 0x080000) 
    gyro_x |= 0xFFF00000; 

  if (gyro_y & 0x080000) 
    gyro_y |= 0xFFF00000; 

  if (gyro_z & 0x080000) 
    gyro_z |= 0xFFF00000; 

  /* Convert to float and compensate */
  constexpr float ACCEL_SCALE = 0.00006103515625f; // 32G / 2^19
  accel.x = static_cast<float>(accel_x) * ACCEL_SCALE - _accel_bias.x;
  accel.y = static_cast<float>(accel_y) * ACCEL_SCALE - _accel_bias.y;
  accel.z = static_cast<float>(accel_z) * ACCEL_SCALE - _accel_bias.z;

  constexpr float GYRO_SCALE = 0.0076293945f; // 4000DPS / 2^19
  gyro.x = static_cast<float>(gyro_x) * GYRO_SCALE - _gyro_bias.x;
  gyro.y = static_cast<float>(gyro_y) * GYRO_SCALE - _gyro_bias.y;
  gyro.z = static_cast<float>(gyro_z) * GYRO_SCALE - _gyro_bias.z;

  return rslt;
}

bool sensors::ICM_45686::write_reg(uint16_t reg, const uint8_t *data, uint16_t size) {
  bool rslt = true; 

  _cs_pin.select();
  rslt &= spi::write(_spi, reg, data, size, 10);
  _cs_pin.deselect();

  return rslt;
}

bool sensors::ICM_45686::read_reg(uint16_t reg, uint8_t *data, uint16_t size) {
  bool rslt = true;

  _cs_pin.select();
  rslt &= spi::read(_spi, reg | 0x80, data, size, 10);
  _cs_pin.deselect();

  return rslt;
}
