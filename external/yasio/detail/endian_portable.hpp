//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
/*
The MIT License (MIT)

Copyright (c) 2012-2021 HALX99

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
#ifndef YASIO__ENDIAN_PORTABLE_HPP
#define YASIO__ENDIAN_PORTABLE_HPP

#include <assert.h>
#include <string.h>
#include <stdint.h>

#ifdef _WIN32
#  if !defined(WIN32_LEAN_AND_MEAN)
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <WinSock2.h>
#  include <Windows.h>
#else
#  include <arpa/inet.h>
#endif
#include "yasio/detail/fp16.hpp"

namespace yasio
{
// clang-format off
#  define YASIO__SWAP_SHORT(s) ((((s) >> 8) & 0x00ff) | (((s) << 8) & 0xff00))
/*
 * Byte order conversion functions for 64-bit integers and 32 + 64 bit
 * floating-point numbers.  IEEE big-endian format is used for the
 * network floating point format.
 */
#define YASIO__SWAP_LONG(l)                      \
            ( ( ((l) >> 24) & 0x000000FFL ) |    \
              ( ((l) >>  8) & 0x0000FF00L ) |    \
              ( ((l) <<  8) & 0x00FF0000L ) |    \
              ( ((l) << 24) & 0xFF000000L ) )

#define YASIO__SWAP_LONGLONG(l)                           \
            ( ( ((l) >> 56) & 0x00000000000000FFLL ) |    \
              ( ((l) >> 40) & 0x000000000000FF00LL ) |    \
              ( ((l) >> 24) & 0x0000000000FF0000LL ) |    \
              ( ((l) >>  8) & 0x00000000FF000000LL ) |    \
              ( ((l) <<  8) & 0x000000FF00000000LL ) |    \
              ( ((l) << 24) & 0x0000FF0000000000LL ) |    \
              ( ((l) << 40) & 0x00FF000000000000LL ) |    \
              ( ((l) << 56) & 0xFF00000000000000LL ) )

// clang-format on
inline uint64_t(htonll)(uint64_t Value)
{
  const uint64_t Retval = YASIO__SWAP_LONGLONG(Value);
  return Retval;
}

inline uint64_t(ntohll)(uint64_t Value)
{
  const uint64_t Retval = YASIO__SWAP_LONGLONG(Value);
  return Retval;
}

YASIO__NS_INLINE
namespace endian
{
template <typename _Ty, size_t n> struct byte_order_impl {};

template <typename _Ty> struct byte_order_impl<_Ty, sizeof(int8_t)> {
  static inline _Ty host_to_network(_Ty value) { return static_cast<_Ty>(value); }
  static inline _Ty network_to_host(_Ty value) { return static_cast<_Ty>(value); }
};

template <typename _Ty> struct byte_order_impl<_Ty, sizeof(int16_t)> {
  static inline _Ty host_to_network(_Ty value) { return static_cast<_Ty>(htons(static_cast<u_short>(value))); }
  static inline _Ty network_to_host(_Ty value) { return static_cast<_Ty>(ntohs(static_cast<u_short>(value))); }
};

template <typename _Ty> struct byte_order_impl<_Ty, sizeof(int32_t)> {
  static inline _Ty host_to_network(_Ty value) { return static_cast<_Ty>(htonl(static_cast<uint32_t>(value))); }
  static inline _Ty network_to_host(_Ty value) { return static_cast<_Ty>(ntohl(static_cast<uint32_t>(value))); }
};

template <typename _Ty> struct byte_order_impl<_Ty, sizeof(int64_t)> {
  static inline _Ty host_to_network(_Ty value) { return static_cast<_Ty>((::yasio::htonll)(static_cast<uint64_t>(value))); }
  static inline _Ty network_to_host(_Ty value) { return static_cast<_Ty>((::yasio::ntohll)(static_cast<uint64_t>(value))); }
};

#if defined(YASIO_HAVE_HALF_FLOAT)
template <> struct byte_order_impl<fp16_t, sizeof(fp16_t)> {
  static inline fp16_t host_to_network(fp16_t value)
  {
    uint16_t* p = (uint16_t*)&value;
    *p          = YASIO__SWAP_SHORT(*p);
    return value;
  }
  static inline fp16_t network_to_host(fp16_t value) { return host_to_network(value); }
};
#endif

template <> struct byte_order_impl<float, sizeof(float)> {
  static inline float host_to_network(float value)
  {
    uint32_t* p = (uint32_t*)&value;
    *p          = YASIO__SWAP_LONG(*p);
    return value;
  }
  static inline float network_to_host(float value) { return host_to_network(value); }
};

template <> struct byte_order_impl<double, sizeof(double)> {
  static inline double host_to_network(double value)
  {
    uint64_t* p = (uint64_t*)&value;
    *p          = YASIO__SWAP_LONGLONG(*p);
    return value;
  }
  static inline double network_to_host(double value) { return host_to_network(value); }
};

template <typename _Ty> inline _Ty host_to_network(_Ty value) { return byte_order_impl<_Ty, sizeof(_Ty)>::host_to_network(value); }
template <typename _Ty> inline _Ty network_to_host(_Ty value) { return byte_order_impl<_Ty, sizeof(_Ty)>::network_to_host(value); }
inline int host_to_network(int value, int size)
{
  auto netval = host_to_network<unsigned int>(value);
  if (size < YASIO_SSIZEOF(int))
    netval >>= ((YASIO_SSIZEOF(int) - size) * 8);
  return static_cast<int>(netval);
}
inline int network_to_host(int value, int size)
{
  auto hostval = network_to_host<unsigned int>(value);
  if (size < YASIO_SSIZEOF(int))
    hostval >>= ((YASIO_SSIZEOF(int) - size) * 8);
  return static_cast<int>(hostval);
}

/// <summary>
/// CLASS TEMPLATE convert_traits
/// </summary>
struct network_convert_tag {};
struct host_convert_tag {};
template <typename _TT> struct convert_traits {};

template <> struct convert_traits<network_convert_tag> {
  template <typename _Ty> static inline _Ty to(_Ty value) { return host_to_network<_Ty>(value); }
  template <typename _Ty> static inline _Ty from(_Ty value) { return network_to_host<_Ty>(value); }
  static int toint(int value, int size) { return host_to_network(value, size); }
  static int fromint(int value, int size) { return network_to_host(value, size); }
};

template <> struct convert_traits<host_convert_tag> {
  template <typename _Ty> static inline _Ty to(_Ty value) { return value; }
  template <typename _Ty> static inline _Ty from(_Ty value) { return value; }
  static int toint(int value, int) { return value; }
  static int fromint(int value, int) { return value; }
};
} // namespace endian
#if !YASIO__HAS_NS_INLINE
using namespace yasio::endian;
#endif

