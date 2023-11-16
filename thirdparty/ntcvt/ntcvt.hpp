// v1.0.1 add vs2022 17.8.0 support
// v1.0.0 works on vs2010 to vs2022 17.7.x

#ifndef SIMDSOFT__NTCVT_HPP
#define SIMDSOFT__NTCVT_HPP

#pragma once
#if !defined(WIN32_LEAN_AND_MEAN)
#  define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <string>

#if !defined(NTCVT_CP_DEFAULT)
#  define NTCVT_CP_DEFAULT CP_UTF8
#endif

namespace ntcvt
{

namespace detail
{
// different with resize, not all of fill new memory with '\0'
template <class _Elem, class _Traits = std::char_traits<_Elem>,
          class _Alloc = std::allocator<_Elem>>
class intrusive_string : public std::basic_string<_Elem, _Traits, _Alloc>
{
public:
#if _MSC_VER >= 1920 // VS2019+
  using _Alty        = std::_Rebind_alloc_t<_Alloc, _Elem>;
  using _Alty_traits = std::allocator_traits<_Alty>;

  using _Scary_val = std::_String_val<
      std::conditional_t<std::_Is_simple_alloc_v<_Alty>, std::_Simple_types<_Elem>,
                         std::_String_iter_types<
                             _Elem, typename _Alty_traits::size_type,
                             typename _Alty_traits::difference_type, typename _Alty_traits::pointer,
#   if _MSC_VER < 1938
                             typename _Alty_traits::const_pointer, _Elem&, const _Elem&>>>;
#   else //  VS2022 17.8.0+
                             typename _Alty_traits::const_pointer>>>;
#   endif

#endif
  // See also afxmfc CString::GetBufferSetLength
  // Why do this hack?
  // stupid: because the default c++ standard resize always fill with '\0'
  // std::string: use memset (usually implemented with SIMD)
  // std::wstring: for loop (slow performance)
  // only works on msvc currently
#if !defined(_MSVC) && (!defined(_HAS_CXX23) || !_HAS_CXX23)
  template <class _Operation> void resize_and_overwrite(const size_t _New_size, _Operation&& _Op)
  {
    this->reserve(_New_size);

#  if _MSC_VER >= 1920 // VS2019+
    std::_Compressed_pair<_Alty, _Scary_val>* _Myval =
        (std::_Compressed_pair<_Alty, _Scary_val>*)this;
    auto& _Myval2                 = _Myval->_Myval2;
    auto result_size              = _Op(_Myval2._Myptr(), _New_size);
    _Myval2._Mysize               = result_size;
    _Myval2._Myptr()[result_size] = '\0';
#  else
    auto result_size = _Op(this->_Myptr(), _New_size);
    this->_Eos(result_size);
#  endif
  }
#endif
};

template <typename _Cont> struct buffer_traits
{
  typedef _Cont container_type;

  template <typename _Operation>
  static inline void resize_and_overwrite(container_type& str, size_t size, _Operation&& op)
  {
    typedef typename container_type::value_type _Elem;
    intrusive_string<_Elem>& helper = (intrusive_string<_Elem>&)str;
    helper.resize_and_overwrite(size, std::move(op));
  }
};

#if defined(_AFX)
template <> struct buffer_traits<CStringW>
{
  using container_type = CStringW;
  template <typename _Operation>
  static inline void resize_and_overwrite(container_type& str, size_t size, _Operation&& op)
  {
    auto ptr  = str.GetBufferSetLength(static_cast<int>(size));
    auto nret = op(ptr, size);
    str.ReleaseBufferSetLength(nret);
  }
};

template <> struct buffer_traits<CStringA>
{
  using container_type = CStringA;
  template <typename _Operation>
  static inline void resize_and_overwrite(container_type& str, size_t size, _Operation&& op)
  {
    auto ptr  = str.GetBufferSetLength(static_cast<int>(size));
    auto nret = op(ptr, size);
    str.ReleaseBufferSetLength(nret);
  }
};
#endif
} // namespace detail

template <typename _StringContType>
inline _StringContType wcbs2a(const wchar_t* wcb, int len, UINT cp = NTCVT_CP_DEFAULT)
{
  if (len == -1)
    len = static_cast<int>(wcslen(wcb));
  _StringContType buffer;
  int cch;
  if (len > 0 && (cch = ::WideCharToMultiByte(cp, 0, wcb, len, NULL, 0, NULL, NULL)) > 0)
  {
    detail::buffer_traits<_StringContType>::resize_and_overwrite(
        buffer, cch, [cp, wcb, len, cch](char* out, size_t) {
          return ::WideCharToMultiByte(cp, 0, wcb, len, out, cch, NULL, NULL);
        });
  }
  return buffer;
}

template <typename _StringContType>
inline _StringContType mcbs2w(const char* mcb, int len, UINT cp = NTCVT_CP_DEFAULT)
{
  if (len == -1)
    len = static_cast<int>(strlen(mcb));
  _StringContType buffer;
  int cch;
  if (len > 0 && (cch = ::MultiByteToWideChar(cp, 0, mcb, len, NULL, 0)) > 0)
  {
    detail::buffer_traits<_StringContType>::resize_and_overwrite(
        buffer, cch, [cp, mcb, len, cch](wchar_t* out, size_t) {
          return ::MultiByteToWideChar(cp, 0, mcb, len, out, cch);
        });
  }

  return buffer;
}

inline int mcbs2w(const char* mcb, int len, wchar_t* wbuf, int wbuf_len, UINT cp = NTCVT_CP_DEFAULT)
{
  if (len == -1)
    len = static_cast<int>(strlen(mcb));
  int cch;
  if (len > 0 && (cch = ::MultiByteToWideChar(cp, 0, mcb, len, NULL, 0)) > 0)
    return ::MultiByteToWideChar(cp, 0, mcb, len, wbuf, wbuf_len);

  return 0;
}

inline wchar_t* mcbs2wdup(const char* mcb, int len, int* wbuf_len, UINT cp = NTCVT_CP_DEFAULT)
{
  if (len == -1)
    len = static_cast<int>(strlen(mcb));
  int cch;
  if (len > 0 && (cch = ::MultiByteToWideChar(cp, 0, mcb, len, NULL, 0)) > 0)
  {
    wchar_t* wbuf = (wchar_t*)calloc(cch + 1, 2);
    *wbuf_len     = ::MultiByteToWideChar(cp, 0, mcb, len, wbuf, cch);
    return wbuf;
  }

  return (wchar_t*)calloc(1, 2);
}

#if _HAS_CXX17
inline std::string from_chars(const std::wstring_view& wcb, UINT cp = NTCVT_CP_DEFAULT)
{
  return wcbs2a<std::string>(wcb.data(), static_cast<int>(wcb.length()), cp);
}

inline std::wstring from_chars(const std::string_view& mcb, UINT cp = NTCVT_CP_DEFAULT)
{
  return mcbs2w<std::wstring>(mcb.data(), static_cast<int>(mcb.length()), cp);
}
#else
inline std::string from_chars(const std::wstring& wcb, UINT cp = NTCVT_CP_DEFAULT)
{
  return wcbs2a<std::string>(wcb.c_str(), static_cast<int>(wcb.length()), cp);
}

inline std::wstring from_chars(const std::string& mcb, UINT cp = NTCVT_CP_DEFAULT)
{
  return mcbs2w<std::wstring>(mcb.c_str(), static_cast<int>(mcb.length()), cp);
}
#endif

inline std::string from_chars(const wchar_t* str, UINT cp = NTCVT_CP_DEFAULT)
{
  return wcbs2a<std::string>(str, -1, cp);
}

inline std::wstring from_chars(const char* str, UINT cp = NTCVT_CP_DEFAULT)
{
  return mcbs2w<std::wstring>(str, -1, cp);
}

// ntcs or std::string to CStringW
#if defined(_AFX)
inline std::string from_chars(const CStringW& wcb, UINT cp = NTCVT_CP_DEFAULT)
{
  return wcbs2a<std::string>(wcb.GetString(), wcb.GetLength(), cp);
}
namespace afx
{
#  if _HAS_CXX17
inline CStringW from_chars(std::string_view mcb, UINT cp = NTCVT_CP_DEFAULT)
{
  return mcbs2w<CStringW>(mcb.data(), static_cast<int>(mcb.length()), cp);
}
#  else
inline CStringW from_chars(const char* str, UINT cp = NTCVT_CP_DEFAULT)
{
  return mcbs2w<CStringW>(str, -1, cp);
}
inline CStringW from_chars(const std::string& mcb, UINT cp = NTCVT_CP_DEFAULT)
{
  return mcbs2w<CStringW>(mcb.c_str(), static_cast<int>(mcb.length()), cp);
}
#  endif
} // namespace afx
#endif
} // namespace ntcvt

#endif
