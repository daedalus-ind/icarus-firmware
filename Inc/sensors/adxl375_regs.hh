/**
 ******************************************************************************
 * @file    adxl375_regs.hh
 * @brief   This file contains the register definitions for the ADXL375
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
#ifndef __ADXL375_REGS_HH__
#define __ADXL375_REGS_HH__

#include "main.hh"

namespace sensors {

  namespace adxl { 

    // Default I2C address for ADXL375
    constexpr uint8_t I2C_ADDRESS = 0x53;

    /*************************************
    * @defgroup Output Data Rates (ODR) *
    *************************************/
    enum class ODR : uint8_t {
      Hz_3200       = 0x0F,
      Hz_1600       = 0x0E,
      Hz_800        = 0x0D,
      Hz_400        = 0x0C,
      Hz_200        = 0x0B,
      Hz_100        = 0x0A,
      Hz_50         = 0x09,
      Hz_25         = 0x08,
      Hz_12_5       = 0x07,
      Hz_6_25       = 0x06,
      Hz_3_125      = 0x05,
      Hz_1_5625     = 0x04,
      Hz_0_78125    = 0x03,
      Hz_0_390625   = 0x02,
      Hz_0_1953125  = 0x01,
      Hz_0_09765625 = 0x00
    };
    
    /*********************************
    * @defgroup Device ID register. *
    *********************************/
    constexpr uint8_t REG_DEVID     = 0x00;
    constexpr uint8_t DEVID_CONTENT = 0xE5;

    /******************************
    * @defgroup Offset Registers *
    ******************************/
    constexpr uint8_t REG_OFSX = 0x1E;
    constexpr uint8_t REG_OFSY = 0x1F; 
    constexpr uint8_t REG_OFSZ = 0x20; 

    // Offset registers sensitivity in g/LSB
    constexpr float OFS_SENSITIVITY = 0.196f; 

    /*****************************
    * @defgroup Shock Registers *
    *****************************/
    constexpr uint8_t REG_SHOCK_THRESHOLD  = 0x1D;
    constexpr uint8_t REG_SHOCK_DUR        = 0x21;
    constexpr uint8_t REG_SHOCK_LATENCY    = 0x22;
    constexpr uint8_t REG_SHOCK_WINDOW     = 0x23;
    constexpr uint8_t REG_SHOCK_AXES       = 0x2A;
    constexpr uint8_t REG_ACT_SHOCK_STATUS = 0x2B;

    /********************************
    * @defgroup Activity Registers *
    ********************************/
    constexpr uint8_t REG_THRESH_ACT   = 0x24;
    constexpr uint8_t REG_THRESH_INACT = 0x25;

    // Activity/inactivity registers sensitivity in g/LSB
    constexpr float ACT_INACT_SENSITIVITY = 0.780f; 

    constexpr uint8_t REG_TIME_INACT    = 0x26;
    constexpr uint8_t REG_ACT_INACT_CTL = 0x27;

    /*******************************
    * @defgroup Control Registers *
    *******************************/
    constexpr uint8_t REG_BW_RATE = 0x2C; 
    struct BWRate {
      ODR rate : 4; 
      uint8_t low_power : 1; 
      uint8_t reserved : 3; // Always 0
    }; 

    constexpr uint8_t REG_POWER_CTL = 0x2D; 
    struct PowerCtl {
      uint8_t weakeup : 2;   
      uint8_t sleep : 1;    
      uint8_t measure : 1;    
      uint8_t autosleep : 1;
      uint8_t link : 1;      
      uint8_t reserved : 2; // Always 0
    };

    constexpr uint8_t REG_DATA_FORMAT = 0x31; 
    // Default value for DATA_FORMAT register
    constexpr uint8_t DATA_FORMAT_RESET_VAL = 0x0B;
    struct DataFormat {
      uint8_t reserved1 : 2;  // Always 0x03
      uint8_t justify : 1;    
      uint8_t reserved2 : 2;  // Always 0x01
      uint8_t int_invert : 1;
      uint8_t spi : 1;        
      uint8_t self_test : 1;
    };

    /*********************************
    * @defgroup Interrupt Registers *
    *********************************/
    constexpr uint8_t REG_INT_ENABLE = 0x2E; 
    constexpr uint8_t REG_INT_MAP    = 0x2F;    
    constexpr uint8_t REG_INT_SOURCE = 0x30; 

    /****************************
    * @defgroup Data Registers *
    ****************************/
    constexpr uint8_t REG_DATAX0 = 0x32;
    constexpr uint8_t REG_DATAX1 = 0x33;
    constexpr uint8_t REG_DATAY0 = 0x34;
    constexpr uint8_t REG_DATAY1 = 0x35;
    constexpr uint8_t REG_DATAZ0 = 0x36;
    constexpr uint8_t REG_DATAZ1 = 0x37;

    // Sensitivity of data registers in g/LSB
    constexpr float DATA_SENSITIVITY = 0.04805f;

    /****************************
    * @defgroup FIFO Registers *
    ****************************/
    constexpr uint8_t REG_FIFO_CTL    = 0x38;    
    constexpr uint8_t REG_FIFO_STATUS = 0x39;

  } 

} 

#endif // __ADXL375_REGS_HH__
