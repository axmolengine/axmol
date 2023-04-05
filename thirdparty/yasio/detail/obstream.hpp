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
#ifndef YASIO__OBSTREAM_HPP
#define YASIO__OBSTREAM_HPP
#include <stddef.h>
#include <vector>
#include <array>
#include <limits>
#include <stack>
#include <fstream>
#include "yasio/stl/string_view.hpp"
#include "yasio/detail/endian_portable.hpp"
#include "yasio/detail/utils.hpp"
#include "yasio/detail/byte_buffer.hpp"
namespace yasio
{
enum : size_t
{
  dynamic_extent = (size_t)-1
};

namespace detail
{
template <typename _Stream, typename _Intty>
inline void write_ix_impl(_Stream* stream, _Intty value)
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
template <typename _Stream, typename _Intty, bool _LargeInt>
struct write_ix_helper {};

template <typename _Stream, typename _Intty>
struct write_ix_helper<_Stream, _Intty, false> {
  static void write_ix(_Stream* stream, _Intty value) { write_ix_impl<_Stream>(stream, static_cast<int32_t>(value)); }
};

template <typename _Stream, typename _Intty>
struct write_ix_helper<_Stream, _Intty, true> {
  static void write_ix(_Stream* stream, _Intty value) { write_ix_impl<_Stream>(stream, static_cast<int64_t>(value)); }
};
} // namespace detail

class fixed_buffer_span {
public:
  using implementation_type = fixed_buffer_span;
  implementation_type& get_implementation() { return *this; }
  const implementation_type& get_implementation() const { return *this; }

  template <size_t _Extent>
  fixed_buffer_span(std::array<char, _Extent>& fb) : first_(fb.data()), last_(fb.data() + _Extent)
  {}

  template <size_t _Extent>
  fixed_buffer_span(char (&fb)[_Extent]) : first_(fb), last_(fb + _Extent)
  {}

  fixed_buffer_span(char* fb, size_t extent) : first_(fb), last_(fb + extent) {}
  fixed_buffer_span(char* first, char* last) : first_(first), last_(last) {}

  void write_byte(uint8_t value)
  {
    if (pos_ < this->max_size())
    {
      this->first_[this->pos_] = value;
      ++this->pos_;
    }
  }

  void write_bytes(const void* d, int n)
  {
    if (n > 0)
      write_bytes(this->pos_, d, n);
  }
  void write_bytes(size_t offset, const void* d, int n)
  {
    if (yasio__unlikely((offset + n) > this->max_size()))
      YASIO__THROW0(std::out_of_range("fixed_buffer_span: out of range"));
    ::memcpy(this->data() + offset, d, n);
    this->pos_ += n;
  }

  void resize(size_t newsize)
  {
    if (yasio__unlikely(newsize > max_size()))
      YASIO__THROW0(std::out_of_range("fixed_buffer_span: out of range"));
    this->pos_ = newsize;
  }

  void reserve(size_t /*capacity*/){};
  void shrink_to_fit(){};
  void clear() { this->pos_ = 0; }
  char* data() { return first_; }
  size_t length() const { return this->pos_; }
  bool empty() const { return first_ == last_; }
  size_t max_size() const { return last_ - first_; }

private:
  char* first_;
  char* last_;
  size_t pos_ = 0;
};
using fixed_buffer_view = fixed_buffer_span;

template <size_t _Extent>
class fixed_buffer : public fixed_buffer_span {
public:
  fixed_buffer() : fixed_buffer_span(impl_) {}

private:
  std::array<char, _Extent> impl_;
};

template <typename _Cont = sbyte_buffer>
class dynamic_buffer_span {
public:
  using implementation_type = _Cont;
  implementation_type& get_implementation() { return *this->outs_; }
  const implementation_type& get_implementation() const { return *this->impl_; }

  dynamic_buffer_span(_Cont* outs) : outs_(outs) {}

  void write_byte(uint8_t value) { outs_->push_back(value); }
  void write_bytes(const void* d, int n)
  {
    if (n > 0)
      outs_->insert(outs_->end(), static_cast<const uint8_t*>(d), static_cast<const uint8_t*>(d) + n);
  }
  size_t write_bytes(size_t offset, const void* d, int n)
  {
    if ((offset + n) > outs_->size())
      outs_->resize(offset + n);

    ::memcpy(outs_->data() + offset, d, n);
    return n;
  }

  void resize(size_t newsize) { outs_->resize(newsize); }
  void reserve(size_t capacity) { outs_->reserve(capacity); }
  void shrink_to_fit() { outs_->shrink_to_fit(); };
  void clear() { outs_->clear(); }
  char* data() { return outs_->data(); }
  size_t length() const { return outs_->size(); }
  bool empty() const { return outs_->empty(); }

private:
  implementation_type* outs_;
};

template <typename _Cont = sbyte_buffer>
class dynamic_buffer : public dynamic_buffer_span<_Cont> {
public:
  dynamic_buffer() : dynamic_buffer_span<_Cont>(&impl_) {}

private:
  _Cont impl_;
};

template <typename _ConvertTraits, typename _BufferType = fixed_buffer_span>
class binary_writer_impl {
public:
  using convert_traits_type        = _ConvertTraits;
  using buffer_type                = _BufferType;
  using buffer_implementation_type = typename buffer_type::implementation_type;
  using my_type                    = binary_writer_impl<convert_traits_type, buffer_type>;

