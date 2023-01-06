//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2012-2023 halx99 (halx99 at live dot com)
// Copyright Vicente J. Botet Escriba 2012.
// Copyright Howard Hinnant 2007-2010.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// see also: https://github.com/boostorg/thread/blob/develop/include/boost/thread/v2/shared_mutex.hpp
//

#ifndef YASIO__SHARED_MUTEX_HPP
#define YASIO__SHARED_MUTEX_HPP

#include "yasio/compiler/feature_test.hpp"
#include <limits.h>

/// The shared_mutex workaround on c++11
#if YASIO__HAS_CXX17 && !defined(__APPLE__)
#  include <shared_mutex>
#else
#  include <mutex>
#  include <condition_variable>
namespace cxx17
{
// CLASS TEMPLATE shared_mutex
class shared_mutex {
  typedef std::mutex mutex_t;
  typedef std::condition_variable cond_t;
  typedef unsigned count_t;

  mutex_t mut_;
  cond_t gate1_;
  // the gate2_ condition variable is only used by functions that
  // have taken write_entered_ but are waiting for no_readers()
  cond_t gate2_;
  count_t state_;

  static const count_t write_entered_ = 1U << (sizeof(count_t) * CHAR_BIT - 1);
  static const count_t n_readers_     = ~write_entered_;

  bool no_writer() const { return (state_ & write_entered_) == 0; }

  bool one_writer() const { return (state_ & write_entered_) != 0; }

  bool no_writer_no_readers() const
  {
    // return (state_ & write_entered_) == 0 &&
    //       (state_ & n_readers_) == 0;
    return state_ == 0;
  }

  bool no_writer_no_max_readers() const { return (state_ & write_entered_) == 0 && (state_ & n_readers_) != n_readers_; }

  bool no_readers() const { return (state_ & n_readers_) == 0; }

  bool one_or_more_readers() const { return (state_ & n_readers_) > 0; }

  shared_mutex(shared_mutex const&) = delete;
  shared_mutex& operator=(shared_mutex const&) = delete;

public:
  shared_mutex() : state_(0) {}
  ~shared_mutex() { std::lock_guard<mutex_t> _(mut_); }

  // Exclusive ownership

  void lock()
  {
    std::unique_lock<mutex_t> lk(mut_);
    gate1_.wait(lk, std::bind(&shared_mutex::no_writer, std::ref(*this)));
    state_ |= write_entered_;
    gate2_.wait(lk, std::bind(&shared_mutex::no_readers, std::ref(*this)));
  }
  bool try_lock()
  {
    std::unique_lock<mutex_t> lk(mut_);
    if (!no_writer_no_readers())
    {
      return false;
    }
    state_ = write_entered_;
    return true;
  }
  void unlock()
  {
    std::lock_guard<mutex_t> _(mut_);
    assert(one_writer());
    assert(no_readers());
    state_ = 0;
    // notify all since multiple *lock_shared*() calls may be able
    // to proceed in response to this notification
    gate1_.notify_all();
  }

  // Shared ownership

  void lock_shared()
  {
    std::unique_lock<mutex_t> lk(mut_);
    gate1_.wait(lk, std::bind(&shared_mutex::no_writer_no_max_readers, std::ref(*this)));
    count_t num_readers = (state_ & n_readers_) + 1;
    state_ &= ~n_readers_;
    state_ |= num_readers;
  }
  bool try_lock_shared()
  {
    std::unique_lock<mutex_t> lk(mut_);
    if (!no_writer_no_max_readers())
    {
      return false;
    }
    count_t num_readers = (state_ & n_readers_) + 1;
    state_ &= ~n_readers_;
    state_ |= num_readers;
    return true;
  }
  void unlock_shared()
  {
    std::lock_guard<mutex_t> _(mut_);
    assert(one_or_more_readers());
    count_t num_readers = (state_ & n_readers_) - 1;
    state_ &= ~n_readers_;
    state_ |= num_readers;
    if (no_writer())
    {
      if (num_readers == n_readers_ - 1)
        gate1_.notify_one();
    }
    else
    {
      if (num_readers == 0)
        gate2_.notify_one();
    }
  }
};

// CLASS TEMPLATE shared_lock
template <class _Mutex>
class shared_lock {
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
      yasio__throw_error0(std::errc::operation_not_permitted);

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
      yasio__throw_error0(std::errc::operation_not_permitted);

    if (_Owns)
      yasio__throw_error0(std::errc::resource_deadlock_would_occur);
  }
};
} // namespace cxx17
#endif

#endif
