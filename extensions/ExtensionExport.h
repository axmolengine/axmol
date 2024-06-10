/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 https://axmol.dev/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef __EXTENSIONEXPORT_H__
#define __EXTENSIONEXPORT_H__

#include "platform/PlatformDefine.h"

#if defined(_WIN32)
#  if defined(AX_EX_DLLEXPORT)
#    define AX_EX_API __declspec(dllexport)
#  elif defined(AX_EX_DLLIMPORT)
#    define AX_EX_API __declspec(dllimport)
#  endif
#elif defined(AX_EX_DLLEXPORT) || defined(AX_EX_DLLIMPORT)
#  define AX_EX_API __attribute__((visibility("default")))
#endif
#ifndef AX_EX_API
#  define AX_EX_API
#endif

#define AX_EX_DLL AX_EX_API

#endif /* __EXTENSIONEXPORT_H__*/