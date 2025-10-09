/**
 ******************************************************************************
 * @file    parser.hxx
 * @brief   This file contains a simple UBX protocol parser supporting
 *          only NAV-PVT messages.
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
#ifndef __UBX_PARSER_HXX__
#define __UBX_PARSER_HXX__

#include "main.hxx"
#include <cstddef>

namespace ubx {

  /**
   * @brief First sync character of UBX protocol, corresponds to 'µ' (mu) in ISO 8859-1.
   */
  constexpr uint8_t SYNC_CHAR_1 = 0xB5;
  /**
   * @brief Second sync character of UBX protocol, corresponds to 'b' in ASCII.
   */
  constexpr uint8_t SYNC_CHAR_2 = 0x62;
  /**
   * @brief UBX NAV message class.
   */
  constexpr uint8_t NAV_CLASS = 0x01;
  /**
   * @brief UBX NAV-PVT message ID.
   */
  constexpr uint8_t NAV_PVT_ID = 0x07;
  /**
   * @brief UBX NAV-PVT message class.
   */
  constexpr size_t NAV_PVT_LENGTH = 92;

  /**
   * @brief UBX protocol parser.
   */
  class Parser {
  public:
    Parser(); 

    /**
     * @brief Parse a byte from the UBX protocol stream.
     */
    void process(uint8_t byte);

  private:
    enum State {
      WAIT_SYNC_1,
      WAIT_SYNC_2,
      WAIT_CLASS,
      WAIT_ID,
      WAIT_LENGTH_1,
      WAIT_LENGTH_2,
      WAIT_PAYLOAD,
      WAIT_CK_A,
      WAIT_CK_B
    } _state;
    uint8_t _msg_class;
    uint8_t _msg_id;
    uint16_t _len;
    uint16_t _payload_idx;
    uint8_t _payload[NAV_PVT_LENGTH];
    uint8_t _ck_a;
    uint8_t _ck_b;
  };

  extern Parser parser;

}

#endif /* __UBX_PARSER_HXX__ */
