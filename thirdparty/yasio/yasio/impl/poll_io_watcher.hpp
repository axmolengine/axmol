//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2012-2024 HALX99 (halx99 at live dot com)
#ifndef YASIO__POLL_IO_WATCHER_HPP
#define YASIO__POLL_IO_WATCHER_HPP
#include "yasio/pod_vector.hpp"
#include "yasio/impl/socket.hpp"
#include "yasio/impl/select_interrupter.hpp"

namespace yasio
{
YASIO__NS_INLINE
namespace inet
{
class poll_io_watcher {
public:
  poll_io_watcher() { this->mod_event(interrupter_.read_descriptor(), socket_event::read, 0); }

  void mod_event(socket_native_type fd, int add_events, int remove_events)
  {
    pollfd_mod(this->events_, fd, to_underlying_events(add_events), to_underlying_events(remove_events));
  }

  int poll_io(int64_t waitd_us)
  {
    revents_ = this->events_;
#if YASIO__HAS_PPOLL
    timespec timeout = {(decltype(timespec::tv_sec))(waitd_us / std::micro::den),
                        (decltype(timespec::tv_nsec))((waitd_us % std::micro::den) * std::milli::den)};
    int num_events   = ::ppoll(this->revents_.data(), static_cast<int>(this->revents_.size()), &timeout, nullptr);
#else
    int num_events = ::poll(this->revents_.data(), static_cast<int>(this->revents_.size()), static_cast<int>(waitd_us / std::milli::den));
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
    auto it = std::find_if(this->revents_.begin(), this->revents_.end(), [fd](const pollfd& pfd) { return pfd.fd == fd; });
    return it != this->revents_.end() ? (it->revents & underlying_events) : 0;
  }

  int max_descriptor() const { return -1; }

protected:
  int to_underlying_events(int events)
  {
    int underlying_events = 0;
    if (events)
    {
      if (yasio__testbits(events, socket_event::read))
        underlying_events |= POLLIN;

      if (yasio__testbits(events, socket_event::write))
        underlying_events |= POLLOUT;

      if (yasio__testbits(events, socket_event::error))
        underlying_events |= POLLERR;
    }
    return underlying_events;
  }
  static void pollfd_mod(yasio::pod_vector<pollfd>& fdset, socket_native_type fd, int add_events, int remove_events)
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
        fdset.emplace_back(fd, static_cast<short>(events), static_cast<short>(0));
    }
  }

protected:
  yasio::pod_vector<pollfd> events_;
  yasio::pod_vector<pollfd> revents_;

  select_interrupter interrupter_;
};
} // namespace inet
} // namespace yasio
#endif
