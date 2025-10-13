/**
 ******************************************************************************
 * @file    vec3.hxx
 * @brief   This file provides a 3D vector class with common 
 *          vector operations
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
#ifndef __MATH_VEC3_HXX__
#define __MATH_VEC3_HXX__

#include <cmath>
#include <cstddef>
#include <initializer_list>

namespace math {

  /**
   * @brief 3D vector class with common vector operations.
   */
  class Vec3 {
  public:
    /**
     * @brief Zero vector (0, 0, 0).
     */
    static constexpr Vec3 zero() {
      return Vec3(0.0f, 0.0f, 0.0f);
    }

    /**
     * @brief Unit X vector (1, 0, 0).
     */
    static constexpr Vec3 unit_x() {
      return Vec3(1.0f, 0.0f, 0.0f);
    }

    /**
     * @brief Unit Y vector (0, 1, 0).
     */
    static constexpr Vec3 unit_y() {
      return Vec3(0.0f, 1.0f, 0.0f);
    }

    /**
     * @brief Unit Z vector (0, 0, 1).
     */
    static constexpr Vec3 unit_z() {
      return Vec3(0.0f, 0.0f, 1.0f);
    }
    
  public:
    union {
      struct {
        float x;
        float y;
        float z;
      };

      float data[3];
    };
    
    /**
     * @brief Default constructor.
     */
    constexpr Vec3() = default;
    /**
     * @brief Constructs a vector with specified components.
     */
    constexpr Vec3(float x, float y, float z) : x{x}, y{y}, z{z} {}
    /**
     * @brief Constructs a vector from an initializer list.
     * @note If the list has fewer than 3 elements, remaining components are set to 0.
     *       If the list has more than 3 elements, extra elements are ignored.
     */
    constexpr Vec3(std::initializer_list<float> init) : data{} {
      size_t i = 0;
      for (auto val : init) {
        if (i < 3) {
          data[i] = val;
        }
        ++i;
      } 
    }
    /**
     * @brief Constructs a vector from a C-style array of 3 floats.
     * @note The array must have exactly 3 elements.
     */
    constexpr Vec3(float (&arr)[3]) : x{arr[0]}, y{arr[1]}, z{arr[2]} {}

    /**
     * @brief Compares two vectors for equality.
     */
    constexpr bool operator==(Vec3 const& other) const {
      return x == other.x && y == other.y && z == other.z;
    }
    /**
     * @brief Compares two vectors for inequality.
     */
    constexpr bool operator!=(Vec3 const& other) const {
      return !(*this == other);
    }

    /**
     * @brief Access vector components by index.
     * @note Index 0 = x, 1 = y, 2 = z.
     */
    constexpr float& operator[](size_t index) {
      return data[index];
    }
    /**
     * @brief Access vector components by index (const version).
     * @note Index 0 = x, 1 = y, 2 = z.
     */
    constexpr float operator[](size_t index) const {
      return data[index];
    }

    /**
     * @brief Unary plus operator (returns the vector itself).
     */
    constexpr Vec3 operator+() const {
      return *this;
    }
    /**
     * @brief Unary minus operator (negates the vector).
     */
    constexpr Vec3 operator-() const {
      return Vec3(-x, -y, -z);
    }

    /** 
    * @brief Vector addition.
     */
    constexpr Vec3 operator+(Vec3 const& other) const {
      return Vec3(x + other.x, y + other.y, z + other.z);
    }
    /**
    * @brief Vector subtraction.
     */
    constexpr Vec3 operator-(Vec3 const& other) const {
      return Vec3(x - other.x, y - other.y, z - other.z);
    }
    /**
     * @brief Scalar multiplication.
     */
    constexpr Vec3 operator*(float scalar) const {
      return Vec3(x * scalar, y * scalar, z * scalar);
    }
    /**
     * @brief Scalar division.
     */
    constexpr Vec3 operator/(float scalar) const {
      float inv_scalar = 1.0f / scalar;
      return *this * inv_scalar;
    }

    /**
     * @brief Vector addition assignment.
     */ 
    constexpr Vec3& operator+=(Vec3 const& other) {
      x += other.x;
      y += other.y;
      z += other.z;
      return *this;
    }
    /**
     * @brief Vector subtraction assignment.
     */
    constexpr Vec3& operator-=(Vec3 const& other) {
      x -= other.x;
      y -= other.y;
      z -= other.z;
      return *this;
    }
    /**
     * @brief Scalar multiplication assignment.
     */
    constexpr Vec3& operator*=(float scalar) {
      x *= scalar;
      y *= scalar;
      z *= scalar;
      return *this;
    }
    /**
     * @brief Scalar division assignment.
     */
    constexpr Vec3& operator/=(float scalar) {
      float inv_scalar = 1.0f / scalar;
      return *this *= inv_scalar;
    }

    /**
     * @brief Computes the dot product of this vector with another.
     */
    constexpr float dot(Vec3 const& other) const {
      return x * other.x + y * other.y + z * other.z;
    }

    /**
     * @brief Computes the cross product of this vector with another.
     */
    constexpr Vec3 cross(Vec3 const& other) const {
      return Vec3(
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
      );
    }

    /**
     * @brief Returns the Euclidean norm (magnitude) of the vector.
     */
    constexpr float norm() const {
      return std::sqrt(x*x + y*y + z*z);
    }
    
    /**
     * @brief Returns a normalized (unit length) version of the vector.
     * @note The vector must be non-zero.
     */
    constexpr Vec3 normalized() const {
      float n = norm();
      return *this / n;
    }
  }; 

  /**
   * @brief Computes the dot product of two vectors.
   */
  constexpr float dot(Vec3 const& lhs, Vec3 const& rhs) {
    return lhs.dot(rhs);
  }

  /**
   * @brief Computes the cross product of two vectors.
   */
  constexpr Vec3 cross(Vec3 const& lhs, Vec3 const& rhs) {
    return lhs.cross(rhs);
  }

  /**
   * @brief Computes the Euclidean norm (magnitude) of a vector.
   */
  constexpr float norm(Vec3 const& v) {
    return v.norm();
  }

  /**
   * @brief Computes the angle in radians between two vectors.
    * @note Both vectors must be non-zero.
   */
  constexpr float angle_between(Vec3 const& lhs, Vec3 const& rhs) {
    float dot_product = dot(lhs, rhs);
    float norms_product = norm(lhs) * norm(rhs);

    float cos_theta = dot_product / norms_product;

    /* Clamp to valid range to avoid NaNs from acos */
    if (cos_theta > 1.0f)
      cos_theta = 1.0f;
    else if (cos_theta < -1.0f)
      cos_theta = -1.0f;

    return std::acos(cos_theta);
  }

  /**
   * @brief Projects vector 'vec' onto vector 'onto'.
   * @note The 'onto' vector must be non-zero.
   */
  constexpr Vec3 project(Vec3 const& vec, Vec3 const& onto) {
    float onto_norm_sq = dot(onto, onto);

    float scalar = dot(vec, onto) / onto_norm_sq;
    return onto * scalar;
  }

} 

/**
 * @brief Scalar multiplication with scalar on the left.
 */
constexpr math::Vec3 operator*(float scalar, math::Vec3 const& vec) {
  return vec * scalar;
}

#endif /* __VEC3_HXX__ */
