/**
 * @file nmea_check.c 
 * @brief This file contains the function definitions for checking NMEA messages.
 * 
 * @author Mattia Gregnanin
 * @date 31-03-2025
 */

#include "NMEA/nmea.h"

#include <string.h>

static uint16_t parse_hex(char c);
static void talker_on_list(NMEA_Talker_t* talker, const char *token, const NMEA_Talker_t list[]);
static void sentence_on_list(NMEA_Sentence_t *sentence, const char *token, const NMEA_Sentence_t list[]);

static const NMEA_Talker_t nmea_talkers[] = {
    { "P", 1, NMEA_TALKER_P }, 
    { "WI", 2, NMEA_TALKER_WI }, 
    { "GP", 2, NMEA_TALKER_GP }, 
    { "GN", 2, NMEA_TALKER_GN },
    { NULL, 0, NMEA_TALKER_INVALID }
};

static const NMEA_Sentence_t nmea_parseable_sentences[] = {
    { "GGA", NMEA_SENTENCE_GGA }, 
    { "GLL", NMEA_SENTENCE_GLL }, 
    { "GSA", NMEA_SENTENCE_GSA }, 
    { "RMC", NMEA_SENTENCE_RMC }, 
    { NULL, NMEA_SENTENCE_INVALID }
};

void NMEA_Check_Msg(NMEA_Check_Result_t *rslt, const char *nmea_msg) {
    rslt->flag = NMEA_BAD;

    // Check if first character is '$'
    if (*nmea_msg != '$') return;
    rslt->flag |= NMEA_HAS_DOLLAR;

    // Compute message length
    size_t msg_len = strlen(nmea_msg);

    // Find '*' character at the end of the message 
    const char *astk = &nmea_msg[msg_len - 1]; // Should be '\n' 
    while (*astk != '*' && astk > nmea_msg) { 
        astk--;
    }
    // There must be at lest two character after the asterisk.
    if (*astk != '*' || &nmea_msg[msg_len - 1] - astk < 2) return; 

    // Compute checksum
    uint16_t checksum = 0;
    checksum += 16 * parse_hex(astk[1]);
    checksum += parse_hex(astk[2]);
    // Compare checksum
    for (const char *p = nmea_msg + 1; p < astk; p++) {
        checksum ^= *p;
    }
    if (checksum != 0) return;
    rslt->flag |= NMEA_HAS_CHECKSUM;

    // Find talker
    talker_on_list(&rslt->talker, nmea_msg + 1, nmea_talkers);
    if (rslt->talker.type == NMEA_TALKER_INVALID) return;
    rslt->flag |= NMEA_HAS_TALKER;

    // Find sentence 
    sentence_on_list(&rslt->sentence, nmea_msg + 1 + rslt->talker.length, nmea_parseable_sentences);
    if (rslt->sentence.type == NMEA_SENTENCE_INVALID) return;
    rslt->flag |= NMEA_HAS_SENTENCE;
    rslt->flag |= NMEA_HAS_PARSEABLE_SENTENCE;
}

static uint16_t parse_hex(char c) {
    if (c < '0') {
        return 0;
    }
    if (c <= '9') {
        return c - '0';
    }
    if (c < 'A') {
        return 0;
    }
    if (c <= 'F') {
        return (c - 'A') + 10;
    }
    return 0;
}

static void talker_on_list(NMEA_Talker_t* talker, const char *token, const NMEA_Talker_t list[]) {
    size_t index = 0;
    while (list[index].token) {
        if (strncmp(list[index].token, token, list[index].length) == 0) {
            break;
        } 
        index++;
    }
    *talker = list[index];
}

static void sentence_on_list(NMEA_Sentence_t *sentence, const char *token, const NMEA_Sentence_t list[]) {
    size_t index = 0;
    while (list[index].token) {
        if (strncmp(list[index].token, token, 3) == 0) {
            break;
        }
        index++;
    }
    *sentence = list[index];
}