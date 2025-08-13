/****************************************************************************
// STIMER V2.0
Copyright (c) 2014-2020 x-studio - All Rights Reserved
Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

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

#pragma once

#include "base/Macros.h"
#include <functional>

namespace ax
{
    namespace stimer
    {
        typedef void* TIMER_ID;
        typedef std::function<void(void)> vcallback_t;
        AX_DLL TIMER_ID loop(unsigned int n, float interval, vcallback_t callback, bool bNative = true);
        AX_DLL TIMER_ID delay(float delay, vcallback_t callback, bool bNative = true);
        AX_DLL void kill(TIMER_ID timerId, bool bNative = true);
        AX_DLL void killAll(bool bNative = true);
    }  // namespace stimer
}

