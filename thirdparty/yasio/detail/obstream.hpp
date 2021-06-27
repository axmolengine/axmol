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
#ifndef YASIO__OBSTREAM_HPP
#define YASIO__OBSTREAM_HPP
#include <stddef.h>
#include <vector>
#include <stack>
#include <fstream>
#include "yasio/cxx17/string_view.hpp"
#include "yasio/detail/endian_portable.hpp"
#include "yasio/detail/utils.hpp"
namespace yasio
{
namespace detail
{
template <typename _Stream, typename _Intty> inline void write_ix_impl(_Stream* stream, _Intty value)
{
  // Write out an int 7 bits at a time.  The high bit of the byte,
  // when on, tells reader to continue reading more bytes.
  auto v = (typename std::make_unsigned<_Intty>::type)value; // support negative numbers
  while (v >= 0x80)
  {
    stream->write_byte((uint8_t)((uint32_t)v | 0x80));
    v >>= 7;
  }
  stream->write_byte((uint8_t)v);
}
template <typename _Stream, typename _Intty> struct write_ix_helper {};

template <typename _Stream> struct write_ix_helper<_Stream, int32_t> {
  static void write_ix(_Stream* stream, int32_t value) { write_ix_impl<_Stream, int32_t>(stream, value); }
};

template <typename _Stream> struct write_ix_helper<_Stream, int64_t> {
  static void write_ix(_Stream* stream, int64_t value) { write_ix_impl<_Stream, int64_t>(stream, value); }
};
} // namespace detail

template <typename _Traits> class basic_obstream {
public:
  using convert_traits_type = _Traits;
  using this_type           = basic_obstream<_Traits>;

  basic_obstream(size_t capacity = 128) { buffer_.reserve(capacity); }
  basic_obstream(const basic_obstream& rhs) : buffer_(rhs.buffer_) {}
  basic_obstream(basic_obstream&& rhs) : buffer_(std::move(rhs.buffer_)) {}
  ~basic_obstream() {}

  void push8()
  {
    offset_stack_.push(buffer_.size());
    this->write(static_cast<uint8_t>(0));
  }
  void pop8()
  {
    auto offset = offset_stack_.top();
    this->pwrite(offset, static_cast<uint8_t>(buffer_.size() - offset - sizeof(uint8_t)));
    offset_stack_.pop();
  }
  void pop8(uint8_t value)
  {
    auto offset = offset_stack_.top();
    this->pwrite(offset, value);
    offset_stack_.pop();
  }

  void push16()
  {
    offset_stack_.push(buffer_.size());
    this->write(static_cast<uint16_t>(0));
  }
  void pop16()
  {
    auto offset = offset_stack_.top();
    this->pwrite(offset, static_cast<uint16_t>(buffer_.size() - offset - sizeof(uint16_t)));
    offset_stack_.pop();
  }
  void pop16(uint16_t value)
  {
    auto offset = offset_stack_.top();
    this->pwrite(offset, value);
    offset_stack_.pop();
  }

  void push32()
  {
    offset_stack_.push(buffer_.size());
    this->write(static_cast<uint32_t>(0));
  }
  void pop32()
  {
    auto offset = offset_stack_.top();
    this->pwrite(offset, static_cast<uint32_t>(buffer_.size() - offset - sizeof(uint32_t)));
    offset_stack_.pop();
  }
  void pop32(uint32_t value)
  {
    auto offset = offset_stack_.top();
    this->pwrite(offset, value);
    offset_stack_.pop();
  }

  void push(int size)
  {
    size = yasio::clamp(size, 1, YASIO_SSIZEOF(int));

    auto bufsize = buffer_.size();
    offset_stack_.push(bufsize);
    buffer_.resize(bufsize + size);
  }

  void pop(int size)
  {
    size = yasio::clamp(size, 1, YASIO_SSIZEOF(int));

    auto offset = offset_stack_.top();
    auto value  = static_cast<int>(buffer_.size() - offset - size);
    value       = convert_traits_type::toint(value, size);
    ::memcpy(this->data() + offset, &value, size);
    offset_stack_.pop();
  }

  void pop(int value, int size)
  {
    size = yasio::clamp(size, 1, YASIO_SSIZEOF(int));

    auto offset = offset_stack_.top();
    value       = convert_traits_type::toint(value, size);
    ::memcpy(this->data() + offset, &value, size);
    offset_stack_.pop();
  }

  basic_obstream& operator=(const basic_obstream& rhs)
  {
    buffer_ = rhs.buffer_;
    return *this;
  }
  basic_obstream& operator=(basic_obstream&& rhs)
  {
    buffer_ = std::move(rhs.buffer_);
    return *this;
  }
  /* write blob data with '7bit encoded int' length field */
  void write_v(cxx17::string_view value)
  {
    int len = static_cast<int>(value.length());
    write_ix(len);
    write_bytes(value.data(), len);
  }

  /* 32 bits length field */
  void write_v32(cxx17::string_view value) { write_v_fx<uint32_t>(value); }
  /* 16 bits length field */
  void write_v16(cxx17::string_view value) { write_v_fx<uint16_t>(value); }
  /* 8 bits length field */
  void write_v8(cxx17::string_view value) { write_v_fx<uint8_t>(value); }

  void write_byte(uint8_t value) { buffer_.push_back(value); }

  void write_bytes(cxx17::string_view v) { return write_bytes(v.data(), static_cast<int>(v.size())); }
  void write_bytes(const void* d, int n)
  {
    if (n > 0)
      buffer_.insert(buffer_.end(), (const char*)d, (const char*)d + n);
  }
  void write_bytes(std::streamoff offset, const void* d, int n)
  {
    if ((offset + n) < static_cast<std::streamoff>(buffer_.size()))
      ::memcpy(buffer_.data() + offset, d, n);
  }

  bool empty() const { return buffer_.empty(); }
  size_t length() const { return buffer_.size(); }
  const char* data() const { return buffer_.data(); }
  char* data() { return buffer_.data(); }

  const std::vector<char>& buffer() const { return buffer_; }
  std::vector<char>& buffer() { return buffer_; }

  void clear()
  {
    buffer_.clear();
    std::stack<size_t> tmp;
    tmp.swap(offset_stack_);
  }
  void shrink_to_fit() { buffer_.shrink_to_fit(); }

  template <typename _Nty> inline void write(_Nty value)
  {
    auto nv = convert_traits_type::template to<_Nty>(value);
    write_bytes(&nv, sizeof(nv));
  }

  template <typename _Intty> void write_ix(_Intty value) { detail::write_ix_helper<this_type, _Intty>::write_ix(this, value); }

  void write_varint(int value, int size)
  {
    size = yasio::clamp(size, 1, YASIO_SSIZEOF(int));

    value = convert_traits_type::toint(value, size);
    write_bytes(&value, size);
  }

  template <typename _Nty> inline void pwrite(ptrdiff_t offset, const _Nty value) { swrite(this->data() + offset, value); }
  template <typename _Nty> static void swrite(void* ptr, const _Nty value)
  {
    auto nv = convert_traits_type::template to<_Nty>(value);
    ::memcpy(ptr, &nv, sizeof(nv));
  }

  basic_obstream sub(size_t offset, size_t count = -1)
  {
    basic_obstream obs;
    auto n = length();
    if (offset < n)
    {
      if (count > (n - offset))
        count = (n - offset);

      obs.buffer_.assign(this->data() + offset, this->data() + offset + count);
    }
    return obs;
  }

  void save(const char* filename) const
  {
    std::ofstream fout;
    fout.open(filename, std::ios::binary);
    if (!this->empty())
      fout.write(this->data(), this->length());
  }

private:
  template <typename _LenT> inline void write_v_fx(cxx17::string_view value)
  {
    int size = static_cast<int>(value.size());
    this->write<_LenT>(static_cast<_LenT>(size));
    if (size)
      write_bytes(value.data(), size);
  }

  template <typename _Intty> inline void write_ix_impl(_Intty value)
  {
    // Write out an int 7 bits at a time.  The high bit of the byte,
    // when on, tells reader to continue reading more bytes.
    auto v = (typename std::make_unsigned<_Intty>::type)value; // support negative numbers
    while (v >= 0x80)
    {
      write_byte((uint8_t)((uint32_t)v | 0x80));
      v >>= 7;
    }
    write_byte((uint8_t)v);
  }

protected:
  std::vector<char> buffer_;
  std::stack<size_t> offset_stack_;
}; // CLASS basic_obstream

using obstream      = basic_obstream<convert_traits<network_convert_tag>>;
using fast_obstream = basic_obstream<convert_traits<host_convert_tag>>;

} // namespace yasio

#endif
