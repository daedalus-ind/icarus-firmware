/**
 ******************************************************************************
 * @file    altitude.hh
 * @brief   This file provides a function to convert pressure to altitude.
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
#ifndef __ALTITUDE_HH__
#define __ALTITUDE_HH__

#include <cmath>

namespace geo {

  /**
   * @brief Convert pressure to altitude using the barometric formula.
   * @param pressure Measured pressure in hPa.
   * @param sea_level_pressure Sea level standard atmospheric pressure in hPa (default is 1013.25 hPa).
   * @return Altitude in meters.
   */
  constexpr float altitude_from_pressure(float pressure, float sea_level_pressure = 1013.25f) 
  {
    /**
     * @note Barometric formula 
     * @see https://en.wikipedia.org/wiki/Barometric_formula
     * 
     * altitude = 44330 * (1 - (P / P0)^(1/5.255))
     * where:
     *   P = pressure at altitude (in hPa)
     *   P0 = sea level standard atmospheric pressure (in hPa)
     */
    return 44330.0f * (1.0f - std::pow(pressure / sea_level_pressure, 0.19026f));
  }
  
}

#endif // __ALTITUDE_HH__