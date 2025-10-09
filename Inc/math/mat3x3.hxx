/**
  ******************************************************************************
  * @file    mat3x3.hxx
  * @brief   This file provides a 3x3 matrix class with common 
  *          matrix operations
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
#ifndef __MATH_MAT3X3_HXX__
#define __MATH_MAT3X3_HXX__

#include <cmath>
#include <cstddef>
#include <initializer_list>

#include "vec3.hxx"

namespace math {

  /**
   * @brief 3x3 matrix class with common matrix operations.
   */
  class Mat3x3 {
  public:
    /**
     * @brief Returns a zero matrix.
     */
    static constexpr Mat3x3 zero() {
      return Mat3x3();
    }

    /**
     * @brief Returns an identity matrix.
     */
    static constexpr Mat3x3 eye() {
      return diag(1.0f);
    }

    /**
     * @brief Returns a diagonal matrix with given diagonal value.
     */
    static constexpr Mat3x3 diag(float d) {
      return Mat3x3(
        d,    0.0f, 0.0f,
        0.0f, d,    0.0f,
        0.0f, 0.0f, d
      );
    }

    /**
     * @brief Returns a diagonal matrix with given diagonal vector.
     */
    static constexpr Mat3x3 diag(Vec3 const& v) {
      return Mat3x3(
        v.x,  0.0f, 0.0f,
        0.0f, v.y,  0.0f,
        0.0f, 0.0f, v.z
      );
    }

  public:
    union {
      struct {
        float m00, m01, m02;
        float m10, m11, m12;
        float m20, m21, m22;
      };
      float data[3][3];
      Vec3 rows[3];
    };

    /**
     * @brief Default constructor.
     */
    constexpr Mat3x3() = default;
    /** 
     * @brief Constructs a matrix with specified elements.
     */
    constexpr Mat3x3(float m00, float m01, float m02,
                     float m10, float m11, float m12,
                     float m20, float m21, float m22) 
      : m00{m00}, m01{m01}, m02{m02},
        m10{m10}, m11{m11}, m12{m12},
        m20{m20}, m21{m21}, m22{m22} {}
    /**
     * @brief Constructs a matrix from a nested initializer list.
     * @note The outer list represents rows, and each inner list represents columns.
     *       If the initializer list has fewer than 3 rows or columns, the remaining
     *       elements are set to 0. If there are more than 3 rows or columns, extra elements are ignored.
     */
    constexpr Mat3x3(std::initializer_list<std::initializer_list<float>> init) : data{} {
      size_t i = 0;
      for (auto row : init) {
        if (i < 3) {
          size_t j = 0;
          for (auto val : row) {
            if (j < 3) {
              data[i][j] = val;
            }
            ++j;
          }
        }
        ++i;
      }
    }
    /**
     * @brief Constructs a matrix from a C-style 2D array.
     * @note The array must have exactly 3 rows and 3 columns.
     */
    constexpr Mat3x3(float (&arr)[3][3]) : data{} {
      for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
          data[i][j] = arr[i][j];
        }
      }
    }

    /**
     * @brief Compares two matrices for equality.
     */
    constexpr bool operator==(Mat3x3 const& other) const {
      for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
          if (data[i][j] != other.data[i][j]) {
            return false;
          }
        }
      }
      return true;
    }
    /**
     * @brief Compares two matrices for inequality. 
     */
    constexpr bool operator!=(Mat3x3 const& other) const {
      return !(*this == other);
    }

    /**
     * @brief Access matrix elements by row and column.
     */
    constexpr float& operator()(size_t row, size_t col) {
      return data[row][col];
    }
    /**
     * @brief Access matrix elements by row and column (const version).
     */
    constexpr float operator()(size_t row, size_t col) const {
      return data[row][col];
    }

    /**
     * @brief Unary plus operator.
     */
    constexpr Mat3x3 operator+() const {
      return *this;
    }
    /**
     * @brief Unary minus operator (negates all elements).
     */
    constexpr Mat3x3 operator-() const {
      Mat3x3 result;
      for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
          result.data[i][j] = -data[i][j];
        }
      }
      return result;
    }

    /**
     * @brief Matrix addition.
     */
    constexpr Mat3x3 operator+(Mat3x3 const& other) const {
      Mat3x3 result;
      for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
          result.data[i][j] = data[i][j] + other.data[i][j];
        }
      }
      return result;
    }
    /**
     * @brief Matrix subtraction.
     */
    constexpr Mat3x3 operator-(Mat3x3 const& other) const {
      Mat3x3 result;
      for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
          result.data[i][j] = data[i][j] - other.data[i][j];
        }
      }
      return result;
    }
    /** 
     * @brief Scalar multiplication. 
     */
    constexpr Mat3x3 operator*(float scalar) const {
      Mat3x3 result;
      for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
          result.data[i][j] = data[i][j] * scalar;
        }
      }
      return result;
    }
    /**
     * @brief Scalar division.
     */
    constexpr Mat3x3 operator/(float scalar) const {
      Mat3x3 result;
      for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
          result.data[i][j] = data[i][j] / scalar;
        }
      }
      return result;
    }
    /**
     * @brief Matrix multiplication.
     */
    constexpr Mat3x3 operator*(Mat3x3 const& other) const {
      Mat3x3 result;
      for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
          result.data[i][j] = 0.0f;
          for (size_t k = 0; k < 3; ++k) {
            result.data[i][j] += data[i][k] * other.data[k][j];
          }
        }
      }
      return result;
    }
    /**
     * @brief Matrix-vector multiplication.
     */
    constexpr Vec3 operator*(Vec3 const& vec) const {
      return Vec3(
        m00 * vec.x + m01 * vec.y + m02 * vec.z,
        m10 * vec.x + m11 * vec.y + m12 * vec.z,
        m20 * vec.x + m21 * vec.y + m22 * vec.z
      );
    }

    /**
     * @brief Matrix addition assignment.
     */
    constexpr Mat3x3& operator+=(Mat3x3 const& other) {
      for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
          data[i][j] += other.data[i][j];
        }
      }
      return *this;
    }
    /**
     * @brief Matrix subtraction assignment.
     */
    constexpr Mat3x3& operator-=(Mat3x3 const& other) {
      for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
          data[i][j] -= other.data[i][j];
        }
      }
      return *this;
    }
    /**
     * @brief Scalar multiplication assignment.
     */
    constexpr Mat3x3& operator*=(float scalar) {
      for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
          data[i][j] *= scalar;
        }
      }
      return *this;
    }
    /**
     * @brief Scalar division assignment.
     */
    constexpr Mat3x3& operator/=(float scalar) {
      for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
          data[i][j] /= scalar;
        }
      }
      return *this;
    }

    /**
     * @brief Transposes the matrix.
     */
    constexpr Mat3x3 t() const {
      return Mat3x3(
        m00, m10, m20,
        m01, m11, m21,
        m02, m12, m22
      );
    }

    /**
     * @brief Computes the determinant of the matrix.
     */
    constexpr float det() const {
      return m00 * (m11 * m22 - m12 * m21)
           - m01 * (m10 * m22 - m12 * m20)
           + m02 * (m10 * m21 - m11 * m20);
    }

    /**
     * @brief Computes the inverse of the matrix.
     * @note If the matrix is non-invertible (determinant is zero),
     *       a zero matrix is returned.
     */
    constexpr Mat3x3 inv() const {
      float determinant = det(); 

      /* Handle non-invertible matrix */
      if (determinant == 0.0f) {
        return Mat3x3::zero(); 
      } 

      float inv_det = 1.0f / determinant;
      return Mat3x3(
        (m11 * m22 - m12 * m21) * inv_det,
        (m02 * m21 - m01 * m22) * inv_det,
        (m01 * m12 - m02 * m11) * inv_det,
        (m12 * m20 - m10 * m22) * inv_det,
        (m00 * m22 - m02 * m20) * inv_det,
        (m02 * m10 - m00 * m12) * inv_det,
        (m10 * m21 - m11 * m20) * inv_det,
        (m01 * m20 - m00 * m21) * inv_det,
        (m00 * m11 - m01 * m10) * inv_det
      );
    }

  };

  /**
   * @brief Tensorial dot product of a matrix and a vector.
   */
  constexpr Vec3 dot(Mat3x3 const& mat, Vec3 const& vec) {
    return mat * vec;
  }

}

#endif /* __MATH_MAT3X3_HXX__ */
