/**
 ******************************************************************************
 * @file    kalman.cc
 * @brief   This file contains the implementation of the Kalman filter class.
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
#include "ahrs/kalman.hh"
#include "math/vec3.hh"
#include "math/angles.hh"

#include "main.hh"
#include "arm_math.h"

#include <algorithm>
#include <cstring>
#include <array>
#include <cmath>

template <size_t MeasDim>
struct UpdateBuffer {
  float meas_sigma_points[ahrs::Kalman::SIGMA_POINT_COUNT][MeasDim];
  float z_pred[MeasDim];
  float z_err[ahrs::Kalman::SIGMA_POINT_COUNT][MeasDim];
  float Pzz[MeasDim][MeasDim];
  float Pzz_inv[MeasDim][MeasDim];
  float Pxz[ahrs::Kalman::STATE_DIM][MeasDim];
  float K[ahrs::Kalman::STATE_DIM][MeasDim];
  float Kt[MeasDim][ahrs::Kalman::STATE_DIM];
  float y[MeasDim];
  ahrs::Kalman::Error K_dot_y;
  float K_Pzz[ahrs::Kalman::STATE_DIM][MeasDim];
  float K_Pzz_Kt[ahrs::Kalman::STATE_DIM][ahrs::Kalman::STATE_DIM];
};

template <size_t MeasDim>
static inline void _update_generic(
  ahrs::Kalman::State& x,
  float P[ahrs::Kalman::STATE_DIM][ahrs::Kalman::STATE_DIM],
  const float z[MeasDim],
  const float R_diag[MeasDim],
  const ahrs::Kalman::State sigma_points[ahrs::Kalman::SIGMA_POINT_COUNT],
  const ahrs::Kalman::Error sigma_point_errors[ahrs::Kalman::SIGMA_POINT_COUNT],
  const float weights_mean[ahrs::Kalman::SIGMA_POINT_COUNT],
  const float weights_cov[ahrs::Kalman::SIGMA_POINT_COUNT],
  UpdateBuffer<MeasDim>& buf
);

ahrs::Kalman::Kalman(float alpha, float beta, float kappa)
  : magnetic_field_reference{math::Vec3::unitX()},
    _alpha{alpha},
    _beta{beta},
    _lambda_plus_n{alpha * alpha * (STATE_DIM + kappa)},
    _lambda{_lambda_plus_n - STATE_DIM},
    _sqrt_lambda_plus_n{std::sqrt(_lambda_plus_n)}
{
  x.orientation = math::Quat::identity();
  x.position = math::Vec3::zero();
  x.velocity = math::Vec3::zero();

  for (size_t i = 0; i < STATE_DIM; ++i) {
    for (size_t j = 0; j < STATE_DIM; ++j) {
      P[i][j] = (i == j) ? 1.0f : 0.0f;
    }
    Q_diag[i] = 1.0f;
  }

  std::fill(std::begin(R_accel), std::end(R_accel), 1.0f);
  std::fill(std::begin(R_mag), std::end(R_mag), 1.0f);
  std::fill(std::begin(R_gps), std::end(R_gps), 1.0f);
  std::fill(std::begin(R_baro), std::end(R_baro), 1.0f);

  _computeWeights();
}

void ahrs::Kalman::_computeWeights() 
{
  float denom = 1.0f / _lambda_plus_n;

  _weights_mean[0] = _lambda * denom;
  _weights_cov[0]  = _weights_mean[0] + (1.0f - _alpha * _alpha + _beta);

  float half_denom = 0.5f * denom;
  std::fill(&_weights_mean[1], &_weights_mean[SIGMA_POINT_COUNT], half_denom);
  std::fill(&_weights_cov[1], &_weights_cov[SIGMA_POINT_COUNT], half_denom);
}

void ahrs::Kalman::predict(const math::Vec3& gyro, const math::Vec3& accel, float dt) 
{
  // Precompute omega quaternionn
  math::Quat q_omega = math::Quat::fromVector(math::degrees_to_radians(gyro));
  // Precompute dt squared 
  float dt_sq = dt * dt;

  auto f =[&q_omega, &accel, dt, dt_sq](const State& s) -> State {
    constexpr float G = 9.81f;
    // Gravity vector in world frame
    constexpr math::Vec3 g_w{0.0f, 0.0f, G};

    State s_pred;

    // q_k+1 = q_k * quat(0.5 * omega * dt)
    math::Quat q_dot = 0.5f * s.orientation * q_omega;
    s_pred.orientation = (s.orientation + q_dot * dt).normalized();

    // Compute acceleration in world frame
    math::Vec3 a_w = math::rotate_vector(s.orientation, accel * G) - g_w;

    // p_k+1 = p_k + v_k * dt + 0.5 * a_k * dt^2
    s_pred.position = s.position + s.velocity * dt + 0.5f * a_w * dt_sq;

    // v_k+1 = v_k + a_k * dt
    s_pred.velocity = s.velocity + a_w * dt;

    return s_pred;
  };

  _generateSigmaPoints();

  // Propagate each sigma point through the process model
  for (size_t i = 0; i < SIGMA_POINT_COUNT; i++)
    _predicted_sigma_points[i] = f(_sigma_points[i]);

  _predictMean();

  // Compute predicted covariance
  for (size_t i = 0; i < STATE_DIM; i++) {
    for (size_t j = i; j < STATE_DIM; j++) {
      float cov_ij = 0.0f;
      for (size_t k = 0; k < SIGMA_POINT_COUNT; k++) {
        cov_ij += _weights_cov[k] * _predicted_errors[k].data[i] * _predicted_errors[k].data[j];
      }
      P[i][j] = cov_ij;
      P[j][i] = cov_ij; // Symmetric
    }
  }
}

void ahrs::Kalman::_generateSigmaPoints()
{
  // Compute P + Q
  static CCMRAM float P_plus_Q[STATE_DIM][STATE_DIM];
  arm_matrix_instance_f32 P_plus_Q_mat = {STATE_DIM, STATE_DIM, &P_plus_Q[0][0]};
  std::memcpy(P_plus_Q, P, sizeof(P));
  for (size_t i = 0; i < STATE_DIM; i++)
    P_plus_Q[i][i] += Q_diag[i];

  // Compute the square root of (P + Q) using Cholesky decomposition
  static CCMRAM float L[STATE_DIM][STATE_DIM];
  arm_matrix_instance_f32 L_mat = {STATE_DIM, STATE_DIM, &L[0][0]};
  std::memset(L, 0, sizeof(L));
  arm_mat_cholesky_f32(&P_plus_Q_mat, &L_mat);

  // Scale by sqrt(lambda + n)
  arm_mat_scale_f32(&L_mat, _sqrt_lambda_plus_n, &L_mat);

  // First sigma point is current state
  _sigma_points[0] = x;

  // Remaining sigma points
  for (size_t i = 0; i < STATE_DIM; i++) {
    // For quaternion part, handle the rotation
    float angle = std::sqrt(SQ(L[0][i]) + SQ(L[1][i]) + SQ(L[2][i]));
    math::Quat q_delta;
    if (angle > 1e-6f) {
      float half_angle = 0.5f * angle;
      float scale = std::sin(half_angle) / angle;
      q_delta.w = std::cos(half_angle);
      q_delta.x = L[0][i] * scale;
      q_delta.y = L[1][i] * scale;
      q_delta.z = L[2][i] * scale;
    } else {
      /* Use small angle approximation */
      q_delta.w = 1.0f;
      q_delta.x = 0.5f * L[0][i];
      q_delta.y = 0.5f * L[1][i];
      q_delta.z = 0.5f * L[2][i];
    }

    _sigma_points[i + 1].orientation = (q_delta * x.orientation).normalized();
    _sigma_points[i + 1 + STATE_DIM].orientation = (q_delta.conjugate() * x.orientation).normalized();

    // For position and velocity, just add/subtract the column
    for (size_t j = POS_BEGIN_IDX; j < VEL_END_IDX; j++) {
      _sigma_points[i + 1].data[j] = x.data[j] + L[j - 1][i];
      _sigma_points[i + 1 + STATE_DIM].data[j] = x.data[j] - L[j - 1][i];
    }
  }
}

