//////////////////////////////////////////////////////////////////////////////////////////
// A cross platform socket APIs, support ios & android & wp8 & window store
// universal app
//////////////////////////////////////////////////////////////////////////////////////////
/*
The MIT License (MIT)

Copyright (c) 2012-2020 HALX99

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
#ifndef YASIO__IBSTREAM_CPP
#define YASIO__IBSTREAM_CPP

#include "yasio/obstream.hpp"
#if !defined(YASIO_HEADER_ONLY)
#  include "yasio/ibstream.hpp"
#endif

#ifdef _WIN32
#  pragma comment(lib, "ws2_32.lib")
#endif

namespace yasio
{

ibstream_view::ibstream_view() { this->reset("", 0); }

ibstream_view::ibstream_view(const void* data, size_t size) { this->reset(data, size); }

ibstream_view::ibstream_view(const obstream* obs) { this->reset(obs->data(), obs->length()); }

ibstream_view::~ibstream_view() {}

void ibstream_view::reset(const void* data, size_t size)
{
  first_ = ptr_ = static_cast<const char*>(data);
  last_         = first_ + size;
}

template <> int32_t ibstream_view::read_ix<int32_t>()
{
  // Unlike writing, we can't delegate to the 64-bit read on
  // 64-bit platforms. The reason for this is that we want to
  // stop consuming bytes if we encounter an integer overflow.
  uint32_t result = 0;
  uint8_t byteReadJustNow;

  // Read the integer 7 bits at a time. The high bit
  // of the byte when on means to continue reading more bytes.
  //
  // There are two failure cases: we've read more than 5 bytes,
  // or the fifth byte is about to cause integer overflow.
  // This means that we can read the first 4 bytes without
  // worrying about integer overflow.
  const int MaxBytesWithoutOverflow = 4;
  for (int shift = 0; shift < MaxBytesWithoutOverflow * 7; shift += 7)
  {
    // ReadByte handles end of stream cases for us.
    byteReadJustNow = read_byte();
    result |= static_cast<uint32_t>(byteReadJustNow & 0x7Fu) << shift;

    if (byteReadJustNow <= 0x7Fu)
      return (int)result; // early exit
  }

  // Read the 5th byte. Since we already read 28 bits,
  // the value of this byte must fit within 4 bits (32 - 28),
  // and it must not have the high bit set.
  byteReadJustNow = read_byte();
  if (byteReadJustNow <= 0x0fu)
  {
    result |= (uint32_t)byteReadJustNow << (MaxBytesWithoutOverflow * 7);
    return (int)result;
  }

  YASIO__THROW(std::logic_error("Format_Bad7BitInt32"), 0);
}

template <> int64_t ibstream_view::read_ix<int64_t>()
{
  uint64_t result = 0;
  uint8_t byteReadJustNow;

  // Read the integer 7 bits at a time. The high bit
  // of the byte when on means to continue reading more bytes.
  //
  // There are two failure cases: we've read more than 10 bytes,
  // or the tenth byte is about to cause integer overflow.
  // This means that we can read the first 9 bytes without
  // worrying about integer overflow.
  const int MaxBytesWithoutOverflow = 9;
  for (int shift = 0; shift < MaxBytesWithoutOverflow * 7; shift += 7)
  {
    // ReadByte handles end of stream cases for us.
    byteReadJustNow = read_byte();
    result |= static_cast<uint64_t>(byteReadJustNow & 0x7Fu) << shift;

    if (byteReadJustNow <= 0x7Fu)
      return (int64_t)result; // early exit
  }

  // Read the 10th byte. Since we already read 63 bits,
  // the value of this byte must fit within 1 bit (64 - 63),
  // and it must not have the high bit set.
  byteReadJustNow = read_byte();
  if (byteReadJustNow <= 1u)
  {
    result |= (uint64_t)byteReadJustNow << (MaxBytesWithoutOverflow * 7);
    return (int64_t)result;
  }

  YASIO__THROW(std::logic_error("Format_Bad7BitInt64"), 0);
}

int32_t ibstream_view::read_i24()
{
  int32_t value = 0;
  auto ptr      = consume(3);
  memcpy(&value, ptr, 3);
  value = ntohl(value) >> 8;

  if (value >> 23)
    return -(0x7FFFFF - (value & 0x7FFFFF)) - 1;
  else
    return value & 0x7FFFFF;
}

uint32_t ibstream_view::read_u24()
{
  uint32_t value = 0;
  auto ptr       = consume(3);
  memcpy(&value, ptr, 3);
  return ntohl(value) >> 8;
}

cxx17::string_view ibstream_view::read_v()
{
  int count = read_ix<int>();
  return read_bytes(count);
}

cxx17::string_view ibstream_view::read_v32() { return read_v_fx<uint32_t>(); }
cxx17::string_view ibstream_view::read_v16() { return read_v_fx<uint16_t>(); }
cxx17::string_view ibstream_view::read_v8() { return read_v_fx<uint8_t>(); }

uint8_t ibstream_view::read_byte() { return *consume(1); }

void ibstream_view::read_bytes(std::string& oav, int len)
{
  if (len > 0)
  {
    oav.resize(len);
    read_bytes(&oav.front(), len);
  }
}

void ibstream_view::read_bytes(void* oav, int len)
{
  if (len > 0)
    ::memcpy(oav, consume(len), len);
}

cxx17::string_view ibstream_view::read_bytes(int len)
{
  if (len > 0)
    return cxx17::string_view(consume(len), len);
  return cxx17::string_view{};
}

const char* ibstream_view::consume(size_t size)
{
  if (ptr_ >= last_)
    YASIO__THROW(std::out_of_range("ibstream_view::consume out of range!"), nullptr);

  auto ptr = ptr_;
  ptr_ += size;

  return ptr;
}

ptrdiff_t ibstream_view::seek(ptrdiff_t offset, int whence)
{
  switch (whence)
  {
    case SEEK_CUR:
      ptr_ += offset;
      if (ptr_ < first_)
        ptr_ = first_;
      else if (ptr_ > last_)
        ptr_ = last_;
      break;
    case SEEK_END:
      ptr_ = first_;
      break;
    case SEEK_SET:
      ptr_ = last_;
      break;
    default:;
  }
  return ptr_ - first_;
}

/// --------------------- CLASS ibstream ---------------------
ibstream::ibstream(std::vector<char> blob) : ibstream_view(), blob_(std::move(blob)) { this->reset(blob_.data(), static_cast<int>(blob_.size())); }
ibstream::ibstream(const obstream* obs) : ibstream_view(), blob_(obs->buffer()) { this->reset(blob_.data(), static_cast<int>(blob_.size())); }

} // namespace yasio

#endif
