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
#ifndef YASIO__BYTE_BUFFER_HPP
#define YASIO__BYTE_BUFFER_HPP
#include "yasio/pod_vector.hpp"

namespace yasio
{

template <typename _Ty, typename _Alloc = buffer_allocator<_Ty>, enable_if_t<is_byte_type<_Ty>::value, int> = 0>
using basic_byte_buffer = array_buffer<_Ty, _Alloc>;

using sbyte_buffer = basic_byte_buffer<char>;
using byte_buffer  = basic_byte_buffer<unsigned char>;

} // namespace yasio
#endif
