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

#include <port.h>
#include "yasio/detail/socket.hpp"
#include "yasio/core/pod_vector.hpp"
#include "yasio/core/select_interrupter.hpp"

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
  evport_io_watcher() : port_handle_(::port_create()) { this->ready_events_.reserve(128); }
  ~evport_io_watcher() { ::close(port_handle_); }

  void mod_event(socket_native_type fd, int add_events, int remove_events, int flags = 0)
  {
    auto it               = registered_events_.find(fd);
    const auto registered = it != registered_events_.end();
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
        ::port_dissociate(port_handle_, PORT_SOURCE_FD, fd);
        registered_events_.erase(it);
        ready_events_.resize(registered_events_.size());
      }
    }
  }

  int poll_io(int64_t waitd_us)
  {
    ::memset(ready_events_.data(), 0x0, sizeof(port_event_t) * ready_events_.size());

    timespec timeout = {(decltype(timespec::tv_sec))(waitd_us / std::micro::den),
                        (decltype(timespec::tv_nsec))((waitd_us % std::micro::den) * std::milli::den)};

    // The nget argument points to the desired number of events to be retrieved.
    // On return, the value pointed to by nget is updated to the actual number of events retrieved in list.
    uint_t num_events = 1;
    auto ret          = ::port_getn(port_handle_, ready_events_.data(), static_cast<int>(ready_events_.size()), &num_events, &timeout);

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
      auto event_source = ready_events_[i].portev_source;
      if (event_source != PORT_SOURCE_FD)
      {
        interrupt_hint = 1;
        continue;
      }
      int fd                 = static_cast<int>(ready_events_[i].portev_object);
      auto underlying_events = registered_events_[fd];
      if (underlying_events)
        ::port_associate(port_handle_, PORT_SOURCE_FD, fd, underlying_events, nullptr);
    }

    nevents_ = num_events;
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
    auto it = std::find_if(ready_events_.begin(), this->ready_events_.begin() + static_cast<int>(nevents_),
                           [fd](const port_event_t& ev) { return static_cast<int>(ev.portev_object) == fd; });
    return it != this->ready_events_.end() ? (it->portev_events & underlying_events) : 0;
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
  std::map<socket_native_type, int> registered_events_;
  yasio::pod_vector<port_event_t> ready_events_;
  uint_t nevents_ = 0;
};
} // namespace inet
} // namespace yasio
