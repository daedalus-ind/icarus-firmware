/**
 ******************************************************************************
 * @file    icm-45686.cc
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
#include "sensors/icm-45686.hh"
#include "time.hh"
#include "spi.hh"

using namespace sensors::icm;

bool sensors::ICM_45686::init() 
{
  bool result = true;

  // Enable SPI line, if not already enabled
  if (!(_spi->CR1 & SPI_CR1_SPE))
    _spi->CR1 |= SPI_CR1_SPE;
 
  // Configure 4 wire SPI interface
  INTFConfig1Ovrd intf_config1_ovrd{};
  intf_config1_ovrd.ap_spi_34_mode_ovrd = 1;
  intf_config1_ovrd.ap_spi_34_mode_ovrd_val = SPI34Mode::Mode4Wire;
  result &= _writeReg(REG_INTF_CONFIG1_OVRD, reinterpret_cast<uint8_t *>(&intf_config1_ovrd), sizeof(intf_config1_ovrd));

  // Read WHO_AM_I register to verify communication
  uint8_t who_am_i = 0;
  result &= _readReg(REG_WHO_AM_I, &who_am_i, sizeof(who_am_i));

  // Check WHO_AM_I register content
  result &= (who_am_i == WHO_AM_I_CONTENT);

  // Perform a soft reset
  result &= softReset();

  return result;
}

bool sensors::ICM_45686::softReset() 
{
  bool result = true; 

  // Save previous INTF_CONFIG1_OVRD register
  INTFConfig1Ovrd intf_config1_ovrd;
  result &= _readReg(REG_INTF_CONFIG1_OVRD, reinterpret_cast<uint8_t *>(&intf_config1_ovrd), sizeof(intf_config1_ovrd));

  // Save previous INTF_CONFIG0 register
  INTFConfig0 intf_config0;
  result &= _readReg(REG_INTF_CONFIG0, reinterpret_cast<uint8_t *>(&intf_config0), sizeof(intf_config0));

  // Trigger soft reset
  Misc2 misc2{};
  misc2.soft_rst = 1;
  result &= _writeReg(REG_MISC2, reinterpret_cast<uint8_t *>(&misc2), sizeof(misc2));

  // Wait for reset to complete
  time::delay(1);
  
  // Restore DRIVE_CONFIG0 register
  result &= _writeReg(REG_INTF_CONFIG0, reinterpret_cast<uint8_t *>(&intf_config0), sizeof(intf_config0));

  // Restore INTF_CONFIG1_OVRD register
  result &= _writeReg(REG_INTF_CONFIG1_OVRD, reinterpret_cast<uint8_t *>(&intf_config1_ovrd), sizeof(intf_config1_ovrd));

  // Clear the RESET_DONE interrupt
  INT1Status0 int1_status0{};
  result &= _readReg(REG_INT1_STATUS0, reinterpret_cast<uint8_t *>(&int1_status0), sizeof(int1_status0));
  result &= (int1_status0.reset_done == 1); 

  return result; 
}

bool sensors::ICM_45686::enableAccelerometer(ODR odr, AccelFS fs) 
{
  bool result = true;

  // Set ODR and scale
  AccelConfig0 accel_config0{};
  accel_config0.odr = odr;
  accel_config0.fs_sel = fs;

  result &= _writeReg(REG_ACCEL_CONFIG0, reinterpret_cast<uint8_t *>(&accel_config0), sizeof(accel_config0));

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

  // Set accelerometer to Low Noise mode
  PwrMgmt0 pwr_mgmt0{};
  result &= _readReg(REG_PWR_MGMT_0, reinterpret_cast<uint8_t *>(&pwr_mgmt0), sizeof(pwr_mgmt0));

  pwr_mgmt0.accel_mode = AccelMode::LowNoise;

  result &= _writeReg(REG_PWR_MGMT_0, reinterpret_cast<uint8_t *>(&pwr_mgmt0), sizeof(pwr_mgmt0));

  return result;
}

bool sensors::ICM_45686::enableGyroscope(ODR odr, GyroFS fs) 
{
  bool result = true;

  // Set ODR and scale
  GyroConfig0 gyro_config0{};
  gyro_config0.odr = odr;
  gyro_config0.fs_sel = fs;

  result &= _writeReg(REG_GYRO_CONFIG0, reinterpret_cast<uint8_t *>(&gyro_config0), sizeof(gyro_config0));

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

  // Set gyroscope to Low Noise mode
  PwrMgmt0 pwr_mgmt0{};
  result &= _readReg(REG_PWR_MGMT_0, reinterpret_cast<uint8_t *>(&pwr_mgmt0), sizeof(pwr_mgmt0));

  pwr_mgmt0.gyro_mode = GyroMode::LowNoise;

  result &= _writeReg(REG_PWR_MGMT_0, reinterpret_cast<uint8_t *>(&pwr_mgmt0), sizeof(pwr_mgmt0));

  return result;
}

bool sensors::ICM_45686::calibrateGyroscope(size_t samples) 
{
  math::Vec3 sum = math::Vec3::zero();

  for (size_t i = 0; i < samples; ++i) {
    auto gyro = readGyroscope();
    if (!gyro.has_value())
      return false;
    sum = sum + gyro.value();
    // Wait 1 ms between samples
    time::delay(1);
  }

  _gyro_bias = sum / static_cast<float>(samples);

  return true;
}

std::optional<math::Vec3> sensors::ICM_45686::readAcceleration() 
{
  bool result = true;

  int16_t accel_data[3];
  result &= _readReg(REG_ACCEL_DATA_X1, reinterpret_cast<uint8_t *>(accel_data), sizeof(accel_data));

  if (!result)
    return std::nullopt;

  return math::Vec3 {
  accel_data[0] * _accel_scale - _accel_bias.x,
  accel_data[1] * _accel_scale - _accel_bias.y,
  accel_data[2] * _accel_scale - _accel_bias.z
  };
}

std::optional<math::Vec3> sensors::ICM_45686::readGyroscope() 
{
  bool result = true;

  int16_t gyro_data[3];
  result &= _readReg(REG_GYRO_DATA_X1, reinterpret_cast<uint8_t *>(gyro_data), sizeof(gyro_data));

  if (!result)
    return std::nullopt;

  return math::Vec3 {
  gyro_data[0] * _gyro_scale - _gyro_bias.x,
  gyro_data[1] * _gyro_scale - _gyro_bias.y,
  gyro_data[2] * _gyro_scale - _gyro_bias.z
  };
}

bool sensors::ICM_45686::enableFIFO() 
{
  bool result = true;

  // Set FIFO depth to 2K bytes
  FIFOConfig0 fifo_config0{};
  fifo_config0.depth = FIFODepth::Depth2K;
  result &= _writeReg(REG_FIFO_CONFIG0, reinterpret_cast<uint8_t *>(&fifo_config0), sizeof(fifo_config0));

  // Enable high resolution FIFO
  FIFOConfig3 fifo_config3{};
  fifo_config3.hires_en = 1;
  result &= _writeReg(REG_FIFO_CONFIG3, reinterpret_cast<uint8_t *>(&fifo_config3), sizeof(fifo_config3));

  // Enable timestamp in FIFO
  FIFOConfig4 fifo_config4{};
  fifo_config4.fifo_tmst_fsync_en = 1;
  result &= _writeReg(REG_FIFO_CONFIG4, reinterpret_cast<uint8_t *>(&fifo_config4), sizeof(fifo_config4));

  // Enable timestamp
  SMCControl0 smc_control0{};
  result &= _readReg(REG_SMC_CONTROL_0, reinterpret_cast<uint8_t *>(&smc_control0), sizeof(smc_control0));
  time::delay(1); // Wait at least 4 us after accessing indirect registers

  smc_control0.tmst_fsync_en = 1;

  result &= _writeReg(REG_SMC_CONTROL_0, reinterpret_cast<uint8_t *>(&smc_control0), sizeof(smc_control0));

  // Enable FIFO
  fifo_config0.mode = FIFOMode::Stream;
  result &= _writeReg(REG_FIFO_CONFIG0, reinterpret_cast<uint8_t *>(&fifo_config0), sizeof(fifo_config0));

  fifo_config3.if_en = 1;
  result &= _writeReg(REG_FIFO_CONFIG3, reinterpret_cast<uint8_t *>(&fifo_config3), sizeof(fifo_config3));

  return result;
}

std::optional<uint16_t> sensors::ICM_45686::readFIFOCount() 
{
  bool result = true;

  uint16_t count = 0;
  result &= _readReg(REG_FIFO_COUNT_0, reinterpret_cast<uint8_t *>(&count), sizeof(count));

  /**
   * @note Errata AN-000364 (2.2)
   * Read FIFO_COUNT value twice, and use the second value.
   */
  result &= _readReg(REG_FIFO_COUNT_0, reinterpret_cast<uint8_t *>(&count), sizeof(count));

  if (!result)
    return std::nullopt;

  return count;
}

