/**
 ******************************************************************************
 * @file    radio.hxx
 * @brief   This file contains functions for handling the radio module.
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
#ifndef __RADIO_RADIO_HXX__
#define __RADIO_RADIO_HXX__

#include "main.hxx"
#include "time.hxx"

namespace radio {

  /**
   * @brief Check if the radio module is free (not transmitting nor receiving).
   */
  inline bool is_free() {
    return LL_GPIO_IsInputPinSet(RADIO_AUX_GPIO_Port, RADIO_AUX_Pin) == 1;
  }

  /**
   * @brief Check if the radio module has been free for a certain amount of time.
   */
  inline bool has_been_free_for(uint32_t ms) {
    static time::Timer timer;
    if (is_free()) {
      if (!timer.is_running())
        timer.start(ms);
      return timer.is_expired();
    } else {
      timer.stop();
      return false;
    }
  }

}

#endif /* __RADIO_RADIO_HXX__ */
