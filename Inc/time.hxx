/**
  ******************************************************************************
  * @file    time.hxx
  * @brief   This file provides simple time management
  *          utilities.
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
#ifndef __TIME_HXX__
#define __TIME_HXX__

#include <cstdint>

namespace time {

  /**
   * @brief Milliseconds since the system started.
   */
  extern volatile uint32_t ms_ticks;
  
  /**
   * @brief Returns the current time in milliseconds.
   */
  inline uint32_t millis() {
    return ms_ticks;
  }

  /**
   * @brief Increments the time by one millisecond.
   * @note  This function should be only called from a timer interrupt handler.
   */
  inline void inc() {
    ms_ticks += 1;
  }

  /**
   * @brief Delays the program for a specified number of milliseconds.
   */
  inline void delay(uint32_t ms) {
    uint32_t start = millis();
    while (millis() - start < ms) { }
  }

  /**
   * @brief Simple timer class for managing timeouts.
   */
  class Timer {
  public:
    /* Constructor */
    Timer() : _start_time{0}, _duration{0}, _running{false} {}

    /**
     * @brief Starts the timer with the specified duration in milliseconds.
     */
    inline void start(uint32_t duration_ms) {
      _start_time = millis();
      _duration = duration_ms;
      _running = true;
    }

    /**
     * @brief Stops the timer.
     */ 
    inline void stop() {
      _running = false;
    }

    /**
     * @brief Checks if the timer is currently running.
     */
    inline bool is_running() const {
      return _running;
    }
    
    /**
     * @brief Checks if the timer has expired.
     */
    inline bool is_expired() const {
      if (!_running) return false;
      return (millis() - _start_time) >= _duration;
    }

  private:
    uint32_t _start_time;
    uint32_t _duration;
    bool _running;
  };

}

#endif /* __TIME_HXX__ */
