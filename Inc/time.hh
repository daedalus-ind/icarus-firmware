/**
 ******************************************************************************
 * @file    time.hh
 * @brief   This file contains all the function definitions for 
 *          time management.
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
#ifndef __TIME_HH__
#define __TIME_HH__

#include "main.hh"
#include <cstdint>

namespace time {

  /**
   * @brief Current system time in milliseconds.
   */
  extern volatile uint32_t ms_ticks;

  /**
   * @brief Get the current system time in milliseconds.
   * @return Current time in milliseconds.
   */
  inline uint32_t now() 
  {
    return ms_ticks;
  }

  /**
   * @brief Get the current system time in seconds.
   * @return Current time in seconds.
   */
  inline uint32_t seconds() 
  {
    return ms_ticks / 1000;
  }

  /**
   * @brief Delays execution for a specified number of milliseconds.
   * @param ms Number of milliseconds to delay.
   */
  inline void delay(uint32_t ms)
  {
    uint32_t start = now();
    while (now() - start < ms) {}
  }

  class Timer {
  public:
    /**
     * @brief Construct a new Timer object.
     */
    inline Timer() : _start{0}, _duration{0}, _running{false} {}
    
    /**
     * @brief Starts the timer with the specified duration.
     * @param duration Duration in milliseconds.
     */
    inline void start(uint32_t duration) 
    {
      _start = now();
      _duration = duration;
      _running = true;
    }

    /**
     * @brief Stops the timer.
     */
    inline void stop() 
    {
      _running = false;
    }

    /**
     * @brief Checks if the timer is currently running.
     * @return `true` if the timer is running, `false` otherwise.
     */
    inline bool isRunning() const 
    {
      return _running;
    }

    /**
     * @brief Returns the elapsed time since the timer was started.
     * @return Elapsed time in milliseconds. 
     */
    inline uint32_t getElapsed() const
    {
      if (!_running) 
        return 0;
      return now() - _start;
    }

    /**
     * @brief Checks if the timer has expired.
     * @return `true` if the timer has expired, `false` otherwise.
     */
     inline bool isExpired() const
     {
      return getElapsed() >= _duration;
    }

  private:
    uint32_t _start;
    uint32_t _duration;
    bool _running;
  };

  /**
   * @brief Scoped timer that starts upon creation.
   */
  class ScopedTimer {
  public:
    /**
     * @brief Construct a new Scoped Timer object and starts the timer.
     * @param duration Duration in milliseconds.
     */
    inline ScopedTimer(uint32_t duration) 
      : _timer{} 
    {
      _timer.start(duration);
    }

    /**
     * @brief Checks if the timer has expired.
     * @return `true` if the timer has expired, `false` otherwise.
     */
    inline bool isExpired() const 
    {
      return _timer.isExpired();
    }

  private:
    Timer _timer;
  };

}

#endif // __TIME_HH__
