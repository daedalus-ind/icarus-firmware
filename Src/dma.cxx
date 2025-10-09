/**
 ******************************************************************************
 * @file    dma.cxx
 * @brief   This file provides code for the configuration
 *          of all the requested memory to memory DMA transfers.
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
#include "dma.hxx"

/*----------------------------------------------------------------------------*/
/* Configure DMA                                                              */
/*----------------------------------------------------------------------------*/

/**
 * Enable DMA controller clock
 */
void MX_DMA_Init() {

  /* Init with LL driver */
  /* DMA controller clock enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

  /* DMA interrupt init */
  /* DMA1_Stream6_IRQn interrupt configuration */
  NVIC_SetPriority(DMA1_Stream6_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(DMA1_Stream6_IRQn);

  LL_DMA_EnableIT_TC(DMA1, LL_DMA_STREAM_6);
  LL_DMA_EnableIT_TE(DMA1, LL_DMA_STREAM_6);
  
}
