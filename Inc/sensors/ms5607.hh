/**
 ******************************************************************************
 * @file    ms5607.hh
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
#ifndef __MS5607_HH__
#define __MS5607_HH__

#include "main.hh"
#include "time.hh"
#include "spi.hh"
#include "ms5607_regs.hh"

#include <optional>
#include <utility>

namespace sensors {

  /**
   * @brief Class to interface with the MS5607 pressure sensor via SPI.
   */
  class MS5607 {
  public:
    MS5607(SPI_TypeDef* spi, GPIO_TypeDef* cs_port, uint32_t cs_pin) 
      : _spi{spi}, _cs_pin{cs_port, cs_pin} {}

    /**
     * @brief Initializes the MS5607 sensor.
     * @return `true` if initialization was successful, `false` otherwise. 
     */
    bool init();

    /**
     * @brief Resets the MS5607 sensor to default settings.
     * @return `true` if reset was successful, `false` otherwise.
     */
    bool reset();

    /**
     * @brief Starts an ADC conversion for the specified sensor type and oversampling rate.
     * @param sensor_type The type of sensor to convert (Pressure or Temperature).
     * @param osr The oversampling rate for the conversion.
     * @return `true` if the conversion was started successfully, `false` otherwise.
     */
    bool startADC(ms::SensorType sensor_type, ms::OSR osr);

    /**
     * @brief Reads the ADC conversion result.
     * @return An optional containing the raw ADC value if the read was successful, or `std::nullopt` otherwise.
     */
    std::optional<uint32_t> readADC();

    /**
     * @brief Compensates the raw pressure and temperature readings.
     * @param raw_pressure The raw pressure reading from the ADC.
     * @param raw_temperature The raw temperature reading from the ADC.
     * @return A pair containing the compensated pressure (in Pa) and temperature (in °C).
     */
    std::pair<float, float> compensate(uint32_t raw_pressure, uint32_t raw_temperature);

  private:
    SPI_TypeDef *_spi;
    spi::ChipSelectPin _cs_pin;

    uint16_t _prom[8];
    int32_t _T_ref;
    int64_t _off_t1;
    int64_t _sens_t1;

    inline bool _readPROM();
    inline uint8_t _calCRC4();

    inline bool _writeReg(uint8_t reg_addr, const uint8_t* data, uint16_t size);
    inline bool _readReg(uint8_t reg_addr, uint8_t* data, uint16_t size);
  };
  
  /**
   * @brief Finite State Machine (FSM) for managing MS5607 sensor readings.
   */
  class MS5607_FSM {
  private: 
    MS5607 _sensor; 
    ms::State _state;

    ms::OSR _pressure_osr;
    ms::OSR _temperature_osr;

    time::Timer _timer;

    uint32_t _raw_pressure;
    uint32_t _raw_temperature;

  public: 
    MS5607_FSM(SPI_TypeDef *spi, GPIO_TypeDef *cs_port, uint32_t cs_pin) 
      : _sensor{spi, cs_port, cs_pin}, 
        _state{ms::State::Idle},
        _pressure_osr{ms::OSR::OSR_256}, 
        _temperature_osr{ms::OSR::OSR_256},
        _raw_pressure{0}, 
        _raw_temperature{0} {}

    /**
     * @brief Initializes the FSM with the specified oversampling rates.
     * @param pressure_osr The oversampling rate for pressure readings.
     * @param temperature_osr The oversampling rate for temperature readings.
     * @return `true` if initialization was successful, `false` otherwise.
     */
    bool init(ms::OSR pressure_osr, ms::OSR temperature_osr); 

    /**
     * @brief Processes the FSM to perform sensor readings.
     * @return An optional pair containing the compensated pressure (in Pa) and temperature (in °C) 
     * if the reading is complete, or `std::nullopt` otherwise.
     */
    std::optional<std::pair<float, float>> process();
  };

} 

#endif /* __SENSORS_MS5607_HH__ */
