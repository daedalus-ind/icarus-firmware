/**
 ******************************************************************************
 * @file    parser.hxx
 * @brief   This file contains functions for handling radio
 *          protocol messages.
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
#include "radio/parser.hxx"

radio::Parser radio::parser;

radio::Parser::Parser() : _state{WAIT_SYNC_1} {}

void radio::Parser::process(uint8_t byte) {
  switch (_state) {
    case WAIT_SYNC_1:
      if (byte == SYNC_CHAR_1)
        _state = WAIT_SYNC_2;
      break;

    case WAIT_SYNC_2:
      if (byte != SYNC_CHAR_2) {
        _state = WAIT_SYNC_1;
        break;
      }
      _state = WAIT_CLASS;
      break;

    case WAIT_CLASS:
      _msg_class = byte;
      _checksum = byte;
      _state = WAIT_ID;
      break;

    case WAIT_ID:
      _msg_id = byte;
      _checksum += byte;
      _state = WAIT_LENGTH;
      break;
    
    case WAIT_LENGTH:
      if (byte > MAX_MESSAGE_LENGTH - 6) {
        _state = WAIT_SYNC_1;
        break;
      }
      _len = byte;
      _checksum += byte;
      _payload_idx = 0;
      if (_len == 0)
        _state = WAIT_CHECKSUM;
      else
        _state = WAIT_PAYLOAD;
      break;

    case WAIT_PAYLOAD:
      _payload[_payload_idx++] = byte;
      _checksum += byte;
      if (_payload_idx >= _len)
        _state = WAIT_CHECKSUM;
      break;

    case WAIT_CHECKSUM:
      if (byte == _checksum) {
        message_rx.load(_msg_class, _msg_id, _payload, _len, byte);
      }
      _state = WAIT_SYNC_1;
      break;

    default:
      _state = WAIT_SYNC_1;
      break;
  }
}
