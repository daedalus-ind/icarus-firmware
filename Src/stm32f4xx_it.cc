/**
 ******************************************************************************
 * @file    stm32f4xx_it.cc
 * @brief   Interrupt Service Routines.
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
#include "stm32f4xx_ll_usart.h"
#include "time.hh"
#include "stm32f4xx_it.hh"

#include "radio/parser.hh"
#include "ublox/parser.hh"

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/

/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void)
{
  while (1) {}
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void)
{
  while (1) {}
}

/**
 * @brief This function handles Memory management fault.
 */
void MemManage_Handler(void)
{
  while (1) {}
}

/**
 * @brief This function handles Pre-fetch fault, memory access fault.
 */
void BusFault_Handler(void)
{
  while (1) {}
}

/**
 * @brief This function handles Undefined instruction or illegal state.
 */
void UsageFault_Handler(void)
{
  while (1) {}
}

/**
 * @brief This function handles System service call via SWI instruction.
 */
void SVC_Handler(void)
{

}

/**
 * @brief This function handles Debug monitor.
 */
void DebugMon_Handler(void)
{

}

/**
 * @brief This function handles Pendable request for system service.
 */
void PendSV_Handler(void)
{

}

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void)
{
  time::ms_ticks += 1;
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
 * @brief This function handles USART2 global interrupt.
 */
void USART2_IRQHandler(void)
{
  if (USART2->SR & LL_USART_SR_RXNE) {
    uint8_t byte = LL_USART_ReceiveData8(USART2);
    radio::parser.parseByte(byte);
  } else if (USART2->SR & LL_USART_SR_ORE) {
    (void)USART2->SR;
    (void)USART2->DR;
  } else if (USART2->SR & LL_USART_SR_FE) {
    (void)USART2->SR;
    (void)USART2->DR;
  } else if (USART2->SR & LL_USART_SR_NE) {
    (void)USART2->SR;
    (void)USART2->DR;
  }
}

/**
 * @brief This function handles USART3 global interrupt.
 */
void USART3_IRQHandler(void)
{
  if (USART3->SR & LL_USART_SR_RXNE) {
    uint8_t byte = LL_USART_ReceiveData8(USART3);
    ubx::parser.parseByte(byte);
  } else if (USART3->SR & LL_USART_SR_ORE) {
    (void)USART3->SR;
    (void)USART3->DR;
  } else if (USART3->SR & LL_USART_SR_FE) {
    (void)USART3->SR;
    (void)USART3->DR;
  } else if (USART3->SR & LL_USART_SR_NE) {
    (void)USART3->SR;
    (void)USART3->DR;
  }
}
