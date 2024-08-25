/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
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

#ifndef __AX_COMMON_H__
#define __AX_COMMON_H__
/// @cond DO_NOT_SHOW

#include "platform/PlatformMacros.h"

namespace ax
{

/**
 * @addtogroup platform
 * @{
 */

/**
@brief Pop out a message box
*/
void AX_DLL messageBox(const char* msg, const char* title);

/**
@brief Enum the language type supported now
*/
enum class LanguageType
{
    ENGLISH = 0,
    CHINESE,
    FRENCH,
    ITALIAN,
    GERMAN,
    SPANISH,
    DUTCH,
    RUSSIAN,
    KOREAN,
    JAPANESE,
    HUNGARIAN,
    PORTUGUESE,
    ARABIC,
    NORWEGIAN,
    POLISH,
    TURKISH,
    UKRAINIAN,
    ROMANIAN,
    BULGARIAN,
    BELARUSIAN
};

// END of platform group
/// @}

}

/// @endcond
#endif  // __AX_COMMON_H__
