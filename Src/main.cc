/**
 ******************************************************************************
 * @file    main.cc
 * @brief   Main program body.
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
#include "main.hh"
#include "i2c.hh"
#include "spi.hh"
#include "usart.hh"
#include "gpio.hh"
#include "time.hh"

#include "radio/radio.hh"
#include "radio/parser.hh"
#include "radio/message.hh"

#include "sensors/icm-45686.hh"
#include "sensors/lis2mdl.hh"
#include "sensors/ms5607.hh"

#include "ublox/gps.hh"

#include "math/vec3.hh"
#include "math/quat.hh"

#include "geo/altitude.hh"
#include "geo/ecef.hh"
#include "geo/enu.hh"
#include "geo/wmm.hh"

#include "ahrs/kalman.hh"
#include "ahrs/triad.hh"

using namespace sensors;

// Configuration function prototypes
static inline void SystemClock_Config();
static inline bool Radio_Config();
static inline bool IMU_Config();
static inline bool Mag_Config();
static inline bool Baro_Config();
static inline void GPS_Config();

// Incoming radio messages handler 
static inline void Radio_Message_Handler();

// Sensor instances
ICM_45686 imu{SPI2, IMU_CS_GPIO_Port, IMU_CS_Pin};
LIS2MDL mag{I2C3};
MS5607_FSM baro{SPI2, BARO_CS_GPIO_Port, BARO_CS_Pin};

// Kalman filter instance
CCMRAM ahrs::Kalman kalman;

// Message to be sent via radio
radio::Message tx_message;

// Calibration and reference data
float sea_level_pressure = 1013.25f;
geo::ECEF reference_ecef = geo::ECEF::fromGeodetic({45.0f, 0.0f, 0.0f});
math::Mat3x3 ecef_to_enu_matrix = geo::ecef_to_enu_matrix(45.0f, 0.0f);

volatile bool kalman_running = false;
time::Timer kalman_timer;

/**
 * @brief The application entry point.
 */
int main()
{
  // Reset of all peripherals, Initializes the Flash interface and the Systick.
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  // System interrupt init
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  // SysTick_IRQn interrupt configuration
  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),15, 0));

  // Configure the system clock
  SystemClock_Config();

  // Initialize all configured peripherals
  MX_GPIO_Init();
  MX_I2C3_Init();
  MX_SPI2_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();

  // Init World Magnetic Model coefficients
  geo::WMM::initCoefficients();

  // Wait for peripherals to stabilize
  time::delay(1000);

  // Configure Radio
  if (!Radio_Config()) {
    Error_Handler();
  }

  // Indicate that system is powered on
  tx_message.create(radio::Message::Class::Status, radio::Message::StatusType::PowerOn);
  tx_message.send();

  time::delay(100);
  // Configure IMU
  if (!IMU_Config()) {
    tx_message.create(radio::Message::Class::Status, radio::Message::StatusType::IMUInitError);
    tx_message.send();
    Error_Handler();
  }
  tx_message.create(radio::Message::Class::Status, radio::Message::StatusType::IMUInitOk);
  tx_message.send();

  time::delay(100);
  // Configure Magnetometer
  if (!Mag_Config()) {
    tx_message.create(radio::Message::Class::Status, radio::Message::StatusType::MagInitError);
    tx_message.send();
    Error_Handler();
  }
  tx_message.create(radio::Message::Class::Status, radio::Message::StatusType::MagInitOk);
  tx_message.send();

  time::delay(100);
  // Configure Barometer
  if (!Baro_Config()) {
    tx_message.create(radio::Message::Class::Status, radio::Message::StatusType::BaroInitError);
    tx_message.send();
    Error_Handler();
  }
  tx_message.create(radio::Message::Class::Status, radio::Message::StatusType::BaroInitOk);
  tx_message.send();

  GPS_Config();

  // Infinite loop
  volatile bool baro_data_ready = false;
  float pressure = 0.0f, temperature = 0.0f;
  time::Timer baro_data_timer;
  baro_data_timer.start(2000);

  volatile bool gps_fix_acquired = false;
  time::Timer gps_data_timer;
  while (true) {

    // See if new barometer data is available
    if (auto data = baro.process()) {
      pressure = data->first;
      temperature = data->second;
      baro_data_ready = true;
    }

    if (kalman_running && kalman_timer.isExpired()) {
      float dt = kalman_timer.getElapsed() * 0.001f;
      kalman_timer.start(10);

      // For now, just send once the Kalman state
      tx_message.create(radio::Message::Class::Telemetry, radio::Message::TelemetryType::Kalman, kalman.x);
      kalman_running = false;
    }

    // Handle received radio messages
    if (!tx_message.isNotHandledYet() && radio::rx_message.isNotHandledYet()) {
      Radio_Message_Handler();
    }

    // If GPS fix status has changed, send status message
    if (ubx::gps.hasFix() && !gps_fix_acquired && !tx_message.isNotHandledYet()) {
      gps_fix_acquired = true;
      gps_data_timer.start(2000);
      tx_message.create(radio::Message::Class::Status, radio::Message::StatusType::GPSFixAcquired);
    } else if (!ubx::gps.hasFix() && gps_fix_acquired && !tx_message.isNotHandledYet()) {
      gps_fix_acquired = false;
      gps_data_timer.stop();
      tx_message.create(radio::Message::Class::Status, radio::Message::StatusType::GPSFixLost);
    }

    // Send GPS data at 0.5 Hz if fix is acquired
    if (gps_data_timer.isExpired() && !tx_message.isNotHandledYet()) {
      gps_data_timer.start(2000);
      geo::Geodetic position = ubx::gps.getPosition();
      uint8_t num_sats = ubx::gps.getSatelliteCount();
      uint8_t fix_type = ubx::gps.getFixType();
      tx_message.create(radio::Message::Class::Telemetry, radio::Message::TelemetryType::GPS, position, num_sats, fix_type);
    }

    // Send barometer data if ready at 0.5 Hz
    if (baro_data_ready && baro_data_timer.isExpired() && !tx_message.isNotHandledYet()) {
      baro_data_timer.start(2000);
      float altitude = geo::altitude_from_pressure(pressure, sea_level_pressure);
      tx_message.create(radio::Message::Class::Telemetry, radio::Message::TelemetryType::Baro, pressure, temperature, altitude);
    }

    // Send message if radio is free and message is new
    if (radio::hasBeenFreeFor50ms() && tx_message.isNotHandledYet()) {
      tx_message.send();
    }
  }
}

