/**
 ******************************************************************************
 * @file    triad.hh
 * @brief   This file provides a function to compute the orientation 
 *          quaternion from accelerometer and magnetometer readings.
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
#ifndef __TRIAD_HH__
#define __TRIAD_HH__

#include "math/vec3.hh"
#include "math/quat.hh"

namespace ahrs {

  /**
    * @brief Compute attitude quaternion using the TRIAD algorithm.
    * @param acc_meas Measured acceleration vector.
    * @param mag_meas Measured magnetic field vector.
    * @param mag_ref Reference magnetic field vector.
    * @return Estimated attitude as a quaternion.
    */
  math::Quat triad(const math::Vec3& acc_meas, const math::Vec3& mag_meas, const math::Vec3& mag_ref);

}

#endif // __TRIAD_HH__
