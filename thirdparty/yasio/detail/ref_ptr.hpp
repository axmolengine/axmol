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
#ifndef YASIO__REF_PTR_HPP
#define YASIO__REF_PTR_HPP
#include <iostream>

// clang-format off
#define YASIO__DEFINE_REFERENCE_CLASS  \
private:                               \
  unsigned int __strong_refs = 1;      \
                                       \
public:                                \
  void retain() { ++__strong_refs; }   \
  void release()                       \
  {                                    \
    --__strong_refs;                   \
    if (__strong_refs == 0)            \
      delete this;                     \
  }                                    \
                                       \
private:

#define YASIO__SAFE_RELEASE(p)         \
  if (p)                               \
    (p)->release()

#define YASIO__SAFE_RELEASE_NULL(p)    \
  do                                   \
  {                                    \
    if (p)                             \
    {                                  \
      (p)->release();                  \
      (p) = nullptr;                   \
    }                                  \
  } while (0)

#define YASIO__SAFE_RETAIN(p)          \
  if (p)                               \
    (p)->retain()
// clang-format on

namespace yasio
{
namespace gc
{

struct own_ref_t {};

// TEMPLATE CLASS ref_ptr, allow any time with functions 'retain' and 'release'
template <typename _Ty>
class ref_ptr;

template <typename _Ty>
class ref_ptr { // wrap an object pointer to ensure destruction
public:
  typedef ref_ptr<_Ty> _Myt;
  typedef _Ty element_type;

  explicit ref_ptr(_Ty* _Ptr = nullptr) throw() : ptr_(_Ptr)
  { // construct from object pointer
  }

  ref_ptr(_Ty* _Ptr, own_ref_t) throw() : ptr_(_Ptr)
  { // construct from object pointer
    YASIO__SAFE_RETAIN(ptr_);
  }

  ref_ptr(std::nullptr_t) throw() : ptr_(nullptr) {}

  ref_ptr(const _Myt& _Right) throw()
  { // construct by assuming pointer from _Right ref_ptr
    ptr_ = _Right.get();
    YASIO__SAFE_RETAIN(ptr_);
  }

  template <typename _Other>
  ref_ptr(const ref_ptr<_Other>& _Right) throw()
  { // construct by assuming pointer from _Right
    ptr_ = (_Ty*)_Right.get();
    YASIO__SAFE_RETAIN(ptr_);
  }

  ref_ptr(_Myt&& _Right) throw() : ptr_(_Right.release()) {}

  template <typename _Other>
  ref_ptr(ref_ptr<_Other>&& _Right) throw() : ptr_((_Ty*)_Right.release())
  {}

  _Myt& operator=(const _Myt& _Right) throw()
  { // assign compatible _Right (assume pointer)
    if (this == &_Right)
      return *this;

    reset(_Right.get());
    YASIO__SAFE_RETAIN(ptr_);
    return (*this);
  }

  _Myt& operator=(_Myt&& _Right) throw()
  { // assign compatible _Right (assume pointer)
    reset(_Right.release());
    return (*this);
  }

  // release ownership
  _Ty* release()
  {
    auto tmp = ptr_;
    ptr_     = nullptr;
    return tmp;
  }

  template <typename _Other>
  _Myt& operator=(const ref_ptr<_Other>& _Right) throw()
  { // assign compatible _Right (assume pointer)
    if (this == &_Right)
      return *this;

    reset((_Ty*)_Right.get());
    YASIO__SAFE_RETAIN(ptr_);
    return (*this);
  }

  template <typename _Other>
  _Myt& operator=(ref_ptr<_Other>&& _Right) throw()
  { // assign compatible _Right (assume pointer)
    reset((_Ty*)_Right.release());
    return (*this);
  }

  _Myt& operator=(std::nullptr_t) throw()
  {
    reset();
    return (*this);
  }

  ~ref_ptr()
  { // release the object
    YASIO__SAFE_RELEASE(ptr_);
  }

  _Ty& operator*() const throw()
  {                 // return designated value
    return (*ptr_); // return (*get());
  }

  _Ty** operator&() throw() { return &(ptr_); }

  _Ty* operator->() const throw()
  {                // return pointer to class object
    return (ptr_); // return (get());
  }

  template <typename _Int>
  _Ty& operator[](_Int index) const throw()
  {
    return (ptr_[index]);
  }

  _Ty* get() const throw()
  { // return wrapped pointer
    return (ptr_);
  }

  operator _Ty*() const throw()
  { // convert to basic type
    return (ptr_);
  }

  /*
  ** if already have a valid pointer, will call release firstly
  */
  void reset(_Ty* _Ptr = nullptr)
  { // relese designated object and store new pointer
    if (ptr_ != _Ptr)
    {
      if (ptr_ != nullptr)
        YASIO__SAFE_RELEASE(ptr_);
      ptr_ = _Ptr;
    }
  }

private:
  _Ty* ptr_; // the wrapped object pointer
};

}; // namespace gc

}; // namespace yasio

#endif
