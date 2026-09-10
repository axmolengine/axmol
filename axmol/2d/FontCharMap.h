/****************************************************************************
 Copyright (c) 2013      Zynga Inc.
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

/// @cond DO_NOT_SHOW

#include "axmol/2d/Font.h"

namespace ax
{

class Texture2D;
class FontCharMap : public Font
{
public:
    static FontCharMap* create(std::string_view charMapFile, int itemWidth, int itemHeight, int startCharMap);
    static FontCharMap* create(Texture2D* texture, int itemWidth, int itemHeight, int startCharMap);
    static FontCharMap* create(std::string_view plistFile);

    int* getHorizontalKerningForTextUTF32(const std::u32string& text, int& outNumLetters) const override;
    FontAtlas* newFontAtlas() override;

protected:
    FontCharMap(Texture2D* texture, int itemWidth, int itemHeight, int startCharMap)
        : _texture(texture), _mapStartChar(startCharMap), _itemWidth(itemWidth), _itemHeight(itemHeight)
    {}
    /**
     * @lua NA
     */
    virtual ~FontCharMap();

private:
    Texture2D* _texture;
    int _mapStartChar;
    int _itemWidth;
    int _itemHeight;
};

/// @endcond

}  // namespace ax
