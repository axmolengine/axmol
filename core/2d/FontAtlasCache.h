/****************************************************************************
 Copyright (c) 2013      Zynga Inc.
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

#ifndef _CCFontAtlasCache_h_
#define _CCFontAtlasCache_h_

/// @cond DO_NOT_SHOW

#include <unordered_map>
#include "base/Types.h"

namespace ax
{

class FontAtlas;
class Texture2D;
struct _ttfConfig;

class AX_DLL FontAtlasCache
{
public:
    /**
     * @brief preload a SDF fontatlas
     * since axmol-2.1.0, must call before creating any Label
     */
    static void preloadFontAtlas(std::string_view fontatlasFile);
    static FontAtlas* getFontAtlasTTF(_ttfConfig* config);

    static FontAtlas* getFontAtlasFNT(std::string_view fontFileName);
    static FontAtlas* getFontAtlasFNT(std::string_view fontFileName, std::string_view subTextureKey);
    static FontAtlas* getFontAtlasFNT(std::string_view fontFileName, const Rect& imageRect, bool imageRotated);
#ifndef AX_CORE_PROFILE
    AX_DEPRECATED(2.1) static FontAtlas* getFontAtlasFNT(std::string_view fontFileName, const Vec2& imageOffset);
#endif
    static FontAtlas* getFontAtlasCharMap(std::string_view charMapFile,
                                          int itemWidth,
                                          int itemHeight,
                                          int startCharMap);
    static FontAtlas* getFontAtlasCharMap(Texture2D* texture, int itemWidth, int itemHeight, int startCharMap);
    static FontAtlas* getFontAtlasCharMap(std::string_view plistFile);

    static bool releaseFontAtlas(FontAtlas* atlas);

    /** Removes cached data.
     It will purge the textures atlas and if multiple texture exist in one FontAtlas.
     */
    static void purgeCachedData();

    /** Release current FNT texture and reload it.
     CAUTION : All component use this font texture should be reset font name, though the file name is same!
               otherwise, it will cause program crash!
    */
    static void reloadFontAtlasFNT(std::string_view fontFileName, const Rect& imageRect, bool imageRotated);
#ifndef AX_CORE_PROFILE
    AX_DEPRECATED(2.1) static void reloadFontAtlasFNT(std::string_view fontFileName,
                                                           const Vec2& imageOffset = Vec2::ZERO);
#endif
    /** Unload all texture atlas texture create by special file name.
     CAUTION : All component use this font texture should be reset font name, though the file name is same!
               otherwise, it will cause program crash!
    */
    static void unloadFontAtlasTTF(std::string_view fontFileName);

private:
    static hlookup::string_map<FontAtlas*> _atlasMap;
};

}

/// @endcond
#endif
