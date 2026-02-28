#ifndef __GEO_CONSTANTS_HH__
#define __GEO_CONSTANTS_HH__

namespace geo {

  /**
   * @brief Earth's semi-major axis in meters.
   */
  constexpr float EARTH_A = 6378137.0f;

  /**
   * @brief Earth's semi-minor axis in meters.
   */
  constexpr float EARTH_B = 6356752.3142f;

  /**
   * @brief Earth's mean radius in meters.
   */
  constexpr float EARTH_MEAN_RADIUS = 6371200.0f;

  /**
   * @brief Earth's flattening factor.
   */
  constexpr float EARTH_F = 1.0f / 298.257223563f;

  /**
   * @brief Square of Earth's eccentricity.
   */
  constexpr float EARTH_E_SQ = EARTH_F * (2.0f - EARTH_F);

}

#endif // __GEO_CONSTANTS_HH__
