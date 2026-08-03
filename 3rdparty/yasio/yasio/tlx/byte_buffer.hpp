//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
/*
The MIT License (MIT)

Copyright (c) 2012-2025 HALX99

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
#ifndef YASIO__BYTE_BUFFER_HPP
#define YASIO__BYTE_BUFFER_HPP
#include <span>
#include "yasio/tlx/array_buffer.hpp"

namespace tlx
{
// basic_byte_buffer restricted to byte types
template <typename _Ty, typename _Alloc = tlx::crt_buffer_allocator<_Ty>>
class basic_byte_buffer : public tlx::array_buffer<_Ty, _Alloc> {
  static_assert(tlx::is_byte_type<_Ty>::value, "basic_byte_buffer only supports byte types!");
  using base_type = tlx::array_buffer<_Ty, _Alloc>;

public:
  using base_type::base_type;
  using base_type::operator=;

  std::span<char> as_chars() & noexcept
  {
    return {reinterpret_cast<char*>(this->data()), this->size()};
  }
  std::span<const char> as_chars() const & noexcept
  {
    return {reinterpret_cast<const char*>(this->data()), this->size()};
  }
};

using byte_buffer = basic_byte_buffer<unsigned char>;

} // namespace tlx
#endif
