#include "2d/CCPlistSpriteSheetLoader.h"

#include "platform/CCFileUtils.h"
#include "2d/CCAutoPolygon.h"
#include "2d/CCSpriteFrameCache.h"
#include "base/CCNinePatchImageParser.h"
#include "base/CCNS.h"
#include "base/ccMacros.h"
#include "base/ccUTF8.h"
#include "base/ccUtils.h"
#include "base/CCDirector.h"
#include "renderer/CCTexture2D.h"
#include "renderer/CCTextureCache.h"

#include <vector>

using namespace std;

NS_CC_BEGIN

void PlistSpriteSheetLoader::Load(const std::string& filePath, SpriteFrameCache& cache)
{
    CCASSERT(!filePath.empty(), "plist filename should not be nullptr");

    const auto fullPath = FileUtils::getInstance()->fullPathForFilename(filePath);
    if (fullPath.empty())
    {
        // return if plist file doesn't exist
        CCLOG("cocos2d: SpriteFrameCache: can not find %s", filePath.c_str());
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

        CCLOG("cocos2d: SpriteFrameCache: Trying to use file %s as texture", texturePath.c_str());
    }
    addSpriteFramesWithDictionary(dict, texturePath, filePath, cache);
}

void PlistSpriteSheetLoader::Load(const std::string& filePath, Texture2D* texture, SpriteFrameCache& cache)
{
    const auto fullPath = FileUtils::getInstance()->fullPathForFilename(filePath);
    auto dict = FileUtils::getInstance()->getValueMapFromFile(fullPath);

    addSpriteFramesWithDictionary(dict, texture, filePath, cache);
}

void PlistSpriteSheetLoader::Load(const std::string& filePath, const std::string& textureFileName, SpriteFrameCache& cache)
{
    CCASSERT(!textureFileName.empty(), "texture name should not be null");
    const auto fullPath = FileUtils::getInstance()->fullPathForFilename(filePath);
    auto dict = FileUtils::getInstance()->getValueMapFromFile(fullPath);
    addSpriteFramesWithDictionary(dict, textureFileName, filePath, cache);
}

void PlistSpriteSheetLoader::Load(const Data& content, Texture2D* texture, SpriteFrameCache& cache)
{
    if (content.isNull())
    {
        return;
    }

    auto* text = (char*)content.getBytes();
    const auto length = content.getSize();

    auto dict = FileUtils::getInstance()->getValueMapFromData(text, static_cast<int>(length));
    addSpriteFramesWithDictionary(dict, texture, "by#addSpriteFramesWithFileContent()", cache);
}

void PlistSpriteSheetLoader::Reload(const std::string& filePath, SpriteFrameCache& cache)
{
    const auto fullPath = FileUtils::getInstance()->fullPathForFilename(filePath);
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
        CCLOG("cocos2d: SpriteFrameCache: Couldn't load texture");
    }
}

