//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
/*
The MIT License (MIT)

Copyright (c) 2012-2021 HALX99
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

#ifndef YASIO__SHARED_MUTEX_HPP
#define YASIO__SHARED_MUTEX_HPP

#include "yasio/compiler/feature_test.hpp"

/// The shared_mutex workaround on c++11
#if YASIO__HAS_CXX17 && !defined(__APPLE__)
#  include <shared_mutex>
#else
#  include <system_error>
#  if defined(_WIN32)
#    if !defined(WIN32_LEAN_AND_MEAN)
#      define WIN32_LEAN_AND_MEAN
#    endif
#    include <Windows.h>
#    define yasio__smtx_t SRWLOCK
#    define yasio__smtx_init(rwlock, attr) InitializeSRWLock(rwlock)
#    define yasio__smtx_destroy(rwlock)
#    define yasio__smtx_lock_shared(rwlock) AcquireSRWLockShared(rwlock)
#    define yasio__smtx_trylock_shared(rwlock) !!TryAcquireSRWLockShared(rwlock)
#    define yasio__smtx_lock_exclusive(rwlock) AcquireSRWLockExclusive(rwlock)
#    define yasio__smtx_trylock_exclusive(rwlock) !!TryAcquireSRWLockExclusive(rwlock)
#    define yasio__smtx_unlock_shared(rwlock) ReleaseSRWLockShared(rwlock)
#    define yasio__smtx_unlock_exclusive(rwlock) ReleaseSRWLockExclusive(rwlock)
#  else
#    include <pthread.h>
#    define yasio__smtx_t pthread_rwlock_t
#    define yasio__smtx_init(rwlock, attr) pthread_rwlock_init(rwlock, attr)
#    define yasio__smtx_destroy(rwlock) pthread_rwlock_destroy(rwlock)
#    define yasio__smtx_lock_shared(rwlock) pthread_rwlock_rdlock(rwlock)
#    define yasio__smtx_trylock_shared(rwlock) pthread_rwlock_tryrdlock(rwlock) != 0
#    define yasio__smtx_lock_exclusive(rwlock) pthread_rwlock_wrlock(rwlock)
#    define yasio__smtx_trylock_exclusive(rwlock) pthread_rwlock_trywrlock(rwlock) != 0
#    define yasio__smtx_unlock_shared(rwlock) pthread_rwlock_unlock(rwlock)
#    define yasio__smtx_unlock_exclusive(rwlock) pthread_rwlock_unlock(rwlock)
#  endif
#  define yaso__throw_error(e) YASIO__THROW0(std::system_error(std::make_error_code(e), ""))
#  include <mutex>

// CLASS TEMPLATE shared_lock
namespace cxx17
{
class shared_mutex {
public:
  typedef yasio__smtx_t* native_handle_type;

  shared_mutex() // strengthened
  {
    yasio__smtx_init(&this->_Myhandle, nullptr);
  }

  ~shared_mutex() { yasio__smtx_destroy(&this->_Myhandle); }

  void lock() /* strengthened */
  {           // lock exclusive
    yasio__smtx_lock_exclusive(&this->_Myhandle);
  }

  bool try_lock() /* strengthened */
  {               // try to lock exclusive
    return yasio__smtx_trylock_exclusive(&this->_Myhandle);
  }

  void unlock() /* strengthened */
  {             // unlock exclusive
    yasio__smtx_unlock_exclusive(&this->_Myhandle);
  }

  void lock_shared() /* strengthened */
  {                  // lock non-exclusive
    yasio__smtx_lock_shared(&this->_Myhandle);
  }

  bool try_lock_shared() /* strengthened */
  {                      // try to lock non-exclusive
    return yasio__smtx_trylock_shared(&this->_Myhandle);
  }

  void unlock_shared() /* strengthened */
  {                    // unlock non-exclusive
    yasio__smtx_unlock_shared(&this->_Myhandle);
  }

  native_handle_type native_handle() /* strengthened */
  {                                  // get native handle
    return &_Myhandle;
  }

  shared_mutex(const shared_mutex&) = delete;
  shared_mutex& operator=(const shared_mutex&) = delete;

private:
  yasio__smtx_t _Myhandle; // the lock object
};
// CLASS TEMPLATE shared_lock
template <class _Mutex> class shared_lock { // shareable lock
public:
  using mutex_type = _Mutex;

  shared_lock() : _Pmtx(nullptr), _Owns(false) {}

  explicit shared_lock(mutex_type& _Mtx) : _Pmtx(YASIO__STD addressof(_Mtx)), _Owns(true)
  { // construct with mutex and lock shared
    _Mtx.lock_shared();
  }

  explicit shared_lock(mutex_type& _Mtx, YASIO__STD defer_lock_t) : _Pmtx(YASIO__STD addressof(_Mtx)), _Owns(false) {} // // construct with unlocked mutex

  explicit shared_lock(mutex_type& _Mtx, YASIO__STD try_to_lock_t)
      : _Pmtx(YASIO__STD addressof(_Mtx)), _Owns(_Mtx.try_lock_shared()) {} // construct with mutex and try to lock shared

  explicit shared_lock(mutex_type& _Mtx, YASIO__STD adopt_lock_t) : _Pmtx(YASIO__STD addressof(_Mtx)), _Owns(true) {} // construct with mutex and adopt owership

  ~shared_lock()
  {
    if (_Owns)
      _Pmtx->unlock_shared();
  }

  shared_lock(shared_lock&& _Other) : _Pmtx(_Other._Pmtx), _Owns(_Other._Owns)
  {
    _Other._Pmtx = nullptr;
    _Other._Owns = false;
  }

  shared_lock& operator=(shared_lock&& _Right)
  {
    if (_Owns)
      _Pmtx->unlock_shared();

    _Pmtx        = _Right._Pmtx;
    _Owns        = _Right._Owns;
    _Right._Pmtx = nullptr;
    _Right._Owns = false;
    return *this;
  }

  shared_lock(const shared_lock&) = delete;
  shared_lock& operator=(const shared_lock&) = delete;

  void lock()
  { // lock the mutex
    _Validate();
    _Pmtx->lock_shared();
    _Owns = true;
  }

  bool try_lock()
  { // try to lock the mutex
    _Validate();
    _Owns = _Pmtx->try_lock_shared();
    return _Owns;
  }

  void unlock()
  { // try to unlock the mutex
    if (!_Pmtx || !_Owns)
      yaso__throw_error(std::errc::operation_not_permitted);

    _Pmtx->unlock_shared();
    _Owns = false;
  }

  // MUTATE
  void swap(shared_lock& _Right)
  {
    YASIO__STD swap(_Pmtx, _Right._Pmtx);
    YASIO__STD swap(_Owns, _Right._Owns);
  }

  mutex_type* release()
  {
    _Mutex* _Res = _Pmtx;
    _Pmtx        = nullptr;
    _Owns        = false;
    return _Res;
  }

  // OBSERVE
  bool owns_lock() const { return _Owns; }

  explicit operator bool() const { return _Owns; }

  mutex_type* mutex() const { return _Pmtx; }

private:
  _Mutex* _Pmtx;
  bool _Owns;

  void _Validate() const
  { // check if the mutex can be locked
    if (!_Pmtx)
      yaso__throw_error(std::errc::operation_not_permitted);

    if (_Owns)
      yaso__throw_error(std::errc::resource_deadlock_would_occur);
  }
};
} // namespace cxx17
#endif

#endif
