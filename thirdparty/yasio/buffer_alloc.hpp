//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
/*
The MIT License (MIT)

Copyright (c) 2012-2023 HALX99

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

#ifndef YASIO__BUFFER_ALLOC_HPP
#define YASIO__BUFFER_ALLOC_HPP
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <type_traits>
#include <stdexcept>

#define _YASIO_VERIFY_RANGE(cond, mesg)                 \
  do                                                    \
  {                                                     \
    if (cond)                                           \
      ; /* contextually convertible to bool paranoia */ \
    else                                                \
    {                                                   \
      throw std::out_of_range(mesg);                    \
    }                                                   \
                                                        \
  } while (false)

namespace yasio
{
template <bool _Test, class _Ty = void>
using enable_if_t = typename ::std::enable_if<_Test, _Ty>::type;

template <typename _Elem>
struct is_byte_type {
  static const bool value = std::is_same<_Elem, char>::value || std::is_same<_Elem, unsigned char>::value;
};

template <typename _Elem, enable_if_t<std::is_integral<_Elem>::value, int> = 0>
struct default_buffer_allocator {
  static _Elem* reallocate(void* block, size_t /*size*/, size_t new_size) { return static_cast<_Elem*>(::realloc(block, new_size * sizeof(_Elem))); }
  static void deallocate(void* block, size_t /*size*/) { ::free(block); }
};
} // namespace yasio

#endif