void PlistSpriteSheetLoader::addSpriteFramesWithDictionary(ValueMap& dictionary, Texture2D* texture, const std::string& plist, SpriteFrameCache& cache)
{
    /*
    Supported Zwoptex Formats:

    ZWTCoordinatesFormatOptionXMLLegacy = 0, // Flash Version
    ZWTCoordinatesFormatOptionXML1_0 = 1, // Desktop Version 0.0 - 0.4b
    ZWTCoordinatesFormatOptionXML1_1 = 2, // Desktop Version 1.0.0 - 1.0.1
    ZWTCoordinatesFormatOptionXML1_2 = 3, // Desktop Version 1.0.2+

    Version 3 with TexturePacker 4.0 polygon mesh packing
    */

    if (dictionary["frames"].getType() != cocos2d::Value::Type::MAP)
        return;

    auto spriteSheet = std::make_shared<SpriteSheet>();
    spriteSheet->format = "PLIST";
    spriteSheet->path = plist;

    auto& framesDict = dictionary["frames"].asValueMap();
    int format = 0;

    Size textureSize;

    // get the format
    auto metaItr = dictionary.find("metadata");
    if (metaItr != dictionary.end())
    {
        auto& metadataDict = metaItr->second.asValueMap();
        format = metadataDict["format"].asInt();

        if (metadataDict.find("size") != metadataDict.end())
        {
            textureSize = SizeFromString(metadataDict["size"].asString());
        }
    }

    // check the format
    CCASSERT(format >= 0 && format <= 3, "format is not supported for SpriteFrameCache addSpriteFramesWithDictionary:textureFilename:");

    std::vector<std::string> frameAliases;
    auto textureFileName = Director::getInstance()->getTextureCache()->getTextureFilePath(texture);
    Image* image = nullptr;
    NinePatchImageParser parser;
    for (auto& iter : framesDict)
    {
        auto& frameDict = iter.second.asValueMap();
        auto spriteFrameName = iter.first;
        auto* spriteFrame = cache.findFrame(spriteFrameName);
        if (spriteFrame)
        {
            continue;
        }

        if (format == 0)
        {
            auto x = frameDict["x"].asFloat();
            auto y = frameDict["y"].asFloat();
            auto w = frameDict["width"].asFloat();
            auto h = frameDict["height"].asFloat();
            auto ox = frameDict["offsetX"].asFloat();
            auto oy = frameDict["offsetY"].asFloat();
            auto ow = frameDict["originalWidth"].asInt();
            auto oh = frameDict["originalHeight"].asInt();
            // check ow/oh
            if (!ow || !oh)
            {
                CCLOGWARN("cocos2d: WARNING: originalWidth/Height not found on the SpriteFrame. AnchorPoint won't work as expected. Regenerate the .plist");
            }
            // abs ow/oh
            ow = std::abs(ow);
            oh = std::abs(oh);
            // create frame
            spriteFrame = SpriteFrame::createWithTexture(texture,
                Rect(x, y, w, h),
                false,
                Vec2(ox, oy),
                Size((float)ow, (float)oh)
            );
        }
        else if (format == 1 || format == 2)
        {
            auto frame = RectFromString(frameDict["frame"].asString());
            auto rotated = false;

            // rotation
            if (format == 2)
            {
                rotated = frameDict["rotated"].asBool();
            }

            auto offset = PointFromString(frameDict["offset"].asString());
            auto sourceSize = SizeFromString(frameDict["sourceSize"].asString());

            // create frame
            spriteFrame = SpriteFrame::createWithTexture(texture,
                frame,
                rotated,
                offset,
                sourceSize
            );
        }
        else if (format == 3)
        {
            // get values
            auto spriteSize = SizeFromString(frameDict["spriteSize"].asString());
            auto spriteOffset = PointFromString(frameDict["spriteOffset"].asString());
            auto spriteSourceSize = SizeFromString(frameDict["spriteSourceSize"].asString());
            auto textureRect = RectFromString(frameDict["textureRect"].asString());
            auto textureRotated = frameDict["textureRotated"].asBool();

            // get aliases
            auto& aliases = frameDict["aliases"].asValueVector();

            for (const auto& value : aliases)
            {
                auto oneAlias = value.asString();
                if (std::find(frameAliases.begin(), frameAliases.end(), oneAlias) == frameAliases.end())
                {
                    frameAliases.push_back(std::move(oneAlias));
                }
                else
                {
                    CCLOGWARN("cocos2d: WARNING: an alias with name %s already exists", oneAlias.c_str());
                }
            }

            // create frame
            spriteFrame = SpriteFrame::createWithTexture(texture,
                Rect(textureRect.origin.x, textureRect.origin.y, spriteSize.width, spriteSize.height),
                textureRotated,
                spriteOffset,
                spriteSourceSize);

            if (frameDict.find("vertices") != frameDict.end())
            {
                using cocos2d::utils::parseIntegerList;
                auto vertices = parseIntegerList(frameDict["vertices"].asString());
                auto verticesUV = parseIntegerList(frameDict["verticesUV"].asString());
                auto indices = parseIntegerList(frameDict["triangles"].asString());

                PolygonInfo info;
                initializePolygonInfo(textureSize, spriteSourceSize, vertices, verticesUV, indices, info);
                spriteFrame->setPolygonInfo(info);
            }
            if (frameDict.find("anchor") != frameDict.end())
            {
                spriteFrame->setAnchorPoint(PointFromString(frameDict["anchor"].asString()));
            }
        }

        auto flag = NinePatchImageParser::isNinePatchImage(spriteFrameName);
        if (flag)
        {
            if (image == nullptr) {
                image = new (std::nothrow) Image();
                image->initWithImageFile(textureFileName);
            }
            parser.setSpriteFrameInfo(image, spriteFrame->getRectInPixels(), spriteFrame->isRotated());
            //texture->addSpriteFrameCapInset(spriteFrame, parser.parseCapInset());
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

    CC_SAFE_DELETE(image);
}

void PlistSpriteSheetLoader::addSpriteFramesWithDictionary(ValueMap& dict, const std::string& texturePath, const std::string& plist, SpriteFrameCache& cache)
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

    Texture2D* texture = nullptr;
    static std::unordered_map<std::string, backend::PixelFormat> pixelFormats = {
        {"RGBA8888", backend::PixelFormat::RGBA8},
        {"RGBA4444", backend::PixelFormat::RGBA4},
        {"RGB5A1", backend::PixelFormat::RGB5A1},
        {"RGBA5551", backend::PixelFormat::RGB5A1},
        {"RGB565", backend::PixelFormat::RGB565},
        {"A8", backend::PixelFormat::A8},
        {"ALPHA", backend::PixelFormat::A8},
        {"I8", backend::PixelFormat::L8},
        {"AI88", backend::PixelFormat::LA8},
        {"ALPHA_INTENSITY", backend::PixelFormat::LA8},
        //{"BGRA8888", backend::PixelFormat::BGRA8888}, no Image conversion RGBA -> BGRA
        {"RGB888", backend::PixelFormat::RGB8}
    };

    const auto pixelFormatIt = pixelFormats.find(pixelFormatName);
    if (pixelFormatIt != pixelFormats.end())
    {
        const backend::PixelFormat pixelFormat = (*pixelFormatIt).second;
        const backend::PixelFormat currentPixelFormat = Texture2D::getDefaultAlphaPixelFormat();
        Texture2D::setDefaultAlphaPixelFormat(pixelFormat);
        texture = Director::getInstance()->getTextureCache()->addImage(texturePath);
        Texture2D::setDefaultAlphaPixelFormat(currentPixelFormat);
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
        CCLOG("cocos2d: SpriteFrameCache: Couldn't load texture");
    }
}

void PlistSpriteSheetLoader::reloadSpriteFramesWithDictionary(ValueMap& dict, Texture2D* texture, const std::string& plist, SpriteFrameCache& cache)
{
    auto& framesDict = dict["frames"].asValueMap();
    int format = 0;

    // get the format
    if (dict.find("metadata") != dict.end())
    {
        auto& metadataDict = dict["metadata"].asValueMap();
        format = metadataDict["format"].asInt();
    }

    // check the format
    CCASSERT(format >= 0 && format <= 3, "format is not supported for SpriteFrameCache addSpriteFramesWithDictionary:textureFilename:");

    auto spriteSheet = std::make_shared<SpriteSheet>();
    spriteSheet->format = "PLIST";
    spriteSheet->path = plist;

    for (auto& iter : framesDict)
    {
        ValueMap& frameDict = iter.second.asValueMap();
        std::string spriteFrameName = iter.first;

        cache.eraseFrame(spriteFrameName);

        SpriteFrame* spriteFrame = nullptr;
        std::vector<std::string> frameAliases;

        if (format == 0)
        {
            const auto x = frameDict["x"].asFloat();
            const auto y = frameDict["y"].asFloat();
            const auto w = frameDict["width"].asFloat();
            const auto h = frameDict["height"].asFloat();
            const auto ox = frameDict["offsetX"].asFloat();
            const auto oy = frameDict["offsetY"].asFloat();
            auto ow = frameDict["originalWidth"].asInt();
            auto oh = frameDict["originalHeight"].asInt();
            // check ow/oh
            if (!ow || !oh)
            {
                CCLOGWARN("cocos2d: WARNING: originalWidth/Height not found on the SpriteFrame. AnchorPoint won't work as expected. Regenerate the .plist");
            }
            // abs ow/oh
            ow = std::abs(ow);
            oh = std::abs(oh);
            // create frame
            spriteFrame = SpriteFrame::createWithTexture(texture,
                Rect(x, y, w, h),
                false,
                Vec2(ox, oy),
                Size((float)ow, (float)oh)
            );
        }
        else if (format == 1 || format == 2)
        {
            auto frame = RectFromString(frameDict["frame"].asString());
            auto rotated = false;

            // rotation
            if (format == 2)
            {
                rotated = frameDict["rotated"].asBool();
            }

            auto offset = PointFromString(frameDict["offset"].asString());
            auto sourceSize = SizeFromString(frameDict["sourceSize"].asString());

            // create frame
            spriteFrame = SpriteFrame::createWithTexture(texture,
                frame,
                rotated,
                offset,
                sourceSize
            );
        }
        else if (format == 3)
        {
            // get values
            const auto spriteSize = SizeFromString(frameDict["spriteSize"].asString());
            auto spriteOffset = PointFromString(frameDict["spriteOffset"].asString());
            auto spriteSourceSize = SizeFromString(frameDict["spriteSourceSize"].asString());
            const auto textureRect = RectFromString(frameDict["textureRect"].asString());
            const auto textureRotated = frameDict["textureRotated"].asBool();

            // get aliases
            ValueVector& aliases = frameDict["aliases"].asValueVector();

            for (const auto& value : aliases)
            {
                auto oneAlias = value.asString();
                if (std::find(frameAliases.begin(), frameAliases.end(), oneAlias) == frameAliases.end())
                {
                    frameAliases.push_back(std::move(oneAlias));
                }
                else
                {
                    CCLOGWARN("cocos2d: WARNING: an alias with name %s already exists", oneAlias.c_str());
                }
            }

            // create frame
            spriteFrame = SpriteFrame::createWithTexture(texture,
                Rect(textureRect.origin.x, textureRect.origin.y, spriteSize.width, spriteSize.height),
                textureRotated,
                spriteOffset,
                spriteSourceSize);
        }

        // add sprite frame
        cache.insertFrame(spriteSheet, spriteFrameName, spriteFrame);
        for (auto&& frameAlias : frameAliases)
        {
            cache.insertFrame(spriteSheet, frameAlias, spriteFrame);
        }
    }
}

NS_CC_END
