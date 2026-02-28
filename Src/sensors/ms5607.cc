/**
 ******************************************************************************
 * @file    ms5607.cc
 * @brief   This file contains the driver for the MS5607 pressure sensor.
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
#include "sensors/ms5607.hh"
#include <cmath>

using namespace sensors::ms;

bool sensors::MS5607::init() 
{
  bool result = true;

  // Enable SPI line, if not already enabled
  if (!(_spi->CR1 & SPI_CR1_SPE))
    _spi->CR1 |= SPI_CR1_SPE;

  // Reset the device
  result &= reset();

  return result;
}

bool sensors::MS5607::reset() 
{
  bool result = true;

  _cs_pin.select();
  result &= spi::write(_spi, CMD_RESET, nullptr, 0, 10);
  /**
   * @note AN520 
   * Wait 3 ms after sending the reset command.
   */
  time::delay(3);
  _cs_pin.deselect();

  // Read PROM coefficients
  result &= _readPROM();

  // Check CRC
  uint8_t crc_calc = _calCRC4();
  uint8_t crc_read = static_cast<uint8_t>(_prom[7] & 0x000F);

  result &= (crc_calc == crc_read);

  // Save compensation coefficients
  _T_ref = _prom[5] << 8;                          // T_ref = C5 * 2^8
  _off_t1 = static_cast<int64_t>(_prom[2]) << 17;  // OFF_T1 = C2 * 2^17
  _sens_t1 = static_cast<int64_t>(_prom[1]) << 16; // SENS_T1 = C1 * 2^16

  return result;
}

bool sensors::MS5607::startADC(SensorType sensor_type, OSR osr) 
{
  bool result = true;
  
  // Write conversion command
  uint8_t cmd = CMD_ADC_CONV + static_cast<uint8_t>(sensor_type) + static_cast<uint8_t>(osr);
  result &= _writeReg(cmd, nullptr, 0);

  return result;
}

inline bool sensors::MS5607::_readPROM() 
{
  bool result = true;

  for (uint8_t i = 0; i < 8; i++) {
    // Read each coefficient data
    uint8_t coeff_data[2];
    result &= _readReg(CMD_READ_PROM + (i * 2), coeff_data, sizeof(coeff_data));

    // Save the 16 bit coefficient
    _prom[i] = static_cast<uint16_t>((coeff_data[0] << 8) + coeff_data[1]);
  }

  return result;
}

std::optional<uint32_t> sensors::MS5607::readADC() 
{
  bool result = true;

  // Read conversion result
  uint8_t meas_bytes[3];
  result &= _readReg(CMD_ADC_READ, meas_bytes, sizeof(meas_bytes));

  if (!result)
    return std::nullopt;

  // Convert result to 24 bit
  uint32_t measure = static_cast<uint32_t>((meas_bytes[0] << 16) + (meas_bytes[1] << 8) + meas_bytes[2]);

  // If the result is zero, then the conversion did not finish
  if (measure == 0)
    return std::nullopt;

  return measure;
}

std::pair<float, float> sensors::MS5607::compensate(uint32_t raw_pressure, uint32_t raw_temperature) 
{
  /**
   * @note First order compensation (based on datasheet).
   */
  int32_t dT = raw_temperature - _T_ref;                                  // dT = D2 - T_ref
  int64_t OFF = _off_t1 + ((static_cast<int64_t>(_prom[4]) * dT) >> 6);   // OFF = C2 * 2^17 + (C4 * dT) / 64
  int64_t SENS = _sens_t1 + ((static_cast<int64_t>(_prom[3]) * dT) >> 7); // SENS = C1 * 2^16 + (C3 * dT) / 128

  float temperature = (2000 + ((static_cast<uint64_t>(_prom[6]) * dT) >> 23)) * 0.01f; // T = 2000 + (C6 * dT) / 2^23
  float pressure = ((((raw_pressure * SENS) >> 21) - OFF) >> 15) * 0.01f;              // P = ((D1 * SENS) / 2^21 - OFF) / 2^15

  return std::make_pair(pressure, temperature);
}

