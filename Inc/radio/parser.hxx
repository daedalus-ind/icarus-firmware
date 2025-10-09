/**
 ******************************************************************************
 * @file    parser.hxx
 * @brief   This file contains the class definitions for parsing 
 *          radio protocol messages.
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
#ifndef __RADIO_PARSER_HXX__
#define __RADIO_PARSER_HXX__

#include "main.hxx"
#include "message.hxx"

namespace radio {

  /**
   * @brief Radio message parser.
   */
  class Parser {
  public:
    Parser(); 

    /**
     * @brief Parse a byte from the radio protocol stream.
     */
    void process(uint8_t byte);

  private:
    enum State {
      WAIT_SYNC_1,
      WAIT_SYNC_2,
      WAIT_CLASS,
      WAIT_ID,
      WAIT_LENGTH,
      WAIT_PAYLOAD,
      WAIT_CHECKSUM
    } _state;
    uint8_t _msg_class;
    uint8_t _msg_id;
    uint8_t _len;
    uint8_t _payload_idx;
    uint8_t _payload[MAX_MESSAGE_LENGTH];
    uint8_t _checksum;
  };

  extern Parser parser;

}

#endif /* __RADIO_PARSER_HXX__ */
