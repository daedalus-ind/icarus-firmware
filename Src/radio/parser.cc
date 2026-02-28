/**
 ******************************************************************************
 * @file    parser.cc
 * @brief   This file contains the implementation of the radio message 
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
#include "radio/parser.hh"

volatile radio::Message radio::rx_message;

radio::Parser radio::parser;

void radio::Parser::parseByte(uint8_t byte)
{
  switch (_state) {
  case State::WaitSync1:
    if (byte == Message::SYNC_CHAR_1) {
      _state = State::WaitSync2;
    }
    break;

  case State::WaitSync2:
    if (byte == Message::SYNC_CHAR_2) {
      _state = State::WaitClass;
    } else {
      _state = State::WaitSync1;
    }
    break;

  case State::WaitClass:
    _buffer[2] = byte;
    _checksum = byte;
    _state = State::WaitType;
    break;

  case State::WaitType:
    _buffer[3] = byte;
    _checksum += byte;
    _state = State::WaitLength;
    break;

  case State::WaitLength:
    _buffer[4] = byte;
    _checksum += byte;
    _payload_index = 0;
    if (byte > 0) {
      _state = State::WaitPayload;
    } else {
      _state = State::WaitChecksum;
    }
    break;

  case State::WaitPayload:
    _buffer[5 + _payload_index++] = byte;
    _checksum += byte;
    if (_payload_index >= _buffer[4]) {
      _state = State::WaitChecksum;
    }
    break;

  case State::WaitChecksum:
    if (_checksum == byte) {
      _buffer[5 + _buffer[4]] = byte;
      rx_message.loadFromBuffer(_buffer, 6 + _buffer[4]);
    }
    _state = State::WaitSync1;
    break;

  default:
    _state = State::WaitSync1;
    break;
  }
}