  static const size_t npos = -1;

  binary_writer_impl(buffer_type* outs) : outs_(outs) {}
  ~binary_writer_impl() {}

#if defined(YASIO_OBS_BUILTIN_STACK)
  void push8()
  {
    offset_stack_.push(outs_->length());
    this->write(static_cast<uint8_t>(0));
  }
  void pop8()
  {
    auto offset = offset_stack_.top();
    this->pwrite(offset, static_cast<uint8_t>(outs_->length() - offset - sizeof(uint8_t)));
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
    offset_stack_.push(outs_->length());
    this->write(static_cast<uint16_t>(0));
  }
  void pop16()
  {
    auto offset = offset_stack_.top();
    this->pwrite(offset, static_cast<uint16_t>(outs_->length() - offset - sizeof(uint16_t)));
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
    offset_stack_.push(outs_->length());
    this->write(static_cast<uint32_t>(0));
  }
  void pop32()
  {
    auto offset = offset_stack_.top();
    this->pwrite(offset, static_cast<uint32_t>(outs_->length() - offset - sizeof(uint32_t)));
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

    auto bufsize = outs_->length();
    offset_stack_.push(bufsize);
    outs_->resize(bufsize + size);
  }

  void pop(int size)
  {
    size = yasio::clamp(size, 1, YASIO_SSIZEOF(int));

    auto offset = offset_stack_.top();
    auto value  = static_cast<int>(outs_->length() - offset - size);
    value       = convert_traits_type::toint(value, size);
    write_bytes(offset, &value, size);
    offset_stack_.pop();
  }

  void pop(int value, int size)
  {
    size = yasio::clamp(size, 1, YASIO_SSIZEOF(int));

    auto offset = offset_stack_.top();
    value       = convert_traits_type::toint(value, size);
    write_bytes(offset, &value, size);
    offset_stack_.pop();
  }
#else
  template <typename _Intty>
  size_t push()
  {
    auto where = outs_->length();
    this->outs_->resize(where + sizeof(_Intty));
    return where;
  }
  template <typename _Intty>
  void pop(size_t offset)
  {
    this->pwrite(offset, static_cast<_Intty>(outs_->length() - offset - sizeof(_Intty)));
  }
  template <typename _Intty>
  void pop(size_t offset, _Intty value)
  {
    this->pwrite(offset, value);
  }
#endif

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

  void write_byte(uint8_t value) { outs_->write_byte(value); }

  void write_bytes(cxx17::string_view v) { return write_bytes(v.data(), static_cast<int>(v.size())); }
  void write_bytes(const void* d, int n) { outs_->write_bytes(d, n); }
  void write_bytes(size_t offset, const void* d, int n) { outs_->write_bytes(offset, d, n); }

  bool empty() const { return outs_->empty(); }
  size_t length() const { return outs_->length(); }
  const char* data() const { return outs_->data(); }
  char* data() { return outs_->data(); }

  const buffer_implementation_type& buffer() const { return outs_->get_implementation(); }
  buffer_implementation_type& buffer() { return outs_->get_implementation(); }

  void clear()
  {
    outs_->clear();
#if defined(YASIO_OBS_BUILTIN_STACK)
    std::stack<size_t> tmp;
    tmp.swap(offset_stack_);
#endif
  }
  void shrink_to_fit() { outs_->shrink_to_fit(); }

  template <typename _Nty>
  inline void write(_Nty value)
  {
    auto nv = convert_traits_type::template to<_Nty>(value);
    write_bytes(&nv, sizeof(nv));
  }

  template <typename _Intty>
  void write_ix(_Intty value)
  {
    detail::write_ix_helper<my_type, _Intty, sizeof(_Intty) >= sizeof(int64_t)>::write_ix(this, value);
  }

  void write_varint(int value, int size)
  {
    size = yasio::clamp(size, 1, YASIO_SSIZEOF(int));

    value = convert_traits_type::toint(value, size);
    write_bytes(&value, size);
  }

  template <typename _Nty>
  inline void pwrite(ptrdiff_t offset, const _Nty value)
  {
    swrite(this->data() + offset, value);
  }
  template <typename _Nty>
  static void swrite(void* ptr, const _Nty value)
  {
    auto nv = convert_traits_type::template to<_Nty>(value);
    ::memcpy(ptr, &nv, sizeof(nv));
  }

