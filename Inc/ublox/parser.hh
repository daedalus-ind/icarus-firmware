/**
 ******************************************************************************
 * @file    parser.hh
 * @brief   This file contains the definition of the UBX message parser class.
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
#ifndef __UBLOX_PARSER_HH__
#define __UBLOX_PARSER_HH__

#include "main.hh"
#include "gps.hh"

namespace ubx {

  /**
   * @brief UBX message parser class.
   */
  class Parser {
  public:
    /**
     * @brief Constructor for the UBX message parser.
     */
    inline Parser() 
      : _state{State::WaitSync1},
        _payload_length{0},
        _payload_index{0},
        _checksum_a{0},
        _checksum_b{0} {}
    
    /**
     * @brief Parses a single byte from the UBX data stream.
     * @param byte The byte to parse.
     */
    void parseByte(uint8_t byte);

  private:
    enum class State : uint8_t {
      WaitSync1,
      WaitSync2,
      WaitClass,
      WaitId,
      WaitLength1,
      WaitLength2,
      WaitPayload,
      WaitChecksumA,
      WaitChecksumB
    } _state;
    uint16_t _payload_length;
    uint16_t _payload_index;
    uint8_t _payload[NAV_PVT_LENGTH];
    uint8_t _checksum_a;
    uint8_t _checksum_b;
  };

  extern Parser parser;

}

#endif // __UBLOX_PARSER_HH__