namespace bits
{
static const unsigned char bits_wmask_table[8][8] = {
    {0xFE /*11111110*/},
    {0xFD /*11111101*/, 0xFC /*11111100*/},
    {0xFB /*11111011*/, 0xF9 /*11111001*/, 0xF8 /*11111000*/},
    {0xF7 /*11110111*/, 0xF3 /*11110011*/, 0xF1 /*11110001*/, 0xF0 /*11110000*/},
    {0xEF /*11101111*/, 0xE7 /*11100111*/, 0xE3 /*11100011*/, 0xE1 /*11100001*/, 0xE0 /*11100000*/},
    {0xDF /*11011111*/, 0xCF /*11001111*/, 0xC7 /*11000111*/, 0xC3 /*11000011*/, 0xC1 /*11000001*/, 0xC0 /*11000000*/},
    {0xBF /*10111111*/, 0x9F /*10011111*/, 0x8F /*10001111*/, 0x87 /*10000111*/, 0x83 /*10000011*/, 0x81 /*10000001*/, 0x80 /*10000000*/},
    {0x7F /*01111111*/, 0x3F /*00111111*/, 0x1F /*00011111*/, 0x0F /*00001111*/, 0x07 /*00000111*/, 0x03 /*00000011*/, 0x01 /*00000001*/, 0x00 /*00000000*/}};

static const unsigned char bits_rmask_table[8] = {
    0x01 /*00000001*/, 0x03 /*00000011*/, 0x07 /*00000111*/, 0x0F /*00001111*/, 0x1F /*00011111*/, 0x3F /*00111111*/, 0x7F /*01111111*/, 0xFF /*11111111*/
};

inline void set_bits_value(void* pByteValue, unsigned int pos, unsigned char bitsValue, unsigned int bits)
{
  assert(bits > 0 && bits <= (pos + 1) && pos < 8);

  *((unsigned char*)pByteValue) = ((*((unsigned char*)pByteValue) & bits_wmask_table[pos][bits - 1]) | (bitsValue << (pos + 1 - bits)));
}

inline unsigned char get_bits_value(unsigned char byteValue, unsigned int pos, unsigned int bits)
{
  assert(bits > 0 && bits <= (pos + 1) && pos < 8);

  return ((byteValue & bits_rmask_table[pos]) >> (pos + 1 - bits));
}
} // namespace bits

} // namespace yasio

#endif
