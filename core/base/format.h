/****************************************************************************
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmol.dev/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#ifndef _AX_FORMAT_H_
#define _AX_FORMAT_H_

#include "fmt/format.h"

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
    return std::string_view(out, std::min(result.size, static_cast<size_t>(_Size - 1)));
}
}  // namespace fmt

#endif
