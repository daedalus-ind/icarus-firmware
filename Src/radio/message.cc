/**
 ******************************************************************************
 * @file    message.cc
 * @brief   This file contains the implementation of the radio message
 *          handling functions.
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
#include "radio/message.hh"
#include "usart.hh"

bool radio::Message::send() 
{
  _not_handled = false;
  return serial::write(RADIO_UART, _buffer, _length, 10);
}

radio::Message::Command radio::Message::handle() volatile
{
  if (!_not_handled) {
    return Command::None;
  }
  // Mark the message as processed
  _not_handled = false;

  Class msg_class = static_cast<Class>(_buffer[2]);
  switch (msg_class) { 
  case Class::Ack: 
    switch (static_cast<AckType>(_buffer[3])) {
    case AckType::Acknowledge:
      return Command::None;

    case AckType::NotAcknowledge:
      // TODO: Resend last message
      return Command::None;
    }

  case Class::Ping:
    switch (static_cast<PingType>(_buffer[3])) {
    case PingType::Request:
      return Command::SendPingResponse;

    case PingType::Response:
      return Command::None;
    }

  case Class::Config:
    switch (static_cast<ConfigType>(_buffer[3])) {
    case ConfigType::GyroCalibration:
      return Command::CalibrateGyro;
    
    case ConfigType::MagCalibration:
      return Command::CalibrateMag;

    case ConfigType::BaroCalibration:
      return Command::CalibrateBaro;

    case ConfigType::GPSCalibration:
      return Command::CalibrateGPS;

    case ConfigType::StartKalman:
      return Command::StartKalman;
    }

  default:
    return Command::SendNack;
  }

  return Command::None;
}
