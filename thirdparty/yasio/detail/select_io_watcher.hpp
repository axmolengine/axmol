//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
//
// detail/select_io_watcher.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2012-2023 HALX99 (halx99 at live dot com)
#pragma once

#include <vector>
#include <chrono>
#include "yasio/detail/socket.hpp"
#include "yasio/detail/select_interrupter.hpp"

namespace yasio
{
YASIO__NS_INLINE
namespace inet
{
class select_io_watcher {
public:
  select_io_watcher()
  {
    FD_ZERO(&registered_events_[read_op]);
    FD_ZERO(&registered_events_[write_op]);
    FD_ZERO(&registered_events_[except_op]);
    this->add_event(interrupter_.read_descriptor(), socket_event::read);
  }

  void add_event(socket_native_type fd, int events)
  {
    if (yasio__testbits(events, socket_event::read))
      FD_SET(fd, &(registered_events_[read_op]));

    if (yasio__testbits(events, socket_event::write))
      FD_SET(fd, &(registered_events_[write_op]));

    if (yasio__testbits(events, socket_event::error))
      FD_SET(fd, &(registered_events_[except_op]));

    if (max_descriptor_ < static_cast<int>(fd) + 1)
      max_descriptor_ = static_cast<int>(fd) + 1;
  }

  void del_event(socket_native_type fd, int events)
  {
    if (yasio__testbits(events, socket_event::read))
      FD_CLR(fd, &(registered_events_[read_op]));

    if (yasio__testbits(events, socket_event::write))
      FD_CLR(fd, &(registered_events_[write_op]));

    if (yasio__testbits(events, socket_event::error))
      FD_CLR(fd, &(registered_events_[except_op]));
  }

  int poll_io(int64_t waitd_us)
  {
    ::memcpy(this->ready_events_, registered_events_, sizeof(ready_events_));
    timeval timeout = {(decltype(timeval::tv_sec))(waitd_us / std::micro::den), (decltype(timeval::tv_usec))(waitd_us % std::micro::den)};
    int num_events  = ::select(this->max_descriptor_, &(ready_events_[read_op]), &(ready_events_[write_op]), nullptr, &timeout);
    if (num_events > 0 && is_ready(this->interrupter_.read_descriptor(), socket_event::read))
    {
      if (!interrupter_.reset())
        interrupter_.recreate();
      --num_events;
    }
    return num_events;
  }

  void wakeup() { interrupter_.interrupt(); }

  int is_ready(socket_native_type fd, int events) const
  {
    int retval = 0;
    if (events & socket_event::read)
      retval |= FD_ISSET(fd, &ready_events_[read_op]);
    if (events & socket_event::write)
      retval |= FD_ISSET(fd, &ready_events_[write_op]);
    if (events & socket_event::error)
      retval |= FD_ISSET(fd, &ready_events_[except_op]);
    return retval;
  }

  int max_descriptor() const { return max_descriptor_; }

protected:
  enum
  {
    read_op,
    write_op,
    except_op,
    max_ops,
  };
  fd_set registered_events_[max_ops];
  fd_set ready_events_[max_ops];
  int max_descriptor_ = 0;

  select_interrupter interrupter_;
};
} // namespace inet
} // namespace yasio
