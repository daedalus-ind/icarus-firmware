/**
 ******************************************************************************
 * @file    wmm.hh
 * @brief   This file provides the definition of the World Magnetic Model 
 *          (WMM) class to compute the Earth's magnetic field vector.
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
#ifndef __WMM_HH__
#define __WMM_HH__

#include "geo/constants.hh"
#include "main.hh"
#include "math/vec3.hh"
#include "geodetic.hh"

#include <cstddef>
#include <cstdint>
#include <tuple>

namespace geo {

  /**
   * @brief Structure to represent a date.
   */
  struct Date {
    uint16_t year;
    uint8_t month;
    uint8_t day;

    /**
    * @brief Convert the date to a decimal year representation.
    * @return The decimal year as a float.
    */
    constexpr float toDecimalYear() const 
    {
      float days_in_year = year % 4 == 0 ? 366.0f : 365.0f;
      return year + (month - 1) / 12.0f + (day - 1) / days_in_year;
    }
  };

  /**
   * @brief Class to compute the Earth's magnetic field vector using the World Magnetic Model (WMM).
   */
  class WMM {
  public:
    /**
    * @brief Maximum degree of the spherical harmonic model.
    */
    static constexpr size_t MAX_DEGREE = 12;

    /**
    * @brief Earth's equatorial radius in kilometers.
    */
    static constexpr float A = geo::EARTH_A * 0.001f;
    /**
    * @brief Earth's polar radius in kilometers.
    */
    static constexpr float B = geo::EARTH_B * 0.001f;
    /**
    * @brief Earth's mean radius in kilometers.
    */
    static constexpr float RE = geo::EARTH_MEAN_RADIUS * 0.001f;

    /**
    * @brief Epoch year of the WMM model.
    */
    static constexpr float EPOCH = 2025.0f;

    WMM() = delete;

    /**
     * @brief Initialize WMM coefficients.
     */
    static void initCoefficients();

    /**
     * @brief Calculate the magnetic field vector at a given location and date.
     * @param position Geodetic position (latitude, longitude, altitude in meters).
     * @param date Date for which to calculate the magnetic field.
     * @return The magnetic field vector at the given location and date in ENU coordinates (uT).
     */
    static math::Vec3 computeMagneticField(const Geodetic& position, const Date& date);

  private:
    static constexpr size_t SIZE = MAX_DEGREE + 1;
    static constexpr size_t COEFF_COUNT = (SIZE * (SIZE + 1)) / 2 - 1;

    static CCMRAM float c[SIZE][SIZE];
    static CCMRAM float cd[SIZE][SIZE];

    static CCMRAM float snorm[SIZE * SIZE];
    static CCMRAM float k[SIZE][SIZE];
    static CCMRAM float fn[SIZE];
    static CCMRAM float fm[SIZE];

    struct Coefficient {
      float g;
      float h;
      float dg;
      float dh;
    };
    static Coefficient coeffs[COEFF_COUNT];
  };

}

#endif // __WMM_HH__
