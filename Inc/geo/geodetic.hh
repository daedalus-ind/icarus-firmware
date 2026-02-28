/**
 ******************************************************************************
 * @file    geodetic.hh
 * @brief   This file provides the definition of geodetic coordinates 
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
#ifndef __GEODETIC_HH__
#define __GEODETIC_HH__

namespace geo {
  
  /**
   * @brief Geodetic coordinates structure.
   */
  struct Geodetic {
    float latitude;
    float longitude;
    float altitude;
  };

}

#endif // __GEODETIC_HH__
