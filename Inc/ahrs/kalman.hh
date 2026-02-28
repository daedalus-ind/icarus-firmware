/**
 ******************************************************************************
 * @file    kalman.hh
 * @brief   This file contains the definition of the Kalman filter class.
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
#ifndef __KALMAN_HH__
#define __KALMAN_HH__

#include "math/vec3.hh"
#include "math/quat.hh"
#include <cstdlib>

namespace ahrs {

  /**
   * @brief Kalman filter for attitude and position estimation.
   * 
   * This class implements an Unscented Kalman Filter (UKF) for estimating the
   * orientation, position, and velocity of a system based on sensor measurements
   * from an IMU, GPS, and barometer. The state vector includes a quaternion for
   * orientation, a 3D position vector, and a 3D velocity vector.
   */
  class Kalman {
  public:
    /**
     * @brief Dimensions of the state vector and measurement vectors.
     * The state vector consists of:
     * - Quaternion (4 elements, 3 DoF)
     * - Position (3 elements, 3 DoF)
     * - Velocity (3 elements, 3 DoF)
     */
    static constexpr size_t STATE_DIM = 9;
    /**
     * @brief Number of sigma points for the Unscented Kalman Filter.
     * The number of sigma points is calculated as 2 * STATE_DIM + 1.
     */
    static constexpr size_t SIGMA_POINT_COUNT = 2 * STATE_DIM + 1;

    /**
     * @brief Quaternion indexing constants for the state vector.
     */
    static constexpr size_t QUAT_BEGIN_IDX = 0;
    static constexpr size_t QUAT_END_IDX = 4;

    /**
     * @brief Position indexing constants for the state vector. 
     */
    static constexpr size_t POS_BEGIN_IDX = 4;
    static constexpr size_t POS_END_IDX = 7;

    /**
     * @brief Velocity indexing constants for the state vector.
     */
    static constexpr size_t VEL_BEGIN_IDX = 7;
    static constexpr size_t VEL_END_IDX = 10;

    /**
     * @brief Measurement dimensions for different sensors.
     */
    static constexpr size_t ACCEL_MEAS_DIM = 3;
    static constexpr size_t MAG_MEAS_DIM = 3;
    static constexpr size_t GPS_POS_MEAS_DIM = 3;
    static constexpr size_t GPS_VEL_MEAS_DIM = 3;
    static constexpr size_t BARO_MEAS_DIM = 1;

    /**
     * @brief State vector structure for the Kalman filter.
     */
    struct State {
      union {
        struct {
          math::Quat orientation;
          math::Vec3 position;
          math::Vec3 velocity;
        };
        float data[10];
      };
    };

    /**
     * @brief Error vector structure for the Kalman filter, representing the error in the state estimation.
     */
    struct Error {
      union {
        struct {
          math::Vec3 delta_angle;
          math::Vec3 delta_position;
          math::Vec3 delta_velocity;
        };
        float data[9];
      };
    };

    /**
     * @brief Construct a new Kalman filter object.
     * @param alpha UKF parameter.
     * @param beta UKF parameter.
     * @param kappa UKF parameter.
     */
    Kalman(float alpha = 1e-3f, float beta = 2.0f, float kappa = 0.0f);

    /**
     * @brief Predict the next state based on accelerometer and gyroscope data.
     * @param accel The accelerometer reading.
     * @param gyro The gyroscope reading.
     * @param dt The time step for prediction.
     */
    void predict(const math::Vec3& accel, const math::Vec3& gyro, float dt);
    
    /**
     * @brief Update the state with a new accelerometer measurement.
     * @param accel The accelerometer reading.
     */
    void updateAccel(const math::Vec3& accel);

    /**
     * @brief Update the state with a new magnetometer measurement.
     * @param mag The magnetometer reading.
     */
    void updateMag(const math::Vec3& mag);

    /**
     * @brief Update the state with a new GPS measurement.
     * @param position The GPS position reading.
     * @param velocity The GPS velocity reading.
     */
    void updateGPS(const math::Vec3& position, const math::Vec3& velocity);

    /**
     * @brief Update the state with a new barometric altitude measurement.
     * @param altitude The barometric altitude reading.
     */
    void updateBaro(float altitude);

  private:
    void _computeWeights();
    void _generateSigmaPoints();
    void _predictMean();

  public:
    State x;
    float P[STATE_DIM][STATE_DIM];
    float Q_diag[STATE_DIM];
  
    float R_accel[ACCEL_MEAS_DIM];
    float R_mag[MAG_MEAS_DIM];
    float R_gps[GPS_POS_MEAS_DIM + GPS_VEL_MEAS_DIM];
    float R_baro[BARO_MEAS_DIM];

    math::Vec3 magnetic_field_reference;

  private:
    float _alpha;
    float _beta;
    float _lambda_plus_n;
    float _lambda;
    float _sqrt_lambda_plus_n;

    float _weights_mean[SIGMA_POINT_COUNT];
    float _weights_cov[SIGMA_POINT_COUNT];

    State _sigma_points[SIGMA_POINT_COUNT];
    State _predicted_sigma_points[SIGMA_POINT_COUNT];

    Error _predicted_errors[SIGMA_POINT_COUNT];
  };
  
}

#endif // __KALMAN_HH__
