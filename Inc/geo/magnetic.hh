/**
 ******************************************************************************
 * @file    magnetic.hh
 * @brief   This file provides a function to compute the magnetic field 
 *          vector in East-North-Up (ENU) coordinates.
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
#ifndef __MAGNETIC_HH__
#define __MAGNETIC_HH__

#include "math/vec3.hh"
#include "math/angles.hh"

namespace geo {

  /**
   * @brief Compute the direction of the magnetic field vector in ENU coordinates.
   * @param dec Magnetic declination in radians.
   * @param inc Magnetic inclination in radians.
   * @return Unit vector representing the magnetic field direction in ENU frame.
   */
  constexpr math::Vec3 compute_magfield_direction_enu(float dec, float inc) 
  {
    return math::Vec3{std::cos(inc) * std::sin(dec), std::cos(inc) * std::cos(dec), -std::sin(inc)};
  }

  /**
   * @brief Reference magnetic field vector (in nT) at Villadose, RO.
   * @note Location: 4° 6' 1'' E, 61° 34' 1'' N.
   */
  constexpr math::Vec3 MAGFIELD_REF = 47.8f * compute_magfield_direction_enu(
    math::degrees_to_radians(4.0f, 6.0f, 1.0f),
    math::degrees_to_radians(61.0f, 34.0f, 1.0f)
  );
}

#endif // __MAGNETIC_HH__
