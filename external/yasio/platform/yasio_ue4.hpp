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
#ifndef YASIO__UE4_HPP
#define YASIO__UE4_HPP

THIRD_PARTY_INCLUDES_START
#pragma push_macro("check")
#undef check
#define YASIO_HEADER_ONLY 1
/*
UE4 builtin namespace 'UI' conflicit with openssl typedef strcut st_UI UI;
ossl_typ.h(143): error C2365: 'UI': redefinition; previous definition was 'namespace'
*/
// #define YASIO_HAVE_SSL 1
#include "yasio/yasio.hpp"
#include "yasio/obstream.hpp"
#include "yasio/ibstream.hpp"
using namespace yasio;
using namespace yasio::inet;
#pragma pop_macro("check")
THIRD_PARTY_INCLUDES_END

#endif
