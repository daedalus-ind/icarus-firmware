/**
 ******************************************************************************
 * @file    message.cxx
 * @brief   This file contains functions for handling radio messages.
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
#include "radio/message.hxx"

volatile radio::Message radio::message_rx;

void radio::Message::load(uint8_t msg_class, uint8_t msg_id, uint8_t const* payload, uint8_t payload_len, uint8_t ck) volatile {
  _data[0] = SYNC_CHAR_1;
  _data[1] = SYNC_CHAR_2;
  _data[2] = msg_class;
  _data[3] = msg_id;
  _data[4] = payload_len;
  utils::memcpy_volatile(&_data[5], payload, payload_len);
  _data[5 + payload_len] = ck;
  _length = payload_len + 6;
  _is_new = true;
}

void radio::Message::send() {
  LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_6);
  while (LL_DMA_IsEnabledStream(DMA1, LL_DMA_STREAM_6)) {}
  LL_DMA_ConfigAddresses(DMA1, LL_DMA_STREAM_6, reinterpret_cast<uint32_t>(_data), LL_USART_DMA_GetRegAddr(USART2), LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
  LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_6, _length);
  LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_6);
  _is_new = false;
}

radio::Command radio::Message::handle() volatile {
  if (!_is_new) return Command::None;
  _is_new = false;

  switch (static_cast<MessageClass>(_data[2])) {
    case radio::MessageClass::Ack:
      return Command::None;
    
    case radio::MessageClass::Ping:
      switch (static_cast<MessagePingID>(_data[3])) {
        case radio::MessagePingID::Ping:
          return Command::SendPong;

          case radio::MessagePingID::Pong:
            return Command::None;

        default:
          return Command::SendNack;
      }

    case radio::MessageClass::Configure:
      switch (static_cast<MessageConfigureID>(_data[3])) {
        case radio::MessageConfigureID::CalibrateGyroscope:
          return Command::CalibrateGyroscope;

        case radio::MessageConfigureID::CalibrateReferencePressure:
          return Command::CalibrateReferencePressure;

        case radio::MessageConfigureID::CalibrateReferencePosition:
          return Command::CalibrateReferencePosition;

        case radio::MessageConfigureID::StartKalmanFilter:
          return Command::StartKalmanFilter;

        default:
          return Command::SendNack;
      }

    default:
      return Command::SendNack;
  }
}
