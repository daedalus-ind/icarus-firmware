/**
 ******************************************************************************
 * @file    stm32f4xx_it.hxx
 * @brief   This file contains the headers of the interrupt handlers.
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
#ifndef __STM32F4xx_IT_HXX__
#define __STM32F4xx_IT_HXX__

extern "C" {

  void NMI_Handler();
  void HardFault_Handler();
  void MemManage_Handler();
  void BusFault_Handler();
  void UsageFault_Handler();
  void SVC_Handler();
  void DebugMon_Handler();
  void PendSV_Handler();
  void SysTick_Handler();
  void DMA1_Stream6_IRQHandler();
  void USART2_IRQHandler();
  void USART3_IRQHandler();

}

#endif /* __STM32F4xx_IT_HXX__ */
