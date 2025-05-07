/**
 * @file nmea_defs.h 
 * @brief This file contains all the definitions and structures used in the NMEA protocol.
 * 
 * @author Mattia Gregnanin
 * @date 31-03-2025
 */

#ifndef __NMEA_DEFS_H
#define __NMEA_DEFS_H

#include "main.h"

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Flags representing the validity of a NMEA message.
 */
typedef enum {
    /**
     * @brief None of the checks is satisfied.
     */
    NMEA_BAD                    = 0,
    /**
     * @brief Message begins with the dollar sign '$'.
     */
    NMEA_HAS_DOLLAR             = 1 << 0,
    /**
     * @brief Message has a valid checksum.
     */
    NMEA_HAS_CHECKSUM           = 1 << 1,
    /**
     * @brief Message has a known talker (i.e. 'GL').
     */
    NMEA_HAS_TALKER             = 1 << 2,
    /**
     * @brief Message is a know sentence (i.e. 'GGA').
     */
    NMEA_HAS_SENTENCE           = 1 << 3,
    /**
     * @brief Message is a known and parseable sentence.
     */
    NMEA_HAS_PARSEABLE_SENTENCE = 1 << 4
} NMEA_Check_Flag_t;


/**
 * @brief NMEA message talker type.
 */
typedef enum {
    /**
     * @brief Invalid or unknown talker.
     */
    NMEA_TALKER_INVALID = -1,
    /**
     * @brief Proprietary talker.
     */
    NMEA_TALKER_P,
    /**
     * @brief GPS.
     */
    NMEA_TALKER_GP,
    /**
     * @brief Generic GNSS or combination.
     */
    NMEA_TALKER_GN,
    /**
     * @brief Weather instrument.
     */
    NMEA_TALKER_WI,
} NMEA_Talker_Type_t;


/**
 * @brief Struct representing a known talker. The talker is who sent the message.
 */
typedef struct {
    /**
     * @brief Token to recognize the talker in the message.
     */
    const char *token;
    /**
     * @brief Length of the token (2 or 3 bytes).
     */
    uint8_t length;
    /**
     * @brief Type of the token.
     */
    NMEA_Talker_Type_t type;
} NMEA_Talker_t;


/**
 * @brief Type of NMEA message.
 */
typedef enum {
    /**
     * @brief Invalid or unknown message.
     */
    NMEA_SENTENCE_INVALID = -1,
    /**
     * @brief Time, position and fix related data.
     */
    NMEA_SENTENCE_GGA,
    /**
     * @brief Position data.
     */
    NMEA_SENTENCE_GLL,
    /**
     * @brief GPS DOP and active satellites.
     */
    NMEA_SENTENCE_GSA,
    /**
     * @brief Position, velocity and time.
     */
    NMEA_SENTENCE_RMC,
} NMEA_Sentence_Type_t;


/**
 * @brief Struct representing a known sentence.
 */
typedef struct {
    /**
     * @brief Token to recognize the sentence in the message.
     */
    const char *token;
    /**
     * @brief Type of sentence.
     */
    NMEA_Sentence_Type_t type;
} NMEA_Sentence_t;


/**
 * @brief Return type of the `NMEA_Check_Msg` function.
 */
typedef struct {
    /**
     * @brief Message flag.
     */
    NMEA_Check_Flag_t flag;
    /**
     * @brief Message sender.
     */
    NMEA_Talker_t talker;
    /**
     * @brief Message sentence type.
     */
    NMEA_Sentence_t sentence;
} NMEA_Check_Result_t;


/**
 * @brief Struct containing current UTC time and date data.
 */
typedef struct {
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    uint16_t milliseconds;
    uint8_t year;
    uint8_t month;
    uint8_t day;
} NMEA_Time_t;


/**
 * @brief Struct containing coordinate data.
 */
typedef struct {
    /**
     * @brief Floating point coordinate value in degrees/minutes as received from the GPS (DDDMM.MMMM).
     */
    float degrees_minutes;
    /**
     * @brief Floating point coordinate value in decimal degrees.
     */
    float degrees;
    /**
     * @brief Direction of the coordinate (i.e. N, S, E, W).
     */
    char direction;
} NMEA_Coordinate_t;


/**
 * @brief Fix quality of the GPS signal.
 */
typedef enum {
    NMEA_NO_FIX           = 0,
    NMEA_FIX              = 1,
    NMEA_DIFFERENTIAL_FIX = 2
} NMEA_Fix_Quality_t;

/**
 * @brief Struct containing fix quality and status.
 */
typedef struct {
    bool fix;
    NMEA_Fix_Quality_t quality;
} NMEA_Fix_t;

/**
 * @brief Struct containing the status of the GPS module.
 */
typedef struct {
    NMEA_Check_Result_t check;

    /**
     * @brief UTC time of position fix.
     */
    NMEA_Time_t time;

    /**
     * @brief Latitude of position fix.
     */
    NMEA_Coordinate_t latitude;

    /// @brief Longitude of position fix.
    NMEA_Coordinate_t longitude;

    /**
     * @brief Speed over Ground.  
     */ 
    float speed;
    /**
     * @brief Angle (in degrees) of flight path relative to Geographic (true) North, measured clockwise.
     */
    float track_angle;

    /**
     * @brief Informations relative to fix.
     */
    NMEA_Fix_t fix;
    /**
     * @brief Number of satellites used in the position fix.
     */
    uint8_t satellites;
    
    /**
     * @brief Horizontal Diluition of Position.
     * @note HDOP is a measure of the accuracy of the horizontal position.
     */
    float HDOP;
    /**
     * @brief Height from geoid surface.
     * @note Orthometric height is the height of a point above the geoid, which is a model of the Earth's mean sea level.
     */
    float orthometric_height;
    /**
     * @brief Difference between ellipsoid and geoid heights.
     */
    float geoid_height;
} NMEA_Status_t;

#ifdef __cplusplus
}
#endif

#endif