std::optional<std::pair<math::Vec3, math::Vec3>> sensors::ICM_45686::readLatestFIFOData() 
{
  bool result = true;

  // Get current FIFO count
  auto fifo_count_opt = readFIFOCount();

  if (!fifo_count_opt.has_value() || fifo_count_opt.value() < 1)
    return std::nullopt;

  // Discard all but the latest FIFO frame
  _cs_pin.select();
  result &= spi::read_dummy(_spi, REG_FIFO_DATA, (fifo_count_opt.value() - 1) * 20, 10);
  _cs_pin.deselect();

  // Read latest FIFO frame
  uint8_t fifo_frame[20]; 
  result &= _readReg(REG_FIFO_DATA, fifo_frame, sizeof(fifo_frame));

  if (!result)
    return std::nullopt;

  // Format FIFO frame data
  int32_t accel_x = (fifo_frame[2] << 12) + (fifo_frame[1] << 4) + (fifo_frame[17] >> 4);
  int32_t accel_y = (fifo_frame[4] << 12) + (fifo_frame[3] << 4) + (fifo_frame[18] >> 4);
  int32_t accel_z = (fifo_frame[6] << 12) + (fifo_frame[5] << 4) + (fifo_frame[19] >> 4);

  int32_t gyro_x = (fifo_frame[8]  << 12) + (fifo_frame[7]  << 4) + (fifo_frame[17] & 0x0F);
  int32_t gyro_y = (fifo_frame[10] << 12) + (fifo_frame[9]  << 4) + (fifo_frame[18] & 0x0F);
  int32_t gyro_z = (fifo_frame[12] << 12) + (fifo_frame[11] << 4) + (fifo_frame[19] & 0x0F);

  // Convert accelerometer and gyroscope data to signed 32-bit integers
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

  // Convert to float and compensate for bias
  constexpr float ACCEL_SCALE = 0.00006103515625f; // 32G / 2^19
  math::Vec3 accel{
  static_cast<float>(accel_x) * ACCEL_SCALE - _accel_bias.x,
  static_cast<float>(accel_y) * ACCEL_SCALE - _accel_bias.y,
  static_cast<float>(accel_z) * ACCEL_SCALE - _accel_bias.z
  };

  constexpr float GYRO_SCALE = 0.0076293945f; // 4000DPS / 2^19
  math::Vec3 gyro{
    static_cast<float>(gyro_x) * GYRO_SCALE - _gyro_bias.x,
    static_cast<float>(gyro_y) * GYRO_SCALE - _gyro_bias.y,
    static_cast<float>(gyro_z) * GYRO_SCALE - _gyro_bias.z
  };

  return {{accel, gyro}};
}

bool sensors::ICM_45686::_writeReg(uint16_t reg, const uint8_t *data, uint16_t size) 
{
  bool result = true; 

  _cs_pin.select();
  result &= spi::write(_spi, reg, data, size, _timeout);
  _cs_pin.deselect();

  return result;
}

bool sensors::ICM_45686::_readReg(uint16_t reg, uint8_t *data, uint16_t size) 
{
  bool rslt = true;

  _cs_pin.select();
  rslt &= spi::read(_spi, reg | 0x80, data, size, _timeout);
  _cs_pin.deselect();

  return rslt;
}