/**
 * @brief System Clock Configuration.
 */
static inline void SystemClock_Config()
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_5);
  while (LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_5) {}
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  LL_RCC_HSE_Enable();

  // Wait till HSE is ready
  while (LL_RCC_HSE_IsReady() != 1) {}
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_8, 168, LL_RCC_PLLP_DIV_2);
  LL_RCC_PLL_Enable();

  // Wait till PLL is ready
  while (LL_RCC_PLL_IsReady() != 1) {}
  while (LL_PWR_IsActiveFlag_VOS() == 0) {}
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_4);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

  // Wait till System clock is ready
  while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {}
  LL_Init1msTick(168000000);
  LL_SetSystemCoreClock(168000000);

  // Enable SysTick interrupt
  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
  LL_SYSTICK_EnableIT();
}

/**
 * @brief Radio Module Configuration.
 */
static inline bool Radio_Config()
{
  // Wait until radio is ready
  time::Timer timeout_timer;
  timeout_timer.start(1000);
  while (!radio::isFree()) {
    if (timeout_timer.isExpired()) {
      return false;
    }
  }

  // Set M0 and M1 to configuration mode
  radio::setMode(radio::Mode::Configuration);
  time::delay(20);

  // Configure radio parameters
  if (!radio::configure(
    radio::BaudRate::BR_115200,
    radio::Parity::None,
    radio::AirSpeed::AS_62500,
    radio::TXPower::P_22dBm)
  ) {
    return false;
  }

  time::delay(20);
  // Set M0 and M1 back to normal mode
  radio::setMode(radio::Mode::Normal);

  // Change UART baud rate to 115200 baud/s
  serial::setBaudRate(RADIO_UART, 115200);

  // Wait until radio is ready
  timeout_timer.start(1000);
  while (!radio::isFree()) {
    if (timeout_timer.isExpired()) {
      return false;
    }
  }
  time::delay(100);

  // Enable interrupts
  LL_USART_EnableIT_RXNE(RADIO_UART);
  LL_USART_EnableIT_ERROR(RADIO_UART);

  return true;
}

/**
 * @brief IMU Configuration.
 */
static inline bool IMU_Config()
{
  bool result = true;

  result &= imu.init();
  result &= imu.enableAccelerometer(sensors::icm::AccelODR::Hz_800, sensors::icm::AccelFS::G_2);
  result &= imu.enableGyroscope(sensors::icm::GyroODR::Hz_800, sensors::icm::GyroFS::DPS_31_25);
  result &= imu.enableFIFO();

  return result;
}

/**
 * @brief Magnetometer Configuration.
 */
static inline bool Mag_Config()
{
  bool result = true;

  result &= mag.init();
  result &= mag.setODR(sensors::lismdl::ODR::Hz_100);
  result &= mag.setMode(sensors::lismdl::Mode::Continuous);
  result &= mag.setLPF();

  return result;
}

/**
 * @brief Barometer Configuration.
 */
static inline bool Baro_Config()
{
  return baro.init(sensors::ms::OSR::OSR_4096, sensors::ms::OSR::OSR_256);
}

