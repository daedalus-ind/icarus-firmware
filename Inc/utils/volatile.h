/**
 ******************************************************************************
 * @file    volatile.h
 * @brief   This file contains the declarations for volatile memory
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
#ifndef __VOLATILE_H__
#define __VOLATILE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/**
 * @brief Set a block of memory to a specified value, treating the destination as volatile.
 * @param dest Pointer to the destination memory block (volatile).
 * @param ch   Value to set (converted to unsigned char).
 * @param count Number of bytes to set.
 * @return Pointer to the destination memory block.
 */
volatile void* memset_volatile(volatile void* dest, int ch, size_t count);

/**
 * @brief Copy a block of memory from source to destination, treating both as volatile.
 * @param dest Pointer to the destination memory block (volatile).
 * @param src  Pointer to the source memory block (volatile).
 * @param count Number of bytes to copy.
 * @return Pointer to the destination memory block.
 */
volatile void* memcpy_volatile(volatile void* dest, const volatile void* __restrict__ src, size_t count);

#ifdef __cplusplus
}
#endif

#endif // __VOLATILE_H__
