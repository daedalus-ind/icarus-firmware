/**
 ******************************************************************************
 * @file    ms5607_regs.hh
 * @brief   This file contains the register definitions for the
 *          MS5607 pressure sensor.
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
#ifndef __MS5607_REGS_HH__
#define __MS5607_REGS_HH__

#include "main.hh"

namespace sensors {

  namespace ms {

    /*********************************************
    * @defgroup Commands for the MS5607 sensor. *
    *********************************************/
    constexpr uint8_t CMD_RESET     = 0x1E;
    constexpr uint8_t CMD_READ_PROM = 0xA0;
    constexpr uint8_t CMD_ADC_CONV  = 0x40;
    constexpr uint8_t CMD_ADC_READ  = 0x00;

    /**********************************************
    * @defgroup Sensor types for ADC conversion. *
    **********************************************/
    enum class SensorType : uint8_t {
      Pressure    = 0x00,
      Temperature = 0x10
    };

    /****************************************************
    * @defgroup Oversampling rates for ADC conversion. *
    ****************************************************/
    enum class OSR : uint8_t {
      OSR_256  = 0x00,
      OSR_512  = 0x02,
      OSR_1024 = 0x04,
      OSR_2048 = 0x06,
      OSR_4096 = 0x08
    };

    /****************************************
    * @defgroup States for the MS5607 FSM. *
    ****************************************/
    enum class State : uint8_t {
      Idle, 
      WaitPressure,
      WaitTemperature
    };
  }

} 

#endif // __MS5607_REGS_HH__
