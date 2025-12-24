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
#pragma once

#include <type_traits>
#include <memory>
#include <string_view>
#include <ranges>
#include <iterator>
#include <string.h>
#include <assert.h>
#include "axmol/tlx/feature_test.hpp"

namespace tlx
{

template <typename, typename = void>
struct has_resize_and_overwrite : std::false_type
{};

template <typename T>
struct has_resize_and_overwrite<
    T,
    std::void_t<decltype(std::declval<T&>().resize_and_overwrite(std::declval<typename T::size_type>(),
                                                                 std::declval<int>()))>> : std::true_type
{};

/* make_unique_for_overwrite since c++20, but not all platformm support */
template <class _Ty, std::enable_if_t<!std::is_array_v<_Ty>, int> = 0>
inline std::unique_ptr<_Ty> make_unique_for_overwrite()
{
    // make a unique_ptr with default initialization
    return std::unique_ptr<_Ty>(new _Ty);
}

template <class _Ty, std::enable_if_t<std::is_array_v<_Ty> && std::is_trivially_copyable_v<_Ty>, int> = 0>
inline std::unique_ptr<_Ty> make_unique_for_overwrite(const size_t _Size)
{
    // make a unique_ptr with default initialization
    using _Elem = std::remove_extent_t<_Ty>;
    return std::unique_ptr<_Ty>(new _Elem[_Size]);
}

// If the _OutCont is `std::vector`, the resize still with zero filled which is not needs at here
// some follow concepts needs standardized in the future, may be c++23
//  - resize_and_overwrite:
//    - https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p1072r10.html
//  - boost vector resize with init behavior parameter
//    - https://github.com/boostorg/container/blob/develop/include/boost/container/vector.hpp
//  - pod_vector
//    -
//    https://stackoverflow.com/questions/15219984/using-vectorchar-as-a-buffer-without-initializing-it-on-resize/15220853#15220853
//    - https://github.com/yasio/yasio/blob/perftest/tests/perf/pod_vector.h
template <class _InIt, class _OutCont, class _Fn>
inline auto resize_and_transform(const _InIt _First, const _InIt _Last, _OutCont& _Dest, _Fn _Func)
{
    auto count = std::distance(_First, _Last);
    _Dest.resize(static_cast<size_t>(count));
    return std::transform(_First, _Last, _Dest.begin(), _Func);
}

template <typename _SeqCont, typename _Operation>
inline void resize_and_overrite(_SeqCont& cont, size_t size, _Operation op)
{
    if constexpr (has_resize_and_overwrite<_SeqCont>::value)
    {
        cont.resize_and_overwrite(size, op);
    }
    else
    {
        cont.resize(size);
        const auto ret = op(cont.data(), size);
        if (ret < size)
            cont.resize(ret);
    }
}

// Convert a subrange into a std::string_view.
// Requirements:
// 1. The subrange must originate from contiguous memory (e.g. a split_view of std::string or std::string_view).
// 2. &*subrgn.begin() is used to obtain a pointer to the first element.
// 3. std::ranges::distance(subrgn) is used to compute the length.
// 4. No copy is performed; the returned string_view references the original buffer.
// 5. If the subrange comes from a non‑contiguous container (like std::list), this is undefined behavior.
template <typename _Subrgn>
inline std::string_view to_string_view(_Subrgn&& subrgn)
{
    return std::string_view{&*subrgn.begin(), static_cast<size_t>(std::ranges::distance(subrgn))};
}


/**
 * @brief Safely copy a string into a fixed-size C-style buffer.
 *
 * Copies characters from the source string into the destination buffer,
 * ensuring that the destination is always null-terminated. At most
 * (buffer_size - 1) characters are copied, so truncation may occur if
 * the source string is longer than the destination capacity.
 *
 * @param dest       Destination buffer (C-style string).
 * @param destSize   Size of the destination buffer in characters.
 * @param src        Source string to copy (std::string_view).
 *
 * @return The number of characters actually copied into the destination
 *         (excluding the terminating null character).
 *
 * @note This function behaves similarly to BSD's strlcpy, but returns
 *       the number of characters copied rather than the full source length.
 *       Callers can detect truncation by comparing the return value with
 *       src.size().
 *
 * Example:
 *   char buf[16];
 *   size_t copied = tlx::strlcpy(buf, sizeof(buf), "Hello, world!");
 *   // buf contains "Hello, world!" truncated if necessary.
 */
template <size_t _N>
inline size_t strlcpy(char (&dest)[_N], std::string_view src)
{
    static_assert(_N > 0, "Destination buffer size must be greater than zero.");
    size_t copy_len = (std::min)(src.size(), _N - 1);
    ::memcpy(dest, src.data(), copy_len);
    dest[copy_len] = '\0';
    return copy_len;
}

inline size_t strlcpy(char* dest, size_t destSize, std::string_view src)
{
    assert(destSize > 1);
    size_t copy_len = (std::min)(src.size(), destSize - 1);
    ::memcpy(dest, src.data(), copy_len);
    dest[copy_len] = '\0';
    return copy_len;
}

}  // namespace tlx
