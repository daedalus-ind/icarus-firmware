/**
 ******************************************************************************
 * @file    bytes.hh
 * @brief   This file contains template functions for packing values 
 *          into byte buffers.
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
#ifndef __BYTES_H__
#define __BYTES_H__

#include <cstddef>
#include <cstring>

namespace utils {

  /**
   * @brief Packs multiple values into a byte buffer.
   * @tparam T The type of the current value to pack.
   * @tparam Args The types of the remaining values to pack.
   * @param buffer The byte buffer to pack the values into.
   * @param offset The current offset in the buffer where the next value will be packed.
   * @param value The current value to pack.
   * @param args The remaining values to pack.
   */
  template <typename T, typename ...Args>
  constexpr void pack_bytes(uint8_t* buffer, size_t& offset, const T& value, const Args& ...args) 
  {
    std::memcpy(buffer + offset, &value, sizeof(T));
    offset += sizeof(T);
    if constexpr (sizeof...(args) > 0) {
      pack_bytes(buffer, offset, args...);
    }
  }

}

#endif // __BYTES_H__
