//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2012-2024 HALX99 (halx99 at live dot com)
#ifndef YASIO__SELECT_IO_WATCHER_HPP
#define YASIO__SELECT_IO_WATCHER_HPP
#include <vector>
#include <chrono>
#include "yasio/impl/socket.hpp"
#include "yasio/impl/select_interrupter.hpp"

namespace yasio
{
YASIO__NS_INLINE
namespace inet
{
class select_io_watcher {
public:
  select_io_watcher()
  {
    FD_ZERO(&events_[read_op]);
    FD_ZERO(&events_[write_op]);
    FD_ZERO(&events_[except_op]);
    this->mod_event(interrupter_.read_descriptor(), socket_event::read, 0);
  }

  void mod_event(socket_native_type fd, int add_events, int remove_events)
  {
    if (add_events)
    {
      if (yasio__testbits(add_events, socket_event::read))
        FD_SET(fd, &(events_[read_op]));

      if (yasio__testbits(add_events, socket_event::write))
        FD_SET(fd, &(events_[write_op]));

      if (yasio__testbits(add_events, socket_event::error))
        FD_SET(fd, &(events_[except_op]));

      if (max_descriptor_ < static_cast<int>(fd) + 1)
        max_descriptor_ = static_cast<int>(fd) + 1;
    }

    if (remove_events)
    {
      if (yasio__testbits(remove_events, socket_event::read))
        FD_CLR(fd, &(events_[read_op]));

      if (yasio__testbits(remove_events, socket_event::write))
        FD_CLR(fd, &(events_[write_op]));

      if (yasio__testbits(remove_events, socket_event::error))
        FD_CLR(fd, &(events_[except_op]));
    }
  }

  int poll_io(int64_t waitd_us)
  {
    ::memcpy(this->revents_, events_, sizeof(revents_));
    timeval timeout = {(decltype(timeval::tv_sec))(waitd_us / std::micro::den), (decltype(timeval::tv_usec))(waitd_us % std::micro::den)};
    int num_events  = ::select(this->max_descriptor_, &(revents_[read_op]), &(revents_[write_op]), nullptr, &timeout);
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
      retval |= FD_ISSET(fd, &revents_[read_op]);
    if (events & socket_event::write)
      retval |= FD_ISSET(fd, &revents_[write_op]);
    if (events & socket_event::error)
      retval |= FD_ISSET(fd, &revents_[except_op]);
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
  fd_set events_[max_ops];
  fd_set revents_[max_ops];
  int max_descriptor_ = 0;

  select_interrupter interrupter_;
};
} // namespace inet
} // namespace yasio
#endif
