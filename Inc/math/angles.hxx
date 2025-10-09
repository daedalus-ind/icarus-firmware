/**
 ******************************************************************************
 * @file    angles.hxx
 * @brief   This file provides simple angle conversion utilities
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
#ifndef __MATH_ANGLES_HXX__
#define __MATH_ANGLES_HXX__

#include <cmath>
#include <numbers>

namespace math {
  
  /**
   * @brief Converts degrees to radians.
   * @param degrees Angle in degrees.
   * @return Angle in radians.
   */
  constexpr float deg_to_rad(float degrees) {
    return degrees * std::numbers::pi_v<float> / 180.0f;
  }

  /**
   * @brief Converts radians to degrees.
   * @param radians Angle in radians.
   * @return Angle in degrees.
   */
  constexpr float rad_to_deg(float radians) {
    return radians * 180.0f / std::numbers::pi_v<float>;
  }

}

#endif /* __MATH_ANGLES_HXX__ */
