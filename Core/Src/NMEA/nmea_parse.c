/**
 * @file nmea_parse.c 
 * @brief This file contains the function definitions for parsing NMEA messages.
 * 
 * @author Mattia Gregnanin
 * @date 01-04-2025
 */

#include "NMEA/nmea.h"

#include <stdlib.h>
#include <string.h>

#define ADVANCE(p) p = strchr(p, ',') + 1

static inline void parse_GGA(NMEA_Status_t *stat, const char *data);
static inline void parse_RMC(NMEA_Status_t *status, const char *data);
static inline void parse_GLL(NMEA_Status_t *status, const char *data);

static inline bool is_empty(const char *s);
static inline const void* min(const void *x, const void *y);

void NMEA_Parse_Msg(NMEA_Status_t *stat, const char *nmea_msg) {
    // Check if the message is parseable
    NMEA_Check_Msg(&stat->check, nmea_msg);
    if (!(stat->check.flag & NMEA_HAS_PARSEABLE_SENTENCE)) return;

    // Data is after the first comma
    const char *data = strchr(nmea_msg, ',') + 1;

    switch (stat->check.sentence.type) {
        case NMEA_SENTENCE_GGA:
        parse_GGA(stat, data);
        break;

        case NMEA_SENTENCE_RMC:
        parse_RMC(stat, data);
        break;

        case NMEA_SENTENCE_GLL: 
        parse_GLL(stat, data);
        break;

        default:
        break;
    }
}

/**
 * @brief GGA sentences contain time, position and fix related data.
 */
static inline void parse_GGA(NMEA_Status_t *stat, const char *data) {
    // Parse time
    NMEA_Parse_Time(&stat->time, data); 
    ADVANCE(data);
    // Parse latitude
    NMEA_Parse_Coordinate(&stat->latitude, data); 
    ADVANCE(data);
    ADVANCE(data);
    // Parse longitude
    NMEA_Parse_Coordinate(&stat->longitude, data); 
    ADVANCE(data);
    ADVANCE(data);
    // Parse fix
    NMEA_Parse_Fix(&stat->fix, data); 
    ADVANCE(data);
    // Parse satellites data
    if (!is_empty(data)) { 
        stat->satellites = atoi(data);
    }
    ADVANCE(data);
    // Parse HDOP
    if (!is_empty(data)) { 
        stat->HDOP = atof(data);
    }
    ADVANCE(data);
    // Parse height
    if (!is_empty(data)) { 
        stat->orthometric_height = atof(data);
    }
    ADVANCE(data);
    ADVANCE(data); // Skip measure unit
    // Parse geoid-ellipsoid height
    if (!is_empty(data)) { 
        stat->geoid_height = atof(data);
    }
}

static inline void parse_RMC(NMEA_Status_t *stat, const char *data) {
    // Parse time
    NMEA_Parse_Time(&stat->time, data); 
    ADVANCE(data);
    // Parse fix validity
    NMEA_Parse_Fix_Validity(&stat->fix, data); 
    ADVANCE(data);
    // Parse latitude
    NMEA_Parse_Coordinate(&stat->latitude, data); 
    ADVANCE(data);
    ADVANCE(data);
    // Parse longitude
    NMEA_Parse_Coordinate(&stat->longitude, data); 
    ADVANCE(data);
    ADVANCE(data);
    // Parse SOD
    if (!is_empty(data)) { 
        stat->speed = atof(data);
    }
    ADVANCE(data);
    // Parse track angle
    if (!is_empty(data)) { 
        stat->track_angle = atof(data);
    }
    ADVANCE(data);
    // Parse date 
    NMEA_Parse_Date(&stat->time, data);     

    // Magnetic Variation 
    // ...

    // Fix mode
    // ...
}

static inline void parse_GLL(NMEA_Status_t *stat, const char *data) {
    // Parse latitude
    NMEA_Parse_Coordinate(&stat->latitude, data); 
    ADVANCE(data);
    ADVANCE(data);
    // Parse longitude
    NMEA_Parse_Coordinate(&stat->longitude, data); 
    ADVANCE(data);
    ADVANCE(data);
    // Parse time
    NMEA_Parse_Time(&stat->time, data); 
    ADVANCE(data);
    // Parse fix validity
    NMEA_Parse_Fix_Validity(&stat->fix, data); 
}

void NMEA_Parse_Time(NMEA_Time_t *time, const char *time_str) {
    if (is_empty(time_str)) return;

    uint32_t t = atol(time_str);
    time->hours = t / 10000;
    time->minutes = (t % 10000) / 100;
    time->seconds = (t % 100);

    const char *decimal = strchr(time_str, '.');
    const char *comma_or_star = min(strchr(time_str, '*'), strchr(time_str, ','));

    if (decimal && comma_or_star && decimal < comma_or_star) {
        time->milliseconds = atof(decimal) * 1000;
    } else {
        time->milliseconds = 0;
    }
}

void NMEA_Parse_Coordinate(NMEA_Coordinate_t *nmea_coord, const char *coord_str) {
    if (is_empty(coord_str)) return;

    char degree_buffer[10] = {0};
    const char *dot = strchr(coord_str, '.'); // Find dot
    // There should not be more than 5 character between beginning and dot (DDDMM)
    if (!dot || dot - coord_str > 6) return;

    // Parse the DDDMM part of the coordinate
    strncpy(degree_buffer, coord_str, dot - coord_str);
    int32_t dddmm = atol(degree_buffer);
    int32_t degrees = dddmm / 100;
    int32_t minutes = dddmm - degrees * 100;

    // Parse the fractional part of the coordinate
    float dec_minutes = atof(dot);

    // Parse the direction
    const char *direction = strchr(dot, ',') + 1;
    if (is_empty(direction)) return;

    nmea_coord->degrees_minutes = degrees * 100 + minutes + dec_minutes;
    nmea_coord->degrees = degrees + ((minutes + dec_minutes)) / 60.0f;
    nmea_coord->direction = *direction;

    // Change sign if direction is South or West
    if (*direction == 'S' || *direction == 'W') {
        nmea_coord->degrees = -nmea_coord->degrees;
    }
}

void NMEA_Parse_Fix(NMEA_Fix_t *fix, const char *fix_str) {
    if (is_empty(fix_str)) return;

    fix->quality = atoi(fix_str);
    fix->fix = fix->quality > 0;
}

void NMEA_Parse_Fix_Validity(NMEA_Fix_t *nmea_fix, const char *fix_val_str) {
    if (is_empty(fix_val_str)) return;

    if (*fix_val_str == 'A') {
        nmea_fix->fix = true;
    } else if (*fix_val_str == 'V') {
        nmea_fix->fix = false;
    } 
}

void NMEA_Parse_Date(NMEA_Time_t *time, const char *date_str) {
    if (!is_empty(date_str)) return;

    uint32_t full_date = atoi(date_str);
    time->day = full_date / 10000;
    time->month = (full_date % 10000) / 100;
    time->year = full_date % 100;
}

static inline bool is_empty(const char *s) {
    return s == NULL || *s == '*' || *s == ',';
}

static inline const void* min(const void *x, const void *y) {
    return x < y ? x : y;
}