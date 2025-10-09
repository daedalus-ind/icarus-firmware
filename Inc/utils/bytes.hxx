/**
 ******************************************************************************
 * @file    bytes.hxx
 * @brief   This file provides simple byte packing utilities.
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
#ifndef __UTILS_BYTES_HXX__
#define __UTILS_BYTES_HXX__

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace utils {

  /**
   * @brief Packs multiple values into a byte buffer.
   */
  template <typename T, typename ...Args>
  constexpr void pack_bytes(uint8_t* buffer, size_t& offset, T const& value, Args const& ...args) {
    std::memcpy(buffer + offset, &value, sizeof(T)); 
    offset += sizeof(T);
    if constexpr (sizeof...(args) > 0)
      pack_bytes(buffer, offset, args...);
  }

}

#endif /* __UTILS_BYTES_HXX__ */
