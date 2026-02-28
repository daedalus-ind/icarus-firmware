/**
 ******************************************************************************
 * @file    enu.hh
 * @brief   This file provides the function to convert ECEF coordinates to 
 *          ENU coordinates.
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
#ifndef __ENU_HH__
#define __ENU_HH__

#include "math/vec3.hh"
#include "math/mat3x3.hh"
#include "math/angles.hh"

namespace geo {

  /**
   * @note Compute the rotation matrix to convert from ECEF (Earth-Centered, Earth-Fixed)
   * coordinates to ENU (East-North-Up) coordinates.
   * @param lat Latitude in degrees.
   * @param lon Longitude in degrees.
   * @return Rotation matrix as a `Mat3x3` object.
   * @see https://gssc.esa.int/navipedia/index.php/Transformations_between_ECEF_and_ENU_coordinates
   */
  constexpr math::Mat3x3 ecef_to_enu_matrix(float lat, float lon)
  {
    float lat_rad = math::degrees_to_radians(lat);
    float lon_rad = math::degrees_to_radians(lon);

    float sin_lat = std::sin(lat_rad);
    float cos_lat = std::cos(lat_rad);
    float sin_lon = std::sin(lon_rad);
    float cos_lon = std::cos(lon_rad);

    return math::Mat3x3{
      -sin_lon,            cos_lon,           0.0f,
      -sin_lat * cos_lon, -sin_lat * sin_lon, cos_lat,
       cos_lat * cos_lon,  cos_lat * sin_lon, sin_lat
    };
  }

}

#endif // __ENU_HH__
