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
#ifndef YASIO__FEATURE_TEST_HPP
#define YASIO__FEATURE_TEST_HPP

// Tests whether compiler has fully c++11 support
// About preprocessor '_MSC_VER', please see:
// https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=vs-2019
#if defined(_MSC_VER)
#  if _MSC_VER < 1900
#    define YASIO__HAS_FULL_CXX11 0
#  else
#    define YASIO__HAS_FULL_CXX11 1
#    if _MSC_VER > 1900 // VS2017 or later
#      include <vcruntime.h>
#      include <sdkddkver.h>
#    endif
#  endif
#else
#  define YASIO__HAS_FULL_CXX11 1
#endif

// Tests whether compiler has c++14 support
#if (defined(__cplusplus) && __cplusplus >= 201402L) || (defined(_MSC_VER) && _MSC_VER >= 1900 && (defined(_MSVC_LANG) && (_MSVC_LANG >= 201402L)))
#  ifndef YASIO_HAS_CXX14
#    define YASIO__HAS_CXX14 1
#  endif // C++14 features macro
#endif   // C++14 features check
#if !defined(YASIO__HAS_CXX14)
#  define YASIO__HAS_CXX14 0
#endif

// Tests whether compiler has c++17 support
#if (defined(__cplusplus) && __cplusplus >= 201703L) ||                                                                                                        \
    (defined(_MSC_VER) && _MSC_VER > 1900 && ((defined(_HAS_CXX17) && _HAS_CXX17 == 1) || (defined(_MSVC_LANG) && (_MSVC_LANG > 201402L))))
#  ifndef YASIO_HAS_CXX17
#    define YASIO__HAS_CXX17 1
#  endif // C++17 features macro
#endif   // C++17 features check
#if !defined(YASIO__HAS_CXX17)
#  define YASIO__HAS_CXX17 0
#endif

// Tests whether compiler has c++20 support
#if (defined(__cplusplus) && __cplusplus > 201703L) ||                                                                                                         \
    (defined(_MSC_VER) && _MSC_VER > 1900 && ((defined(_HAS_CXX20) && _HAS_CXX20 == 1) || (defined(_MSVC_LANG) && (_MSVC_LANG > 201703L))))
#  ifndef YASIO__HAS_CXX20
#    define YASIO__HAS_CXX20 1
#  endif // C++20 features macro
#endif   // C++20 features check
#if !defined(YASIO__HAS_CXX20)
#  define YASIO__HAS_CXX20 0
#endif

// Workaround for compiler without fully c++11 support, such as vs2013
#if YASIO__HAS_FULL_CXX11
#  define YASIO__HAS_NS_INLINE 1
#  define YASIO__NS_INLINE inline
#else
#  define YASIO__HAS_NS_INLINE 0
#  define YASIO__NS_INLINE
#  if defined(constexpr)
#    undef constexpr
#  endif
#  define constexpr const
#endif

// Unix domain socket feature test
#if !defined(_WIN32) || defined(NTDDI_WIN10_RS5)
#  define YASIO__HAS_UDS 1
#else
#  define YASIO__HAS_UDS 0
#endif

// Test whether sockaddr has member 'sa_len'
#if defined(__linux__) || defined(_WIN32)
#  define YASIO__HAS_SA_LEN 0
#else
#  if defined(__unix__) || defined(__APPLE__)
#    define YASIO__HAS_SA_LEN 1
#  else
#    define YASIO__HAS_SA_LEN 0
#  endif
#endif

#if !defined(_WIN32) || defined(NTDDI_VISTA)
#  define YASIO__HAS_NTOP 1
#else
#  define YASIO__HAS_NTOP 0
#endif

// 64bits Sense Macros
#if defined(_M_X64) || defined(_WIN64) || defined(__LP64__) || defined(_LP64) || defined(__x86_64) || defined(__arm64__) || defined(__aarch64__)
#  define YASIO__64BITS 1
#else
#  define YASIO__64BITS 0
#endif

// Try detect compiler exceptions
#if !defined(__cpp_exceptions)
#  define YASIO__NO_EXCEPTIONS 1
#endif

#if !defined(YASIO__NO_EXCEPTIONS)
#  define YASIO__THROW(x, retval) throw(x)
#  define YASIO__THROW0(x) throw(x)
#  define YASIO__THROWV(x, val) (throw(x), (val))
#else
#  define YASIO__THROW(x, retval) return (retval)
#  define YASIO__THROW0(x) return
#  define YASIO__THROWV(x, val) (val)
#endif

// Compatibility with non-clang compilers...
#ifndef __has_attribute
#  define __has_attribute(x) 0
#endif
#ifndef __has_builtin
#  define __has_builtin(x) 0
#endif

/*
 * Helps the compiler's optimizer predicting branches
 */
#if __has_builtin(__builtin_expect)
#  ifdef __cplusplus
#    define yasio__likely(exp) (__builtin_expect(!!(exp), true))
#    define yasio__unlikely(exp) (__builtin_expect(!!(exp), false))
#  else
#    define yasio__likely(exp) (__builtin_expect(!!(exp), 1))
#    define yasio__unlikely(exp) (__builtin_expect(!!(exp), 0))
#  endif
#else
#  define yasio__likely(exp) (!!(exp))
#  define yasio__unlikely(exp) (!!(exp))
#endif

#define YASIO__STD ::std::

#if YASIO__HAS_CXX14
namespace cxx14
{
using namespace std;
};
#endif

#if YASIO__HAS_CXX17
namespace cxx17
{
using namespace std;
};
#endif

#endif
