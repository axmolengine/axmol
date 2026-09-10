/****************************************************************************
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

#include <string>

#include "axmol/2d/SpriteSheetLoader.h"
#include "axmol/base/Value.h"
#include "axmol/base/Data.h"

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
    SpriteFrame* parseFrame(const ValueMap& frameDict,
                            std::vector<std::string>& outFrameAlias,
                            int format,
                            Texture2D* texture,
                            const ax::Vec2& textureSize);

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

}  // namespace ax
