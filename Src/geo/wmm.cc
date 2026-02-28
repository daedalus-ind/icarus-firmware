/**
 ******************************************************************************
 * @file    wmm.cc
 * @brief   This file provides the implementation of the World Magnetic Model
 *          (WMM) class to compute the Earth's magnetic field vector.
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
#include "geo/wmm.hh"
#include "math/angles.hh"
#include <cmath>

float geo::WMM::c[SIZE][SIZE];
float geo::WMM::cd[SIZE][SIZE];
float geo::WMM::snorm[SIZE * SIZE];
float geo::WMM::k[SIZE][SIZE];
float geo::WMM::fn[SIZE];
float geo::WMM::fm[SIZE];

geo::WMM::Coefficient geo::WMM::coeffs[geo::WMM::COEFF_COUNT]
{
  Coefficient{-29351.8f,     0.0f,    12.0f,     0.0f},
  Coefficient{ -1410.8f,  4545.4f,     9.7f,   -21.5f},
  Coefficient{ -2556.6f,     0.0f,   -11.6f,     0.0f},
  Coefficient{  2951.1f, -3133.6f,    -5.2f,   -27.7f},
  Coefficient{  1649.3f,  -815.1f,    -8.0f,   -12.1f},
  Coefficient{  1361.0f,     0.0f,    -1.3f,     0.0f},
  Coefficient{ -2404.1f,   -56.6f,    -4.2f,     4.0f},
  Coefficient{  1243.8f,   237.5f,     0.4f,    -0.3f},
  Coefficient{   453.6f,  -549.5f,   -15.6f,    -4.1f},
  Coefficient{   895.0f,     0.0f,    -1.6f,     0.0f},
  Coefficient{   799.5f,   278.6f,    -2.4f,    -1.1f},
  Coefficient{    55.7f,  -133.9f,    -6.0f,     4.1f},
  Coefficient{  -281.1f,   212.0f,     5.6f,     1.6f},
  Coefficient{    12.1f,  -375.6f,    -7.0f,    -4.4f},
  Coefficient{  -233.2f,     0.0f,     0.6f,     0.0f},
  Coefficient{   368.9f,    45.4f,     1.4f,    -0.5f},
  Coefficient{   187.2f,   220.2f,     0.0f,     2.2f},
  Coefficient{  -138.7f,  -122.9f,     0.6f,     0.4f},
  Coefficient{  -142.0f,    43.0f,     2.2f,     1.7f},
  Coefficient{    20.9f,   106.1f,     0.9f,     1.9f},
  Coefficient{    64.4f,     0.0f,    -0.2f,     0.0f},
  Coefficient{    63.8f,   -18.4f,    -0.4f,     0.3f},
  Coefficient{    76.9f,    16.8f,     0.9f,    -1.6f},
  Coefficient{  -115.7f,    48.8f,     1.2f,    -0.4f},
  Coefficient{   -40.9f,   -59.8f,    -0.9f,     0.9f},
  Coefficient{    14.9f,    10.9f,     0.3f,     0.7f},
  Coefficient{   -60.7f,    72.7f,     0.9f,     0.9f},
  Coefficient{    79.5f,     0.0f,     0.0f,     0.0f},
  Coefficient{   -77.0f,   -48.9f,    -0.1f,     0.6f},
  Coefficient{    -8.8f,   -14.4f,    -0.1f,     0.5f},
  Coefficient{    59.3f,    -1.0f,     0.5f,    -0.8f},
  Coefficient{    15.8f,    23.4f,    -0.1f,     0.0f},
  Coefficient{     2.5f,    -7.4f,    -0.8f,    -1.0f},
  Coefficient{   -11.1f,   -25.1f,    -0.8f,     0.6f},
  Coefficient{    14.2f,    -2.3f,     0.8f,    -0.2f},
  Coefficient{    23.2f,     0.0f,    -0.1f,     0.0f},
  Coefficient{    10.8f,     7.1f,     0.2f,    -0.2f},
  Coefficient{   -17.5f,   -12.6f,     0.0f,     0.5f},
  Coefficient{     2.0f,    11.4f,     0.5f,    -0.4f},
  Coefficient{   -21.7f,    -9.7f,    -0.1f,     0.4f},
  Coefficient{    16.9f,    12.7f,     0.3f,    -0.5f},
  Coefficient{    15.0f,     0.7f,     0.2f,    -0.6f},
  Coefficient{   -16.8f,    -5.2f,     0.0f,     0.3f},
  Coefficient{     0.9f,     3.9f,     0.2f,     0.2f},
  Coefficient{     4.6f,     0.0f,     0.0f,     0.0f},
  Coefficient{     7.8f,   -24.8f,    -0.1f,    -0.3f},
  Coefficient{     3.0f,    12.2f,     0.1f,     0.3f},
  Coefficient{    -0.2f,     8.3f,     0.3f,    -0.3f},
  Coefficient{    -2.5f,    -3.3f,    -0.3f,     0.3f},
  Coefficient{   -13.1f,    -5.2f,     0.0f,     0.2f},
  Coefficient{     2.4f,     7.2f,     0.3f,    -0.1f},
  Coefficient{     8.6f,    -0.6f,    -0.1f,    -0.2f},
  Coefficient{    -8.7f,     0.8f,     0.1f,     0.4f},
  Coefficient{   -12.9f,    10.0f,    -0.1f,     0.1f},
  Coefficient{    -1.3f,     0.0f,     0.1f,     0.0f},
  Coefficient{    -6.4f,     3.3f,     0.0f,     0.0f},
  Coefficient{     0.2f,     0.0f,     0.1f,     0.0f},
  Coefficient{     2.0f,     2.4f,     0.1f,    -0.2f},
  Coefficient{    -1.0f,     5.3f,     0.0f,     0.1f},
  Coefficient{    -0.6f,    -9.1f,    -0.3f,    -0.1f},
  Coefficient{    -0.9f,     0.4f,     0.0f,     0.1f},
  Coefficient{     1.5f,    -4.2f,    -0.1f,     0.0f},
  Coefficient{     0.9f,    -3.8f,    -0.1f,    -0.1f},
  Coefficient{    -2.7f,     0.9f,     0.0f,     0.2f},
  Coefficient{    -3.9f,    -9.1f,     0.0f,     0.0f},
  Coefficient{     2.9f,     0.0f,     0.0f,     0.0f},
  Coefficient{    -1.5f,     0.0f,     0.0f,     0.0f},
  Coefficient{    -2.5f,     2.9f,     0.0f,     0.1f},
  Coefficient{     2.4f,    -0.6f,     0.0f,     0.0f},
  Coefficient{    -0.6f,     0.2f,     0.0f,     0.1f},
  Coefficient{    -0.1f,     0.5f,    -0.1f,     0.0f},
  Coefficient{    -0.6f,    -0.3f,     0.0f,     0.0f},
  Coefficient{    -0.1f,    -1.2f,     0.0f,     0.1f},
  Coefficient{     1.1f,    -1.7f,    -0.1f,     0.0f},
  Coefficient{    -1.0f,    -2.9f,    -0.1f,     0.0f},
  Coefficient{    -0.2f,    -1.8f,    -0.1f,     0.0f},
  Coefficient{     2.6f,    -2.3f,    -0.1f,     0.0f},
  Coefficient{    -2.0f,     0.0f,     0.0f,     0.0f},
  Coefficient{    -0.2f,    -1.3f,     0.0f,     0.0f},
  Coefficient{     0.3f,     0.7f,     0.0f,     0.0f},
  Coefficient{     1.2f,     1.0f,     0.0f,    -0.1f},
  Coefficient{    -1.3f,    -1.4f,     0.0f,     0.1f},
  Coefficient{     0.6f,     0.0f,     0.0f,     0.0f},
  Coefficient{     0.6f,     0.6f,     0.1f,     0.0f},
  Coefficient{     0.5f,    -0.1f,     0.0f,     0.0f},
  Coefficient{    -0.1f,     0.8f,     0.0f,     0.0f},
  Coefficient{    -0.4f,     0.1f,     0.0f,     0.0f},
  Coefficient{    -0.2f,    -1.0f,    -0.1f,     0.0f},
  Coefficient{    -1.3f,     0.1f,     0.0f,     0.0f},
  Coefficient{    -0.7f,     0.2f,    -0.1f,    -0.1f}
};

void geo::WMM::initCoefficients()
{
  c[0][0] = 0.0f;
  cd[0][0] = 0.0f;
  size_t index = 0;
  // Write the coefficients into the c and cd arrays
  for (size_t n = 1; n <= MAX_DEGREE; ++n) {
    for (size_t m = 0; m <= n; ++m) {
      float gnm = coeffs[index].g;
      float hnm = coeffs[index].h;
      float dgnm = coeffs[index].dg;
      float dhnm = coeffs[index].dh;
      ++index;

      c[m][n] = gnm;
      cd[m][n] = dgnm;
      if (m != 0) {
        c[n][m - 1] = hnm;
        cd[n][m - 1] = dhnm;
      }
    }
  }

  // Convert Schmidt normalized Gauss coefficients to unnormalized
  snorm[0] = 1.0f;
  fm[0] = 0.0f;
  for (size_t n = 1; n <= MAX_DEGREE; ++n) {
    snorm[n] = snorm[n - 1] * (2.0f * n - 1.0f) / static_cast<float>(n);
    size_t jfactor = 2;
    for (size_t m = 0; m <= n; ++m) {
      k[m][n] = static_cast<float>(((n - 1) * (n - 1)) - (m * m)) / static_cast<float>((2 * n - 1) * (2 * n - 3));
      if (m > 0) {
        float flnmj = static_cast<float>((n - m + 1) * jfactor) / static_cast<float>(n + m);
        snorm[n + m * SIZE] = snorm[n + (m - 1) * SIZE] * std::sqrt(flnmj);
        jfactor = 1;
        c[n][m - 1] = snorm[n + m * SIZE] * c[n][m - 1];
        cd[n][m - 1] = snorm[n + m * SIZE] * cd[n][m - 1];
      }
      c[m][n] = snorm[n + m * SIZE] * c[m][n];
      cd[m][n] = snorm[n + m * SIZE] * cd[m][n];
    }
    fn[n] = static_cast<float>(n + 1);
    fm[n] = static_cast<float>(n);
  }
  k[1][1] = 0.0f;
}

math::Vec3 geo::WMM::computeMagneticField(const Geodetic& position, const Date& date)
{
  float lat = math::degrees_to_radians(position.latitude);
  float lon = math::degrees_to_radians(position.longitude);
  float alt = position.altitude / 1000.0f;
  float dtime = date.toDecimalYear() - EPOCH;

  // Precompute trigonometric functions
  float sin_lat = std::sin(lat);
  float cos_lat = std::cos(lat);
  float sin_lon = std::sin(lon);
  float cos_lon = std::cos(lon);
  float sin2_lat = sin_lat * sin_lat;
  float cos2_lat = cos_lat * cos_lat;

  // Convert geodetic to spherical coordinates
  static constexpr float A2 = A * A;
  static constexpr float B2 = B * B;
  static constexpr float C2 = A2 - B2;
  static constexpr float A4 = A2 * A2;
  static constexpr float B4 = B2 * B2;
  static constexpr float C4 = A4 - B4;

  float q = std::sqrt(A2 - C2 * sin2_lat);
  float q1 = alt * q;
  float q2 = ((q1 + A2) / (q1 + B2)) * ((q1 + A2) / (q1 + B2));
  float ct = sin_lat / std::sqrt(q2 * cos2_lat + sin2_lat);
  float st = std::sqrt(1.0f - ct * ct);
  float r = std::sqrt(alt * alt + 2.0f * q1 + (A4 - C4 * sin2_lat) / (q * q));
  float d = std::sqrt(A2 * cos2_lat + B2 * sin2_lat);
  float ca = (alt + d) / r;
  float sa = (C2 * cos_lat * sin_lat) / (r * d);

  // Precompute sin(m*lon) and cos(m*lon)
  static CCMRAM float sp[SIZE], cp[SIZE];
  sp[0] = 0.0f;
  cp[0] = 1.0f;
  sp[1] = sin_lon;
  cp[1] = cos_lon;
  for (size_t m = 2; m <= MAX_DEGREE; ++m) {
    sp[m] = sp[1] * cp[m - 1] + cp[1] * sp[m - 1];
    cp[m] = cp[1] * cp[m - 1] - sp[1] * sp[m - 1];
  }

  static CCMRAM float tc[SIZE][SIZE];
  static CCMRAM float dp[SIZE][SIZE];
  static CCMRAM float pp[SIZE];
  dp[0][0] = 0.0f;
  pp[0] = 0.0f;

  float aor = RE / r;
  float ar = aor * aor;
  // Initialize variables for magnetic field components
  float Br = 0.0f;
  float Bt = 0.0f;
  float Bp = 0.0f;
  float Bpp = 0.0f;

  for (size_t n = 1; n <= MAX_DEGREE; ++n) {
    ar *= aor;
    for (size_t m = 0; m <= n; ++m) {
      // Compute unnormalized associated Legendre polynomials and derivatives via recursion relations
      if (n == m) {
        snorm[n + m * SIZE] = st * snorm[n - 1 + (m - 1) * SIZE];
        dp[m][n] = st * dp[m - 1][n - 1] + ct * snorm[n - 1 + (m - 1) * SIZE];
      } else if (n == 1 && m == 0) {
        snorm[n + m * SIZE] = ct * snorm[n - 1 + m * SIZE];
        dp[m][n] = ct * dp[m][n - 1] - st * snorm[n - 1 + m * SIZE];
      } else if (n > 1 && n != m) {
        if (m > n - 2) {
          snorm[n - 2 + m * SIZE] = 0.0f;
          dp[m][n - 2] = 0.0f;
        }
        snorm[n + m * SIZE] = ct * snorm[n - 1 + m * SIZE] - k[m][n] * snorm[n - 2 + m * SIZE];
        dp[m][n] = ct * dp[m][n - 1] - st * snorm[n - 1 + m * SIZE] - k[m][n] * dp[m][n - 2];
      }

      // Time adjust the Gauss coefficients
      tc[m][n] = c[m][n] + dtime * cd[m][n];
      if (m != 0) {
        tc[n][m - 1] = c[n][m - 1] + dtime * cd[n][m - 1];
      }

      // Accumulate the magnetic field components
      float par = ar * snorm[n + m * SIZE];
      float temp1, temp2;
      if (m == 0) {
        temp1 = tc[m][n] * cp[m];
        temp2 = tc[m][n] * sp[m];
      } else {
        temp1 = tc[m][n] * cp[m] + tc[n][m - 1] * sp[m];
        temp2 = tc[m][n] * sp[m] - tc[n][m - 1] * cp[m];
      }

      Bt -= ar * temp1 * dp[m][n];
      Bp += fm[m] * temp2 * par;
      Br += fn[n] * temp1 * par;

      // Special case: north/south geographic poles
      if (st == 0.0f && m == 1) {
        if (n == 1) {
          pp[n] = pp[n - 1];
        } else {
          pp[n] = ct * pp[n - 1] - k[m][n] * pp[n - 2];
        }
        Bpp += fm[m] * temp2 * ar * pp[n];
      }
    }
  }

  if (st == 0.0f) {
    Bp = Bpp;
  } else {
    Bp /= st;
  }

  // Rotate magnetic vector components from spherical to geodetic ENU coordinates
  float Be = Bp;
  float Bn = -Bt * ca - Br * sa;
  float Bu = -Bt * sa + Br * ca;
  // Convert from nT to uT
  return math::Vec3{Be * 0.001f, Bn * 0.001f, Bu * 0.001f};
}
