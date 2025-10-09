/**
 ******************************************************************************
 * @file    volatile.hxx
 * @brief   This file provides simple volatile memory access utilities.
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
#ifndef __UTILS_VOLATILE_HXX__
#define __UTILS_VOLATILE_HXX__

#include <cstddef>

namespace utils {

  /**
   * @brief Volatile version of memset.
   */
  volatile void* memset_volatile(volatile void* dest, int value, size_t count);
  
  /**
   * @brief Volatile version of memcpy.
   */
  volatile void* memcpy_volatile(volatile void* dest, const volatile void* __restrict src, size_t count);

}

#endif /* __UTILS_VOLATILE_HXX__ */
