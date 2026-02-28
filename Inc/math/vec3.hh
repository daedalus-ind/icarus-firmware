/**
 ******************************************************************************
 * @file    vec3.hh
 * @brief   This file contains a 3D vector structure with common 
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
#ifndef __MATH_VEC3_HH__
#define __MATH_VEC3_HH__

#include <cmath>

namespace math {
  
  /**
   * @brief A structure representing a 3D vector.
   */
  struct Vec3 {
    float x, y, z; 

    /**
     * @brief Creates a zero vector.
     * @return A zero vector.
     */
    static constexpr Vec3 zero() 
    {
      return Vec3{0.0f, 0.0f, 0.0f};
    }

    /**
     * @brief Creates a unit vector along the X axis.
     * @return A unit vector along the X axis.
     */
    static constexpr Vec3 unitX() 
    {
      return Vec3{1.0f, 0.0f, 0.0f};
    }

    /**
     * @brief Creates a unit vector along the Y axis.
     * @return A unit vector along the Y axis.
     */
    static constexpr Vec3 unitY() 
    {
      return Vec3{0.0f, 1.0f, 0.0f};
    }

    /**
     * @brief Creates a unit vector along the Z axis.
     * @return A unit vector along the Z axis.
     */
    static constexpr Vec3 unitZ() 
    {
      return Vec3{0.0f, 0.0f, 1.0f};
    }

    /**
     * @brief Access operator for vector components.
     * @param index The index of the component (0 for x, 1 for y, 2 for z).
     * @return Reference to the component.
     */
    constexpr float& operator[](size_t index)
    {
      return index == 0 ? x : (index == 1 ? y : z);
    }

    /**
     * @brief Const access operator for vector components.
     * @param index The index of the component (0 for x, 1 for y, 2 for z).
     * @return Const reference to the component.
     */
    constexpr const float& operator[](size_t index) const
    {
      return index == 0 ? x : (index == 1 ? y : z);
    }

    /**
     * @brief Computes the dot product of two vectors.
     * @param other The other vector.
     * @return The dot product.
     */
    constexpr float dot(const Vec3& other) const 
    {
      return x * other.x + y * other.y + z * other.z;
    }

    /**
     * @brief Computes the cross product of two vectors.
     * @param other The other vector.
     * @return The cross product vector.
     */
    constexpr Vec3 cross(const Vec3& other) const 
    {
      return Vec3{
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
      };
    }

    /**
     * @brief Computes the norm (magnitude) of the vector.
     * @return The norm of the vector.
     */
    constexpr float norm() const 
    {
      return std::sqrt(x * x + y * y + z * z);
    }

    /**
     * @brief Normalizes the vector to unit length.
     * @return The normalized vector.
     */
    constexpr Vec3 normalized() const 
    {
      float n = norm();
      return Vec3{x / n, y / n, z / n};
    }
  };

}

/**
 * @brief Equality operator for Vec3.
 * @param a The first vector.
 * @param b The second vector.
 * @return True if the vectors are equal, false otherwise.
 */
constexpr bool operator==(const math::Vec3& a, const math::Vec3& b) 
{
  return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
}

/**
 * @brief Inequality operator for Vec3.
 * @param a The first vector.
 * @param b The second vector.
 * @return True if the vectors are not equal, false otherwise.
 */
constexpr bool operator!=(const math::Vec3& a, const math::Vec3& b) 
{
  return !(a == b);
}

/**
 * @brief Addition operator for Vec3.
 * @param a The first vector.
 * @param b The second vector.
 * @return The sum of the two vectors.
 */
constexpr math::Vec3 operator+(const math::Vec3& a, const math::Vec3& b) 
{
  return math::Vec3{a.x + b.x, a.y + b.y, a.z + b.z};
}

/**
 * @brief Unary negation operator for Vec3.
 * @param v The vector to negate.
 * @return The negated vector.
 */
constexpr math::Vec3 operator-(const math::Vec3& v) 
{
  return math::Vec3{-v.x, -v.y, -v.z};
}

/**
 * @brief Subtraction operator for Vec3.
 * @param a The first vector.
 * @param b The second vector.
 * @return The difference of the two vectors.
 */
constexpr math::Vec3 operator-(const math::Vec3& a, const math::Vec3& b) 
{
  return math::Vec3{a.x - b.x, a.y - b.y, a.z - b.z};
}

/**
 * @brief Scalar multiplication operator for Vec3.
 * @param v The vector.
 * @param scalar The scalar value.
 * @return The scaled vector.
 */
constexpr math::Vec3 operator*(const math::Vec3& v, float scalar) 
{
  return math::Vec3{v.x * scalar, v.y * scalar, v.z * scalar};
}

/**
 * @brief Scalar multiplication operator for Vec3 (scalar first).
 * @param scalar The scalar value.
 * @param v The vector.
 * @return The scaled vector.
 */
constexpr math::Vec3 operator*(float scalar, const math::Vec3& v) 
{
  return v * scalar;
}

/**
 * @brief Scalar division operator for Vec3.
 * @param v The vector.
 * @param scalar The scalar value.
 * @return The scaled vector.
 */
constexpr math::Vec3 operator/(const math::Vec3& v, float scalar) 
{
  return math::Vec3{v.x / scalar, v.y / scalar, v.z / scalar};
}

#endif // __MATH_VEC3_HH__
