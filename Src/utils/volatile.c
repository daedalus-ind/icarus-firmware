/**
 ******************************************************************************
 * @file    volatile.c
 * @brief   This file contains the definitions for volatile memory
 *          operations.
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
#include "utils/volatile.h"

volatile void* memset_volatile(volatile void* dest, int ch, size_t count)
{
  volatile unsigned char* ptr = (volatile unsigned char*)dest;
  while (count--) {
    *ptr++ = (unsigned char)ch;
  }
  return dest;  
}

volatile void* memcpy_volatile(volatile void* dest, const volatile void* restrict src, size_t count)
{
  volatile unsigned char* d = (volatile unsigned char*)dest;
  const volatile unsigned char* s = (const volatile unsigned char*)src;
  while (count--) {
    *d++ = *s++;
  }
  return dest;  
}
