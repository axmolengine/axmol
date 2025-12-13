#pragma once

#include "xxhash/xxhash.h"
#include <stdint.h>

namespace tlx
{
inline uintptr_t hash_bytes(const void* data, size_t length, uintptr_t seed = 0)
{
    if constexpr (sizeof(void*) == 8)
        return XXH64(data, length, seed);
    else
        return XXH32(data, length, static_cast<unsigned int>(seed));
}
inline uint64_t hash64_bytes(const void* data, size_t length, uint64_t seed = 0)
{
    return XXH64(data, length, seed);
}
inline uint32_t hash32_bytes(const void* data, size_t length, uint32_t seed = 0)
{
    return XXH32(data, length, seed);
}
}  // namespace tlx
