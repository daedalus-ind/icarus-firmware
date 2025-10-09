/**
  ******************************************************************************
  * @file    ecef.hxx
  * @brief   This file provides a class to represent Earth-Centered, 
  *.         Earth-Fixed (ECEF) coordinates
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
#ifndef __GEO_ECEF_HXX__
#define __GEO_ECEF_HXX__

#include <cmath>
#include <numbers>

#include "math/vec3.hxx"
#include "math/angles.hxx"

namespace geo {

  /**
   * @brief Earth-Centered, Earth-Fixed (ECEF) coordinates.
   */
  class ECEF {
  public:
    /**
     * @brief Convert geodetic coordinates (latitude, longitude, altitude) to ECEF.
     * @param lat Latitude in degrees.
     * @param lon Longitude in degrees.
     * @param alt Altitude in meters.
     * @return ECEF coordinates.
     * @note https://en.wikipedia.org/wiki/Geographic_coordinate_conversion#From_geodetic_to_ECEF_coordinates
     */
    static constexpr ECEF from_geodetic(float lat, float lon, float alt) {      
      constexpr float a = 6378137.0f; // Semi-major axis in meters
      constexpr float f = 1.0f / 298.257223563f; // Flattening
      constexpr float b = a * (1.0f - f); // Semi-minor axis in meters
      constexpr float e_sq = f * (2.0f - f); // Square of eccentricity
      
      float lat_rad = math::deg_to_rad(lat);
      float lon_rad = math::deg_to_rad(lon);
      
      float sin_lat = std::sin(lat_rad);
      float cos_lat = std::cos(lat_rad);
      float sin_lon = std::sin(lon_rad);
      float cos_lon = std::cos(lon_rad);
      
      float N = a / std::sqrt(1.0f - e_sq * sin_lat * sin_lat); // Radius of curvature in the prime vertical
      
      float x = (N + alt) * cos_lat * cos_lon;
      float y = (N + alt) * cos_lat * sin_lon;
      float z = ((b * b) / (a * a) * N + alt) * sin_lat;
      
      return ECEF(x, y, z);
    }
    
  public:
    float x;
    float y;
    float z;

    constexpr ECEF() : x{0.0f}, y{0.0f}, z{0.0f} {}
    constexpr ECEF(float x, float y, float z) : x{x}, y{y}, z{z} {}
    constexpr ECEF(math::Vec3 const& v) : x{v.x}, y{v.y}, z{v.z} {}
  };

  constexpr math::Vec3 operator-(const ECEF& a, const ECEF& b) {
    return math::Vec3(a.x - b.x, a.y - b.y, a.z - b.z);
  }
}

#endif /* __GEO_ECEF_HXX__ */
