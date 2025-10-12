/**
 ******************************************************************************
 * @file    ms5607.hxx
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
#ifndef __SENSORS_MS5607_HXX__
#define __SENSORS_MS5607_HXX__

#include "main.hxx"
#include "time.hxx"
#include "spi.hxx"
#include "ms5607_regs.hxx"

#include <cstdint>

namespace sensors {

  class MS5607 {
  private:
    SPI_TypeDef *_spi;
    spi::ChipSelectPin _cs_pin;

    uint16_t _prom[8];
    int32_t _T_ref;
    int64_t _off_t1;
    int64_t _sens_t1;

    inline bool read_prom();
    inline uint8_t calc_crc4();

    inline bool write_reg(uint8_t reg_addr, const uint8_t* data, uint16_t size);
    inline bool read_reg(uint8_t reg_addr, uint8_t* data, uint16_t size);

  public:
    MS5607(SPI_TypeDef* spi, GPIO_TypeDef* cs_port, uint32_t cs_pin) 
      : _spi{spi}, _cs_pin{cs_port, cs_pin} {}

    bool init();
    bool reset();
    bool start_adc(ms::Sensor_Type sensor_type, ms::OSR osr);
    bool read_adc(uint32_t& measure);
    bool compensate(uint32_t raw_pressure, uint32_t raw_temperature, float& pressure, float& temperature);
  };
  
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
        _state{ms::State::IDLE},
        _pressure_osr{ms::OSR::OSR_256}, 
        _temperature_osr{ms::OSR::OSR_256},
        _raw_pressure{0}, 
        _raw_temperature{0} {}

    bool init(ms::OSR pressure_osr, ms::OSR temperature_osr); 
    bool process(float& pressure);
  };

} 

#endif /* __SENSORS_MS5607_HXX__ */
