#include "2d/PlistSpriteSheetLoader.h"

#include "platform/FileUtils.h"
#include "2d/AutoPolygon.h"
#include "2d/SpriteFrameCache.h"
#include "base/NinePatchImageParser.h"
#include "base/NS.h"
#include "base/Macros.h"
#include "base/UTF8.h"
#include "base/Utils.h"
#include "base/Director.h"
#include "renderer/Texture2D.h"
#include "renderer/TextureCache.h"

#include <vector>

using namespace std;

NS_AX_BEGIN

void PlistSpriteSheetLoader::load(std::string_view filePath, SpriteFrameCache& cache)
{
    AXASSERT(!filePath.empty(), "plist filename should not be nullptr");

    const auto fullPath = FileUtils::getInstance()->fullPathForFilename(filePath);
    if (fullPath.empty())
    {
        // return if plist file doesn't exist
        AXLOGW("SpriteFrameCache: can not find {}", filePath);
        return;
    }

    auto dict = FileUtils::getInstance()->getValueMapFromFile(fullPath);

    std::string texturePath;

    if (dict.find("metadata") != dict.end())
    {
        auto& metadataDict = dict["metadata"].asValueMap();
        // try to read  texture file name from meta data
        texturePath = metadataDict["textureFileName"].asString();
    }

    if (!texturePath.empty())
    {
        // build texture path relative to plist file
        texturePath = FileUtils::getInstance()->fullPathFromRelativeFile(texturePath, filePath);
    }
    else
    {
        // build texture path by replacing file extension
        texturePath = filePath;

        // remove .xxx
        const auto startPos = texturePath.find_last_of('.');
        if (startPos != string::npos)
        {
            texturePath = texturePath.erase(startPos);
        }

        // append .png
        texturePath = texturePath.append(".png");

        AXLOGD("SpriteFrameCache: Trying to use file {} as texture", texturePath);
    }
    addSpriteFramesWithDictionary(dict, texturePath, filePath, cache);
}

void PlistSpriteSheetLoader::load(std::string_view filePath, Texture2D* texture, SpriteFrameCache& cache)
{
    const auto fullPath = FileUtils::getInstance()->fullPathForFilename(filePath);
    auto dict           = FileUtils::getInstance()->getValueMapFromFile(fullPath);

    addSpriteFramesWithDictionary(dict, texture, filePath, cache);
}

void PlistSpriteSheetLoader::load(std::string_view filePath, std::string_view textureFileName, SpriteFrameCache& cache)
{
    AXASSERT(!textureFileName.empty(), "texture name should not be null");
    const auto fullPath = FileUtils::getInstance()->fullPathForFilename(filePath);
    auto dict           = FileUtils::getInstance()->getValueMapFromFile(fullPath);
    addSpriteFramesWithDictionary(dict, textureFileName, filePath, cache);
}

void PlistSpriteSheetLoader::load(const Data& content, Texture2D* texture, SpriteFrameCache& cache)
{
    if (content.isNull())
    {
        return;
    }

    auto* text        = (char*)content.getBytes();
    const auto length = content.getSize();

    auto dict = FileUtils::getInstance()->getValueMapFromData(text, static_cast<int>(length));
    addSpriteFramesWithDictionary(dict, texture, "by#addSpriteFramesWithFileContent()", cache);
}

void PlistSpriteSheetLoader::reload(std::string_view filePath, SpriteFrameCache& cache)
{
    const auto fullPath = FileUtils::getInstance()->fullPathForFilename(filePath);
    auto dict           = FileUtils::getInstance()->getValueMapFromFile(fullPath);

    std::string texturePath;

    if (dict.find("metadata") != dict.end())
    {
        auto& metadataDict = dict["metadata"].asValueMap();
        // try to read  texture file name from meta data
        texturePath = metadataDict["textureFileName"].asString();
    }

    if (!texturePath.empty())
    {
        // build texture path relative to plist file
        texturePath = FileUtils::getInstance()->fullPathFromRelativeFile(texturePath, filePath);
    }
    else
    {
        // build texture path by replacing file extension
        texturePath = filePath;

        // remove .xxx
        const auto startPos = texturePath.find_last_of('.');
        if (startPos != string::npos)
        {
            texturePath = texturePath.erase(startPos);
        }

        // append .png
        texturePath = texturePath.append(".png");
    }

    Texture2D* texture = nullptr;
    if (Director::getInstance()->getTextureCache()->reloadTexture(texturePath))
    {
        texture = Director::getInstance()->getTextureCache()->getTextureForKey(texturePath);
    }

    if (texture)
    {
        reloadSpriteFramesWithDictionary(dict, texture, filePath, cache);
    }
    else
    {
        AXLOGD("SpriteFrameCache: Couldn't load texture");
    }
}

