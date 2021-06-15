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
#ifndef YASIO__IBSTREAM_HPP
#define YASIO__IBSTREAM_HPP
#include "yasio/detail/obstream.hpp"
namespace yasio
{
namespace detail
{
template <typename _Stream, typename _Intty> struct read_ix_helper {};

template <typename _Stream> struct read_ix_helper<_Stream, int32_t> {
  static int32_t read_ix(_Stream* stream)
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
      byteReadJustNow = stream->read_byte();
      result |= static_cast<uint32_t>(byteReadJustNow & 0x7Fu) << shift;

      if (byteReadJustNow <= 0x7Fu)
        return (int)result; // early exit
    }

    // Read the 5th byte. Since we already read 28 bits,
    // the value of this byte must fit within 4 bits (32 - 28),
    // and it must not have the high bit set.
    byteReadJustNow = stream->read_byte();
    if (byteReadJustNow <= 0x0fu)
    {
      result |= (uint32_t)byteReadJustNow << (MaxBytesWithoutOverflow * 7);
      return (int)result;
    }

    YASIO__THROW(std::logic_error("Format_Bad7BitInt32"), 0);
  }
};

template <typename _Stream> struct read_ix_helper<_Stream, int64_t> {
  static int64_t read_ix(_Stream* stream)
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
      byteReadJustNow = stream->read_byte();
      result |= static_cast<uint64_t>(byteReadJustNow & 0x7Fu) << shift;

      if (byteReadJustNow <= 0x7Fu)
        return (int64_t)result; // early exit
    }

    // Read the 10th byte. Since we already read 63 bits,
    // the value of this byte must fit within 1 bit (64 - 63),
    // and it must not have the high bit set.
    byteReadJustNow = stream->read_byte();
    if (byteReadJustNow <= 1u)
    {
      result |= (uint64_t)byteReadJustNow << (MaxBytesWithoutOverflow * 7);
      return (int64_t)result;
    }

    YASIO__THROW(std::logic_error("Format_Bad7BitInt64"), 0);
  }
};
} // namespace detail

template <typename _Traits> class basic_ibstream_view {
public:
  using convert_traits_type = _Traits;
  using this_type           = basic_ibstream_view<_Traits>;
  basic_ibstream_view() { this->reset("", 0); }
  basic_ibstream_view(const void* data, size_t size) { this->reset(data, size); }
  basic_ibstream_view(const basic_obstream<_Traits>* obs) { this->reset(obs->data(), obs->length()); }
  basic_ibstream_view(const basic_obstream<_Traits>* obs, ptrdiff_t offset)
  {
    this->reset(obs->data(), obs->length());
    this->advance(offset);
  }
  basic_ibstream_view(const basic_ibstream_view&) = delete;
  basic_ibstream_view(basic_ibstream_view&&)      = delete;

  ~basic_ibstream_view() {}

  void reset(const void* data, size_t size)
  {
    first_ = ptr_ = static_cast<const char*>(data);
    last_         = first_ + size;
  }

  basic_ibstream_view& operator=(const basic_ibstream_view&) = delete;
  basic_ibstream_view& operator=(basic_ibstream_view&&) = delete;

  /* read 7bit encoded variant integer value
  ** @dotnet BinaryReader.Read7BitEncodedInt(64)
  */
  template <typename _Intty> _Intty read_ix() { return detail::read_ix_helper<this_type, _Intty>::read_ix(this); }

  int read_varint(int size)
  {
    size = yasio::clamp(size, 1, YASIO_SSIZEOF(int));

    int value = 0;
    ::memcpy(&value, consume(size), size);
    return convert_traits_type::fromint(value, size);
  }

  /* read blob data with '7bit encoded int' length field */
  cxx17::string_view read_v()
  {
    int count = read_ix<int>();
    return read_bytes(count);
  }

  uint8_t read_byte() { return *consume(1); }

  void read_bytes(std::string& oav, int len)
  {
    if (len > 0)
    {
      oav.resize(len);
      read_bytes(&oav.front(), len);
    }
  }
  void read_bytes(void* oav, int len)
  {
    if (len > 0)
      ::memcpy(oav, consume(len), len);
  }

  cxx17::string_view read_v32() { return read_v_fx<uint32_t>(); }
  cxx17::string_view read_v16() { return read_v_fx<uint16_t>(); }
  cxx17::string_view read_v8() { return read_v_fx<uint8_t>(); }

  cxx17::string_view read_bytes(int len)
  {
    if (len > 0)
      return cxx17::string_view(consume(len), len);
    return cxx17::string_view{};
  }

  bool empty() const { return first_ == last_; }
  size_t length(void) const { return last_ - first_; }
  const char* data() const { return first_; }

  void advance(ptrdiff_t offset) { ptr_ += offset; }
  ptrdiff_t tell() const { return ptr_ - first_; }
  ptrdiff_t seek(ptrdiff_t offset, int whence)
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
        ptr_ = last_ - offset;
        break;
      case SEEK_SET:
        ptr_ = first_ + offset;
        break;
      default:;
    }
    return ptr_ - first_;
  }

  template <typename _Nty> inline _Nty read() { return sread<_Nty>(consume(sizeof(_Nty))); }
  template <typename _Nty> static _Nty sread(const void* ptr)
  {
    _Nty value;
    ::memcpy(&value, ptr, sizeof(value));
    return convert_traits_type::template from<_Nty>(value);
  }

  template <typename _LenT> inline cxx17::string_view read_v_fx()
  {
    _LenT n = this->read<_LenT>();
    if (n > 0)
      return read_bytes(n);
    return {};
  }

protected:
  // will throw std::out_of_range
  const char* consume(size_t size)
  {
    if ((ptr_ + size) <= last_)
    {
      auto ptr = ptr_;
      ptr_ += size;
      return ptr;
    }
    YASIO__THROW(std::out_of_range("ibstream_view::consume out of range!"), nullptr);
  }

protected:
  const char* first_;
  const char* last_;
  const char* ptr_;
};

/// --------------------- CLASS ibstream ---------------------
template <typename _Traits> class basic_ibstream : public basic_ibstream_view<_Traits> {
public:
  basic_ibstream() {}
  basic_ibstream(std::vector<char> blob) : basic_ibstream_view<_Traits>(), blob_(std::move(blob)) { this->reset(blob_.data(), static_cast<int>(blob_.size())); }
  basic_ibstream(const basic_obstream<_Traits>* obs) : basic_ibstream_view<_Traits>(), blob_(obs->buffer())
  {
    this->reset(blob_.data(), static_cast<int>(blob_.size()));
  }

  bool load(const char* filename)
  {
    std::ifstream fin;
    fin.open(filename, std::ios::binary);
    if (fin.is_open())
    {
      fin.seekg(0, std::ios_base::end);
      auto size = fin.tellg();
      if (size > 0)
      {
        blob_.resize(static_cast<size_t>(size));
        fin.seekg(0, std::ios_base::beg);
        fin.read(blob_.data(), blob_.size());
        this->reset(blob_.data(), static_cast<int>(blob_.size()));
        return true;
      }
    }
    return false;
  }

private:
  std::vector<char> blob_;
};

using ibstream_view = basic_ibstream_view<convert_traits<network_convert_tag>>;
using ibstream      = basic_ibstream<convert_traits<network_convert_tag>>;

using fast_ibstream_view = basic_ibstream_view<convert_traits<host_convert_tag>>;
using fast_ibstream      = basic_ibstream<convert_traits<host_convert_tag>>;

} // namespace yasio

#endif
