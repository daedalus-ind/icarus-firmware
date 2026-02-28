/**
 ******************************************************************************
 * @file    quat.hh
 * @brief   This file contains a quaternion structure with common 
 *          mathematical operations.
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
#ifndef __MATH_QUAT_HH__
#define __MATH_QUAT_HH__

#include "math/vec3.hh"
#include "math/mat3x3.hh"

#include <cmath>

namespace math {

  /**
   * @brief A structure representing a quaternion.
   */
  struct Quat {
    float w, x, y, z;

    /**
     * @brief Returns a zero quaternion.
     * @return A quaternion with all components set to zero.
     */
    static constexpr Quat zero() 
    {
      return Quat{0.0f, 0.0f, 0.0f, 0.0f};
    }

    /**
     * @brief Returns an identity quaternion.
     * @return A quaternion representing no rotation.
     */
    static constexpr Quat identity() 
    {
      return Quat{1.0f, 0.0f, 0.0f, 0.0f};
    }

    /**
     * @brief Creates a quaternion from a scalar and a vector.
     * @param scalar The scalar part of the quaternion.
     * @param vector The vector part of the quaternion.
     * @return A quaternion constructed from the given scalar and vector.
     */
    static constexpr Quat fromScalarVector(float scalar, const Vec3& vector) 
    {
      return Quat{scalar, vector.x, vector.y, vector.z};
    }

    /**
     * @brief Creates a quaternion from a vector, with zero scalar part.
     * @param vector The vector part of the quaternion.
     * @return A pure quaternion constructed from the given vector.
     */
    static constexpr Quat fromVector(const Vec3& vector) 
    {
      return Quat{0.0f, vector.x, vector.y, vector.z};
    }

    /**
     * @brief Creates a quaternion from a rotation matrix.
     * @param R The rotation matrix.
     * @return A quaternion representing the same rotation as the matrix.
     */
    static constexpr Quat fromRotationMatrix(const Mat3x3& R)
    {
      float trace = R(0,0) + R(1,1) + R(2,2);
      float w, x, y, z;

      if (trace > 0.0f) {
        float s = std::sqrt(trace + 1.0f) * 2.0f;
        w = 0.25f * s;
        x = (R(2,1) - R(1,2)) / s;
        y = (R(0,2) - R(2,0)) / s;
        z = (R(1,0) - R(0,1)) / s;
      } else if ((R(0,0) > R(1,1)) && (R(0,0) > R(2,2))) {
        float s = std::sqrt(1.0f + R(0,0) - R(1,1) - R(2,2)) * 2.0f;
        w = (R(2,1) - R(1,2)) / s;
        x = 0.25f * s;
        y = (R(0,1) + R(1,0)) / s;
        z = (R(0,2) + R(2,0)) / s;
      } else if (R(1,1) > R(2,2)) {
        float s = std::sqrt(1.0f + R(1,1) - R(0,0) - R(2,2)) * 2.0f;
        w = (R(0,2) - R(2,0)) / s;
        x = (R(0,1) + R(1,0)) / s;
        y = 0.25f * s;
        z = (R(1,2) + R(2,1)) / s;
      } else {
        float s = std::sqrt(1.0f + R(2,2) - R(0,0) - R(1,1)) * 2.0f;
        w = (R(1,0) - R(0,1)) / s;
        x = (R(0,2) + R(2,0)) / s;
        y = (R(1,2) + R(2,1)) / s;
        z = 0.25f * s;
      }
      return Quat{w, x, y, z};
    }

    /**
     * @brief Computes the conjugate of the quaternion.
     * @return The conjugate of the quaternion.
     */
    constexpr Quat conjugate() const 
    {
      return {w, -x, -y, -z};
    }

    /**
     * @brief Computes the inverse of the quaternion.
     * @return The inverse of the quaternion.
     */
    constexpr Quat inverse() const 
    {
      float norm_sq = w * w + x * x + y * y + z * z;
      return {w / norm_sq, -x / norm_sq, -y / norm_sq, -z / norm_sq};
    }

    /**
     * @brief Computes the norm (magnitude) of the quaternion.
     * @return The norm of the quaternion.
     */
    constexpr float norm() const
    {
      return std::sqrt(w * w + x * x + y * y + z * z);
    }

    /**
     * @brief Normalizes the quaternion to unit length.
     * @return A normalized quaternion.
     */
    constexpr Quat normalized() const
    {
      float n = norm();
      return {w / n, x / n, y / n, z / n};
    }

    /**
     * @brief Extracts the vector part of the quaternion.
     * @return The vector part as a Vec3.
     */
    constexpr Vec3 vectorPart() const
    {
      return Vec3{x, y, z};
    }

    /**
     * @brief Converts the quaternion to a rotation matrix.
     * @return The corresponding rotation matrix.
     */
    constexpr Mat3x3 toRotationMatrix() const
    {
      float xx = x * x;
      float yy = y * y;
      float zz = z * z;
      float xy = x * y;
      float xz = x * z;
      float yz = y * z;
      float wx = w * x;
      float wy = w * y;
      float wz = w * z;

      return Mat3x3{
        1.0f - 2.0f * (yy + zz), 2.0f * (xy - wz),       2.0f * (xz + wy),
        2.0f * (xy + wz),       1.0f - 2.0f * (xx + zz), 2.0f * (yz - wx),
        2.0f * (xz - wy),       2.0f * (yz + wx),       1.0f - 2.0f * (xx + yy)
      };
    }
  }; 

}

