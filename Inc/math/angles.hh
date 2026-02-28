/**
 ******************************************************************************
 * @file    angles.hh
 * @brief   This file contains the definition of angle conversion functions.
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
#ifndef __MATH_ANGLES_HH__
#define __MATH_ANGLES_HH__

#include "vec3.hh"

#include <numbers>

namespace math {

  /**
   * @brief Conversion factor from degrees to radians.
   */
  constexpr float DEG_TO_RAD = std::numbers::pi_v<float> / 180.0f;

  /**
   * @brief Conversion factor from radians to degrees.
   */
  constexpr float RAD_TO_DEG = 180.0f / std::numbers::pi_v<float>;

  /**
   * @brief Converts degrees to radians.
   * @param degrees Angle in degrees.
   * @return Angle in radians.
   */
  constexpr float degrees_to_radians(float degrees)
  {
    return degrees * DEG_TO_RAD;
  }

  /**
   * @brief Converts degrees, minutes, and seconds to radians.
   * @param degrees Degrees part of the angle.
   * @param minutes Minutes part of the angle.
   * @param seconds Seconds part of the angle.
   * @return Angle in radians.
   */
  constexpr float degrees_to_radians(float degrees, float minutes, float seconds)
  {
    return degrees_to_radians(degrees + minutes / 60.0f + seconds / 3600.0f);
  }

  /**
   * @brief Converts a Vec3 of degrees to radians.
   * @param degrees Vec3 containing angles in degrees.
   * @return Vec3 containing angles in radians.
   */
  constexpr Vec3 degrees_to_radians(const Vec3& degrees)
  {
    return degrees * DEG_TO_RAD;
  }

  /**
   * @brief Converts radians to degrees.
   * @param radians Angle in radians.
   * @return Angle in degrees.
   */
  constexpr float radians_to_degrees(float radians)
  {
    return radians * RAD_TO_DEG;
  }

  /**
   * @brief Converts a Vec3 of radians to degrees.
   * @param radians Vec3 containing angles in radians.
   * @return Vec3 containing angles in degrees.
   */
  constexpr Vec3 radians_to_degrees(const Vec3& radians)
  {
    return radians * RAD_TO_DEG;
  }

}

#endif // __ANGLES_HH__
