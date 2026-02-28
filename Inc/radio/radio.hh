/**
 ******************************************************************************
 * @file    radio.hh
 * @brief   This file contains the definition of radio utility functions.
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
#ifndef __RADIO_HH__
#define __RADIO_HH__

#include "main.hh"
#include "time.hh"
#include "usart.hh"
#include <cstdint>
#include <cstring>

namespace radio {
  
  /**
   * @brief Check if the radio module is currently free (not transmitting nor receiving).
   * @return `true` if the radio is free, `false` otherwise.
   */
  static inline bool isFree() 
  {
    return LL_GPIO_IsInputPinSet(RADIO_AUX_GPIO_Port, RADIO_AUX_Pin);
  }

  /**
   * @brief Timer to track how long the radio has been free.
   */
  extern time::Timer free_timer;

  /**
   * @brief Check if the radio module has been free for 50 milliseconds.
   * @return `true` if the radio has been free for at least 50 milliseconds, `false` otherwise.
   */
  static inline bool hasBeenFreeFor50ms()
  {
    if (isFree()) {
      if (!free_timer.isRunning()) {
        free_timer.start(50);
      }
      return free_timer.isExpired();
    } else {
      free_timer.stop();
      return false;
    }
  }

  /**
   * @brief Operating modes for the radio module.
   */
  enum class Mode : uint8_t {
    Normal,
    Configuration
  };

  /**
   * @brief Set the operating mode of the radio module.
   * @param mode The desired operating mode.
   */
  static inline void setMode(Mode mode)
  {
    switch (mode) {
    case Mode::Normal:
      LL_GPIO_ResetOutputPin(RADIO_M0_GPIO_Port, RADIO_M0_Pin);
      LL_GPIO_ResetOutputPin(RADIO_M1_GPIO_Port, RADIO_M1_Pin);
      break;
    case Mode::Configuration:
      LL_GPIO_SetOutputPin(RADIO_M0_GPIO_Port, RADIO_M0_Pin);
      LL_GPIO_SetOutputPin(RADIO_M1_GPIO_Port, RADIO_M1_Pin);
      break;  
    }
  }

  /**
   * @brief Baud rate settings for the radio module.
   */
  enum class BaudRate : uint8_t {
    BR_1200   = 0x00,
    BR_2400   = 0x01,
    BR_4800   = 0x02,
    BR_9600   = 0x03,
    BR_19200  = 0x04,
    BR_38400  = 0x05,
    BR_57600  = 0x06,
    BR_115200 = 0x07
  };

  /**
   * @brief Parity settings for the radio module.
   */
  enum class Parity : uint8_t {
    None = 0x00,
    Odd  = 0x01,
    Even = 0x02
  };

  /**
   * @brief Air speed settings for the radio module.
   */
  enum class AirSpeed : uint8_t {
    AS_2400   = 0x02,
    AS_4800   = 0x03,
    AS_9600   = 0x04,
    AS_19200  = 0x05,
    AS_38400  = 0x06,
    AS_62500  = 0x07
  };

  /**
   * @brief Transmission power settings for the radio module.
   */
  enum class TXPower : uint8_t {
    P_22dBm = 0x00,
    P_17dBm = 0x01,
    P_13dBm = 0x02,
    P_10dBm = 0x03
  };

  /**
   * @brief Configure the radio module with the specified settings.
   * @param baud_rate The desired baud rate.
   * @param parity The desired parity setting.
   * @param air_speed The desired air speed.
   * @param tx_power The desired transmission power.
   * @return `true` if the configuration was successful, `false` otherwise.
   */
  static inline bool configure(BaudRate baud_rate, Parity parity, AirSpeed air_speed, TXPower tx_power)
  {
    struct EG0Register {
      AirSpeed air_speed : 3;
      Parity parity      : 2;
      BaudRate baud_rate : 3; 
    }; 
    EG0Register reg { air_speed, parity, baud_rate };

    uint8_t config_command[] = { 
      0xC0, 
      0x02, 
      0x02, 
      *reinterpret_cast<uint8_t*>(&reg), 
      static_cast<uint8_t>(tx_power) 
    };
    // Send the configuration command to the radio module
    if (!serial::write(RADIO_UART, config_command, sizeof(config_command), 1000)) {
      return false;
    }

    // Wait for the response from the radio module
    uint8_t response[sizeof(config_command)]{};
    if (!serial::read(RADIO_UART, response, sizeof(response), 10'000)) {
      return false;
    }

    // Response should start with 0xC1 and the rest should match the sent command (except for the first byte)
    if (response[0] != 0xC1 || std::memcmp(&response[1], &config_command[1], sizeof(response) - 1) != 0) {
      return false;
    }

    return true;
  }
  
}

#endif // __RADIO_HH__