void PlistSpriteSheetLoader::addSpriteFramesWithDictionary(ValueMap& dictionary,
                                                           Texture2D* texture,
                                                           std::string_view plist,
                                                           SpriteFrameCache& cache)
{
    /*
    Supported Zwoptex Formats:

    ZWTCoordinatesFormatOptionXMLLegacy = 0, // Flash Version
    ZWTCoordinatesFormatOptionXML1_0 = 1, // Desktop Version 0.0 - 0.4b
    ZWTCoordinatesFormatOptionXML1_1 = 2, // Desktop Version 1.0.0 - 1.0.1
    ZWTCoordinatesFormatOptionXML1_2 = 3, // Desktop Version 1.0.2+

    Version 3 with TexturePacker 4.0 polygon mesh packing
    */

    if (dictionary["frames"].getType() != ax::Value::Type::MAP)
        return;

    auto spriteSheet    = std::make_shared<SpriteSheet>();
    spriteSheet->format = getFormat();
    spriteSheet->path   = plist;

    auto& framesDict = dictionary["frames"].asValueMap();
    int format       = 0;

    Vec2 textureSize;

    // get the format
    auto metaItr = dictionary.find("metadata"sv);
    if (metaItr != dictionary.end())
    {
        auto& metadataDict = metaItr->second.asValueMap();
        format             = optValue(metadataDict, "format"sv).asInt();

        if (metadataDict.find("size"sv) != metadataDict.end())
        {
            textureSize = SizeFromString(optValue(metadataDict, "size"sv).asString());
        }
    }

    // check the format
    AXASSERT(format >= 0 && format <= 3,
             "format is not supported for SpriteFrameCache addSpriteFramesWithDictionary:textureFilename:");

    std::vector<std::string> frameAliases;
    auto textureFileName = Director::getInstance()->getTextureCache()->getTextureFilePath(texture);
    Image* image         = nullptr;
    NinePatchImageParser parser;
    for (auto&& iter : framesDict)
    {
        auto& frameDict      = iter.second.asValueMap();
        auto spriteFrameName = iter.first;
        auto* spriteFrame    = cache.findFrame(spriteFrameName);
        if (spriteFrame)
        {
            continue;
        }

        if (format == 0)
        {
            auto x  = optValue(frameDict, "x"sv).asFloat();
            auto y  = optValue(frameDict, "y"sv).asFloat();
            auto w  = optValue(frameDict, "width"sv).asFloat();
            auto h  = optValue(frameDict, "height"sv).asFloat();
            auto ox = optValue(frameDict, "offsetX"sv).asFloat();
            auto oy = optValue(frameDict, "offsetY"sv).asFloat();
            auto ow = optValue(frameDict, "originalWidth"sv).asInt();
            auto oh = optValue(frameDict, "originalHeight"sv).asInt();
            // check ow/oh
            if (!ow || !oh)
            {
                AXLOGW(
                    "WARNING: originalWidth/Height not found on the SpriteFrame. AnchorPoint won't work as "
                    "expected. Regenerate the .plist");
            }
            // abs ow/oh
            ow = std::abs(ow);
            oh = std::abs(oh);
            // create frame
            spriteFrame = SpriteFrame::createWithTexture(texture, Rect(x, y, w, h), false, Vec2(ox, oy),
                                                         Vec2((float)ow, (float)oh));
        }
        else if (format == 1 || format == 2)
        {
            auto frame   = RectFromString(optValue(frameDict, "frame"sv).asString());
            auto rotated = false;

            // rotation
            if (format == 2)
            {
                rotated = optValue(frameDict, "rotated"sv).asBool();
            }

            auto offset     = PointFromString(optValue(frameDict, "offset"sv).asString());
            auto sourceSize = SizeFromString(optValue(frameDict, "sourceSize"sv).asString());

            // create frame
            spriteFrame = SpriteFrame::createWithTexture(texture, frame, rotated, offset, sourceSize);
        }
        else if (format == 3)
        {
            // get values
            auto spriteSize       = SizeFromString(optValue(frameDict, "spriteSize"sv).asString());
            auto spriteOffset     = PointFromString(optValue(frameDict, "spriteOffset"sv).asString());
            auto spriteSourceSize = SizeFromString(optValue(frameDict, "spriteSourceSize"sv).asString());
            auto textureRect      = RectFromString(optValue(frameDict, "textureRect"sv).asString());
            auto textureRotated   = optValue(frameDict, "textureRotated"sv).asBool();

            // get aliases
            auto& aliases = optValue(frameDict, "aliases"sv).asValueVector();

            for (const auto& value : aliases)
            {
                auto oneAlias = value.asString();
                if (std::find(frameAliases.begin(), frameAliases.end(), oneAlias) == frameAliases.end())
                {
                    frameAliases.emplace_back(std::move(oneAlias));
                }
                else
                {
                    AXLOGW("WARNING: an alias with name {} already exists", oneAlias);
                }
            }

            // create frame
            spriteFrame = SpriteFrame::createWithTexture(
                texture, Rect(textureRect.origin.x, textureRect.origin.y, spriteSize.width, spriteSize.height),
                textureRotated, spriteOffset, spriteSourceSize);

            if (frameDict.find("vertices") != frameDict.end())
            {
                using ax::utils::parseIntegerList;
                auto vertices   = parseIntegerList(optValue(frameDict, "vertices"sv).asString());
                auto verticesUV = parseIntegerList(optValue(frameDict, "verticesUV"sv).asString());
                auto indices    = parseIntegerList(optValue(frameDict, "triangles"sv).asString());

                PolygonInfo info;
                initializePolygonInfo(textureSize, spriteSourceSize, vertices, verticesUV, indices, info);
                spriteFrame->setPolygonInfo(info);
            }
            if (frameDict.find("anchor") != frameDict.end())
            {
                spriteFrame->setAnchorPoint(PointFromString(optValue(frameDict, "anchor"sv).asString()));
            }
        }

        auto flag = NinePatchImageParser::isNinePatchImage(spriteFrameName);
        if (flag)
        {
            if (image == nullptr)
            {
                image = new Image();
                image->initWithImageFile(textureFileName);
            }
            parser.setSpriteFrameInfo(image, spriteFrame->getRectInPixels(), spriteFrame->isRotated());
            // texture->addSpriteFrameCapInset(spriteFrame, parser.parseCapInset());
            cache.addSpriteFrameCapInset(spriteFrame, parser.parseCapInset(), texture);
        }

        // add sprite frame
        cache.insertFrame(spriteSheet, spriteFrameName, spriteFrame);
        for (auto&& frameAlias : frameAliases)
        {
            cache.insertFrame(spriteSheet, frameAlias, spriteFrame);
        }
    }

    spriteSheet->full = true;

    AX_SAFE_DELETE(image);
}

