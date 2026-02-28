/**
 ******************************************************************************
 * @file    mat3x3.hh
 * @brief   This file contains a 3x3 matrix structure with common 
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
#ifndef __MATH_MAT3X3_HH__
#define __MATH_MAT3X3_HH__

#include "vec3.hh"

namespace math {

  struct Mat3x3 {
    float m[3][3];

    /**
     * @brief Creates a zero matrix.
     * @return A zero matrix.
     */
    static constexpr Mat3x3 zero() 
    {
      return Mat3x3{
        {
          {0.0f, 0.0f, 0.0f},
          {0.0f, 0.0f, 0.0f},
          {0.0f, 0.0f, 0.0f}
        }
      };
    }

    /**
     * @brief Creates an identity matrix.
     * @return An identity matrix.
     */
    static constexpr Mat3x3 identity() 
    {
      return Mat3x3::diagonal(1.0f);
    }

    /**
     * @brief Creates a diagonal matrix with the given value.
     * @param d The diagonal value.
     * @return A diagonal matrix.
     */
    static constexpr Mat3x3 diagonal(float d) 
    {
      return Mat3x3{
        {
          {d, 0.0f, 0.0f},
          {0.0f, d, 0.0f},
          {0.0f, 0.0f, d}
        }
      };
    }

    /**
     * @brief Access operator for matrix elements.
     * @param row The row index.
     * @param col The column index.
     * @return Reference to the matrix element.
     */
    constexpr float& operator()(size_t row, size_t col)
    {
      return m[row][col];
    }

    /**
     * @brief Const access operator for matrix elements.
     * @param row The row index.
     * @param col The column index.
     * @return Const reference to the matrix element.
     */
    constexpr const float& operator()(size_t row, size_t col) const
    {
      return m[row][col];
    }

    /**
     * @brief Sets a row of the matrix.
     * @param row The row index.
     * @param vec The vector to set as the row.
     */
    constexpr void setRow(size_t row, const Vec3& vec) 
    {
      m[row][0] = vec.x;
      m[row][1] = vec.y;
      m[row][2] = vec.z;
    }

    /**
     * @brief Sets a column of the matrix.
     * @param col The column index.
     * @param vec The vector to set as the column.
     */
    constexpr void setCol(size_t col, const Vec3& vec) 
    {
      m[0][col] = vec.x;
      m[1][col] = vec.y;
      m[2][col] = vec.z;
    }

    /**
     * @brief Transposes the matrix.
     * @return The transposed matrix.
     */
    constexpr Mat3x3 transpose() const 
    {
      return Mat3x3{
        {
          {m[0][0], m[1][0], m[2][0]},
          {m[0][1], m[1][1], m[2][1]},
          {m[0][2], m[1][2], m[2][2]}
        }
      };
    }

    /**
     * @brief Calculates the determinant of the matrix.
     * @return The determinant value.
     */
    constexpr float determinant() const 
    {
      return m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
             m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
             m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
    }
  };

}

/**
 * @brief Equality operator for Mat3x3.
 * @param a The first matrix.
 * @param b The second matrix.
 * @return True if the matrices are equal, false otherwise.
 */
constexpr bool operator==(const math::Mat3x3& a, const math::Mat3x3& b) 
{
  for (size_t i = 0; i < 3; ++i) {
    for (size_t j = 0; j < 3; ++j) {
      if (a.m[i][j] != b.m[i][j]) {
        return false;
      }
    }
  }
  return true;
}

/**
 * @brief Inequality operator for Mat3x3.
 * @param a The first matrix.
 * @param b The second matrix.
 * @return True if the matrices are not equal, false otherwise.
 */
constexpr bool operator!=(const math::Mat3x3& a, const math::Mat3x3& b) 
{
  return !(a == b);
}

/**
 * @brief Addition operator for Mat3x3.
 * @param a The first matrix.
 * @param b The second matrix.
 * @return The sum of the two matrices.
 */
