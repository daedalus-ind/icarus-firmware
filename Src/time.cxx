/**
 ******************************************************************************
 * @file    time.cxx
 * @brief   This file defines the tick counter 
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
#include "time.hxx"

volatile uint32_t time::ms_ticks = 0;
