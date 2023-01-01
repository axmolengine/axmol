//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
//
// detail/socket_select_interrupter.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2012-2023 HALX99 (halx99 at live dot com)
// Copyright (c) 2003-2020 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Copyright (c) 2008 Roelof Naude (roelof.naude at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// see also: https://github.com/chriskohlhoff/asio
//
#ifndef YASIO__SOCKET_SELECT_INTERRUPTER_HPP
#define YASIO__SOCKET_SELECT_INTERRUPTER_HPP
#include "yasio/xxsocket.hpp"

namespace yasio
{
YASIO__NS_INLINE
namespace inet
{
class socket_select_interrupter {
public:
  // Constructor.
  inline socket_select_interrupter() { open_descriptors(); }

  // Destructor.
  inline ~socket_select_interrupter() { close_descriptors(); }

  // Recreate the interrupter's descriptors. Used after a fork.
  inline void recreate()
  {
    close_descriptors();

    write_descriptor_ = invalid_socket;
    read_descriptor_  = invalid_socket;

    open_descriptors();
  }

  // Interrupt the select call.
  inline void interrupt() { xxsocket::send(write_descriptor_, "\0", 1); }

  // Reset the select interrupter. Returns true if the reset was successful.
  inline bool reset()
  {
    char data[1024];
    for (;;)
    {
      int bytes_read = xxsocket::recv(read_descriptor_, data, sizeof(data), 0);
      if (bytes_read == sizeof(data))
        continue;
      if (bytes_read > 0)
        return true;
      if (bytes_read == 0)
        return false;
      int ec = xxsocket::get_last_errno();
      if (ec == EINTR)
        continue;
      return (ec == EWOULDBLOCK || ec == EAGAIN);
    }
  }

  // Get the read descriptor to be passed to select.
  socket_native_type read_descriptor() const { return read_descriptor_; }

private:
  // Open the descriptors. Throws on error.
  inline void open_descriptors()
  {
    xxsocket acceptor(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    acceptor.set_optval(SOL_SOCKET, SO_REUSEADDR, 1);

    ip::endpoint ep(INADDR_LOOPBACK, 0);

    int retval = acceptor.bind(ep);
    if (retval < 0)
      yasio__throw_error(xxsocket::get_last_errno(), "socket_select_interrupter");
    ep = acceptor.local_endpoint();
    // Some broken firewalls on Windows will intermittently cause getsockname to
    // return 0.0.0.0 when the socket is actually bound to 127.0.0.1. We
    // explicitly specify the target address here to work around this problem.
    if (INADDR_ANY == ep.addr_v4())
      ep.addr_v4(INADDR_LOOPBACK);
    retval = acceptor.listen();
    if (retval < 0)
      yasio__throw_error(xxsocket::get_last_errno(), "socket_select_interrupter");

    xxsocket client(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    retval = client.connect(ep);
    if (retval < 0)
      yasio__throw_error(xxsocket::get_last_errno(), "socket_select_interrupter");

    auto server = acceptor.accept();
    if (!server.is_open())
      yasio__throw_error(xxsocket::get_last_errno(), "socket_select_interrupter");

    client.set_nonblocking(true);
    client.set_optval(IPPROTO_TCP, TCP_NODELAY, 1);

    server.set_nonblocking(true);
    server.set_optval(IPPROTO_TCP, TCP_NODELAY, 1);

    read_descriptor_  = server.release_handle();
    write_descriptor_ = client.release_handle();
  }

  // Close the descriptors.
  inline void close_descriptors()
  {
    if (read_descriptor_ != invalid_socket)
      ::closesocket(read_descriptor_);

    if (write_descriptor_ != invalid_socket)
      ::closesocket(write_descriptor_);
  }

  // The read end of a connection used to interrupt the select call. This file
  // descriptor is passed to select such that when it is time to stop, a single
  // byte will be written on the other end of the connection and this
  // descriptor will become readable.
  socket_native_type read_descriptor_;

  // The write end of a connection used to interrupt the select call. A single
  // byte may be written to this to wake up the select which is waiting for the
  // other end to become readable.
  socket_native_type write_descriptor_;
};

} // namespace inet
} // namespace yasio

#endif // YASIO__SOCKET_SELECT_INTERRUPTER_HPP
