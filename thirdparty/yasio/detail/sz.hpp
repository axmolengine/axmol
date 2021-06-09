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
#ifndef YASIO__SZ_HPP
#define YASIO__SZ_HPP

#include <stdint.h>

static const uint64_t YASIO__B  = (1);
static const uint64_t YASIO__KB = (1024 * YASIO__B);
static const uint64_t YASIO__MB = (1024 * YASIO__KB);
static const uint64_t YASIO__GB = (1024 * YASIO__MB);
static const uint64_t YASIO__TB = (1024 * YASIO__GB);
static const uint64_t YASIO__PB = (1024 * YASIO__TB);
static const uint64_t YASIO__EB = (1024 * YASIO__PB); // lgtm [cpp/unused-static-variable]

#define YASIO__K YASIO__KB
#define YASIO__M YASIO__MB
#define YASIO__G YASIO__GB
#define YASIO__T YASIO__TB
#define YASIO__P YASIO__PB
#define YASIO__E YASIO__EB
#define YASIO__b YASIO__B
#define YASIO__k YASIO__K
#define YASIO__m YASIO__M
#define YASIO__g YASIO__G
#define YASIO__t YASIO__T
#define YASIO__p YASIO__P
#define YASIO__e YASIO__E

#define YASIO_SZ(n, u) ((n)*YASIO__##u)

#define YASIO_SZ_ALIGN(d, a) (((d) + ((a)-1)) & ~((a)-1))

#endif
