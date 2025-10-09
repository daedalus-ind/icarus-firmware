/**
 ******************************************************************************
 * @file    gps.hxx
 * @brief   This file contains the declaration of the GPS data structure.
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
#ifndef __UBX_GPS_HXX__
#define __UBX_GPS_HXX__

#include "main.hxx"
#include "utils/volatile.hxx"
#include "parser.hxx"

#include <tuple>

namespace ubx { 

  /**
   * @brief GPS data structure holding the latest NAV-PVT message.
   */
  class GPS {
  public:
    /* Default constructor */
    inline GPS() : _data{0}, _is_new(false) {}

    /**
     * @brief Load NAV-PVT message payload into the GPS object.
     */
    inline void load_nav_pvt(uint8_t const* payload) volatile {
      utils::memcpy_volatile(_data, payload, NAV_PVT_LENGTH);
      _is_new = true;
    }

    /**
     * @brief Check if new data has been loaded since last use.
     */
    inline bool is_data_new() volatile const {
      return _is_new;
    }

    /**
     * @brief Mark the data as used (not new anymore).
     */
    inline void use_data() volatile {
      _is_new = false;
    }

    /**
     * @brief Check if the GPS has a valid fix.
     */
    inline bool has_fix() volatile const {
      return _fixType >= 3 && (_flags & 0x01);
    }

    /**
     * @brief Get number of satellites used in the solution.
     */
    inline uint8_t get_sats() volatile const {
      return _numSV;
    }

    /**
     * @brief Get latitude, longitude, and height in standard units.
     */
    inline std::tuple<float, float, float> get_lla() volatile const {
      return {_lat * 1e-7f, _lon * 1e-7f, _height * 1e-3f};
    }

    /**
     * @brief Get velocity in North, East, Down components in standard units.
     */
    inline std::tuple<float, float, float> get_vnvevd() volatile const {
      return {_velN * 1e-3f, _velE * 1e-3f, _velD * 1e-3f};
    }

  private:
    union {
      struct {
        uint32_t _iTOW;
        uint16_t _year;
        uint8_t _month;
        uint8_t _day;
        uint8_t _hour;
        uint8_t _min;
        uint8_t _sec;
        uint8_t _valid;
        uint32_t _tAcc;
        int32_t _nano;
        uint8_t _fixType;
        uint8_t _flags;
        uint8_t _flags2;
        uint8_t _numSV;
        int32_t _lon;
        int32_t _lat;
        int32_t _height;
        int32_t _hMSL;
        uint32_t _hAcc;
        uint32_t _vAcc;
        int32_t _velN;
        int32_t _velE;
        int32_t _velD;
        int32_t _gSpeed;
        int32_t _heading;
        uint32_t _sAcc;
        uint32_t _headingAcc;
        uint16_t _pDOP;
        uint16_t _flags3;
        uint32_t _reserved0;
        int32_t _headVeh;
        int16_t _magDec;
        uint16_t _magAcc;
      };
      uint8_t _data[NAV_PVT_LENGTH];
    };
    bool _is_new;
  };

  extern volatile GPS gps;

}

#endif /* __UBX_GPS_HXX__ */
