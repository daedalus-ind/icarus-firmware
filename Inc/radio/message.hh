/**
 ******************************************************************************
 * @file    message.hh
 * @brief   This file contains the definition of the radio message 
 *          class and related enumerations.
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
#ifndef __RADIO_MESSAGE_HH__
#define __RADIO_MESSAGE_HH__

#include "main.hh"
#include "utils/bytes.hh"
#include "utils/volatile.h"

#include <numeric>

namespace radio {

  /**
   * @brief Represents a message transmitted or received via the radio module.
   */
  class Message {
  public:
    /**
     * @brief First synchronization character for radio messages. Equivalent to ASCII 'i'.
     */
    static constexpr uint8_t SYNC_CHAR_1 = 0x69;

    /**
     * @brief Second synchronization character for radio messages. Equivalent to ASCII 'c'.
     */
    static constexpr uint8_t SYNC_CHAR_2 = 0x63;

    /**
     * @brief Maximum length of a radio message in bytes.
     */
    static constexpr size_t MAX_LENGTH = 256;

    /**
     * @brief Enumeration of possible message classes.
     */
    enum class Class : uint8_t {
      Invalid   = 0x00,
      Ack       = 0x01,
      Ping      = 0x02,
      Config    = 0x03,
      Status    = 0x04,
      Telemetry = 0x05,
    };

    /**
     * @brief Enumeration of possible types of acknowledgment messages.
     */
    enum class AckType : uint8_t {
      Acknowledge    = 0x01,
      NotAcknowledge = 0x02,
    };

    /**
     * @brief Enumeration of possible types of ping messages.
     */
    enum class PingType : uint8_t {
      Request  = 0x01,
      Response = 0x02,
    };

    /**
     * @brief Enumeration of possible types of configuration messages.
     */
    enum class ConfigType : uint8_t {
      GyroCalibration = 0x01,
      MagCalibration  = 0x02,
      BaroCalibration = 0x03,
      GPSCalibration  = 0x04,
      StartKalman     = 0x05,
    };

    /**
     * @brief Enumeration of possible types of status messages.
     */
    enum class StatusType : uint8_t {
      GenericOk      = 0x01,
      GenericError   = 0x02,
      PowerOn        = 0x03,
      IMUInitOk      = 0x04,
      IMUInitError   = 0x05,
      MagInitOk      = 0x06,
      MagInitError   = 0x07,
      BaroInitOk     = 0x08,
      BaroInitError  = 0x09,
      GPSFixAcquired = 0x0A,
      GPSFixLost     = 0x0B,
    };

    /**
     * @brief Enumeration of possible types of telemetry messages.
     */
    enum class TelemetryType : uint8_t {
      Kalman = 0x01,
      GPS    = 0x02,
      Accel  = 0x03,
      Baro   = 0x04,
    };

    /**
     * @brief Enumeration of possible commands derived from radio messages.
     */
    enum class Command : uint8_t {
      None              = 0x00,
      SendAck           = 0x01,
      SendNack          = 0x02,
      SendPingResponse  = 0x03,
      CalibrateGyro     = 0x04,
      CalibrateMag      = 0x05,
      CalibrateBaro     = 0x06,
      CalibrateGPS      = 0x07,
      StartKalman       = 0x08,
    };

    /**
     * @brief Constructor for an empty radio message.
     */
    constexpr Message() : _buffer{}, _length{0}, _not_handled{false} {}

    /**
     * @brief Checks if the radio message is new (not yet processed).
     * @return `true` if the message has not been handled, `false` otherwise.
     */
    constexpr bool isNotHandledYet() const 
    {
      return _not_handled;
    }

    /**
     * @brief Checks if the radio message is new (not yet processed).
     * @return `true` if the message has not been handled, `false` otherwise.
     */
    bool isNotHandledYet() const volatile
    {
      return _not_handled;
    }

    /**
     * @brief Loads a radio message from a volatile byte buffer.
     * @param data The volatile byte buffer containing the message data.
     * @param length The length of the message data in bytes.
     */
    inline void loadFromBuffer(const uint8_t* data, uint8_t length) volatile
    {
      _length = length;
      _not_handled = true;
      memcpy_volatile(_buffer, data, length);
    }

    /**
     * @brief Creates a radio message with the specified class, type, and payload.
     * @tparam MessageType The type of the message.
     * @tparam Args The types of the payload arguments.
     * @param msg_class The class of the message.
     * @param msg_type The type of the message.
     * @param args The payload arguments to include in the message.
     * @return The constructed radio message.
     */
    template <typename Type, typename ...Args>
    constexpr void create(Class msg_class, Type msg_type, const Args&... args) 
    {
      _buffer[0] = SYNC_CHAR_1;
      _buffer[1] = SYNC_CHAR_2;
      _buffer[2] = static_cast<uint8_t>(msg_class);
      _buffer[3] = static_cast<uint8_t>(msg_type);
     
      // Pack the payload into the message buffer
      size_t payload_length = 0;
      if constexpr (sizeof...(args) > 0) {
        utils::pack_bytes(&_buffer[5], payload_length, args...);
      }
      _buffer[4] = static_cast<uint8_t>(payload_length);

      // Calculate and set the checksum
      _buffer[5 + payload_length] = static_cast<uint8_t>(
        std::accumulate(&_buffer[2], &_buffer[5 + payload_length], 0u)
      );

      _length = 6 + static_cast<uint8_t>(payload_length);
      _not_handled = true;
    }

    /**
     * @brief Transmits the radio message using the radio module.
     * @return `true` if the message was sent successfully, `false` otherwise.
     * @note This functions sets the message as sent (not new) after transmission.
     */
    bool send();

    /**
     * @brief Handles the radio message and returns the corresponding command.
     * @return The command associated with the message.
     * @note This function sets the message as processed (not new) after handling it.
     */
    Command handle() volatile; 

  private: 
    uint8_t _buffer[MAX_LENGTH];
    uint8_t _length; 
    bool _not_handled;
  }; 

}

#endif // __RADIO_MESSAGE_HH__
