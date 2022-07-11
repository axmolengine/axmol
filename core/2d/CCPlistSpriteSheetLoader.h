#pragma once

#include <string>

#include "2d/CCSpriteSheetLoader.h"
#include "base/CCValue.h"
#include "base/CCData.h"

NS_AX_BEGIN

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

NS_AX_END
