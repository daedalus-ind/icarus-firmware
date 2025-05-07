/**
 * @file nmea.h 
 * @brief This file contains the function prototypes for parsing NMEA messages.
 * 
 * @author Mattia Gregnanin
 * @date 31-03-2025
 */

#ifndef __NMEA_H
#define __NMEA_H

#include "nmea_defs.h"

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Check if the NMEA message is valid and parseable.
 * 
 * @param rslt Pointer to the result structure to store the check result.
 * @param nmea_msg Pointer to the NMEA message string.
 */
void NMEA_Check_Msg(NMEA_Check_Result_t *rslt, const char *nmea_msg);

/**
 * @brief Parse the NMEA message and extract relevant information.
 * 
 * @param stat Pointer to the NMEA status structure to store the parsed data.
 * @param nmea_msg Pointer to the NMEA message string.
 */
void NMEA_Parse_Msg(NMEA_Status_t *stat, const char *nmea_msg);

/**
 * @brief Parse the time from the NMEA message.
 * 
 * @param time Pointer to the NMEA time structure to store the parsed time.
 * @param time_str Pointer to the time string in the NMEA message.
 */
void NMEA_Parse_Time(NMEA_Time_t *time, const char *time_str);

/**
 * @brief Parse the coordinate from the NMEA message.
 * 
 * @param coord Pointer to the NMEA coordinate structure to store the parsed coordinate.
 * @param coord_str Pointer to the coordinate string in the NMEA message.
 */
void NMEA_Parse_Coordinate(NMEA_Coordinate_t *coord, const char *coord_str);

/**
 * @brief Parse the fix information from the NMEA message.
 * 
 * @param fix Pointer to the NMEA fix structure to store the parsed fix information.
 * @param fix_str Pointer to the fix string in the NMEA message.
 */
void NMEA_Parse_Fix(NMEA_Fix_t *fix, const char *fix_str);

/**
 * @brief Parse the fix validity from the NMEA message.
 * 
 * @param fix_val Pointer to the NMEA fix structure to store the parsed fix validity.
 * @param fix_val_str Pointer to the fix validity string in the NMEA message.
 * @note The fix validity can be either 'A' (valid) or 'V' (invalid).
 */
void NMEA_Parse_Fix_Validity(NMEA_Fix_t *fix_val, const char *fix_val_str);

/**
 * @brief Parse the date from the NMEA message.
 * 
 * @param time Pointer to the NMEA time structure to store the parsed date.
 * @param date_str Pointer to the date string in the NMEA message.
 * @note The date format must be in the format DDMMYY.
 */
void NMEA_Parse_Date(NMEA_Time_t *time, const char *date_str);

#ifdef __cplusplus
}
#endif

#endif