//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
//
// detail/io_watcher.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2012-2023 HALX99 (halx99 at live dot com)

#pragma once

#include "yasio/detail/config.hpp"

#if YASIO__HAS_KQUEUE && !defined(YASIO_DISABLE_KQUEUE)
#  include "yasio/detail/kqueue_io_watcher.hpp"
#elif YASIO__HAS_EPOLL && !defined(YASIO_DISABLE_EPOLL)
#  include "yasio/detail/epoll_io_watcher.hpp"
#elif YASIO__HAS_EVPORT && !defined(YASIO_DISABLE_EVPORT)
#  include "yasio/detail/evport_io_watcher.hpp"
#elif !defined(YASIO_DISABLE_POLL)
#  include "yasio/detail/poll_io_watcher.hpp"
#else
#  include "yasio/detail/select_io_watcher.hpp"
#endif

namespace yasio
{
YASIO__NS_INLINE
namespace inet
{
#if YASIO__HAS_KQUEUE && !defined(YASIO_DISABLE_KQUEUE)
using io_watcher = kqueue_io_watcher;
#elif YASIO__HAS_EPOLL && !defined(YASIO_DISABLE_EPOLL)
using io_watcher = epoll_io_watcher;
#elif YASIO__HAS_EVPORT && !defined(YASIO_DISABLE_EVPORT)
using io_watcher = evport_io_watcher;
#elif !defined(YASIO_DISABLE_POLL)
using io_watcher = poll_io_watcher;
#else
using io_watcher = select_io_watcher;
#endif
} // namespace inet
} // namespace yasio