void PlistSpriteSheetLoader::addSpriteFramesWithDictionary(ValueMap& dict,
                                                           std::string_view texturePath,
                                                           std::string_view plist,
                                                           SpriteFrameCache& cache)
{
    std::string pixelFormatName;
    if (dict.find("metadata") != dict.end())
    {
        auto& metadataDict = dict.at("metadata").asValueMap();
        if (metadataDict.find("pixelFormat") != metadataDict.end())
        {
            pixelFormatName = metadataDict.at("pixelFormat").asString();
        }
    }

    Texture2D* texture                                                        = nullptr;
    static std::unordered_map<std::string, backend::PixelFormat> pixelFormats = {
        {"RGBA8888", backend::PixelFormat::RGBA8},
        {"RGBA4444", backend::PixelFormat::RGBA4},
        {"RGB5A1", backend::PixelFormat::RGB5A1},
        {"RGBA5551", backend::PixelFormat::RGB5A1},
        {"RGB565", backend::PixelFormat::RGB565},
        {"R8", backend::PixelFormat::R8},
        {"RG8", backend::PixelFormat::RG8},
        //{"BGRA8888", backend::PixelFormat::BGRA8888}, no Image conversion RGBA -> BGRA
        {"RGB888", backend::PixelFormat::RGB8}};

    const auto pixelFormatIt = pixelFormats.find(pixelFormatName);
    if (pixelFormatIt != pixelFormats.end())
    {
        const backend::PixelFormat pixelFormat        = (*pixelFormatIt).second;
        texture = Director::getInstance()->getTextureCache()->addImage(texturePath, pixelFormat);
    }
    else
    {
        texture = Director::getInstance()->getTextureCache()->addImage(texturePath);
    }

    if (texture)
    {
        addSpriteFramesWithDictionary(dict, texture, plist, cache);
    }
    else
    {
        AXLOGD("SpriteFrameCache: Couldn't load texture");
    }
}

