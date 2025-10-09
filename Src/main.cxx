/**
 ******************************************************************************
 * @file    main.cxx
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
#include "main.hxx"
#include "dma.hxx"
#include "i2c.hxx"
#include "spi.hxx"
#include "usart.hxx"
#include "gpio.hxx"
#include "time.hxx"

#include "sensors/icm-45686.hxx"
#include "sensors/lis2mdl.hxx"
#include "sensors/ms5607.hxx"

#include "ublox/gps.hxx"

#include "radio/radio.hxx"
#include "radio/message.hxx"

#include "geo/ecef.hpp"
#include "geo/enu.hpp"

#include "math/vec3.hxx"
#include "math/mat3x3.hxx"

sensors::ICM_45686 imu(SPI2, IMU_CS_GPIO_Port, IMU_CS_Pin);
sensors::LIS2MDL mag(MAG_I2C);
sensors::MS5607_FSM baro(SPI2, BARO_CS_GPIO_Port, BARO_CS_Pin);

void SystemClock_Config();
bool IMU_Config();
bool Mag_Config();
bool Baro_Config();
void GPS_Config();

/**
 * @brief  The application entry point.
 * @retval int
 */
int main() {

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  /* System interrupt init*/
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* SysTick_IRQn interrupt configuration */
  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),15, 0));

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C3_Init();
  MX_SPI2_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();

  /* Message to be sent */
  radio::Message msg_tx;

  /* Indicate that the system is powered on */
  msg_tx.construct(radio::MessageClass::Status, radio::MessageStatusID::PowerOn);
  msg_tx.send();

  /* Wait 1 second */
  time::delay(1000);

  /* Initialize IMU */
  if (IMU_Config()) {
    msg_tx.construct(radio::MessageClass::Status, radio::MessageStatusID::IMUInitOk);
  } else {
    msg_tx.construct(radio::MessageClass::Status, radio::MessageStatusID::IMUInitError);
  }
  msg_tx.send();
  time::delay(100);

  /* Initialize Magnetometer */
  if (Mag_Config()) {
    msg_tx.construct(radio::MessageClass::Status, radio::MessageStatusID::MagnetometerInitOk);
  } else {
    msg_tx.construct(radio::MessageClass::Status, radio::MessageStatusID::MagnetometerInitError);
  }
  msg_tx.send();
  time::delay(100);

  /* Initialize Barometer */
  if (Baro_Config()) {
    msg_tx.construct(radio::MessageClass::Status, radio::MessageStatusID::BarometerInitOk);
  } else {
    msg_tx.construct(radio::MessageClass::Status, radio::MessageStatusID::BarometerInitError);
  }
  msg_tx.send();
  time::delay(100);

  /* Configure GPS */
  GPS_Config();

  /* Infinite loop */
  volatile bool gps_fix_acquired = false;
  time::Timer gps_sats_timer;

  float sea_level_pressure = 1013.25f;
  geo::ECEF reference_position;
  math::Mat3x3 ecef_to_enu_rotation;

  
  while (true) { 
    if (!msg_tx.is_new()) {
      radio::Command cmd = radio::message_rx.handle();

      time::Timer timer;
      switch (cmd) {
      case radio::Command::SendAck:
        msg_tx.construct(radio::MessageClass::Ack, radio::MessageAckID::Ack);
        break;

      case radio::Command::SendNack:
        msg_tx.construct(radio::MessageClass::Ack, radio::MessageAckID::Nack);
        break; 
      
      case radio::Command::SendPong:
        msg_tx.construct(radio::MessageClass::Ping, radio::MessagePingID::Pong);
        break;

      case radio::Command::CalibrateGyroscope:
        if (imu.calibrate_gyro(500)) {
          msg_tx.construct(radio::MessageClass::Configure, radio::MessageConfigureID::Done);
        } else {
          msg_tx.construct(radio::MessageClass::Configure, radio::MessageConfigureID::Error);
        }
        break;
      
      case radio::Command::CalibrateReferencePressure:
        timer.start(100);
        while (!baro.process(sea_level_pressure)) {
          if (timer.is_expired()) {
            msg_tx.construct(radio::MessageClass::Configure, radio::MessageConfigureID::Error);
            break;
          }
        }
        msg_tx.construct(radio::MessageClass::Configure, radio::MessageConfigureID::Done, sea_level_pressure);

      case radio::Command::CalibrateReferencePosition:
        if (ubx::gps.has_fix()) {
          auto [lat, lon, alt] = ubx::gps.get_lla();
          reference_position = geo::ECEF::from_geodetic(lat, lon, alt);
          ecef_to_enu_rotation = geo::ecef_to_enu_matrix(lat, lon);
          msg_tx.construct(radio::MessageClass::Configure, radio::MessageConfigureID::Done, lat, lon, alt);
        } else {
          msg_tx.construct(radio::MessageClass::Configure, radio::MessageConfigureID::Error);
        }
        break;

      default:
        break;
      }
    }

    if (!msg_tx.is_new()) {
      if (!gps_fix_acquired && ubx::gps.has_fix()) {
        gps_fix_acquired = true;
        msg_tx.construct(radio::MessageClass::Status, radio::MessageStatusID::GPSFixAcquired);
        gps_sats_timer.start(3000);
      } else if (gps_fix_acquired && !ubx::gps.has_fix()) {
        gps_fix_acquired = false;
        msg_tx.construct(radio::MessageClass::Status, radio::MessageStatusID::GPSFixLost);
        gps_sats_timer.stop();
      } else if (gps_fix_acquired && gps_sats_timer.is_expired()) {
        msg_tx.construct(radio::MessageClass::Status, radio::MessageStatusID::GPSFixSats, ubx::gps.get_sats());
        gps_sats_timer.start(3000);
      }
    }

    if (radio::has_been_free_for(50) && msg_tx.is_new()) {
      msg_tx.send();
    } 
  }

}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config() {

  LL_FLASH_SetLatency(LL_FLASH_LATENCY_5);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_5) {}
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  LL_RCC_HSE_Enable();

  /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1) {}
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_8, 168, LL_RCC_PLLP_DIV_2);
  LL_RCC_PLL_Enable();

  /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1) {}
  while (LL_PWR_IsActiveFlag_VOS() == 0) {}
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_4);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

  /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {}

  LL_Init1msTick(168000000);
  LL_SetSystemCoreClock(168000000);

  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
  LL_SYSTICK_EnableIT();

}

