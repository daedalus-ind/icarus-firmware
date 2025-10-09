/**
 ******************************************************************************
 * @file    icm-45686.hxx
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
#ifndef __SENSORS_ICM_45686_HXX__
#define __SENSORS_ICM_45686_HXX__

#include "main.hxx"
#include "spi.hxx"
#include "icm-45686_regs.hxx"
#include "math/vec3.hxx"

namespace sensors {

  class ICM_45686 {
  private: 
    SPI_TypeDef* _spi;
    spi::ChipSelectPin _cs_pin;

    float _accel_scale = 0.0f;
    float _gyro_scale = 0.0f;

    math::Vec3 _accel_bias = math::Vec3::zero();
    math::Vec3 _gyro_bias = math::Vec3::zero();

    inline bool write_reg(uint16_t reg, const uint8_t* data, uint16_t size);
    inline bool read_reg(uint16_t reg, uint8_t* data, uint16_t size);

  public: 
    ICM_45686(SPI_TypeDef* spi, GPIO_TypeDef* cs_port, uint32_t cs_pin)
      : _spi{spi}, 
        _cs_pin{cs_port, cs_pin} {}

    bool init();
    bool soft_reset();

    bool enable_accel(icm::AccelODR odr, icm::AccelFS fs);
    bool enable_gyro(icm::GyroODR odr, icm::GyroFS fs);

    bool calibrate_gyro(size_t samples = 100);

    bool get_accel(math::Vec3& accel);
    bool get_gyro(math::Vec3& gyro);

    bool enable_fifo();
    bool get_fifo_count(uint16_t& count);
    bool get_latest_fifo_data(math::Vec3& accel, math::Vec3& gyro);
  };

}

#endif /* __SENSORS_ICM_45686_HXX__ */