void PlistSpriteSheetLoader::reloadSpriteFramesWithDictionary(ValueMap& dict,
                                                              Texture2D* texture,
                                                              std::string_view plist,
                                                              SpriteFrameCache& cache)
{
    auto& framesDict = dict["frames"].asValueMap();
    int format       = 0;

    // get the format
    if (dict.find("metadata") != dict.end())
    {
        auto& metadataDict = dict["metadata"].asValueMap();
        format             = metadataDict["format"].asInt();
    }

    // check the format
    AXASSERT(format >= 0 && format <= 3,
             "format is not supported for SpriteFrameCache addSpriteFramesWithDictionary:textureFilename:");

    auto spriteSheet    = std::make_shared<SpriteSheet>();
    spriteSheet->format = getFormat();
    spriteSheet->path   = plist;

    for (auto&& iter : framesDict)
    {
        const ValueMap& frameDict        = iter.second.asValueMap();
        std::string_view spriteFrameName = iter.first;

        cache.eraseFrame(spriteFrameName);

        SpriteFrame* spriteFrame = nullptr;
        std::vector<std::string> frameAliases;

        if (format == 0)
        {
            const auto x  = optValue(frameDict, "x"sv).asFloat();
            const auto y  = optValue(frameDict, "y"sv).asFloat();
            const auto w  = optValue(frameDict, "width"sv).asFloat();
            const auto h  = optValue(frameDict, "height"sv).asFloat();
            const auto ox = optValue(frameDict, "offsetX"sv).asFloat();
            const auto oy = optValue(frameDict, "offsetY"sv).asFloat();
            auto ow       = optValue(frameDict, "originalWidth"sv).asInt();
            auto oh       = optValue(frameDict, "originalHeight"sv).asInt();
            // check ow/oh
            if (!ow || !oh)
            {
                AXLOGW(
                    "WARNING: originalWidth/Height not found on the SpriteFrame. AnchorPoint won't work as "
                    "expected. Regenerate the .plist");
            }
            // abs ow/oh
            ow = std::abs(ow);
            oh = std::abs(oh);
            // create frame
            spriteFrame = SpriteFrame::createWithTexture(texture, Rect(x, y, w, h), false, Vec2(ox, oy),
                                                         Vec2((float)ow, (float)oh));
        }
        else if (format == 1 || format == 2)
        {
            auto frame   = RectFromString(optValue(frameDict, "frame"sv).asString());
            auto rotated = false;

            // rotation
            if (format == 2)
            {
                rotated = optValue(frameDict, "rotated"sv).asBool();
            }

            auto offset     = PointFromString(optValue(frameDict, "offset"sv).asString());
            auto sourceSize = SizeFromString(optValue(frameDict, "sourceSize"sv).asString());

            // create frame
            spriteFrame = SpriteFrame::createWithTexture(texture, frame, rotated, offset, sourceSize);
        }
        else if (format == 3)
        {
            // get values
            const auto spriteSize     = SizeFromString(optValue(frameDict, "spriteSize"sv).asString());
            auto spriteOffset         = PointFromString(optValue(frameDict, "spriteOffset"sv).asString());
            auto spriteSourceSize     = SizeFromString(optValue(frameDict, "spriteSourceSize"sv).asString());
            const auto textureRect    = RectFromString(optValue(frameDict, "textureRect"sv).asString());
            const auto textureRotated = optValue(frameDict, "textureRotated"sv).asBool();

            // get aliases
            const ValueVector& aliases = optValue(frameDict, "aliases"sv).asValueVector();

            for (const auto& value : aliases)
            {
                auto oneAlias = value.asString();
                if (std::find(frameAliases.begin(), frameAliases.end(), oneAlias) == frameAliases.end())
                {
                    frameAliases.emplace_back(std::move(oneAlias));
                }
                else
                {
                    AXLOGW("WARNING: an alias with name {} already exists", oneAlias);
                }
            }

            // create frame
            spriteFrame = SpriteFrame::createWithTexture(
                texture, Rect(textureRect.origin.x, textureRect.origin.y, spriteSize.width, spriteSize.height),
                textureRotated, spriteOffset, spriteSourceSize);
        }

        // add sprite frame
        cache.insertFrame(spriteSheet, spriteFrameName, spriteFrame);
        for (auto&& frameAlias : frameAliases)
        {
            cache.insertFrame(spriteSheet, frameAlias, spriteFrame);
        }
    }
}

NS_AX_END
