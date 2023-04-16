//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
//
// detail/poll_io_watcher.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2012-2023 HALX99 (halx99 at live dot com)
#pragma once
#include <vector>
#include "yasio/detail/socket.hpp"
#include "yasio/detail/select_interrupter.hpp"

namespace yasio
{
YASIO__NS_INLINE
namespace inet
{
class poll_io_watcher {
public:
  poll_io_watcher() { this->add_event(interrupter_.read_descriptor(), socket_event::read); }

  void add_event(socket_native_type fd, int events)
  {
    int underlying_events = 0;
    if (yasio__testbits(events, socket_event::read))
      underlying_events |= POLLIN;

    if (yasio__testbits(events, socket_event::write))
      underlying_events |= POLLOUT;

    if (yasio__testbits(events, socket_event::error))
      underlying_events |= POLLERR;
    pollfd_mod(this->registered_events_, fd, underlying_events, 0);
  }

  void del_event(socket_native_type fd, int events)
  {
    int underlying_events = 0;
    if (yasio__testbits(events, socket_event::read))
      underlying_events |= POLLIN;

    if (yasio__testbits(events, socket_event::write))
      underlying_events |= POLLOUT;

    if (yasio__testbits(events, socket_event::error))
      underlying_events |= POLLERR;

    pollfd_mod(this->registered_events_, fd, 0, underlying_events);
  }

  int poll_io(int64_t waitd_us)
  {
    ready_events_ = this->registered_events_;
#if YASIO__HAS_PPOLL
    timespec timeout = {(decltype(timespec::tv_sec))(waitd_us / std::micro::den),
                        (decltype(timespec::tv_nsec))((waitd_us % std::micro::den) * std::milli::den)};
    int num_events   = ::ppoll(this->ready_events_.data(), static_cast<int>(this->ready_events_.size()), &timeout, nullptr);
#else
    int num_events = ::poll(this->ready_events_.data(), static_cast<int>(this->ready_events_.size()), static_cast<int>(waitd_us / std::milli::den));
#endif
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
    int underlying_events = 0;
    if (events & socket_event::read)
      underlying_events |= POLLIN;
    if (events & socket_event::write)
      underlying_events |= POLLOUT;
    if (events & socket_event::error)
      underlying_events |= (POLLERR | POLLHUP | POLLNVAL);
    auto it = std::find_if(this->ready_events_.begin(), this->ready_events_.end(), [fd](const pollfd& pfd) { return pfd.fd == fd; });
    return it != this->ready_events_.end() ? (it->revents & underlying_events) : 0;
  }

  int max_descriptor() const { return -1; }

protected:
  static void pollfd_mod(std::vector<pollfd>& fdset, socket_native_type fd, int add_events, int remove_events)
  {
    auto it = std::find_if(fdset.begin(), fdset.end(), [fd](const pollfd& pfd) { return pfd.fd == fd; });
    if (it != fdset.end())
    {
      it->events |= add_events;
      it->events &= ~remove_events;
      if (it->events == 0)
        fdset.erase(it);
    }
    else
    {
      auto events = add_events & ~remove_events;
      if (events)
        fdset.push_back(pollfd{fd, static_cast<short>(events), 0});
    }
  }

protected:
  std::vector<pollfd> registered_events_;
  std::vector<pollfd> ready_events_;

  select_interrupter interrupter_;
};
} // namespace inet
} // namespace yasio
