/**
 ******************************************************************************
 * @file    stm32f4xx_it.cxx
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
#include "main.hxx"
#include "stm32f4xx_it.hxx"
#include "time.hxx"
#include "ublox/parser.hxx"
#include "radio/parser.hxx"

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/

/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler() {
  while (true) {}
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler() {
  while (true) {}
}

/**
 * @brief This function handles Memory management fault.
 */
void MemManage_Handler() {
  while (true) {}
}

/**
 * @brief This function handles Pre-fetch fault, memory access fault.
 */
void BusFault_Handler() {
  while (true) {}
}

/**
 * @brief This function handles Undefined instruction or illegal state.
 */
void UsageFault_Handler() {
  while (true) {}
}

/**
 * @brief This function handles System service call via SWI instruction.
 */
void SVC_Handler() {

}

/**
 * @brief This function handles Debug monitor.
 */
void DebugMon_Handler() {

}

/**
 * @brief This function handles Pendable request for system service.
 */
void PendSV_Handler() {

}

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler() {
  time::inc();
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
 * @brief This function handles DMA1 stream6 global interrupt.
 */
void DMA1_Stream6_IRQHandler() {
  if (LL_DMA_IsActiveFlag_TC6(DMA1)) {
    LL_DMA_ClearFlag_TC6(DMA1);
  } else if (LL_DMA_IsActiveFlag_TE6(DMA1)) {
    LL_DMA_ClearFlag_TE6(DMA1);
  }
}

/**
 * @brief This function handles USART2 global interrupt.
 */
void USART2_IRQHandler() {
  if (LL_USART_IsActiveFlag_RXNE(USART2) && LL_USART_IsEnabledIT_RXNE(USART2)) {
    uint8_t byte = LL_USART_ReceiveData8(USART2);
    radio::parser.process(byte);
  } else if (LL_USART_IsActiveFlag_ORE(USART2)) {
    LL_USART_ClearFlag_ORE(USART2);
  } else if (LL_USART_IsActiveFlag_NE(USART2)) {
    LL_USART_ClearFlag_NE(USART2);
  } else if (LL_USART_IsActiveFlag_FE(USART2)) {
    LL_USART_ClearFlag_FE(USART2);
  } else if (LL_USART_IsActiveFlag_PE(USART2)) {
    LL_USART_ClearFlag_PE(USART2);
  }
}

/**
 * @brief This function handles USART3 global interrupt.
 */
void USART3_IRQHandler() {
  if (LL_USART_IsActiveFlag_RXNE(USART3) && LL_USART_IsEnabledIT_RXNE(USART3)) {
    uint8_t byte = LL_USART_ReceiveData8(USART3);
    ubx::parser.process(byte);
  } else if (LL_USART_IsActiveFlag_ORE(USART3)) {
    LL_USART_ClearFlag_ORE(USART3);
  } else if (LL_USART_IsActiveFlag_NE(USART3)) {
    LL_USART_ClearFlag_NE(USART3);
  } else if (LL_USART_IsActiveFlag_FE(USART3)) {
    LL_USART_ClearFlag_FE(USART3);
  } else if (LL_USART_IsActiveFlag_PE(USART3)) {
    LL_USART_ClearFlag_PE(USART3);
  }
}
