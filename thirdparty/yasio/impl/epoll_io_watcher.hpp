//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2012-2024 HALX99 (halx99 at live dot com)
#ifndef YASIO__EPOLL_IO_WATCHER_HPP
#define YASIO__EPOLL_IO_WATCHER_HPP
#include <vector>
#include <chrono>
#include <map>
#include "yasio/pod_vector.hpp"
#include "yasio/impl/socket.hpp"
#include "yasio/impl/select_interrupter.hpp"

#if !defined(_WIN32)
#  define epoll_close close
typedef int epoll_handle_t;
#else
#  include "wepoll/wepoll.h"
#  undef YASIO__HAS_EPOLL
#  define YASIO__HAS_EPOLL 1
typedef void* epoll_handle_t;
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
    this->revents_.reserve(16);
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
    auto it               = events_.find(fd);
    const auto registered = it != events_.end();
    int underlying_events = registered ? it->second : 0;
    underlying_events |= to_underlying_events(add_events);
    underlying_events &= ~to_underlying_events(remove_events);

    epoll_event ev = {0, {0}};
    ev.events      = underlying_events | flags;
    ev.data.fd     = static_cast<int>(fd);

    if (underlying_events)
    { // add or mod
      if (::epoll_ctl(epoll_handle_, !registered ? EPOLL_CTL_ADD : EPOLL_CTL_MOD, fd, &ev) == 0)
      {
        if (registered)
          it->second = underlying_events;
        else
          events_[fd] = underlying_events;
      }
    }
    else
    { // del if registered
      if (registered)
      {
        ::epoll_ctl(epoll_handle_, EPOLL_CTL_DEL, fd, &ev);
        events_.erase(it);
      }
    }

    max_events_ = (std::min)(static_cast<int>(events_.size()), 128);
  }

  int poll_io(int64_t waitd_us)
  {
    assert(max_events_ > 0);
    this->revents_.reset(max_events_);

#if YASIO__HAS_EPOLL_PWAIT2
    timespec timeout = {(decltype(timespec::tv_sec))(waitd_us / std::micro::den),
                        (decltype(timespec::tv_nsec))((waitd_us % std::micro::den) * std::milli::den)};
    int num_events   = ::epoll_pwait2(epoll_handle_, revents_.data(), static_cast<int>(revents_.size()), &timeout, nullptr);
#else
    int num_events   = ::epoll_wait(epoll_handle_, revents_.data(), static_cast<int>(revents_.size()), static_cast<int>(waitd_us / std::milli::den));
#endif
    nrevents_ = num_events;
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
    auto it = std::find_if(revents_.begin(), revents_.begin() + nrevents_, [fd](const epoll_event& ev) { return ev.data.fd == fd; });
    return it != this->revents_.end() ? (it->events & underlying_events) : 0;
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
      handle = epoll_create(epoll_size);

    return handle;
  }

  epoll_handle_t epoll_handle_;

  int max_events_ = 0;
  int nrevents_   = 0;
  std::map<socket_native_type, int> events_;
  yasio::pod_vector<epoll_event> revents_;

  select_interrupter interrupter_;
};
} // namespace inet
} // namespace yasio
#endif
