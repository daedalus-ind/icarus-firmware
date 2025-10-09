/**
 ******************************************************************************
 * @file    lis2mdl_regs.hxx
 * @brief   This file contains the register definitions for the
 *          LIS2MDL magnetometer.
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
#ifndef __SENSORS_LIS2MDL_REGS_HXX__
#define __SENSORS_LIS2MDL_REGS_HXX__

#include "main.hxx"

namespace sensors {

  namespace lismdl {

    /*********************************
    * @defgroup Hard-iron registers *
    *********************************/
    constexpr uint8_t REG_OFFSET_X_L = 0x45;
    constexpr uint8_t REG_OFFSET_X_H = 0x46;
    constexpr uint8_t REG_OFFSET_Y_L = 0x47;
    constexpr uint8_t REG_OFFSET_Y_H = 0x48;
    constexpr uint8_t REG_OFFSET_Z_L = 0x49;
    constexpr uint8_t REG_OFFSET_Z_H = 0x4A;

    /********************************
    * @defgroup Device ID register *
    ********************************/
    constexpr uint8_t REG_WHO_AM_I     = 0x4F;
    constexpr uint8_t WHO_AM_I_CONTENT = 0x40;

    /*************************************
    * @defgroup Configuration registers *
    *************************************/
    constexpr uint8_t REG_CFG_A = 0x60;

    /*******************************************
    * @defgroup Output data rate (ODR) values *
    *******************************************/
    enum class ODR : uint8_t {
      HZ_10  = 0x00,
      HZ_20  = 0x01,
      HZ_50  = 0x02,
      HZ_100 = 0x03
    };

    /********************************
    * @defgroup Modes of operation *
    ********************************/
    enum class Mode : uint8_t {
      CONTINUOUS = 0x00,
      SINGLE     = 0x01, 
      IDLE       = 0x03 
    };

    struct Cfg_A {
      Mode mode : 2;
      ODR odr : 2;
      uint8_t low_power : 1;
      uint8_t soft_reset : 1;
      uint8_t reboot : 1;
      uint8_t comp_temp_en : 1;
    };

    constexpr uint8_t REG_CFG_B = 0x61;
    struct Cfg_B {
      bool lpf : 1;
      uint8_t off_canc : 1;
      uint8_t set_freq : 1;
      uint8_t int_on_dataoff : 1;
      uint8_t off_canc_one_shot : 1;
      uint8_t reserved : 3; // Always 0
    };

    constexpr uint8_t REG_CFG_C = 0x62;
    struct Cfg_C {
      uint8_t drdy_on_pin : 1;
      uint8_t self_test : 1;
      uint8_t spi_4wire : 1;
      uint8_t ble : 1;
      uint8_t bdu : 1;
      uint8_t i2c_disable : 1;
      uint8_t int_on_pin : 1;
      uint8_t reserved : 1; // Always 0
    };

    /*********************************
    * @defgroup Interrupt registers *
    *********************************/
    constexpr uint8_t REG_INT_CRTL = 0x63;
    struct Int_Crtl {
      uint8_t ien : 1;
      uint8_t iel : 1;
      uint8_t iea : 1;
      uint8_t reserved : 2; // Always 0
      uint8_t zien : 1;
      uint8_t yien : 1;
      uint8_t xien : 1;
    };

    /*****************************
    * @defgroup Status register *
    *****************************/
    constexpr uint8_t REG_STATUS = 0x67;
    struct Status {
      uint8_t xda : 1;
      uint8_t yda : 1;
      uint8_t zda : 1;
      uint8_t zyxda : 1;
      uint8_t xor_ : 1;
      uint8_t yor : 1;
      uint8_t zor : 1;
      uint8_t zyxor : 1;
    };

    /******************************
    * @defgroup Output registers *
    ******************************/
    constexpr uint8_t REG_OUTX_L = 0x68;
    constexpr uint8_t REG_OUTX_H = 0x69;
    constexpr uint8_t REG_OUTY_L = 0x6A;
    constexpr uint8_t REG_OUTY_H = 0x6B;
    constexpr uint8_t REG_OUTZ_L = 0x6C;
    constexpr uint8_t REG_OUTZ_H = 0x6D;

    /* Sensitivity of magnetometer output registers in Gauss/LSB */
    constexpr float MAG_SENSITIVITY = 0.15f;

    /******************************************
    * @defgroup Temperature sensor registers *
    ******************************************/
    constexpr uint8_t REG_TEMP_OUT_L = 0x6E;
    constexpr uint8_t REG_TEMP_OUT_H = 0x6F;

    /* Sensitivity of temperature output registers in degrees Celsius/LSB */
    constexpr float TEMP_SENSITIVITY = 0.125f;

  } 

}

#endif /* __SENSORS_LIS2MDL_REGS_HXX__ */
