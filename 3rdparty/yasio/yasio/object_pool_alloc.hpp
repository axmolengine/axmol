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
#ifndef YASIO__OBJECT_POOL_ALLOC_HPP
#define YASIO__OBJECT_POOL_ALLOC_HPP

#include "yasio/object_pool.hpp"

namespace yasio
{
// TEMPLATE class object_pool_allocator, can't used by sequence container
template <class _Ty, size_t _ElemCount = 128, class _Mutex = ::yasio::null_mutex>
class object_pool_allocator { // generic allocator for objects of class _Ty
public:
  typedef _Ty value_type;

  typedef value_type* pointer;
  typedef value_type& reference;
  typedef const value_type* const_pointer;
  typedef const value_type& const_reference;

  typedef size_t size_type;
#ifdef _WIN32
  typedef ptrdiff_t difference_type;
#else
  typedef long difference_type;
#endif

  using object_pool_type = object_pool<_Ty, _Mutex>;
  using my_type          = object_pool_allocator<_Ty, _ElemCount, _Mutex>;

  template <class _Other>
  struct rebind { // convert this type to _ALLOCATOR<_Other>
    typedef object_pool_allocator<_Other, _ElemCount, _Mutex> other;
  };

  pointer address(reference _Val) const
  { // return address of mutable _Val
    return ((pointer) & (char&)_Val);
  }

  const_pointer address(const_reference _Val) const
  { // return address of nonmutable _Val
    return ((const_pointer) & (const char&)_Val);
  }

  object_pool_allocator() throw()
  { // construct default allocator (do nothing)
  }

  object_pool_allocator(const my_type&) throw()
  { // construct by copying (do nothing)
  }

  template <class _Other>
  object_pool_allocator(const object_pool_allocator<_Other, _ElemCount, _Mutex>&) throw()
  { // construct from a related allocator (do nothing)
  }

  template <class _Other>
  my_type& operator=(const object_pool_allocator<_Other, _ElemCount, _Mutex>&) throw()
  { // assign from a related allocator (do nothing)
    return (*this);
  }

  void deallocate(pointer _Ptr, size_type)
  { // deallocate object at _Ptr, ignore size
    _Spool().deallocate(_Ptr);
  }

  pointer allocate(size_type count)
  { // allocate array of _Count elements
    assert(count == 1);
    (void)count;
    return static_cast<pointer>(_Spool().allocate());
  }

  pointer allocate(size_type count, const void*)
  { // allocate array of _Count elements, not support, such as std::vector
    return allocate(count);
  }

  void construct(_Ty* _Ptr)
  { // default construct object at _Ptr
    ::new ((void*)_Ptr) _Ty();
  }

  void construct(pointer _Ptr, const _Ty& _Val)
  { // construct object at _Ptr with value _Val
    new (_Ptr) _Ty(_Val);
  }

  void construct(pointer _Ptr, _Ty&& _Val)
  { // construct object at _Ptr with value _Val
    new ((void*)_Ptr) _Ty(std::forward<_Ty>(_Val));
  }

  template <class _Other>
  void construct(pointer _Ptr, _Other&& _Val)
  { // construct object at _Ptr with value _Val
    new ((void*)_Ptr) _Ty(std::forward<_Other>(_Val));
  }

  template <class _Objty, class... _Types>
  void construct(_Objty* _Ptr, _Types&&... _Args)
  { // construct _Objty(_Types...) at _Ptr
    ::new ((void*)_Ptr) _Objty(std::forward<_Types>(_Args)...);
  }

  template <class _Uty>
  void destroy(_Uty* _Ptr)
  { // destroy object at _Ptr, do nothing
    _Ptr->~_Uty();
  }

  size_type max_size() const throw()
  { // estimate maximum array size
    size_type _Count = (size_type)(-1) / sizeof(_Ty);
    return (0 < _Count ? _Count : 1);
  }

  static object_pool_type& _Spool()
  {
    static object_pool_type s_pool(_ElemCount);
    return s_pool;
  }
};
} // namespace yasio

#endif
