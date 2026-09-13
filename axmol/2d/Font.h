/****************************************************************************
 Copyright (c) 2013      Zynga Inc.
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

/// @cond DO_NOT_SHOW

#include <string>
#include "axmol/base/Types.h"
#include "axmol/base/text_utils.h"

namespace ax
{

class FontAtlas;

class AX_DLL Font : public Object
{
public:
    virtual FontAtlas* newFontAtlas()                                                                   = 0;
    virtual int* getHorizontalKerningForTextUTF32(const std::u32string& text, int& outNumLetters) const = 0;
    virtual int getFontMaxHeight() const { return 0; }
};

}  // namespace ax

/// @endcond
