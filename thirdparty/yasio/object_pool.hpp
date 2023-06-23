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
#ifndef YASIO__OBJECT_POOL_HPP
#define YASIO__OBJECT_POOL_HPP

#include "yasio/impl/object_pool.hpp"

namespace yasio
{
struct null_mutex {
  void lock() {}
  void unlock() {}
};
template <typename _Ty, typename _Mutex = ::yasio::null_mutex>
class object_pool : public detail::object_pool {
public:
  object_pool(size_t _ElemCount = 128) : detail::object_pool(detail::aligned_storage_size<_Ty>::value, _ElemCount) {}

  template <typename... _Types>
  _Ty* create(_Types&&... args)
  {
    return new (allocate()) _Ty(std::forward<_Types>(args)...);
  }

  void destroy(void* _Ptr)
  {
    ((_Ty*)_Ptr)->~_Ty(); // call the destructor
    release(_Ptr);
  }

  void* allocate()
  {
    std::lock_guard<_Mutex> lk(this->mutex_);
    return get();
  }

  void deallocate(void* _Ptr)
  {
    std::lock_guard<_Mutex> lk(this->mutex_);
    release(_Ptr);
  }

  _Mutex mutex_;
};

#define DEFINE_OBJECT_POOL_ALLOCATION_ANY(ELEMENT_TYPE, ELEMENT_COUNT, MUTEX_TYPE) \
public:                                                                            \
  using object_pool_type = yasio::object_pool<ELEMENT_TYPE, MUTEX_TYPE>;           \
  static void* operator new(size_t /*size*/)                                       \
  {                                                                                \
    return get_pool().allocate();                                                  \
  }                                                                                \
                                                                                   \
  static void* operator new(size_t /*size*/, std::nothrow_t)                       \
  {                                                                                \
    return get_pool().allocate();                                                  \
  }                                                                                \
                                                                                   \
  static void operator delete(void* p)                                             \
  {                                                                                \
    get_pool().deallocate(p);                                                      \
  }                                                                                \
                                                                                   \
  static object_pool_type& get_pool()                                              \
  {                                                                                \
    static object_pool_type s_pool(ELEMENT_COUNT);                                 \
    return s_pool;                                                                 \
  }

// The non thread safe edition
#define DEFINE_FAST_OBJECT_POOL_ALLOCATION(ELEMENT_TYPE, ELEMENT_COUNT) DEFINE_OBJECT_POOL_ALLOCATION_ANY(ELEMENT_TYPE, ELEMENT_COUNT, ::yasio::null_mutex)

// The thread safe edition
#define DEFINE_CONCURRENT_OBJECT_POOL_ALLOCATION(ELEMENT_TYPE, ELEMENT_COUNT) DEFINE_OBJECT_POOL_ALLOCATION_ANY(ELEMENT_TYPE, ELEMENT_COUNT, std::mutex)

} // namespace yasio

#endif
