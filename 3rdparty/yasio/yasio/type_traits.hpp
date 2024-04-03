//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
/*
The MIT License (MIT)

Copyright (c) 2012-2024 HALX99

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <cstddef>
#include <type_traits>
#include "yasio/sz.hpp"

namespace yasio
{
template <typename _Ty>
struct aligned_storage_size {
  static const size_t value = YASIO_SZ_ALIGN(sizeof(_Ty), sizeof(std::max_align_t));
};
template <typename _Ty>
struct is_aligned_storage {
  static const bool value = aligned_storage_size<_Ty>::value == sizeof(_Ty);
};
template <class _Iter>
struct is_iterator : public std::integral_constant<bool, !std::is_integral<_Iter>::value> {};

template <bool _Test, class _Ty = void>
using enable_if_t = typename ::std::enable_if<_Test, _Ty>::type;

template <typename _Ty>
struct is_byte_type {
  static const bool value = std::is_same<_Ty, char>::value || std::is_same<_Ty, unsigned char>::value;
};

template <typename _Ty>
struct is_char_type {
  static const bool value = std::is_integral<_Ty>::value && sizeof(_Ty) <= sizeof(char32_t);
};

} // namespace yasio
