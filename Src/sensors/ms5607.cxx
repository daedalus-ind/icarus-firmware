/**
 ******************************************************************************
 * @file    ms5607.cxx
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
#include "sensors/ms5607.hxx"
#include <cmath>

using namespace sensors::ms;

bool sensors::MS5607::init() {
  bool rslt = true;

  /* Enable SPI line, if not already enabled */
  if (!LL_SPI_IsEnabled(_spi))
    LL_SPI_Enable(_spi);

  /* Reset the device */
  rslt &= reset();

  return rslt;
}

bool sensors::MS5607::reset() {
  bool rslt = true;

  _cs_pin.select();
  rslt &= spi::write(_spi, CMD_RESET, nullptr, 0, 10);
  /**
  * @note AN520 
  * Wait 3 ms after sending the reset command.
  */
  time::delay(3);
  _cs_pin.deselect();

  /* Read PROM coefficients */
  rslt &= read_prom();

  /* Check CRC */
  uint8_t crc_calc = calc_crc4();
  uint8_t crc_read = (uint8_t)(_prom[7] & 0x000F);

  rslt &= (crc_calc == crc_read);

  /* Save compensation coefficients */
  _T_ref = _prom[5] << 8;                          // T_ref = C5 * 2^8
  _off_t1 = static_cast<int64_t>(_prom[2]) << 17;  // OFF_T1 = C2 * 2^17
  _sens_t1 = static_cast<int64_t>(_prom[1]) << 16; // SENS_T1 = C1 * 2^16

  return rslt;
}

bool sensors::MS5607::start_adc(Sensor_Type sensor_type, OSR osr) {
  bool rslt = true;
  
  /* Write conversion command */
  uint8_t cmd = CMD_ADC_CONV + static_cast<uint8_t>(sensor_type) + static_cast<uint8_t>(osr);
  rslt &= write_reg(cmd, nullptr, 0);

  return rslt;
}

inline bool sensors::MS5607::read_prom() {
  bool rslt = true;

  for (uint8_t i = 0; i < 8; i++) {
    /* Read each coefficient data */
    uint8_t coeff_data[2];
    rslt &= read_reg(CMD_READ_PROM + (i * 2), coeff_data, sizeof(coeff_data));

    /* Save the 16 bit coefficient */
    _prom[i] = static_cast<uint16_t>((coeff_data[0] << 8) + coeff_data[1]);
  }

  return rslt;
}

bool sensors::MS5607::read_adc(uint32_t& measure) {
  bool rslt = true;

  /* Read conversion result */
  uint8_t meas_bytes[3];
  rslt &= read_reg(CMD_ADC_READ, meas_bytes, 3);

  /* Convert result to 24 bit */
  measure = static_cast<uint32_t>((meas_bytes[0] << 16) + (meas_bytes[1] << 8) + meas_bytes[2]);

  /* If the result is zero, then the conversion did not finish */
  rslt &= (measure != 0);

  return rslt;
}

bool sensors::MS5607::compensate(uint32_t raw_pressure, uint32_t raw_temperature, float& pressure, float& temperature) {
  /**
   * @note First order compensation (based on datasheet).
   */
  int32_t dT = raw_temperature - _T_ref;                                  // dT = D2 - T_ref
  int64_t OFF = _off_t1 + ((static_cast<int64_t>(_prom[4]) * dT) >> 6);   // OFF = C2 * 2^17 + (C4 * dT) / 64
  int64_t SENS = _sens_t1 + ((static_cast<int64_t>(_prom[3]) * dT) >> 7); // SENS = C1 * 2^16 + (C3 * dT) / 128

  temperature = (2000 + ((static_cast<uint64_t>(_prom[6]) * dT) >> 23)) * 0.01f; // T = 2000 + (C6 * dT) / 2^23
  pressure = ((((raw_pressure * SENS) >> 21) - OFF) >> 15) * 0.01f;              // P = ((D1 * SENS) / 2^21 - OFF) / 2^15

  return true; // Cannot fail
}

inline uint8_t sensors::MS5607::calc_crc4() {
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

bool sensors::MS5607::write_reg(uint8_t reg_addr, const uint8_t* data, int16_t size) {
  bool rslt = true;

  _cs_pin.select();
  rslt &= spi::write(_spi, reg_addr, data, size, 10);
  _cs_pin.deselect();

  return rslt;
}

bool sensors::MS5607::read_reg(uint8_t reg_addr, uint8_t* data, int16_t size) {
  bool rslt = true;

  _cs_pin.select();
  rslt &= spi::read(_spi, reg_addr, data, size, 10);
  _cs_pin.deselect();

  return rslt;
}

bool sensors::MS5607_FSM::init(OSR pressure_osr, OSR temperature_osr) {
  bool rslt = true;

  rslt &= _sensor.init();
  _state = State::IDLE;

  _pressure_osr = pressure_osr;
  _temperature_osr = temperature_osr;

  return rslt;
}

#define OSR_TO_INDEX(osr) (static_cast<uint8_t>(osr) >> 1)

bool sensors::MS5607_FSM::process(float& pressure) {
  constexpr uint32_t wait_times[5] = {1, 2, 3, 5, 10}; // in ms 

  bool rslt = true;

  switch (_state) {
    case State::IDLE: 
    case State::DONE:
      /* Start pressure conversion */
      rslt &= _sensor.start_adc(Sensor_Type::PRESSURE, _pressure_osr);
      _timer.start(wait_times[OSR_TO_INDEX(_pressure_osr)]);
      _state = State::WAIT_PRESSURE;
      break;

    case State::WAIT_PRESSURE:
      /* Wait for conversion to complete */
      if (_timer.is_expired()) {
        /* Read pressure ADC value */
        rslt &= _sensor.read_adc(_raw_pressure);
        /* Start temperature conversion */
        rslt &= _sensor.start_adc(Sensor_Type::TEMPERATURE, _temperature_osr);
        _timer.start(wait_times[OSR_TO_INDEX(_temperature_osr)]);
        _state = State::WAIT_TEMPERATURE;
      }
      break;

    case State::WAIT_TEMPERATURE:
      /* Wait for conversion to complete */
      if (_timer.is_expired()) {
        /* Read temperature ADC value */
        rslt &= _sensor.read_adc(_raw_temperature);
        /* Compensate and get final values */
        float temperature;
        rslt &= _sensor.compensate(_raw_pressure, _raw_temperature, pressure, temperature);
        _state = State::DONE;
      }
      break;

    default:
      _state = State::IDLE;
      break;
  }

  /* Return true only if the process is done */
  return rslt && (_state == State::DONE);
}
