//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2012-2024 HALX99 (halx99 at live dot com)
#ifndef YASIO__EVPORT_IO_WATCHER_HPP
#define YASIO__EVPORT_IO_WATCHER_HPP
#include <chrono>
#include <map>

#include <port.h>

#include "yasio/pod_vector.hpp"
#include "yasio/impl/socket.hpp"
#include "yasio/impl/select_interrupter.hpp"

/*
 * port_event_t
 * struct {
 * int portev_events;
 * ushort_t portev_source;
 * uintptr_t portev_object;
 * void* portev_user;
 * }
 */

namespace yasio
{
YASIO__NS_INLINE
namespace inet
{
class evport_io_watcher {
public:
  evport_io_watcher() : port_handle_(::port_create()) { this->revents_.reserve(16); }
  ~evport_io_watcher() { ::close(port_handle_); }

  void mod_event(socket_native_type fd, int add_events, int remove_events, int flags = 0)
  {
    auto it               = events_.find(fd);
    const auto registered = it != events_.end();
    int underlying_events = registered ? it->second : 0;
    underlying_events |= to_underlying_events(add_events);
    underlying_events &= ~to_underlying_events(remove_events);

    if (underlying_events)
    { // add or mod
      if (::port_associate(port_handle_, PORT_SOURCE_FD, fd, underlying_events, nullptr) == 0)
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
        ::port_dissociate(port_handle_, PORT_SOURCE_FD, fd);
        events_.erase(it);
      }
    }
    max_events_ = (std::min)(static_cast<uint_t>(events_.size()), 128u);
  }

  int poll_io(int64_t waitd_us)
  {
    assert(max_events_ > 0);
    this->revents_.reset(max_events_);

    timespec timeout = {(decltype(timespec::tv_sec))(waitd_us / std::micro::den),
                        (decltype(timespec::tv_nsec))((waitd_us % std::micro::den) * std::milli::den)};

    // The nget argument points to the desired number of events to be retrieved.
    // On return, the value pointed to by nget is updated to the actual number of events retrieved in list.
    uint_t num_events = 1;
    auto ret          = ::port_getn(port_handle_, revents_.data(), static_cast<int>(revents_.size()), &num_events, &timeout);

    // re-associate
    /*
     * PORT_SOURCE_FD events represent a transition in the poll(2) status of a given file descriptor.
     * Once an event is delivered, the file descriptor is no longer associated with the port.
     * A file descriptor is associated (or re-associated) with a port using the port_associate(3C) function.
     * refer to: https://docs.oracle.com/cd/E19253-01/816-5168/port-create-3c/index.html
     */
    int interrupt_hint = 0;
    for (int i = 0; i < num_events; ++i)
    {
      auto event_source = revents_[i].portev_source;
      if (event_source != PORT_SOURCE_FD)
      {
        interrupt_hint = 1;
        continue;
      }
      int fd                 = static_cast<int>(revents_[i].portev_object);
      auto underlying_events = events_[fd];
      if (underlying_events)
        ::port_associate(port_handle_, PORT_SOURCE_FD, fd, underlying_events, nullptr);
    }

    nrevents_ = num_events;
    num_events -= interrupt_hint;
  }

  void wakeup() { ::port_send(port_handle_, POLLIN, nullptr); }

  int is_ready(socket_native_type fd, int events) const
  {
    int underlying_events = 0;
    if (events & socket_event::read)
      underlying_events |= POLLIN;
    if (events & socket_event::write)
      underlying_events |= POLLOUT;
    if (events & socket_event::error)
      underlying_events |= (POLLERR | POLLHUP | POLLPRI);
    auto it = std::find_if(revents_.begin(), this->revents_.begin() + static_cast<int>(nrevents_),
                           [fd](const port_event_t& ev) { return static_cast<int>(ev.portev_object) == fd; });
    return it != this->revents_.end() ? (it->portev_events & underlying_events) : 0;
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

  int port_handle_;
  uint_t max_events_ = 0;
  uint_t nrevents_   = 0;
  std::map<socket_native_type, int> events_;
  yasio::pod_vector<port_event_t> revents_;
};
} // namespace inet
} // namespace yasio
#endif
