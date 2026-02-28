/**
 ******************************************************************************
 * @file    parser.cc
 * @brief   This file contains the definition of the UBX message parse
 *          method.
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
#include "ublox/parser.hh"

ubx::Parser ubx::parser;

void ubx::Parser::parseByte(uint8_t byte) 
{
  switch (_state) {
    case State::WaitSync1:
      if (byte == SYNC_CHAR_1) {
        _state = State::WaitSync2;
      }
      break;

    case State::WaitSync2:
      if (byte == SYNC_CHAR_2) {
        _state = State::WaitClass;
      } else {
        _state = State::WaitSync1;
      }
      break;

    case State::WaitClass:
      // Only process NAV class messages
      if (byte != NAV_CLASS) {
        _state = State::WaitSync1;
        break;
      }
      _checksum_a = byte;
      _checksum_b = _checksum_a;
      _state = State::WaitId;
      break;

    case State::WaitId:
      // Only process NAV-PVT messages
      if (byte != NAV_PVT_ID) {
        _state = State::WaitSync1;
        break;
      }
      _checksum_a += byte;
      _checksum_b += _checksum_a;
      _state = State::WaitLength1;
      break;

    case State::WaitLength1:
      _payload_length = byte;
      _checksum_a += byte;
      _checksum_b += _checksum_a;
      _state = State::WaitLength2;
      break;

    case State::WaitLength2:
      _payload_length |= (byte << 8);
      _checksum_a += byte;
      _checksum_b += _checksum_a;
      _payload_index = 0;
      if (_payload_length > 0) {
        _state = State::WaitPayload;
      } else {
        _state = State::WaitChecksumA;
      }
      break;

    case State::WaitPayload:
      _payload[_payload_index++] = byte;
      _checksum_a += byte;
      _checksum_b += _checksum_a;
      if (_payload_index >= _payload_length) {
        _state = State::WaitChecksumA;
      }
      break;

    case State::WaitChecksumA:
      if (_checksum_a == byte) {
        _state = State::WaitChecksumB;
      } else {
        _state = State::WaitSync1;
      }
      break;

    case State::WaitChecksumB:
      if (_checksum_b == byte) {
        ubx::gps.loadFromBuffer(_payload);
      }
      _state = State::WaitSync1;
      break;

    default:
      _state = State::WaitSync1;
      break;
  }
}
