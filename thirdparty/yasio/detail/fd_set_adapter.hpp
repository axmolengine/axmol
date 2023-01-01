//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
//
// detail/fd_set_adapter.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2012-2023 HALX99 (halx99 at live dot com)

#pragma once

#include "yasio/detail/config.hpp"

#if !defined(YASIO_DISABLE_POLL)
#  include "yasio/detail/poll_fd_set.hpp"
#else
#  include "yasio/detail/select_fd_set.hpp"
#endif

namespace yasio
{
YASIO__NS_INLINE
namespace inet
{
#if !defined(YASIO_DISABLE_POLL)
using fd_set_adapter = poll_fd_set;
#else
using fd_set_adapter = select_fd_set;
#endif
} // namespace inet
} // namespace yasio
