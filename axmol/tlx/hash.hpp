#pragma once

#include "xxhash/xxhash.h"
#include <stdint.h>

namespace axstd
{
inline uintptr_t hash_bytes(const void* data, size_t length, uintptr_t seed = 0)
{
    if constexpr (sizeof(uintptr_t) == 64)
        return XXH64(data, length, seed);
    else
        return XXH32(data, length, static_cast<unsigned int>(seed));
}
}  // namespace axstd
