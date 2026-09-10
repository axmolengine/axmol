/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include "fmt/format.h"
#include <iterator>

namespace fmt
{
// Formats into a std::string, clearing it first
template <typename... _Args>
inline std::string& format_to(std::string& out, format_string<_Args...> fmt, _Args&&... args)
{
    out.clear();
    vformat_to(std::back_inserter(out), fmt, fmt::make_format_args(args...));
    return out;
}

// Formats into a C-style char buffer and always adds a trailing '\0'
template <size_t _Size, typename... _Args>
inline std::string_view format_to_z(char (&out)[_Size], format_string<_Args...> fmt, _Args&&... args)
{
    static_assert(_Size > 1, "Buffer size must be greater than 1");
    const auto result = fmt::format_to_n(out, _Size - 1, fmt, std::forward<_Args>(args)...);
    *result.out       = '\0';
    return std::string_view(out, (std::min)(result.size, static_cast<size_t>(_Size - 1)));
}
}  // namespace fmt
