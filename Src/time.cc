/**
 ******************************************************************************
 * @file    time.cc
 * @brief   This file provides the definition of the counter for system time. 
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
#include "time.hh"

volatile uint32_t time::ms_ticks = 0;
