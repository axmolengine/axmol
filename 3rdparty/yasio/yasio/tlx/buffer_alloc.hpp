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

#ifndef YASIO__BUFFER_ALLOC_HPP
#define YASIO__BUFFER_ALLOC_HPP
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdexcept>
#include <utility>
#include "yasio/compiler/feature_test.hpp"
#include "yasio/tlx/type_traits.hpp"

namespace tlx
{
template <typename T>
struct crt_buffer_allocator {
  using value_type      = T;
  using pointer         = T*;
  using const_pointer   = const T*;
  using size_type       = std::size_t;
  using difference_type = std::ptrdiff_t;

  // allocate n elements using CRT malloc
  pointer allocate(size_type n)
  {
    if (n > max_size())
      throw std::bad_alloc();
    void* p = ::malloc(n * sizeof(T));
    if (!p)
      throw std::bad_alloc();
    return static_cast<pointer>(p);
  }

  // deallocate memory using CRT free
  void deallocate(pointer p, size_type /*n*/) noexcept { ::free(p); }

  // optional reallocate (non-standard, but useful for vector)
  pointer reallocate(pointer p, size_type old_count, size_type new_count)
  {
    void* np = ::realloc(p, new_count * sizeof(T));
    if (!np)
      throw std::bad_alloc();
    return static_cast<pointer>(np);
  }

  // max_size consistent with std::allocator
  size_type max_size() const noexcept { return static_cast<size_type>(-1) / sizeof(T); }

  // equality operators required by standard
  template <class U>
  struct rebind {
    using other = crt_buffer_allocator<U>;
  };

  bool operator==(const crt_buffer_allocator&) const noexcept { return true; }
  bool operator!=(const crt_buffer_allocator&) const noexcept { return false; }
};

} // namespace tlx

#endif