void ahrs::Kalman::_predictMean()
{
  constexpr size_t MAX_ITER = 10;
  constexpr float TOL = 1e-6f;

  // Initialize mean quaternion to first sigma point
  math::Quat q_mean = _predicted_sigma_points[0].orientation;

  for (size_t iter = 0; iter < MAX_ITER; iter++) {
    math::Quat q_mean_conj = q_mean.conjugate();

    // Compute error vectors for each sigma point
    math::Vec3 mean_error = math::Vec3::zero();
    for (size_t i = 0; i < SIGMA_POINT_COUNT; i++) {
      math::Quat q_err = _predicted_sigma_points[i].orientation * q_mean_conj;
      // Ensure shortest path
      if (q_err.w < 0.0f)
        q_err = -q_err;

      math::Vec3 delta_angle = q_err.vectorPart();
      float error_norm = delta_angle.norm();
      if (error_norm > 1e-6f) {
        float angle = 2.0f * std::atan2(error_norm, q_err.w);
        float scale = angle / error_norm;
        _predicted_errors[i].delta_angle = delta_angle * scale;
      } else {
        _predicted_errors[i].delta_angle = 2.0f * delta_angle;
      }

      // Accumulate mean error
      mean_error = mean_error + _weights_mean[i] * _predicted_errors[i].delta_angle;
    }

    // Check for convergence
    float mean_error_norm = mean_error.norm();
    if (mean_error_norm < TOL)
      break;

    // Update mean quaternion
    math::Quat q_update;
    if (mean_error_norm > 1e-6f) {
      float half_angle = 0.5f * mean_error_norm;
      float scale = std::sin(half_angle) / mean_error_norm;
      q_update = math::Quat::fromScalarVector(std::cos(half_angle), mean_error * scale);
    } else {
      // Small angle approximation
      q_update = math::Quat::fromScalarVector(1.0f, 0.5f * mean_error);
    }

    // Force shortest path
    if (q_update.w < 0.0f)
      q_update = -q_update;

    // Update mean quaternion
    q_mean = (q_update * q_mean).normalized();
  }

  // Set predicted mean state
  x.orientation = q_mean;

  // Compute mean position and velocity
  for (size_t i = POS_BEGIN_IDX; i < VEL_END_IDX; i++) {
    x.data[i] = 0.0f;
    for (size_t j = 0; j < SIGMA_POINT_COUNT; j++) {
      x.data[i] += _weights_mean[j] * _predicted_sigma_points[j].data[i];
    }
  }

  // Compute position and velocity error parts
  for (size_t i = 0; i < SIGMA_POINT_COUNT; i++) {
    for (size_t j = POS_BEGIN_IDX; j < VEL_END_IDX; j++) {
      _predicted_errors[i].data[j - 1] = _predicted_sigma_points[i].data[j] - x.data[j];
    }
  }
}

