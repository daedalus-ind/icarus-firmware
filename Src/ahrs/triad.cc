/**
 ******************************************************************************
 * @file    triad.cc
 * @brief   This file implements the triad algorithm to compute the orientation
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
#include "ahrs/triad.hh"

math::Quat ahrs::triad(const math::Vec3& acc_meas, const math::Vec3& mag_meas, const math::Vec3& mag_ref) 
{
  // Reference vectors in the navigation frame
  static constexpr math::Vec3 acc_ref = math::Vec3{0.0f, 0.0f, 1.0f}; // Gravity vector

  // Measured vectors in the body frame
  const math::Vec3 t1_b = acc_meas.normalized();
  const math::Vec3 t2_b = (t1_b.cross(mag_meas)).normalized();
  const math::Vec3 t3_b = t1_b.cross(t2_b);

  // Reference vectors in the navigation frame
  const math::Vec3 t1_n = acc_ref;
  const math::Vec3 t2_n = (t1_n.cross(mag_ref)).normalized();
  const math::Vec3 t3_n = t1_n.cross(t2_n);

  // Rotation matrix from body to navigation frame
  math::Mat3x3 R_bn;
  R_bn.setCol(0, t2_b);
  R_bn.setCol(1, t3_b);
  R_bn.setCol(2, t1_b);

  math::Mat3x3 R_nn;
  R_nn.setCol(0, t2_n);
  R_nn.setCol(1, t3_n);
  R_nn.setCol(2, t1_n);

  // Compute the rotation matrix from body to navigation frame
  math::Mat3x3 R = R_nn * R_bn.transpose();

  // Convert rotation matrix to quaternion
  return math::Quat::fromRotationMatrix(R);
}
