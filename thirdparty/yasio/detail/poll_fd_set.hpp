//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
//
// detail/poll_fd_set.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2012-2023 HALX99 (halx99 at live dot com)
#pragma once
#include <vector>
#include "yasio/detail/socket.hpp"

namespace yasio
{
YASIO__NS_INLINE
namespace inet
{
class poll_fd_set {
public:
  poll_fd_set& operator=(poll_fd_set& rhs)
  {
    this->fd_set_ = rhs.fd_set_;
    return *this;
  }

  void reset() { this->fd_set_.clear(); }

  int poll_io(int wait_ms) { return ::poll(this->fd_set_.data(), static_cast<int>(this->fd_set_.size()), wait_ms); }

  int is_set(socket_native_type fd, int events) const
  {
    int underlying_events = 0;
    if (events & socket_event::read)
      underlying_events |= POLLIN;
    if (events & socket_event::write)
      underlying_events |= POLLOUT;
    if (events & socket_event::error)
      underlying_events |= (POLLERR | POLLHUP | POLLNVAL);
    auto it = std::find_if(this->fd_set_.begin(), this->fd_set_.end(), [fd](const pollfd& pfd) { return pfd.fd == fd; });
    return it != this->fd_set_.end() ? (it->revents & underlying_events) : 0;
  }

  void set(socket_native_type fd, int events)
  {
    int underlying_events = 0;
    if (yasio__testbits(events, socket_event::read))
      underlying_events |= POLLIN;

    if (yasio__testbits(events, socket_event::write))
      underlying_events |= POLLOUT;

    if (yasio__testbits(events, socket_event::error))
      underlying_events |= POLLERR;
    pollfd_mod(this->fd_set_, fd, underlying_events, 0);
  }

  void unset(socket_native_type fd, int events)
  {
    int underlying_events = 0;
    if (yasio__testbits(events, socket_event::read))
      underlying_events |= POLLIN;

    if (yasio__testbits(events, socket_event::write))
      underlying_events |= POLLOUT;

    if (yasio__testbits(events, socket_event::error))
      underlying_events |= POLLERR;

    pollfd_mod(this->fd_set_, fd, 0, underlying_events);
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
  std::vector<pollfd> fd_set_;
};
} // namespace inet
} // namespace yasio
