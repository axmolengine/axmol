//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2012-2023 HALX99 (halx99 at live dot com)
#pragma once
#include <vector>
#include <chrono>
#include <map>
#include "yasio/core/socket.hpp"
#include "yasio/core/pod_vector.hpp"
#include "yasio/core/select_interrupter.hpp"

namespace yasio
{
YASIO__NS_INLINE
namespace inet
{
class epoll_io_watcher {
public:
  epoll_io_watcher() : epoll_handle_(do_epoll_create())
  {
    this->ready_events_.reserve(128);
    this->mod_event(interrupter_.read_descriptor(), socket_event::read, 0, EPOLLONESHOT);
    interrupter_.interrupt();
    poll_io(1);
  }
  ~epoll_io_watcher()
  {
    this->mod_event(interrupter_.read_descriptor(), 0, socket_event::read, EPOLLONESHOT);
    epoll_close(epoll_handle_);
  }

  void mod_event(socket_native_type fd, int add_events, int remove_events, int flags = 0)
  {
    auto it               = registered_events_.find(fd);
    const auto registered = it != registered_events_.end();
    int underlying_events = registered ? it->second : 0;
    underlying_events |= to_underlying_events(add_events);
    underlying_events &= ~to_underlying_events(remove_events);

    epoll_event ev = {0, {0}};
    ev.events      = underlying_events;
    ev.data.fd     = static_cast<int>(fd);

    if (underlying_events)
    { // add or mod
      if (::epoll_ctl(epoll_handle_, !registered ? EPOLL_CTL_ADD : EPOLL_CTL_MOD, fd, &ev) == 0)
      {
        if (registered)
          it->second = underlying_events;
        else
        {
          registered_events_[fd] = underlying_events;
          ready_events_.resize(registered_events_.size());
        }
      }
    }
    else
    { // del if registered
      if (registered)
      {
        ::epoll_ctl(epoll_handle_, EPOLL_CTL_DEL, fd, &ev);
        registered_events_.erase(it);
        ready_events_.resize(registered_events_.size());
      }
    }
  }

  int poll_io(int64_t waitd_us)
  {
    ::memset(ready_events_.data(), 0x0, sizeof(epoll_event) * ready_events_.size());

#if YASIO__HAS_EPOLL_PWAIT2
    timespec timeout = {(decltype(timespec::tv_sec))(waitd_us / std::micro::den),
                        (decltype(timespec::tv_nsec))((waitd_us % std::micro::den) * std::milli::den)};
    int num_events   = ::epoll_pwait2(epoll_handle_, ready_events_.data(), static_cast<int>(ready_events_.size()), &timeout, nullptr);
#else
    int num_events = ::epoll_wait(epoll_handle_, ready_events_.data(), static_cast<int>(ready_events_.size()), static_cast<int>(waitd_us / std::milli::den));
#endif
    nevents_ = num_events;
    if (num_events > 0 && is_ready(this->interrupter_.read_descriptor(), socket_event::read))
      --num_events;
    return num_events;
  }

  void wakeup()
  {
    epoll_event ev = {0, {0}};
    ev.events      = EPOLLIN | EPOLLERR | EPOLLONESHOT;
    ev.data.ptr    = &interrupter_;
    epoll_ctl(epoll_handle_, EPOLL_CTL_MOD, interrupter_.read_descriptor(), &ev);
  }

  int is_ready(socket_native_type fd, int events) const
  {
    int underlying_events = 0;
    if (events & socket_event::read)
      underlying_events |= EPOLLIN;
    if (events & socket_event::write)
      underlying_events |= EPOLLOUT;
    if (events & socket_event::error)
      underlying_events |= (EPOLLERR | EPOLLHUP | EPOLLPRI);
    auto it = std::find_if(ready_events_.begin(), ready_events_.begin() + nevents_, [fd](const epoll_event& ev) { return ev.data.fd == fd; });
    return it != this->ready_events_.end() ? (it->events & underlying_events) : 0;
  }

  int max_descriptor() const { return -1; }

protected:
  int to_underlying_events(int events)
  {
    int underlying_events = 0;
    if (events)
    {
      if (yasio__testbits(events, socket_event::read))
        underlying_events |= EPOLLIN;

      if (yasio__testbits(events, socket_event::write))
        underlying_events |= EPOLLOUT;

      if (yasio__testbits(events, socket_event::error))
        underlying_events |= EPOLLERR;
    }
    return underlying_events;
  }

  enum
  {
    epoll_size = 20000
  };
  epoll_handle_t do_epoll_create()
  {
#if defined(EPOLL_CLOEXEC)
    epoll_handle_t handle = epoll_create1(EPOLL_CLOEXEC);
#else  // defined(EPOLL_CLOEXEC)
    epoll_handle_t handle = (epoll_handle_t)-1;
    errno                 = EINVAL;
#endif // defined(EPOLL_CLOEXEC)

    if (handle == (epoll_handle_t)-1 && (errno == EINVAL || errno == ENOSYS))
    {
      handle = epoll_create(epoll_size);
    }

    if (handle == (epoll_handle_t)-1)
    {
      yasio__throw_error0(errno);
    }

    return handle;
  }

  epoll_handle_t epoll_handle_;
  std::map<socket_native_type, int> registered_events_;
  yasio::pod_vector<epoll_event> ready_events_;
  int nevents_ = 0;

  select_interrupter interrupter_;
};
} // namespace inet
} // namespace yasio