inline uint8_t sensors::MS5607::_calCRC4() 
{
  /**
   * @note AN520 
   * @see https://www.amsys.de/downloads/notes/MS5XXX-C-code-example-for-MS56xx-MS57xx-MS58xx-AMSYS-an520e.pdf
   */
  uint16_t crc_rem = 0x0000;    // CRC reminder
  uint16_t crc_read = _prom[7]; // Original value of the CRC (read in the PROM)

  _prom[7] &= 0xFF00; // Replace the CRC bits with 0

  for (uint8_t i = 0; i < 16; i++) {
    if (i % 2 == 1) // LSB
      crc_rem ^= static_cast<uint16_t>((_prom[i >> 1]) & 0x00FF);
    else // MSB
      crc_rem ^= static_cast<uint16_t>(_prom[i >> 1] >> 8);

    for (uint8_t j = 8; j > 0; j--) {
      if (crc_rem & 0x8000)
        crc_rem = (crc_rem << 1) ^ 0x3000;
      else
        crc_rem = (crc_rem << 1);
    }
  }
  crc_rem = (crc_rem >> 12) & 0x000F;

  _prom[7] = crc_read;

  return static_cast<uint8_t>(crc_rem);
}

bool sensors::MS5607::_writeReg(uint8_t reg_addr, const uint8_t* data, uint16_t size) 
{
  bool result = true;

  _cs_pin.select();
  result &= spi::write(_spi, reg_addr, data, size, 10);
  _cs_pin.deselect();

  return result;
}

bool sensors::MS5607::_readReg(uint8_t reg_addr, uint8_t* data, uint16_t size) 
{
  bool result = true;

  _cs_pin.select();
  result &= spi::read(_spi, reg_addr, data, size, 10);
  _cs_pin.deselect();

  return result;
}

bool sensors::MS5607_FSM::init(OSR pressure_osr, OSR temperature_osr) 
{
  bool result = true;

  result &= _sensor.init();
  _state = State::Idle;

  _pressure_osr = pressure_osr;
  _temperature_osr = temperature_osr;

  return result;
}

#define OSR_TO_INDEX(osr) (static_cast<uint8_t>(osr) >> 1)

static constexpr uint8_t OSR_to_index(sensors::ms::OSR osr) 
{
  return static_cast<uint8_t>(osr) >> 1;
}

std::optional<std::pair<float, float>> sensors::MS5607_FSM::process()
{
  // Wait times in ms for different oversampling rates
  constexpr static uint32_t wait_times[5] = {1, 2, 3, 5, 10};

  bool result = true;
  switch (_state) {
  case State::Idle:
    // Start pressure conversion
    result &= _sensor.startADC(SensorType::Pressure, _pressure_osr);
    if (result) {
      _timer.start(wait_times[OSR_to_index(_pressure_osr)]);
      _state = State::WaitPressure;
    }
    break;
  
  case State::WaitPressure:
    // Wait for pressure conversion to complete
    if (_timer.isExpired()) {
      auto pressure_opt = _sensor.readADC();
      if (!pressure_opt.has_value()) {
        _state = State::Idle;
      } else {
        _raw_pressure = pressure_opt.value();
        // Start temperature conversion
        result &= _sensor.startADC(SensorType::Temperature, _temperature_osr);
        if (result) {
          _timer.start(wait_times[OSR_to_index(_temperature_osr)]);
          _state = State::WaitTemperature;
        } else {
          _state = State::Idle;
        }
      }
    }
    break;

  case State::WaitTemperature:
    // Wait for temperature conversion to complete
    if (_timer.isExpired()) {
      _state = State::Idle;
      auto temperature_opt = _sensor.readADC();
      // Compensate and get final values
      if (temperature_opt.has_value()) {
        _raw_temperature = temperature_opt.value();
        auto compensated = _sensor.compensate(_raw_pressure, _raw_temperature);
        return compensated;
      }
    }
    break;

  default:
    _state = State::Idle;
    break;
  }

  return std::nullopt;
}
