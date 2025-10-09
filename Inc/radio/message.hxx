/**
 ******************************************************************************
 * @file    message.hxx
 * @brief   This file provides radio message types and utilities.
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
#ifndef __RADIO_MESSAGE_HPP__
#define __RADIO_MESSAGE_HPP__

#include "main.hxx"
#include "utils/bytes.hxx"
#include "utils/volatile.hxx"
#include <numeric>

namespace radio {
  
  /**
   * @brief First sync character of the radio protocol, corresponds to 'i' in ASCII.
   */
  constexpr uint8_t SYNC_CHAR_1 = 0x69;
  /**
   * @brief Second sync character of the radio protocol, corresponds to 'c' in ASCII.
   */
  constexpr uint8_t SYNC_CHAR_2 = 0x63;
  /**
   * @brief Maximum length of a radio message.
   */
  constexpr uint8_t MAX_MESSAGE_LENGTH = 128;

  /**
   * @brief Message classes for the radio protocol.
   */
  enum class MessageClass : uint8_t {
    Ack       = 0x01,
    Ping      = 0x02,
    Configure = 0x03,
    Status    = 0x04,
  };

  /**
   * @brief Message IDs for the ACK message class.
   */
  enum class MessageAckID : uint8_t {
    Ack  = 0x01,
    Nack = 0x02,
  };

  /**
   * @brief Message IDs for the PING message class.
   */
  enum class MessagePingID : uint8_t {
    Ping = 0x01,
    Pong = 0x02,
  };

  /**
   * @brief Message IDs for the CONFIGURE message class.
   */
  enum class MessageConfigureID : uint8_t {
    Done                       = 0x01,
    Error                      = 0xFF,
    CalibrateGyroscope         = 0x02,
    CalibrateReferencePressure = 0x03,
    CalibrateReferencePosition = 0x04,
    StartKalmanFilter          = 0x05,
  };

  /**
   * @brief Message IDs for the STATUS message class.
   */
  enum class MessageStatusID : uint8_t {
    PowerOn               = 0x01,
    IMUInitOk             = 0x02,
    IMUInitError          = 0x03,
    MagnetometerInitOk    = 0x04,
    MagnetometerInitError = 0x05,
    BarometerInitOk       = 0x06,
    BarometerInitError    = 0x07,
    GPSFixAcquired        = 0x08,
    GPSFixLost            = 0x09,
    GPSFixSats            = 0x0A,
  };

  /**
   * @brief Commands that can be issued via radio messages.
   */
  enum class Command : uint8_t {
    None,
    SendAck, 
    SendNack,
    SendPong,
    CalibrateGyroscope,
    CalibrateReferencePressure,
    CalibrateReferencePosition,
    StartKalmanFilter,
  };

  /**
   * @brief Radio message.
   */
  class Message {
  public: 
    Message() : _length{0}, _is_new{false} {}

    /**
     * @brief Check if the message is new (not yet used).
     */
    bool is_new() volatile const {
      return _is_new;
    }

    /**
     * @brief Load a message from a byte array.
     */
    void load(uint8_t msg_class, uint8_t msg_id, uint8_t const* payload, uint8_t payload_len, uint8_t ck) volatile;

    /**
     * @brief Construct a message with the given class, ID, and payload arguments.
     */
    template <typename MessageID, typename ...Args>
    void construct(MessageClass mgs_class, MessageID msg_id, Args const& ...args) {
      _data[0] = SYNC_CHAR_1;
      _data[1] = SYNC_CHAR_2;
      _data[2] = static_cast<uint8_t>(mgs_class);
      _data[3] = static_cast<uint8_t>(msg_id);

      size_t payload_length = 0;
      if constexpr (sizeof...(args) > 0)
        utils::pack_bytes(&_data[5], payload_length, args...);
      
      _data[4] = static_cast<uint8_t>(payload_length);
      _length = static_cast<uint8_t>(5 + payload_length + 1);
      _data[5 + payload_length] = _checksum();
      _is_new = true;
    }

    /**
     * @brief Transmit the message over the radio in a non blocking way.
     */
    void send();

    /**
     * @brief Get the command associated with the message.
     */
    Command handle() volatile;

  private: 
    uint8_t _data[MAX_MESSAGE_LENGTH];
    uint8_t _length;
    bool _is_new;

    inline uint8_t _checksum() const {
      return std::accumulate(&_data[2], &_data[_length - 1], 0);
    }
  };

  extern volatile Message message_rx;

}

#endif /* __RADIO_MESSAGE_HPP__ */