bool IMU_Config() {

  bool rslt = true; 

  rslt &= imu.init();
  rslt &= imu.enable_accel(sensors::icm::AccelODR::HZ_800, sensors::icm::AccelFS::G_2);
  rslt &= imu.enable_gyro(sensors::icm::GyroODR::HZ_800, sensors::icm::GyroFS::DPS_15_625);
  rslt &= imu.enable_fifo();

  return rslt;

}

bool Mag_Config() {

  bool rslt = true;

  rslt &= mag.init();
  rslt &= mag.set_odr(sensors::lismdl::ODR::HZ_100);
  rslt &= mag.set_mode(sensors::lismdl::Mode::CONTINUOUS);

  return rslt;

}

bool Baro_Config() {

  bool rslt = true;

  rslt &= baro.init(sensors::ms::OSR::OSR_4096, sensors::ms::OSR::OSR_256);

  return rslt;

}

/**
 * @brief Configure GPS module
 * @retval None
 */
void GPS_Config() {

  /* Stop NMEA messages */
  static const uint8_t stop_nmea_msgs[] = { 0xB5, 0x62, 0x06, 0x8A, 0x09, 0x00, 0x00, 0x01, 0x00, 0x00, 0x02, 0x00, 0x74, 0x10, 0x00, 0x20, 0xB7 };
  serial::write(GPS_UART, stop_nmea_msgs, sizeof(stop_nmea_msgs), 10);
  time::delay(10);

  /* Enable NAV-PVT messages */
  static const uint8_t enable_nav_pvt_msgs[] =  { 0xB5, 0x62, 0x06, 0x8A, 0x09, 0x00, 0x00, 0x01, 0x00, 0x00, 0x07, 0x00, 0x91, 0x20, 0x01, 0x53, 0x48 };
  serial::write(GPS_UART, enable_nav_pvt_msgs, sizeof(enable_nav_pvt_msgs), 10);
  time::delay(10);

  /* Set update rate to 5 Hz */
  static const uint8_t set_update_rate[] = { 0xB5, 0x62, 0x06, 0x8A, 0x0A, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x21, 0x30, 0xC8, 0x00, 0xB5, 0x81 };
  serial::write(GPS_UART, set_update_rate, sizeof(set_update_rate), 10);

  /* Enable RXNE and Error interrupts */
  LL_USART_EnableIT_RXNE(GPS_UART);
  LL_USART_EnableIT_ERROR(GPS_UART);

}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler() {

  __disable_irq();
  while (true) {}

}