/**
 * @brief Checks if two quaternions are equal.
 * @param a The first quaternion.
 * @param b The second quaternion.
 * @return True if the quaternions are equal, false otherwise.
 */
constexpr bool operator==(const math::Quat& a, const math::Quat& b) 
{
  return (a.w == b.w) && (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
}

/**
 * @brief Checks if two quaternions are not equal.
 * @param a The first quaternion.
 * @param b The second quaternion.
 * @return True if the quaternions are not equal, false otherwise.
 */
constexpr bool operator!=(const math::Quat& a, const math::Quat& b) 
{
  return !(a == b);
}

/**
 * @brief Adds two quaternions component-wise.
 * @param q1 The first quaternion.
 * @param q2 The second quaternion.
 * @return The sum of the two quaternions.
 */
constexpr math::Quat operator+(const math::Quat& q1, const math::Quat& q2) 
{
  return {q1.w + q2.w, q1.x + q2.x, q1.y + q2.y, q1.z + q2.z};
}

/**
 * @brief Negates a quaternion component-wise.
 * @param q The quaternion to negate.
 * @return The negated quaternion.
 */
constexpr math::Quat operator-(const math::Quat& q) 
{
  return {-q.w, -q.x, -q.y, -q.z};
}

/**
 * @brief Subtracts two quaternions component-wise.
 * @param q1 The first quaternion.
 * @param q2 The second quaternion.
 * @return The difference of the two quaternions.
 */
constexpr math::Quat operator-(const math::Quat& q1, const math::Quat& q2) 
{
  return {q1.w - q2.w, q1.x - q2.x, q1.y - q2.y, q1.z - q2.z};
}

/**
 * @brief Multiplies a quaternion by a scalar.
 * @param q The quaternion.
 * @param scalar The scalar value.
 * @return The scaled quaternion.
 */
constexpr math::Quat operator*(const math::Quat& q, float scalar) 
{
  return {q.w * scalar, q.x * scalar, q.y * scalar, q.z * scalar};
}

/**
 * @brief Multiplies a quaternion by a scalar (scalar first).
 * @param q The quaternion.
 * @param scalar The scalar value.
 * @return The scaled quaternion.
 */
constexpr math::Quat operator*(float scalar, const math::Quat& q) 
{
  return q * scalar;
}

/**
 * @brief Multiplies two quaternions.
 * @param q1 The first quaternion.
 * @param q2 The second quaternion.
 * @return The product of the two quaternions.
 */
constexpr math::Quat operator*(const math::Quat& q1, const math::Quat& q2) 
{
  return {
    q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z,
    q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
    q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x,
    q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w
  };
}

/**
 * @brief Divides a quaternion by a scalar.
 * @param q The quaternion.
 * @param scalar The scalar value.
 * @return The scaled quaternion.
 */
constexpr math::Quat operator/(const math::Quat& q, float scalar) 
{
  return {q.w / scalar, q.x / scalar, q.y / scalar, q.z / scalar};
}

namespace math {

  /**
   * @brief Rotates a vector by a quaternion.
   * @param q The rotation quaternion.
   * @param v The vector to rotate.
   * @return The rotated vector.
   */
  constexpr math::Vec3 rotate_vector(const math::Quat& q, const math::Vec3& v) 
  {
    math::Quat q_v = math::Quat::fromVector(v);
    math::Quat q_conj = q.conjugate();
    math::Quat q_result = q * q_v * q_conj;
    return q_result.vectorPart();
  }

}

#endif // __MATH_QUAT_HH__
