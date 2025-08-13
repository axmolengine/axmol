/****************************************************************************
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


#include "2d/Sprite.h"
#include "extensions/ExtensionMacros.h"

NS_AX_EXT_BEGIN

struct FontAtlasGenParams;

namespace xasset
{
class FontAtlas;
}
class SDFGen
{
public:
    static SDFGen* getInstance();
    static void destroyInstance();

    void open(ax::Scene* = nullptr);
    void close();

protected:
    void refreshFontList();

    void onImGuiDraw();

protected:
    FontAtlasGenParams* _atlasParams{nullptr};

    ax::Sprite* _atlasViewer{nullptr};
    xasset::FontAtlas* _fontAtlas{nullptr};

    std::vector<std::string> _fontList;
};

NS_AX_EXT_END
