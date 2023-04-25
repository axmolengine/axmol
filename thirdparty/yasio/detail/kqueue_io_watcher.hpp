//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
//
// detail/kqueue_io_watcher.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2012-2023 HALX99 (halx99 at live dot com)
#pragma once
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <vector>
#include <chrono>
#include <map>
#include "yasio/detail/socket.hpp"
#include "yasio/detail/pod_vector.hpp"
#include "yasio/detail/select_interrupter.hpp"

#if defined(__NetBSD__) && __NetBSD_Version__ < 999001500
#  define YASIO_KQUEUE_EV_SET(ev, ident, filt, flags, fflags, data, udata) \
    EV_SET(ev, ident, filt, flags, fflags, data, reinterpret_cast<intptr_t>(static_cast<void*>(udata)))
#else
#  define YASIO_KQUEUE_EV_SET(ev, ident, filt, flags, fflags, data, udata) EV_SET(ev, ident, filt, flags, fflags, data, udata)
#endif

namespace yasio
{
YASIO__NS_INLINE
namespace inet
{
class kqueue_io_watcher {
public:
  kqueue_io_watcher() : kqueue_fd_(kqueue())
  {
    ready_events_.reserve(128);
    this->add_event(interrupter_.read_descriptor(), socket_event::read);
  }
  ~kqueue_io_watcher()
  {
    this->del_event(interrupter_.read_descriptor(), socket_event::read);
    close(kqueue_fd_);
  }

  void add_event(socket_native_type fd, int events)
  {
    int prev_events = registered_events_[fd];
    int nkv_old     = prev_events > 0 ? ((prev_events >> 1) + 1) : 0;

    struct kevent kevlist[3];
    int nkvlist = 0;
    if (yasio__testbits(events, socket_event::read))
    {
      YASIO_KQUEUE_EV_SET(&kevlist[nkvlist], fd, EVFILT_READ, EV_ADD, 0, 0, reinterpret_cast<void*>(static_cast<intptr_t>(fd)));
      ++nkvlist;
    }

    if (yasio__testbits(events, socket_event::write))
    {
      YASIO_KQUEUE_EV_SET(&kevlist[nkvlist], fd, EVFILT_WRITE, EV_ADD, 0, 0, reinterpret_cast<void*>(static_cast<intptr_t>(fd)));
      ++nkvlist;
    }

#if defined(EVFILT_EXCEPT)
    if (yasio__testbits(events, socket_event::error))
    {
      YASIO_KQUEUE_EV_SET(&kevlist[nkvlist], fd, EVFILT_EXCEPT, EV_ADD, 0, 0, reinterpret_cast<void*>(static_cast<intptr_t>(fd)));
      ++nkvlist;
    }
#endif

    if (nkvlist > 0)
    {
      int ret = ::kevent(kqueue_fd_, kevlist, nkvlist, 0, 0, 0);
      if (ret != -1)
      {
        registered_events_[fd] = events;
        int diff               = nkvlist - nkv_old;
        if (diff != 0)
          ready_events_.resize(registered_events_.size() + diff);
      }
    }
  }

  void del_event(socket_native_type fd, int events)
  {
    int curr_events = registered_events_[fd];
    int curr_count  = curr_events > 0 ? ((curr_events >> 1) + 1) : 0;

    struct kevent kevlist[3];
    int nkvlist = 0;
    if (yasio__testbits(events, socket_event::read))
    {
      curr_events &= ~socket_event::read;
      YASIO_KQUEUE_EV_SET(&kevlist[nkvlist], fd, EVFILT_READ, EV_DELETE, 0, 0, nullptr);
      ++nkvlist;
    }

    if (yasio__testbits(events, socket_event::write))
    {
      curr_events &= ~socket_event::write;
      YASIO_KQUEUE_EV_SET(&kevlist[nkvlist], fd, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr);
      ++nkvlist;
    }

#if defined(EVFILT_EXCEPT)
    if (yasio__testbits(events, socket_event::error))
    {
      curr_events &= ~socket_event::error;
      YASIO_KQUEUE_EV_SET(&kevlist[nkvlist], fd, EVFILT_EXCEPT, EV_DELETE, 0, 0, nullptr);
      ++nkvlist;
    }
#endif

    if (nkvlist > 0)
    {
      int ret = ::kevent(kqueue_fd_, kevlist, nkvlist, 0, 0, 0);
      if (ret == 0)
      {
        if (curr_events != 0)
          registered_events_[fd] = curr_events;
        else
          registered_events_.erase(fd);
        int diff = nkvlist - curr_count;
        if (diff != 0)
          ready_events_.resize(ready_events_.size() + diff);
      }
    }
  }

  int poll_io(int64_t waitd_us)
  {
    ::memset(ready_events_.data(), 0x0, sizeof(struct kevent) * ready_events_.size());

    timespec timeout = {(decltype(timespec::tv_sec))(waitd_us / std::micro::den),
                        (decltype(timespec::tv_nsec))((waitd_us % std::micro::den) * std::milli::den)};
    int num_events   = kevent(kqueue_fd_, 0, 0, ready_events_.data(), static_cast<int>(ready_events_.size()), &timeout);
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
    auto it = std::find_if(ready_events_.begin(), this->ready_events_.end(), [fd, events](const struct kevent& ev) {
      int rfd = static_cast<int>(reinterpret_cast<intptr_t>(ev.udata));
      if (rfd == fd)
      {
        if (ev.flags & EV_ERROR) 
            return !!(events & socket_event::error);
        switch (ev.filter)
        {
          case EVFILT_READ:
            return !!(events & socket_event::read);
          case EVFILT_WRITE:
            return !!(events & socket_event::write);
#if defined(EVFILT_EXCEPT)
          case EVFILT_EXCEPT:
            return !!(events & socket_event::error);
#endif
          default:
            return false;
        }
      }
      return false;
    });
    return it != ready_events_.end() ? -it->filter : 0;
  }

  int max_descriptor() const { return -1; }

protected:
  int kqueue_fd_;
  std::map<socket_native_type, int> registered_events_;
  yasio::pod_vector<struct kevent> ready_events_;
  select_interrupter interrupter_;
};
} // namespace inet
} // namespace yasio
