#pragma once

/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
// llvm clang and Apple clang std::from_chars not support floating-point
// use fast_float instead: https://github.com/fastfloat/fast_float
#if !defined(_WIN32) && defined(__clang__)
#    include "fast_float/fast_float.h"
namespace tlx
{
using fast_float::from_chars;
using from_chars_result = fast_float::from_chars_result;
using chars_format      = fast_float::chars_format;
}  // namespace tlx

#else
#    include <charconv>
namespace tlx
{
using std::from_chars;
using from_chars_result = std::from_chars_result;
using chars_format      = std::chars_format;
}  // namespace tlx
#endif

#include <span>
#include <type_traits>
#include <ctype.h>

namespace tlx
{

template <typename _Tp>
using chars_format_t = std::conditional_t<std::is_integral_v<_Tp>, int, ::tlx::chars_format>;

template <typename _Tp, typename Enable = void>
struct default_chars_format;

template <typename _Tp>
struct default_chars_format<_Tp, std::enable_if_t<std::is_integral_v<_Tp>>>
{
    static constexpr int value = 10;
};

template <typename _Tp>
struct default_chars_format<_Tp, std::enable_if_t<std::is_floating_point_v<_Tp>>>
{
    static constexpr ::tlx::chars_format value = ::tlx::chars_format::general;
};

template <typename _Tp>
inline auto from_chars(std::span<const char> chars,
                       _Tp& outval,
                       chars_format_t<_Tp> fmt = default_chars_format<_Tp>::value)
{
    return ::tlx::from_chars(chars.data(), chars.data() + chars.size(), outval, fmt);
}

template <typename... Ts>
inline ::tlx::from_chars_result from_chars(std::span<const char> chars, char sep, Ts&... outvals)
{
    auto first = chars.data();
    auto last  = chars.data() + chars.size();

    auto&& skip_spaces = [last](const char*& p) {
        while (p != last && ::isspace(static_cast<unsigned char>(*p)))
            ++p;
    };

    ::tlx::from_chars_result res{first, std::errc()};

    auto&& parse_one = [&skip_spaces, &first, last, &res, sep](auto& out) -> bool {
        skip_spaces(first);

        res = ::tlx::from_chars(first, last, out);
        if (res.ec != std::errc())
            return false;  // stop on first error

        first = res.ptr;
        skip_spaces(first);
        if (first != last && *first == sep)
            ++first;
        return true;
    };

    (parse_one(outvals) && ...);
    return res;
}

}  // namespace tlx
