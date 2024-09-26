//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#pragma once

#include <utility>
#include "platform/PlatformMacros.h"

namespace ax
{

namespace base64 {

AX_DLL
char const*
get_alphabet();

AX_DLL
signed char const*
get_inverse();

/// Returns max chars needed to encode a base64 string
inline
std::size_t constexpr
encoded_size(std::size_t n)
{
    return 4 * ((n + 2) / 3);
}

/// Returns max bytes needed to decode a base64 string
inline
std::size_t constexpr
decoded_size(std::size_t n)
{
    return n / 4 * 3; // requires n&3==0, smaller
}

/** Encode a series of octets as a padded, base64 string.

    The resulting string will not be null terminated.

    @par Requires

    The memory pointed to by `out` points to valid memory
    of at least `encoded_size(len)` bytes.

    @return The number of characters written to `out`. This
    will exclude any null termination.
*/
AX_DLL
std::size_t
encode(void* dest, void const* src, std::size_t len);

/** Decode a padded base64 string into a series of octets.

    @par Requires

    The memory pointed to by `out` points to valid memory
    of at least `decoded_size(len)` bytes.

    @return The number of octets written to `out`.
*/
AX_DLL
std::size_t decode(void* dest, char const* src, std::size_t len);

} // base64

}
