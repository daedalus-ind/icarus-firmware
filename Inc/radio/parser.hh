/**
 ******************************************************************************
 * @file    parser.hh
 * @brief   This file contains the definition of the radio message 
 *          parser class.
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
#ifndef __RADIO_PARSER_HH__
#define __RADIO_PARSER_HH__

#include "main.hh"
#include "radio/message.hh"

namespace radio {

  class Parser {
  public:
    /**
     * @brief Constructor for the radio message parser.
     */
    inline Parser() : _state{State::WaitSync1}, _buffer{}, _payload_index{0} {}

    /**
     * @brief Parses a single byte from the radio data stream.
     * @param byte The byte to parse.
     */
    void parseByte(uint8_t byte);

  private:
    /**
     * @brief The different states of the parser.
     */
    enum class State : uint8_t {
      WaitSync1,
      WaitSync2,
      WaitClass,
      WaitType,
      WaitLength,
      WaitPayload,
      WaitChecksum
    } _state;
    uint8_t _buffer[Message::MAX_LENGTH];
    uint8_t _payload_index;
    uint8_t _checksum;
  }; 

  /**
   * @brief The most recently received radio message.
   */
  extern volatile radio::Message rx_message;

  /**
   * @brief The radio message parser instance.
   */
  extern radio::Parser parser;

}

#endif // __RADIO_PARSER_HH__