  void save(const char* filename) const
  {
    std::ofstream fout;
    fout.open(filename, std::ios::binary);
    if (!this->empty())
      fout.write(this->data(), this->length());
  }

private:
  template <typename _LenT>
  inline void write_v_fx(cxx17::string_view value)
  {
    int size = static_cast<int>(value.size());
    this->write<_LenT>(static_cast<_LenT>(size));
    if (size)
      write_bytes(value.data(), size);
  }

  template <typename _Intty>
  inline void write_ix_impl(_Intty value)
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
  buffer_type* outs_;
#if defined(YASIO_OBS_BUILTIN_STACK)
  std::stack<size_t> offset_stack_;
#endif
}; // CLASS binary_writer_impl

using fixed_obstream_span      = binary_writer_impl<convert_traits<network_convert_tag>, fixed_buffer_span>;
using fast_fixed_obstream_span = binary_writer_impl<convert_traits<host_convert_tag>, fixed_buffer_span>;

using obstream_view      = fixed_obstream_span;
using fast_obstream_view = fast_fixed_obstream_span;

// -------- basic_obstream
template <typename _ConvertTraits, size_t _Extent = dynamic_extent>
class basic_obstream;

template <typename _ConvertTraits>
class basic_obstream<_ConvertTraits, dynamic_extent> : public binary_writer_impl<_ConvertTraits, dynamic_buffer<>> {
public:
  using super_type = binary_writer_impl<_ConvertTraits, dynamic_buffer<>>;
  using my_type    = basic_obstream<_ConvertTraits, dynamic_extent>;

  using buffer_type = typename super_type::buffer_type;
  basic_obstream(size_t capacity = 128) : super_type(&buffer_) { buffer_.reserve(capacity); }
  basic_obstream(const basic_obstream& rhs) : super_type(&buffer_), buffer_(rhs.buffer_) {}
  basic_obstream(basic_obstream&& rhs) : super_type(&buffer_), buffer_(std::move(rhs.buffer_)) {}
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

  basic_obstream sub(size_t offset, size_t count = super_type::npos)
  {
    basic_obstream obs;
    auto n = my_type::length();
    if (offset < n)
    {
      if (count > (n - offset))
        count = (n - offset);

      obs.write_bytes(this->data() + offset, count);
    }
    return obs;
  }

protected:
  buffer_type buffer_;
};

template <typename _ConvertTraits, size_t _Extent>
class basic_obstream : public binary_writer_impl<_ConvertTraits, fixed_buffer<_Extent>> {
public:
  using super_type = binary_writer_impl<_ConvertTraits, fixed_buffer<_Extent>>;

  using buffer_type = typename super_type::buffer_type;
  basic_obstream() : super_type(&buffer_) {}

protected:
  buffer_type buffer_;
};

template <size_t _Extent>
using obstream_any = basic_obstream<convert_traits<network_convert_tag>, _Extent>;
template <size_t _Extent>
using fast_obstream_any = basic_obstream<convert_traits<host_convert_tag>, _Extent>;

using obstream      = obstream_any<dynamic_extent>;
using fast_obstream = fast_obstream_any<dynamic_extent>;

//-------- basic_obstream_span
template <typename _ConvertTraits, typename _Cont = sbyte_buffer>
class basic_obstream_span;

template <typename _ConvertTraits, typename _Cont>
class basic_obstream_span : public binary_writer_impl<_ConvertTraits, dynamic_buffer_span<_Cont>> {
public:
  using super_type = binary_writer_impl<_ConvertTraits, dynamic_buffer_span<_Cont>>;
  using my_type    = basic_obstream_span<_ConvertTraits, _Cont>;

  using buffer_type = typename super_type::buffer_type;
  basic_obstream_span(_Cont& outs) : super_type(&span_), span_(&outs) {}

protected:
  buffer_type span_;
};

template <typename _ConvertTraits>
class basic_obstream_span<_ConvertTraits, fixed_buffer_span> : public binary_writer_impl<_ConvertTraits, fixed_buffer_span> {
public:
  using super_type = binary_writer_impl<_ConvertTraits, fixed_buffer_span>;
  using my_type    = basic_obstream_span<_ConvertTraits, fixed_buffer_span>;

  using buffer_type = typename super_type::buffer_type;

  template <size_t _Extent>
  basic_obstream_span(std::array<char, _Extent>& fb) : super_type(&span_), span_(fb)
  {}

  template <size_t _Extent>
  basic_obstream_span(char (&fb)[_Extent]) : super_type(&span_), span_(fb)
  {}

  basic_obstream_span(char* fb, size_t extent) : super_type(&span_), span_(fb, extent) {}

protected:
  buffer_type span_;
};

template <typename _Cont>
using obstream_span = basic_obstream_span<convert_traits<network_convert_tag>, _Cont>;
template <typename _Cont>
using fast_obstream_span = basic_obstream_span<convert_traits<host_convert_tag>, _Cont>;

} // namespace yasio

#endif
