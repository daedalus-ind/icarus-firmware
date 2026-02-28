/**
 ******************************************************************************
 * @file    ecef.hh
 * @brief   This file provides the definition of the ECEF coordinates
 *          structure.
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
#ifndef __ECEF_HH__
#define __ECEF_HH__

#include "math/vec3.hh"
#include "math/angles.hh"

#include "constants.hh"
#include "geodetic.hh"

#include <cmath>

namespace geo {

  /**
   * @brief Represents a point in Earth-Centered, Earth-Fixed (ECEF) coordinates.
   */
  struct ECEF {
    /**
     * @brief Converts geodetic coordinates (latitude, longitude, altitude) to ECEF coordinates.
     * @param geodetic Geodetic coordinates to convert.
     * @return An ECEF struct representing the converted coordinates.
     */
    static constexpr ECEF fromGeodetic(const Geodetic& geodetic)
    {
      float lat_rad = math::degrees_to_radians(geodetic.latitude);
      float lon_rad = math::degrees_to_radians(geodetic.longitude);

      float sin_lat = std::sin(lat_rad);
      float cos_lat = std::cos(lat_rad);
      float sin_lon = std::sin(lon_rad);
      float cos_lon = std::cos(lon_rad);
      
      // Radius of curvature in the prime vertical
      float N = EARTH_A / std::sqrt(1.0f - EARTH_E_SQ * sin_lat * sin_lat);

      float x = (N + geodetic.altitude) * cos_lat * cos_lon;
      float y = (N + geodetic.altitude) * cos_lat * sin_lon;
      float z = ((EARTH_B * EARTH_B) / (EARTH_A * EARTH_A) * N + geodetic.altitude) * sin_lat;

      return ECEF{x, y, z};
    }

    float x, y, z;
  };

}

/**
 * @brief Subtract two ECEF coordinates, resulting in a vector.
 * @param a The first ECEF coordinate.
 * @param b The second ECEF coordinate.
 * @return A Vec3 representing the difference between the two ECEF coordinates.
 */
constexpr math::Vec3 operator-(const geo::ECEF& a, const geo::ECEF& b)
{
  return math::Vec3{a.x - b.x, a.y - b.y, a.z - b.z};
}

#endif // __ECEF_HH__