constexpr math::Mat3x3 operator+(const math::Mat3x3& a, const math::Mat3x3& b) 
{
  math::Mat3x3 result = {};
  for (size_t i = 0; i < 3; ++i) {
    for (size_t j = 0; j < 3; ++j) {
      result.m[i][j] = a.m[i][j] + b.m[i][j];
    }
  }
  return result;
}

/**
 * @brief Unary negation operator for Mat3x3.
 * @param mat The matrix to negate.
 * @return The negated matrix.
 */
constexpr math::Mat3x3 operator-(const math::Mat3x3& mat)
{
  math::Mat3x3 result = {};
  for (size_t i = 0; i < 3; ++i) {
    for (size_t j = 0; j < 3; ++j) {
      result.m[i][j] = -mat.m[i][j];
    }
  }
  return result;
}

/**
 * @brief Subtraction operator for Mat3x3.
 * @param a The first matrix.
 * @param b The second matrix.
 * @return The difference of the two matrices.
 */
constexpr math::Mat3x3 operator-(const math::Mat3x3& a, const math::Mat3x3& b) 
{
  math::Mat3x3 result = {};
  for (size_t i = 0; i < 3; ++i) {
    for (size_t j = 0; j < 3; ++j) {
      result.m[i][j] = a.m[i][j] - b.m[i][j];
    }
  }
  return result;
}

/**
 * @brief Multiplication operator for Mat3x3.
 * @param a The first matrix.
 * @param b The second matrix.
 * @return The product of the two matrices.
 */
constexpr math::Mat3x3 operator*(const math::Mat3x3& a, const math::Mat3x3& b) 
{
  math::Mat3x3 result = {};
  for (size_t i = 0; i < 3; ++i) {
    for (size_t j = 0; j < 3; ++j) {
      result.m[i][j] = 0.0f;
      for (size_t k = 0; k < 3; ++k) {
        result.m[i][j] += a.m[i][k] * b.m[k][j];
      }
    }
  }
  return result;
}

/**
 * @brief Scalar multiplication operator for Mat3x3.
 * @param mat The matrix.
 * @param scalar The scalar value.
 * @return The scaled matrix.
 */
constexpr math::Mat3x3 operator*(const math::Mat3x3& mat, float scalar) 
{
  math::Mat3x3 result = {};
  for (size_t i = 0; i < 3; ++i) {
    for (size_t j = 0; j < 3; ++j) {
      result.m[i][j] = mat.m[i][j] * scalar;
    }
  }
  return result;
}

/**
 * @brief Scalar multiplication operator for Mat3x3 (scalar first).
 * @param scalar The scalar value.
 * @param mat The matrix.
 * @return The scaled matrix.
 */
constexpr math::Mat3x3 operator*(float scalar, const math::Mat3x3& mat) 
{
  return mat * scalar;
}

/**
 * @brief Matrix-vector multiplication operator for Mat3x3 and Vec3.
 * @param mat The matrix.
 * @param vec The vector.
 * @return The resulting vector.
 */
constexpr math::Vec3 operator*(const math::Mat3x3& mat, const math::Vec3& vec) 
{
  return math::Vec3{
    mat.m[0][0] * vec.x + mat.m[0][1] * vec.y + mat.m[0][2] * vec.z,
    mat.m[1][0] * vec.x + mat.m[1][1] * vec.y + mat.m[1][2] * vec.z,
    mat.m[2][0] * vec.x + mat.m[2][1] * vec.y + mat.m[2][2] * vec.z
  };
}

/**
 * @brief Scalar division operator for Mat3x3.
 * @param mat The matrix.
 * @param scalar The scalar value.
 * @return The scaled matrix.
 */
constexpr math::Mat3x3 operator/(const math::Mat3x3& mat, float scalar) 
{
  math::Mat3x3 result = {};
  for (size_t i = 0; i < 3; ++i) {
    for (size_t j = 0; j < 3; ++j) {
      result.m[i][j] = mat.m[i][j] / scalar;
    }
  }
  return result;
}

#endif // __MATH_MAT3X3_HH__
