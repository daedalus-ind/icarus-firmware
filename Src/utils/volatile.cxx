/**
  ******************************************************************************
  * @file    volatile.cxx
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
#include "utils/volatile.hxx"

volatile void* utils::memset_volatile(volatile void* dest, int value, size_t count) {
  auto d = static_cast<volatile unsigned char*>(dest);
  while (count--) {
    *d++ = static_cast<unsigned char>(value);
  }
  return dest;
}

volatile void* utils::memcpy_volatile(volatile void* dest, const volatile void* __restrict src, size_t count) {
  auto d = static_cast<volatile unsigned char*>(dest);
  auto s = static_cast<const volatile unsigned char*>(src);
  while (count--) {
    *d++ = *s++;
  }
  return dest;
}
