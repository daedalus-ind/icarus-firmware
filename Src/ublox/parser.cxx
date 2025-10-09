/**
 ******************************************************************************
 * @file    parser.cxx
 * @brief   This file contains the implementation of a basic UBX protocol
 *          parser supporting only NAV-PVT messages.
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
#include "ublox/parser.hxx"
#include "ublox/gps.hxx"

ubx::Parser ubx::parser;

ubx::Parser::Parser() : _state(WAIT_SYNC_1), _payload_idx(0) {}

void ubx::Parser::process(uint8_t byte) {
  switch (_state) {
    case WAIT_SYNC_1:
      if (byte == SYNC_CHAR_1) {
        _state = WAIT_SYNC_2;
      }
      break;

    case WAIT_SYNC_2:
      if (byte == SYNC_CHAR_2) {
        _state = WAIT_CLASS;
        break;
      }
      _state = WAIT_SYNC_1;
      break;

    case WAIT_CLASS:
      /* Only NAV messages are supported */
      if (byte != NAV_CLASS) {
        _state = WAIT_SYNC_1;
        break;
      }
      _msg_class = byte;
      _ck_a = byte;
      _ck_b = _ck_a;
      _state = WAIT_ID;
      break;

    case WAIT_ID:
      /* Only NAV-PVT messages are supported */
      if (byte != NAV_PVT_ID) {
        _state = WAIT_SYNC_1;
        break;
      }
      _msg_id = byte;
      _ck_a += byte;
      _ck_b += _ck_a;
      _state = WAIT_LENGTH_1;
      break;

    case WAIT_LENGTH_1:
      _len = byte;
      _ck_a += byte;
      _ck_b += _ck_a;
      _state = WAIT_LENGTH_2;
      break;

    case WAIT_LENGTH_2:
      _len |= (byte << 8);
      /* Only NAV-PVT messages are supported */
      if (_len != NAV_PVT_LENGTH) {
        _state = WAIT_SYNC_1;
        break;
      }
      _ck_a += byte;
      _ck_b += _ck_a;
      _payload_idx = 0;
      _state = WAIT_PAYLOAD;
      break;

    case WAIT_PAYLOAD:
      _payload[_payload_idx++] = byte;
      _ck_a += byte;
      _ck_b += _ck_a;
      if (_payload_idx >= _len) {
        _state = WAIT_CK_A;
      }
      break;

    case WAIT_CK_A:
      if (byte != _ck_a) {
        _state = WAIT_SYNC_1;
        break;
      }
      _state = WAIT_CK_B;
      break;

    case WAIT_CK_B:
      if (byte == _ck_b) {
        /* Successfully received a NAV-PVT message */
        gps.load_nav_pvt(_payload);
      }
      _state = WAIT_SYNC_1;
      break;

    default:
      _state = WAIT_SYNC_1;
      break;
  }
}