void ahrs::Kalman::updateAccel(const math::Vec3& accel) 
{
  static CCMRAM UpdateBuffer<ACCEL_MEAS_DIM> buf;

  auto h = [](const State& s, float z[ACCEL_MEAS_DIM]) {
    // Gravity vector in world frame
    constexpr math::Vec3 g_w{0.0f, 0.0f, 1.0f};

    // Rotate gravity into body frame
    math::Vec3 g_b = math::rotate_vector(s.orientation.conjugate(), g_w);

    z[0] = g_b.x;
    z[1] = g_b.y;
    z[2] = g_b.z;
  };

  float norm = accel.norm();
  if (std::abs(norm - 1.0f) > 0.1f)
    return; // Ignore invalid accel measurement

  float normalized_accel[ACCEL_MEAS_DIM] = {
    accel.x / norm,
    accel.y / norm,
    accel.z / norm
  };

  // Transform sigma points into measurement space
  for (size_t i = 0; i < ahrs::Kalman::SIGMA_POINT_COUNT; i++)
    h(_predicted_sigma_points[i], buf.meas_sigma_points[i]);

  _update_generic<ACCEL_MEAS_DIM>(
    x,
    P,
    normalized_accel,
    R_accel,
    _predicted_sigma_points,
    _predicted_errors,
    _weights_mean,
    _weights_cov,
    buf
  );
}

