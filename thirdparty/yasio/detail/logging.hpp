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
#ifndef YASIO__LOGGING_HPP
#define YASIO__LOGGING_HPP
#include "yasio/detail/strfmt.hpp"

#if defined(_WIN32)
#  define YASIO_LOG_TAG(tag, format, ...) OutputDebugStringA(::yasio::strfmt(127, (tag format "\n"), ##__VA_ARGS__).c_str())
#elif defined(ANDROID) || defined(__ANDROID__)
#  include <android/log.h>
#  include <jni.h>
#  define YASIO_LOG_TAG(tag, format, ...) __android_log_print(ANDROID_LOG_INFO, "yasio", (tag format), ##__VA_ARGS__)
#else
#  define YASIO_LOG_TAG(tag, format, ...) printf((tag format "\n"), ##__VA_ARGS__)
#endif

#define YASIO_LOG(format, ...) YASIO_LOG_TAG("[yasio]", format, ##__VA_ARGS__)

#if !defined(YASIO_VERBOSE_LOG)
#  define YASIO_LOGV(fmt, ...) (void)0
#else
#  define YASIO_LOGV YASIO_LOG
#endif

#endif
