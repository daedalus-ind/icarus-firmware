/**
 ******************************************************************************
 * @file    gps.hh
 * @brief   This file contains the definition of the UBX GPS data structure.
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
#ifndef __UBLOX_GPS_HH__
#define __UBLOX_GPS_HH__

#include "main.hh"
#include "usart.hh"
#include "geo/wmm.hh"
#include "geo/geodetic.hh"
#include "utils/volatile.h"

#include <cstddef>
#include <tuple>

namespace ubx {

  /**
   * @brief First sync character of UBX protocol, corresponds to 'µ' (mu) in ISO 8859-1.
   */
  constexpr uint8_t SYNC_CHAR_1 = 0xB5;

  /**
   * @brief Second sync character of UBX protocol, corresponds to 'b' in ASCII.
   */
  constexpr uint8_t SYNC_CHAR_2 = 0x62;

  /**
   * @brief UBX NAV message class.
   */
  constexpr uint8_t NAV_CLASS = 0x01;

  /**
   * @brief UBX NAV-PVT message ID.
   */
  constexpr uint8_t NAV_PVT_ID = 0x07;
  
  /**
   * @brief UBX NAV-PVT message class.
   */
  constexpr size_t NAV_PVT_LENGTH = 92;

  /**
   * @brief Sends a UBX command to the GPS module.
   * @param data Pointer to the command data buffer.
   * @param length Length of the command data buffer.
   * @return `true` if the command was sent successfully, `false` otherwise.
   */
  inline bool sendCommand(const uint8_t* data, uint16_t length)
  {
    return serial::write(GPS_UART, data, length, 10);
  }

  /**
   * @brief Represents the UBX GPS data structure.
   */
  class GPS {
  public: 
    /**
     * @brief Constructor for the GPS data structure.
     */
    inline GPS() : _data{}, _new{false} {}

    /**
     * @brief Loads GPS data from a raw byte buffer.
     * @param buffer Pointer to the buffer containing raw GPS data.
     */
    inline void loadFromBuffer(const uint8_t* buffer) volatile
    {
      memcpy_volatile(_data, buffer, NAV_PVT_LENGTH);
      _new = true;
    }

    /**
     * @brief Returns whether the GPS data is new since last checked.
     * @return `true` if the data is new, `false` otherwise.
     */
    inline bool isDataNew() const volatile
    {
      return _new;
    }

    /**
     * @brief Clears the new data flag.
     */
    inline void clearNewFlag() volatile
    {
      _new = false;
    }
  
    /**
     * @brief Returns whether a valid GPS fix is available.
     * @return `true` if a valid fix is available, `false` otherwise.
     */
    inline bool hasFix() const volatile
    {
      return fixType >= 3 && (flags & 0x01);
    }

    /**
     * @brief Returns the GPS fix type.
     * @return Fix type (0 = No fix, 1 = Dead reckoning only, 2 = 2D fix, 3 = 3D fix, 4 = GNSS + dead reckoning, 5 = Time only fix).
     */
    inline uint8_t getFixType() const volatile
    {
      return fixType;
    }

    /**
     * @brief Returns the current date from the GPS data.
     * @return A Date struct containing year, month, and day.
     */
    inline geo::Date getDate() const volatile 
    {
      return geo::Date{year, month, day};
    }

    /**
     * @brief Returns the number of satellites used in the fix.
     * @return Number of satellites used.
     */
    inline int getSatelliteCount() const volatile
    {
      return numSV;
    }

    /**
     * @brief Returns the current geodetic position (latitude, longitude, altitude).
     * @return A Geodetic struct containing latitude (degrees), longitude (degrees), and altitude (meters).
     */
    inline geo::Geodetic getPosition() const volatile
    {
      return geo::Geodetic{lat * 1e-7f, lon * 1e-7f, height * 1e-3f};
    }

    /**
     * @brief Returns the ENU velocity components.
     * @return A tuple containing East, North, and Up velocities (meters per second).
     */
    inline std::tuple<float, float, float> getENUVelocity() const volatile
    {
      return std::make_tuple(velE * 1e-3f, velN * 1e-3f, -velD * 1e-3f);
    }

  private:
    union {
      struct {
        uint32_t iTOW;
        uint16_t year;
        uint8_t month;
        uint8_t day;
        uint8_t hour;
        uint8_t min;
        uint8_t sec;
        uint8_t valid;
        uint32_t tAcc;
        int32_t nano;
        uint8_t fixType;
        uint8_t flags;
        uint8_t flags2;
        uint8_t numSV;
        int32_t lon;
        int32_t lat;
        int32_t height;
        int32_t hMSL;
        uint32_t hAcc;
        uint32_t vAcc;
        int32_t velN;
        int32_t velE;
        int32_t velD;
        int32_t gSpeed;
        int32_t heading;
        uint32_t sAcc;
        uint32_t headingAcc;
        uint16_t pDOP;
        uint16_t reserved1;
        uint32_t reserved2;
        int32_t headVeh;
        int16_t magDec;
        uint16_t magAcc;
      };
      uint8_t _data[NAV_PVT_LENGTH];
    };
    bool _new;
  };

  /**
   * @brief The GPS data instance.
   */
  extern volatile GPS gps;

}

#endif // __UBLOX_GPS_HH__
