//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2012-2024 HALX99 (halx99 at live dot com)
#ifndef YASIO__KQUEUE_IO_WATCHER_HPP
#define YASIO__KQUEUE_IO_WATCHER_HPP
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <chrono>
#include <map>
#include "yasio/pod_vector.hpp"
#include "yasio/impl/socket.hpp"
#include "yasio/impl/select_interrupter.hpp"

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
    revents_.reserve(32);
    this->register_event(interrupter_.read_descriptor(), socket_event::read);
  }
  ~kqueue_io_watcher()
  {
    this->deregister_event(interrupter_.read_descriptor(), socket_event::read);
    close(kqueue_fd_);
  }

  void mod_event(socket_native_type fd, int add_events, int remove_events)
  {
    if (add_events)
      register_event(fd, add_events);
    if (remove_events)
      deregister_event(fd, remove_events);
  }

  int poll_io(int64_t waitd_us)
  {
    assert(max_events_ > 0);
    revents_.reset(max_events_);

    timespec timeout = {(decltype(timespec::tv_sec))(waitd_us / std::micro::den),
                        (decltype(timespec::tv_nsec))((waitd_us % std::micro::den) * std::milli::den)};
    int num_events   = kevent(kqueue_fd_, 0, 0, revents_.data(), static_cast<int>(revents_.size()), &timeout);
    nrevents_ = num_events;
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
    auto it = std::find_if(revents_.begin(), this->revents_.begin() + nrevents_, [fd, events](const struct kevent& ev) {
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
    return it != revents_.end() ? -it->filter : 0;
  }

  int max_descriptor() const { return -1; }

protected:
  void register_event(socket_native_type fd, int events)
  {
    int prev_events = events_[fd];
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
        events_[fd] = events;
        int diff               = nkvlist - nkv_old;
        if (diff != 0)
          max_events_ += diff;
      }
    }
  }

  void deregister_event(socket_native_type fd, int events)
  {
    int curr_events = events_[fd];
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
          events_[fd] = curr_events;
        else
          events_.erase(fd);
        int diff = nkvlist - curr_count;
        if (diff != 0)
          max_events_ += diff;
      }
    }
  }

  int kqueue_fd_;
  int max_events_ = 0;
  int nrevents_ = 0;
  std::map<socket_native_type, int> events_;
  yasio::pod_vector<struct kevent> revents_;
  select_interrupter interrupter_;
};
} // namespace inet
} // namespace yasio
#endif