/**
 * @brief UBX NEO-M9N Configuration.
 */
static inline void GPS_Config()
{
  // Disable NMEA messages
  const uint8_t stop_nmea_msgs[] = { 0xB5, 0x62, 0x06, 0x8A, 0x09, 0x00, 0x00, 0x01, 0x00, 0x00, 0x02, 0x00, 0x74, 0x10, 0x00, 0x20, 0xB7 };
  ubx::sendCommand(stop_nmea_msgs, sizeof(stop_nmea_msgs));
  time::delay(10);

  // Enable NAV-PVT messages
  const uint8_t enable_nav_pvt_msgs[] =  { 0xB5, 0x62, 0x06, 0x8A, 0x09, 0x00, 0x00, 0x01, 0x00, 0x00, 0x07, 0x00, 0x91, 0x20, 0x01, 0x53, 0x48 };
  ubx::sendCommand(enable_nav_pvt_msgs, sizeof(enable_nav_pvt_msgs));
  time::delay(10);

  // Set update rate to 5 Hz
  const uint8_t set_update_rate[] = { 0xB5, 0x62, 0x06, 0x8A, 0x0A, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x21, 0x30, 0xC8, 0x00, 0xB5, 0x81 };
  ubx::sendCommand(set_update_rate, sizeof(set_update_rate));

  // Enable RXNE and Error interrupts
  LL_USART_EnableIT_RXNE(GPS_UART);
  LL_USART_EnableIT_ERROR(GPS_UART);
}

/**
 * @brief Incoming Radio Messages Handler.
 */
static inline void Radio_Message_Handler()
{
  radio::Message::Command command = radio::rx_message.handle();

  switch (command) {
  case radio::Message::Command::SendAck:
    tx_message.create(radio::Message::Class::Ack, radio::Message::AckType::Acknowledge);
    break;

  case radio::Message::Command::SendNack:
    tx_message.create(radio::Message::Class::Ack, radio::Message::AckType::NotAcknowledge);
    break;

  case radio::Message::Command::SendPingResponse:
    tx_message.create(radio::Message::Class::Ping, radio::Message::PingType::Response);
    break;

  case radio::Message::Command::CalibrateGyro:
    imu.calibrateGyroscope();
    tx_message.create(radio::Message::Class::Ack, radio::Message::AckType::Acknowledge);
    break;

  case radio::Message::Command::CalibrateMag: {
    if (!ubx::gps.hasFix()) {
      tx_message.create(radio::Message::Class::Ack, radio::Message::AckType::NotAcknowledge);
      break;
    }
    // Set magnetic field reference based on current GPS position and date
    geo::Geodetic position = ubx::gps.getPosition();
    geo::Date date = ubx::gps.getDate();
    kalman.magnetic_field_reference = geo::WMM::computeMagneticField(position, date);

    tx_message.create(radio::Message::Class::Ack, radio::Message::AckType::Acknowledge, kalman.magnetic_field_reference);
    break;
  }

  case radio::Message::Command::CalibrateBaro: {
    std::optional<std::pair<float, float>> result;
    while (!(result = baro.process())) {
      time::delay(1);
    }
    sea_level_pressure = result->first;

    tx_message.create(radio::Message::Class::Ack, radio::Message::AckType::Acknowledge, sea_level_pressure);
    break;
  }

  case radio::Message::Command::CalibrateGPS: {
    if (!ubx::gps.hasFix()) {
      tx_message.create(radio::Message::Class::Ack, radio::Message::AckType::NotAcknowledge);
      break;
    }
    // Set reference ECEF position and compute ECEF to ENU rotation matrix
    geo::Geodetic position = ubx::gps.getPosition();
    reference_ecef = geo::ECEF::fromGeodetic(position);
    ecef_to_enu_matrix = geo::ecef_to_enu_matrix(position.latitude, position.longitude);

    tx_message.create(radio::Message::Class::Ack, radio::Message::AckType::Acknowledge, position);
    break;
  }

  case radio::Message::Command::StartKalman: {
    auto imu_data = imu.getLatestFIFOData();
    auto mag_data = mag.getMagneticField();
    if (!imu_data || !mag_data) {
      tx_message.create(radio::Message::Class::Ack, radio::Message::AckType::NotAcknowledge);
      break;
    }
    kalman.x.orientation = ahrs::triad(imu_data->first, *mag_data, kalman.magnetic_field_reference);
    kalman_running = true;
    kalman_timer.start(10);
    tx_message.create(radio::Message::Class::Ack, radio::Message::AckType::Acknowledge);
    break;
  }

  default:
    break;
  }
}

/**
 * @brief  This function is executed in case of error occurrence.
 */
void Error_Handler()
{
  __disable_irq();
  while (1) {
    LL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    LL_mDelay(250); 
  }
}