void ahrs::Kalman::updateMag(const math::Vec3& mag) 
{
  static CCMRAM UpdateBuffer<MAG_MEAS_DIM> buf;

  auto h = [this](const State& s, float z[MAG_MEAS_DIM]) {
    // Rotate magnetic field into body frame
    math::Vec3 m_b = math::rotate_vector(s.orientation.conjugate(), this->magnetic_field_reference);

    z[0] = m_b.x;
    z[1] = m_b.y;
    z[2] = m_b.z;
  };

  float norm = mag.norm();
  if (std::abs(norm - magnetic_field_reference.norm()) > 3.0f)
    return; // Ignore invalid mag measurement

  float normalized_mag[MAG_MEAS_DIM] = {
    mag.x / norm,
    mag.y / norm,
    mag.z / norm
  };

  // Transform sigma points into measurement space
  for (size_t i = 0; i < ahrs::Kalman::SIGMA_POINT_COUNT; i++)
    h(_predicted_sigma_points[i], buf.meas_sigma_points[i]);

  _update_generic<MAG_MEAS_DIM>(
    x,
    P,
    normalized_mag,
    R_mag,
    _predicted_sigma_points,
    _predicted_errors,
    _weights_mean,
    _weights_cov,
    buf
  );
}

void ahrs::Kalman::updateGPS(const math::Vec3& gps_pos, const math::Vec3& gps_vel) 
{
  static CCMRAM UpdateBuffer<GPS_POS_MEAS_DIM + GPS_VEL_MEAS_DIM> buf;

  auto h = [](const State& s, float z[GPS_POS_MEAS_DIM + GPS_VEL_MEAS_DIM]) {
    z[0] = s.position.x;
    z[1] = s.position.y;
    z[2] = s.position.z;

    z[3] = s.velocity.x;
    z[4] = s.velocity.y;
    z[5] = s.velocity.z;
  };

  static CCMRAM float gps_data[GPS_POS_MEAS_DIM + GPS_VEL_MEAS_DIM];
  std::memcpy(gps_data, reinterpret_cast<const float *>(&gps_pos), sizeof(gps_pos));
  std::memcpy(gps_data + GPS_POS_MEAS_DIM, reinterpret_cast<const float *>(&gps_vel), sizeof(gps_vel));

  // Transform sigma points into measurement space
  for (size_t i = 0; i < ahrs::Kalman::SIGMA_POINT_COUNT; i++)
    h(_predicted_sigma_points[i], buf.meas_sigma_points[i]);

  _update_generic<GPS_POS_MEAS_DIM + GPS_VEL_MEAS_DIM>(
    x, 
    P, 
    gps_data, 
    R_gps, 
    _predicted_sigma_points, 
    _predicted_errors, 
    _weights_mean, 
    _weights_cov, 
    buf
  );
}

void ahrs::Kalman::updateBaro(float altitude) 
{
  static CCMRAM UpdateBuffer<BARO_MEAS_DIM> buf;

  auto h = [](const State& s, float z[BARO_MEAS_DIM]) {
    z[0] = s.position.z;
  };

  // Transform sigma points into measurement space
  for (size_t i = 0; i < ahrs::Kalman::SIGMA_POINT_COUNT; i++)
    h(_predicted_sigma_points[i], buf.meas_sigma_points[i]);

  _update_generic<BARO_MEAS_DIM>(
    x,
    P,
    &altitude,
    R_baro,
    _predicted_sigma_points,
    _predicted_errors,
    _weights_mean,
    _weights_cov,
    buf
  );
}

