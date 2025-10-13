/**
 ******************************************************************************
 * @file    quat.hxx
 * @brief   This file provides a quaternion class with common 
 *          quaternion operations
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
#ifndef __MATH_QUAT_HXX__
#define __MATH_QUAT_HXX__

#include <cmath>
#include <cstddef>

#include "vec3.hxx"
#include "mat3x3.hxx"

namespace math {

  /**
   * @brief Quaternion class with common quaternion operations.
   */
  class Quat {
  public:
    /**
     * @brief Zero quaternion (0, 0, 0, 0).
     */
    static constexpr Quat zero() {
      return Quat(0.0f, 0.0f, 0.0f, 0.0f);
    }

    /**
     * @brief Identity quaternion (1, 0, 0, 0).
     */
    static constexpr Quat identity() {
      return Quat(1.0f, 0.0f, 0.0f, 0.0f);
    }

  public:
    union {
      struct {
        float w;
        float x;
        float y;
        float z;
      };
      float data[4];
    };

    /** 
     * @brief Constructs an identity quaternion (1, 0, 0, 0) 
     */
    constexpr Quat() = default;
    /**
     * @brief Constructs a quaternion with specified components.
     */
    constexpr Quat(float w, float x, float y, float z) : w{w}, x{x}, y{y}, z{z} {}
    /**
     * @brief Constructs a quaternion from a scalar and a vector part.
     */
    constexpr Quat(float w, Vec3 const& v) : w{w}, x{v.x}, y{v.y}, z{v.z} {}
    /**
     * @brief Constructs a pure quaternion from a 3D vector (w = 0).
     */
    constexpr Quat(Vec3 const& v) : w{0.0f}, x{v.x}, y{v.y}, z{v.z} {}
    /**
     * @brief Constructs a quaternion from an axis and an angle in radians.
     * @note The axis vector is assumed to be normalized.
     */
    constexpr Quat(Vec3 const& axis, float angle_rad) {
      float half_angle = angle_rad * 0.5f;
      float s = std::sin(half_angle);
      w = std::cos(half_angle);
      x = axis.x * s;
      y = axis.y * s;
      z = axis.z * s;
    }

    /**
     * @brief Compares two quaternions for equality.
     */
    constexpr bool operator==(Quat const& other) const {
      return w == other.w && x == other.x && y == other.y && z == other.z;
    }
    /**
     * @brief Compares two quaternions for inequality.
     */
    constexpr bool operator!=(Quat const& other) const {
      return !(*this == other);
    }

    /**
     * @brief Access quaternion components by index.
     * @note Index 0 = w, 1 = x, 2 = y, 3 = z.
     */
    constexpr float& operator[](size_t index) {
      return data[index];
    }
    /**
     * @brief Access quaternion components by index (const version).
     * @note Index 0 = w, 1 = x, 2 = y, 3 = z.
     */
    constexpr float operator[](size_t index) const {
      return data[index];
    }

    /**
     * @brief Unary plus operator.
     */
    constexpr Quat operator+() const {
      return *this;
    }
    /**
     * @brief Unary minus operator (negates all components).
     */
    constexpr Quat operator-() const {
      return Quat(-w, -x, -y, -z);
    }

    /**
     * @brief Quaternion addition.
     */
    constexpr Quat operator+(Quat const& other) const {
      return Quat(w + other.w, x + other.x, y + other.y, z + other.z);
    }
    /**
     * @brief Quaternion subtraction.
     */
    constexpr Quat operator-(Quat const& other) const {
      return Quat(w - other.w, x - other.x, y - other.y, z - other.z);
    }
    /**
     * @brief Scalar multiplication.
     */
    constexpr Quat operator*(float scalar) const {
      return Quat(w * scalar, x * scalar, y * scalar, z * scalar);
    }
    /**
     * @brief Scalar division.
     */
    constexpr Quat operator/(float scalar) const {
      float inv_scalar = 1.0f / scalar;
      return *this * inv_scalar;
    }
    /**
     * @brief Quaternion multiplication (Hamilton product).
     */
    constexpr Quat operator*(Quat const& other) const {
      return Quat(
        w * other.w - x * other.x - y * other.y - z * other.z,
        w * other.x + x * other.w + y * other.z - z * other.y,
        w * other.y - x * other.z + y * other.w + z * other.x,
        w * other.z + x * other.y - y * other.x + z * other.w
      );
    }

    /**
     * @brief Quaternion addition assignment.
     */
    constexpr Quat& operator+=(Quat const& other) {
      w += other.w;
      x += other.x;
      y += other.y;
      z += other.z;
      return *this;
    }
    /**
     * @brief Quaternion subtraction assignment.
     */
    constexpr Quat& operator-=(Quat const& other) {
      w -= other.w;
      x -= other.x;
      y -= other.y;
      z -= other.z;
      return *this;
    }
    /**
     * @brief Scalar multiplication assignment.
     */
    constexpr Quat& operator*=(float scalar) {
      w *= scalar;
      x *= scalar;
      y *= scalar;
      z *= scalar;
      return *this;
    }
    /**
     * @brief Scalar division assignment.
     */
    constexpr Quat& operator/=(float scalar) {
      float inv_scalar = 1.0f / scalar;
      return *this *= inv_scalar;
    }

    /**
     * @brief Extracts the vector part of the quaternion as a Vec3. 
     */
    constexpr Vec3 vec3() const {
      return Vec3(x, y, z);
    }

    /**
     * @brief Computes the conjugate of the quaternion.
     */
    constexpr Quat conj() const {
      return Quat(w, -x, -y, -z);
    }

    /**
     * @brief Computes the norm (magnitude) of the quaternion.
     */
    constexpr float norm() const {
      return std::sqrt(w*w + x*x + y*y + z*z);
    }

    /**
     * @brief Normalizes the quaternion to unit length.
     * @note The quaternion must be non-zero.
     */
    constexpr Quat normalized() const {
      float n = norm();
      return *this / n;
    }

    /**
     * @brief Computes the inverse of the quaternion.
     * @note The quaternion must be non-zero.
     */
    constexpr Quat inv() const {
      float n = norm();
      return conj() / (n * n);
    }

    /**
     * @brief Rotates a 3D vector by this quaternion.
     * @note The quaternion is assumed to be normalized.
     */
    constexpr Vec3 rotate(Vec3 const& v) const {
      return (*this * v * this->conj()).vec3();
    }

    /**
     * @brief Converts the quaternion to a 3x3 rotation matrix.
     * @note The quaternion is assumed to be normalized. 
     */
    constexpr Mat3x3 to_rotation_mat() const {
      float xx = x * x;
      float yy = y * y;
      float zz = z * z;
      float xy = x * y;
      float xz = x * z;
      float yz = y * z;
      float wx = w * x;
      float wy = w * y;
      float wz = w * z;

      return Mat3x3(
        1.0f - 2.0f * (yy + zz), 2.0f * (xy - wz),        2.0f * (xz + wy),
        2.0f * (xy + wz),        1.0f - 2.0f * (xx + zz), 2.0f * (yz - wx),
        2.0f * (xz - wy),        2.0f * (yz + wx),        1.0f - 2.0f * (xx + yy)
      );
    }

  };

}

/**
 * @brief Scalar multiplication with scalar on the left.
 */
constexpr math::Quat operator*(float scalar, math::Quat const& q) {
  return q * scalar;
}

#endif /* __MATH_QUAT_HXX__ */
