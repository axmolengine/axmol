//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
/*
The MIT License (MIT)

Copyright (c) 2012-2023 HALX99

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef YAISO__ERRC_HPP
#define YAISO__ERRC_HPP

namespace yasio
{
namespace errc
{
enum
{
  no_error              = 0,   // No error.
  read_timeout          = -28, // The remote host did not respond after a period of time.
  invalid_packet        = -27, // Invalid packet.
  resolve_host_failed   = -26, // Resolve host failed.
  no_available_address  = -25, // No available address to connect.
  shutdown_by_localhost = -24, // Local shutdown the connection.
  ssl_handshake_failed  = -23, // SSL handshake failed.
  ssl_write_failed      = -22, // SSL write failed.
  ssl_read_failed       = -21, // SSL read failed.
  eof                   = -20, // end of file.
};
}
} // namespace yasio

#endif
