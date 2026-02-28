/**
 ******************************************************************************
 * @file    message.cc
 * @brief   This file contains the definition of the timer used for radio
 *          timeouts.
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
#include "radio/radio.hh"

time::Timer radio::free_timer;
