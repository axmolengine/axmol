
//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
//
// detail/signal_blocker.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2012-2023 HALX99 (halx99 at live dot com)

#pragma once

#include "yasio/compiler/feature_test.hpp"

#if !defined(_WIN32)
#include <pthread.h>
#include <signal.h>
#endif

namespace yasio
{
YASIO__NS_INLINE
namespace inet
{
#if defined(_WIN32)
class signal_blocker {};
#else
class signal_blocker {
public:
  // Constructor blocks all signals for the calling thread.
  signal_blocker() : blocked_(false)
  {
    sigset_t new_mask;
    sigfillset(&new_mask);
    blocked_ = (pthread_sigmask(SIG_BLOCK, &new_mask, &old_mask_) == 0);
  }

  // Destructor restores the previous signal mask.
  ~signal_blocker()
  {
    if (blocked_)
      pthread_sigmask(SIG_SETMASK, &old_mask_, 0);
  }

private:
  // Have signals been blocked.
  bool blocked_;

  // The previous signal mask.
  sigset_t old_mask_;
};
#endif
} // namespace inet
} // namespace yasio
