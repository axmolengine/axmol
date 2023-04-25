//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
//
// detail/epoll_io_watcher.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2012-2023 HALX99 (halx99 at live dot com)
#pragma once
#include <vector>
#include <chrono>
#include <map>
#include "yasio/detail/socket.hpp"
#include "yasio/detail/pod_vector.hpp"
#include "yasio/detail/select_interrupter.hpp"

#if !defined(__linux__)
#  define EPOLLET 0
#endif

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
    this->add_event(interrupter_.read_descriptor(), socket_event::read, EPOLLET);
#if defined(__linux__)
    interrupter_.interrupt();
    poll_io(1);
#endif
  }
  ~epoll_io_watcher()
  {
    this->del_event(interrupter_.read_descriptor(), socket_event::read, EPOLLET);
    epoll_close(epoll_handle_);
  }

  void add_event(socket_native_type fd, int events, int flags = 0)
  {
    int prev_events       = registered_events_[fd];
    int underlying_events = prev_events;
    if (yasio__testbits(events, socket_event::read))
      underlying_events |= EPOLLIN;

    if (yasio__testbits(events, socket_event::write))
      underlying_events |= EPOLLOUT;

    if (yasio__testbits(events, socket_event::error))
      underlying_events |= EPOLLERR;

    underlying_events |= flags;

    epoll_event ev = {0, {0}};
    ev.events      = underlying_events;
    ev.data.fd     = static_cast<int>(fd);
    int result     = ::epoll_ctl(epoll_handle_, prev_events == 0 ? EPOLL_CTL_ADD : EPOLL_CTL_MOD, fd, &ev);
    if (result == 0)
    {
      registered_events_[fd] = underlying_events;
      if (prev_events == 0)
        ready_events_.resize(registered_events_.size());
    }
  }

  void del_event(socket_native_type fd, int events, int flags = 0)
  {
    int underlying_events = registered_events_[fd];
    if (yasio__testbits(events, socket_event::read))
      underlying_events &= ~EPOLLIN;

    if (yasio__testbits(events, socket_event::write))
      underlying_events &= ~EPOLLOUT;

    if (yasio__testbits(events, socket_event::error))
      underlying_events &= ~EPOLLERR;

    underlying_events &= ~flags;

    epoll_event ev = {0, {0}};
    ev.events      = underlying_events;
    ev.data.fd     = static_cast<int>(fd);
    int result     = ::epoll_ctl(epoll_handle_, underlying_events != 0 ? EPOLL_CTL_MOD : EPOLL_CTL_DEL, fd, &ev);
    if (result == 0)
    {
      if (underlying_events != 0)
        registered_events_[fd] = underlying_events;
      else
      {
        registered_events_.erase(fd);
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
    {
#if defined(_WIN32)
      if (!interrupter_.reset())
        interrupter_.recreate();
#endif
      --num_events;
    }
    return num_events;
  }

  void wakeup()
  {
#if defined(_WIN32)
    interrupter_.interrupt();
#else
    epoll_event ev = {0, {0}};
    ev.events      = EPOLLIN | EPOLLERR | EPOLLET;
    ev.data.ptr    = &interrupter_;
    epoll_ctl(epoll_handle_, EPOLL_CTL_MOD, interrupter_.read_descriptor(), &ev);
#endif
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