template <size_t MeasDim>
static inline void _update_generic(
  ahrs::Kalman::State& x,
  float P[ahrs::Kalman::STATE_DIM][ahrs::Kalman::STATE_DIM],
  float const z[MeasDim],
  float const R_diag[MeasDim],
  ahrs::Kalman::State const sigma_points[ahrs::Kalman::SIGMA_POINT_COUNT],
  ahrs::Kalman::Error const sigma_point_errors[ahrs::Kalman::SIGMA_POINT_COUNT],
  float const weights_mean[ahrs::Kalman::SIGMA_POINT_COUNT],
  float const weights_cov[ahrs::Kalman::SIGMA_POINT_COUNT],
  UpdateBuffer<MeasDim>& buf
) 
{
  // Compute predicted measurement mean
  std::memset(buf.z_pred, 0, sizeof(buf.z_pred));
  for (size_t i = 0; i < ahrs::Kalman::SIGMA_POINT_COUNT; i++) {
    for (size_t j = 0; j < MeasDim; j++)
      buf.z_pred[j] += weights_mean[i] * buf.meas_sigma_points[i][j];
  }

  // Compute measurement error vectors
  for (size_t i = 0; i < ahrs::Kalman::SIGMA_POINT_COUNT; i++) {
    for (size_t j = 0; j < MeasDim; j++)
      buf.z_err[i][j] = buf.meas_sigma_points[i][j] - buf.z_pred[j];
  }

  // Compute innovation covariance Pzz
  for (size_t i = 0; i < MeasDim; i++) {
    for (size_t j = i; j < MeasDim; j++) {
      float cov_ij = R_diag[i] * (i == j);
      for (size_t k = 0; k < ahrs::Kalman::SIGMA_POINT_COUNT; k++)
        cov_ij += weights_cov[k] * buf.z_err[k][i] * buf.z_err[k][j];
      buf.Pzz[i][j] = cov_ij;
      buf.Pzz[j][i] = cov_ij; // Symmetric
    }
  }

  // Compute cross-covariance Pxz
  for (size_t i = 0; i < ahrs::Kalman::STATE_DIM; i++) {
    for (size_t j = 0; j < MeasDim; j++) {
      float cov_ij = 0.0f;
      for (size_t k = 0; k < ahrs::Kalman::SIGMA_POINT_COUNT; k++)
        cov_ij += weights_cov[k] * sigma_point_errors[k].data[i] * buf.z_err[k][j];
      buf.Pxz[i][j] = cov_ij;
    }
  }

  // Invert Pzz
  arm_matrix_instance_f32 Pzz_mat = {MeasDim, MeasDim, &buf.Pzz[0][0]};
  arm_matrix_instance_f32 Pzz_inv_mat = {MeasDim, MeasDim, &buf.Pzz_inv[0][0]};
  arm_mat_inverse_f32(&Pzz_mat, &Pzz_inv_mat);

  // Compute Kalman gain K = Pxz * Pzz_inv
  arm_matrix_instance_f32 Pxz_mat = {ahrs::Kalman::STATE_DIM, MeasDim, &buf.Pxz[0][0]};
  arm_matrix_instance_f32 K_mat = {ahrs::Kalman::STATE_DIM, MeasDim, &buf.K[0][0]};
  arm_mat_mult_f32(&Pxz_mat, &Pzz_inv_mat, &K_mat);

  // Compute innovation y = z - z_pred
  arm_sub_f32(z, buf.z_pred, buf.y, MeasDim);

  // Compute correction K * y
  arm_mat_vec_mult_f32(&K_mat, buf.y, buf.K_dot_y.data);

  // Update quaternion part
  float angle = buf.K_dot_y.delta_angle.norm();
  math::Quat q_update;
  if (angle > 1e-6f) {
    float half_angle = 0.5f * angle;
    float scale = std::sin(half_angle) / angle;
    q_update = math::Quat::fromScalarVector(std::cos(half_angle), buf.K_dot_y.delta_angle * scale);
  } else {
    // Small angle approximation
    q_update = math::Quat::fromScalarVector(1.0f, 0.5f * buf.K_dot_y.delta_angle);
  }

  // Force shortest path
  if (q_update.w < 0.0f)
    q_update = -q_update;

  x.orientation = (q_update * x.orientation).normalized();

  // Update position and velocity parts
  for (size_t i = ahrs::Kalman::POS_BEGIN_IDX; i < ahrs::Kalman::VEL_END_IDX; i++)
    x.data[i] += buf.K_dot_y.data[i - 1];

  // Compute transpose of K
  arm_matrix_instance_f32 Kt_mat = {MeasDim, ahrs::Kalman::STATE_DIM, &buf.Kt[0][0]};
  arm_mat_trans_f32(&K_mat, &Kt_mat);

  // Update covariance P = P - K * Pzz * K^T
  arm_matrix_instance_f32 K_Pzz_mat = {ahrs::Kalman::STATE_DIM, MeasDim, &buf.K_Pzz[0][0]};
  arm_mat_mult_f32(&K_mat, &Pzz_mat, &K_Pzz_mat);

  arm_matrix_instance_f32 K_Pzz_Kt_mat = {ahrs::Kalman::STATE_DIM, ahrs::Kalman::STATE_DIM, &buf.K_Pzz_Kt[0][0]};
  arm_mat_mult_f32(&K_Pzz_mat, &Kt_mat, &K_Pzz_Kt_mat);

  for (size_t i = 0; i < ahrs::Kalman::STATE_DIM; i++) {
    for (size_t j = i; j < ahrs::Kalman::STATE_DIM; j++) {
      P[i][j] -= buf.K_Pzz_Kt[i][j];
      P[j][i] = P[i][j]; // Symmetric
    }
  }
}

