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

#include <string>

#include "2d/SpriteSheetLoader.h"
#include "base/Value.h"
#include "base/Data.h"

namespace ax
{

class PlistSpriteSheetLoader : public SpriteSheetLoader
{
public:
    static constexpr uint32_t FORMAT = SpriteSheetFormat::PLIST;

    uint32_t getFormat() override { return FORMAT; }
    void load(std::string_view filePath, SpriteFrameCache& cache) override;
    void load(std::string_view filePath, Texture2D* texture, SpriteFrameCache& cache) override;
    void load(std::string_view filePath, std::string_view textureFileName, SpriteFrameCache& cache) override;
    void load(const Data& content, Texture2D* texture, SpriteFrameCache& cache) override;
    void reload(std::string_view filePath, SpriteFrameCache& cache) override;

protected:
    /*Adds multiple Sprite Frames with a dictionary. The texture will be associated with the created sprite frames.
     */
    void addSpriteFramesWithDictionary(ValueMap& dictionary,
                                       Texture2D* texture,
                                       std::string_view plist,
                                       SpriteFrameCache& cache);

    /*Adds multiple Sprite Frames with a dictionary. The texture will be associated with the created sprite frames.
     */
    void addSpriteFramesWithDictionary(ValueMap& dict,
                                       std::string_view texturePath,
                                       std::string_view plist,
                                       SpriteFrameCache& cache);

    void reloadSpriteFramesWithDictionary(ValueMap& dict,
                                          Texture2D* texture,
                                          std::string_view plist,
                                          SpriteFrameCache& cache);
};

}
