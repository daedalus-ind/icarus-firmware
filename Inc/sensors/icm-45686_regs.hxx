/**
 ******************************************************************************
 * @file    icm-45686_regs.hxx
 * @brief   This file contains the register definitions for the
 *          ICM-45686 IMU.
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
#ifndef __SENSORS_ICM_45686_REGS_HXX__
#define __SENSORS_ICM_45686_REGS_HXX__

#include "main.hxx"

namespace sensors {

  namespace icm {

    /********************************
    * @defgroup Accelerometer data *
    ********************************/
    constexpr uint8_t REG_ACCEL_DATA_X1 = 0x00; // MSB
    constexpr uint8_t REG_ACCEL_DATA_X0 = 0x01; // LSB
    constexpr uint8_t REG_ACCEL_DATA_Y1 = 0x02;
    constexpr uint8_t REG_ACCEL_DATA_Y0 = 0x03;
    constexpr uint8_t REG_ACCEL_DATA_Z1 = 0x04;
    constexpr uint8_t REG_ACCEL_DATA_Z0 = 0x05;

    /**********************************
    * @defgroup Accelerometer config *
    **********************************/
    constexpr uint8_t REG_ACCEL_CONFIG0 = 0x1B;
    /* Accelerometer ODR selection */
    enum class AccelODR : uint8_t {
      HZ_6400    = 0x03,
      HZ_3200    = 0x04,
      HZ_1600    = 0x05,
      HZ_800     = 0x06,
      HZ_400     = 0x07,
      HZ_200     = 0x08,
      HZ_100     = 0x09,
      HZ_50      = 0x0A,
      HZ_25      = 0x0B,
      HZ_12_5    = 0x0C,
      HZ_6_25    = 0x0D,
      HZ_3_125   = 0x0E,
      HZ_1_5625  = 0x0F
    };
    /* Accelerometer full scale selection */
    enum class AccelFS : uint8_t {
      G_32   = 0x00,
      G_16   = 0x01,
      G_8    = 0x02,
      G_4    = 0x03,
      G_2    = 0x04
    };
    struct AccelConfig_0 {
      AccelODR odr    : 4; 
      AccelFS fs_sel  : 3; 
      uint8_t reserved : 1;
    };

    /****************************
    * @defgroup Gyroscope data *
    ****************************/
    constexpr uint8_t REG_GYRO_DATA_X1  = 0x06; // MSB
    constexpr uint8_t REG_GYRO_DATA_X0  = 0x07; // LSB
    constexpr uint8_t REG_GYRO_DATA_Y1  = 0x08;
    constexpr uint8_t REG_GYRO_DATA_Y0  = 0x09;
    constexpr uint8_t REG_GYRO_DATA_Z1  = 0x0A;
    constexpr uint8_t REG_GYRO_DATA_Z0  = 0x0B;

    /******************************
    * @defgroup Gyroscope config *
    ******************************/
    constexpr uint8_t REG_GYRO_CONFIG0  = 0x1C;
    /* Gyroscope ODR selection */
    enum class GyroODR : uint8_t {
      HZ_6400    = 0x03,
      HZ_3200    = 0x04,
      HZ_1600    = 0x05,
      HZ_800     = 0x06,
      HZ_400     = 0x07,
      HZ_200     = 0x08,
      HZ_100     = 0x09,
      HZ_50      = 0x0A,
      HZ_25      = 0x0B,
      HZ_12_5    = 0x0C,
      HZ_6_25    = 0x0D,
      HZ_3_125   = 0x0E,
      HZ_1_5625  = 0x0F
    };
    /* Gyroscope full scale selection */
    enum class GyroFS : uint8_t {
      DPS_4000   = 0x00,
      DPS_2000   = 0x01,
      DPS_1000   = 0x02,
      DPS_500    = 0x03,
      DPS_250    = 0x04,
      DPS_125    = 0x05,
      DPS_62_5   = 0x06,
      DPS_31_25  = 0x07,
      DPS_15_625 = 0x08
    };
    struct GyroConfig_0 {
      GyroODR odr   : 4;
      GyroFS fs_sel : 4;
    };

    /******************************
    * @defgroup Temperature data *
    ******************************/
    constexpr uint8_t REG_TEMP_DATA1  = 0x0C;
    constexpr uint8_t REG_TEMP_DATA0  = 0x0D;

    /******************************
    * @defgroup Power management *
    ******************************/
    constexpr uint8_t REG_PWR_MGMT_0      = 0x10;
    /* Accelerometer modes */
    enum class AccelMode : uint8_t {
      OFF       = 0x00,
      LOW_POWER = 0x02,
      LOW_NOISE = 0x03
    };
    /* Gyroscope modes */
    enum class GyroMode : uint8_t {
      OFF       = 0x00,
      STANDBY   = 0x01,
      LOW_POWER = 0x02,
      LOW_NOISE = 0x03
    };
    struct PwrMgmt_0 {
      AccelMode accel_mode : 2; 
      GyroMode gyro_mode   : 2;
      uint8_t reserved : 4;
    };

    /***********************
    * @defgroup FIFO data *
    ***********************/
    constexpr uint8_t REG_FIFO_COUNT_0  = 0x12; // MSB
    constexpr uint8_t REG_FIFO_COUNT_1  = 0x13; // LSB
    constexpr uint8_t REG_FIFO_DATA     = 0x14;

    /*************************
    * @defgroup FIFO config *
    *************************/
    constexpr uint8_t REG_FIFO_CONFIG0        = 0x1D;
    /* FIFO depth */
    enum class FIFODepth : uint8_t {
      DEPTH_2K = 0x07,
      DEPTH_8K = 0x1F
    };
    /* FIFO operation modes */
    enum class FIFOMode : uint8_t {
      BYPASS        = 0x00,
      STREAM        = 0x01,
      STOP_ON_FULL  = 0x02,
    };
    struct FIFOConfig_0 {
      FIFODepth depth : 6;
      FIFOMode mode   : 2;
    };

    constexpr uint8_t REG_FIFO_CONFIG1_0  = 0x1E;
    constexpr uint8_t REG_FIFO_CONFIG1_1  = 0x1F;

    constexpr uint8_t REG_FIFO_CONFIG2      = 0x20;
    constexpr uint8_t MASK_FIFO_FLUSH       = 0x80;
    constexpr uint8_t MASK_FIFO_WR_WM_GT_TH = 0x04;

    constexpr uint8_t REG_FIFO_CONFIG3 = 0x21;
    struct FIFOConfig_3 {
      uint8_t if_en    : 1;
      uint8_t accel_en : 1;
      uint8_t gyro_en  : 1;
      uint8_t hires_en : 1;
      uint8_t es0_en   : 1;
      uint8_t es1_en   : 1;
      uint8_t reserved : 2;
    };

    constexpr uint8_t REG_FIFO_CONFIG4 = 0x22;
    struct FIFOConfig_4 {
      uint8_t es0_6b_9b             : 1;
      uint8_t fifo_tmst_fsync_en    : 1;
      uint8_t fifo_comp_en          : 1;
      uint8_t fifo_comp_nc_flow_cfg : 3;
      uint8_t reserved              : 2;
    };

    constexpr uint8_t REG_ODR_DECIMATE_CONFIG = 0x28;
    enum class FIFOODRDecimation : uint8_t {
      DEC_1     = 0x00,
      DEC_2     = 0x01,
      DEC_4     = 0x02,
      DEC_8     = 0x03,
      DEC_16    = 0x04,
      DEC_32    = 0x05,
      DEC_64    = 0x06,
      DEC_128   = 0x07,
      DEC_256   = 0x08,
      DEC_512   = 0x09,
      DEC_1024  = 0x0A,
      DEC_2048  = 0x0B,
      DEC_4096  = 0x0C
    };
    struct ODRDecimateConfig {
      FIFOODRDecimation accel_fifo_odr_dec : 4; 
      FIFOODRDecimation gyro_fifo_odr_dec  : 4;
    };

    /************************
    * @defgroup Interrupts *
    ************************/
    constexpr uint8_t ICM_45686_REG_INT1_CONFIG0 = 0x16;
    struct INT1Config_0 {
      uint8_t fifo_full_en    : 1;
      uint8_t fifo_ths_en     : 1;
      uint8_t drdy_en         : 1;
      uint8_t aux1_drdy_en    : 1;
      uint8_t ap_fsync_en     : 1;
      uint8_t ap_agc_rdy_en   : 1;
      uint8_t aux1_agc_rdy_en : 1;
      uint8_t reset_done_en   : 1;
    };

    constexpr uint8_t REG_INT1_CONFIG1 = 0x17;
    struct INT1Config_1 {
      uint8_t pll_rdy_en          : 1;
      uint8_t wom_x_en            : 1;
      uint8_t wom_y_en            : 1;
      uint8_t wom_z_en            : 1;
      uint8_t i3c_protocol_err_en : 1;
      uint8_t i2cm_done_en        : 1;
      uint8_t apex_event_en       : 1;
      uint8_t reserved            : 1;
    };

    constexpr uint8_t REG_INT1_CONFIG2 = 0x18;
    struct INT1Config_2 {
      uint8_t int1_polarity : 1;
      uint8_t int1_mode     : 1;
      uint8_t int1_drive    : 1;
      uint8_t reserved      : 5;
    };

    constexpr uint8_t REG_INT1_STATUS0 = 0x19;
    struct INT1Status_0 {
      uint8_t fifo_full    : 1;
      uint8_t fifo_ths     : 1;
      uint8_t drdy         : 1;
      uint8_t aux1_drdy    : 1;
      uint8_t ap_fsync     : 1;
      uint8_t ap_agc_rdy   : 1;
      uint8_t aux1_agc_rdy : 1;
      uint8_t reset_done   : 1;
    };

    constexpr uint8_t REG_INT1_STATUS1 = 0x1A;
    struct INT1Status_1 {
      uint8_t pll_rdy          : 1;
      uint8_t wom_x            : 1;
      uint8_t wom_y            : 1;
      uint8_t wom_z            : 1;
      uint8_t i3c_protocol_err : 1;
      uint8_t i2cm_done        : 1;
      uint8_t apex_event       : 1;
      uint8_t reserved         : 1;
    };

    /***********************
    * @defgroup Interface *
    ***********************/
    constexpr uint8_t REG_INTF_CONFIG0 = 0x2C;
    struct INTFConfig_0 {
      uint8_t ap_spi_mode            : 1;
      uint8_t ap_spi_34_mode         : 1;
      uint8_t reserved1              : 3;
      uint8_t virtual_access_aux1_en : 1; 
      uint8_t reserved2              : 2;
    };

    constexpr uint8_t REG_INTF_CONFIG1_OVRD = 0x2D;
    enum class SPIMode : uint8_t {
      MODE_03 = 0x00, 
      MODE_12 = 0x01  
    };
    enum class SPI34Mode : uint8_t {
      MODE_3_WIRE = 0x00,
      MODE_4_WIRE = 0x01
    };
    struct INTFConfig1Ovrd {
      SPIMode ap_spi_mode_ovrd_val      : 1;
      uint8_t ap_spi_mode_ovrd           : 1;
      SPI34Mode ap_spi_34_mode_ovrd_val : 1;
      uint8_t ap_spi_34_mode_ovrd        : 1;
      uint8_t reserved : 4;
    };

    /***************************
    * @defgroup Miscellaneous *
    ***************************/
    constexpr uint8_t REG_WHO_AM_I      = 0x72;
    constexpr uint8_t WHO_AM_I_CONTENT  = 0xE9;

    constexpr uint8_t REG_MISC2 = 0x7F;
    struct Misc2 {
      uint8_t ireg_done : 1;
      uint8_t soft_rst  : 1;
      uint8_t reserved  : 6;
    };

    constexpr uint16_t REG_SMC_CONTROL_0 = 0xA258;
    struct SMCControl_0 {
      uint8_t tmst_fsync_en : 1;
      uint8_t tmst_force_aux_fine_ex : 1;
      uint8_t temp_dis      : 1;
      uint8_t accel_lp_clk_sel : 1;
      uint8_t reserved      : 4;
    };

  } 

}

#endif /* __SENSORS_ICM_45686_REGS_HXX__ */
