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
#ifndef YASIO__SINGLETON_HPP
#define YASIO__SINGLETON_HPP
#include <new>
#include <memory>
#include <functional>

#include "yasio/detail/config.hpp"

#if !defined(YASIO_DISABLE_CONCURRENT_SINGLETON)
#  include <mutex>
#  include <atomic>
#endif

namespace yasio
{
namespace gc
{
template <typename _Ty, bool delay = false> class singleton_constructor
{
public:
  template <typename... _Types> static _Ty* construct(_Types&&... args)
  {
    return new _Ty(std::forward<_Types>(args)...);
  }
};

template <typename _Ty> class singleton_constructor<_Ty, true>
{
public:
  template <typename... _Types> static _Ty* construct(_Types&&... args)
  {
    auto inst = new _Ty();
    delay_init(inst, std::forward<_Types>(args)...);
    return inst;
  }

private:
  template <typename _Fty, typename... _Types>
  static void delay_init(_Ty* inst, _Fty&& memf, _Types&&... args)
  { // init use specific member func with more than 1 args
    std::mem_fn(memf)(inst, std::forward<_Types>(args)...);
  }

  template <typename _Fty, typename _Arg> static void delay_init(_Ty* inst, _Fty&& memf, _Arg&& arg)
  { // init use specific member func with 1 arg
    std::mem_fn(memf)(inst, std::forward<_Arg>(arg));
  }

  template <typename _Fty> static void delay_init(_Ty* inst, _Fty&& memf)
  { // init use specific member func without arg
    std::mem_fn(memf)(inst);
  }

  static void delay_init(_Ty* inst)
  { // dummy init without delay init member func, same as no delay constructor
  }
};

/// CLASS TEMPLATE singleton, support non-delayed or delayed init with variadic args
/// the managed singleton object will be destructed after main function.
template <typename _Ty> class singleton
{
  typedef singleton<_Ty> _Myt;
  typedef _Ty* pointer;

public:
  // Return the singleton instance
  template <typename... _Types> static pointer instance(_Types&&... args)
  {
    if (_Myt::__single__)
      return _Myt::__single__;

#if !defined(YASIO_DISABLE_CONCURRENT_SINGLETON)
    std::lock_guard<std::mutex> lck(__mutex__);
    if (_Myt::__single__)
      return _Myt::__single__;
#endif
    return (_Myt::__single__ =
                singleton_constructor<_Ty>::construct(std::forward<_Types>(args)...));
  }

  // Return the singleton instance with delayed init func
  template <typename... _Types> static pointer delayed(_Types&&... args)
  {
    if (_Myt::__single__)
      return _Myt::__single__;

#if !defined(YASIO_DISABLE_CONCURRENT_SINGLETON)
    std::lock_guard<std::mutex> lck(__mutex__);
    if (_Myt::__single__)
      return _Myt::__single__;
#endif
    return (_Myt::__single__ =
                singleton_constructor<_Ty, true>::construct(std::forward<_Types>(args)...));
  }

  // Peek the singleton instance
  static pointer peek() { return _Myt::__single__; }

  static void destroy(void)
  {
#if !defined(YASIO_DISABLE_CONCURRENT_SINGLETON)
    std::lock_guard<std::mutex> lck(__mutex__);
#endif
    if (_Myt::__single__)
    {
      delete static_cast<_Ty*>(_Myt::__single__);
      _Myt::__single__ = nullptr;
    }
  }

private:
#if !defined(YASIO_DISABLE_CONCURRENT_SINGLETON)
  static std::atomic<_Ty*> __single__;
  static std::mutex __mutex__;
#else
  static _Ty* __single__;
#endif
private:
  // disable construct, assign operation, copy construct also not allowed.
  singleton(void) = delete;
};

#if !defined(YASIO_DISABLE_CONCURRENT_SINGLETON)
template <typename _Ty> std::atomic<_Ty*> singleton<_Ty>::__single__;
template <typename _Ty> std::mutex singleton<_Ty>::__mutex__;
#else
template <typename _Ty> _Ty* singleton<_Ty>::__single__;
#endif

} // namespace gc
} // namespace yasio

#endif
