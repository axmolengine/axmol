/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2009      Jason Booth
Copyright (c) 2009      Robert J Payne
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

http://www.cocos2d-x.org

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

#include "2d/CCSpriteFrameCache.h"

#include <vector>


#include "2d/CCSprite.h"
#include "2d/CCAutoPolygon.h"
#include "platform/CCFileUtils.h"
#include "base/CCNS.h"
#include "base/ccMacros.h"
#include "base/ccUTF8.h"
#include "base/ccUtils.h"
#include "base/CCDirector.h"
#include "renderer/CCTexture2D.h"
#include "renderer/CCTextureCache.h"
#include "base/CCNinePatchImageParser.h"

using namespace std;

NS_CC_BEGIN

static SpriteFrameCache* _sharedSpriteFrameCache = nullptr;

void SpriteSheetLoader::initializePolygonInfo(const Size& textureSize,
    const Size& spriteSize,
    const std::vector<int>& vertices,
    const std::vector<int>& verticesUV,
    const std::vector<int>& triangleIndices,
    PolygonInfo& info)
{
    const auto vertexCount = vertices.size();
    const auto indexCount = triangleIndices.size();

    const auto scaleFactor = CC_CONTENT_SCALE_FACTOR();

    auto* vertexData = new (std::nothrow) V3F_C4B_T2F[vertexCount];
    for (size_t i = 0; i < vertexCount / 2; i++)
    {
        vertexData[i].colors = Color4B::WHITE;
        vertexData[i].vertices = Vec3(vertices[i * 2] / scaleFactor,
            (spriteSize.height - vertices[i * 2 + 1]) / scaleFactor,
            0);
        vertexData[i].texCoords = Tex2F(verticesUV[i * 2] / textureSize.width,
            verticesUV[i * 2 + 1] / textureSize.height);
    }

    auto* indexData = new unsigned short[indexCount];
    for (size_t i = 0; i < indexCount; i++)
    {
        indexData[i] = static_cast<unsigned short>(triangleIndices[i]);
    }

    info.triangles.vertCount = static_cast<int>(vertexCount);
    info.triangles.verts = vertexData;
    info.triangles.indexCount = static_cast<int>(indexCount);
    info.triangles.indices = indexData;
    info.setRect(Rect(0, 0, spriteSize.width, spriteSize.height));
}


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

SpriteFrameCache* SpriteFrameCache::getInstance()
{
    if (!_sharedSpriteFrameCache)
    {
        _sharedSpriteFrameCache = new (std::nothrow) SpriteFrameCache();
        _sharedSpriteFrameCache->init();
    }

    return _sharedSpriteFrameCache;
}

void SpriteFrameCache::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedSpriteFrameCache);
}

bool SpriteFrameCache::init()
{
    _spriteFrames.reserve(20);
    clear();

    registerSpriteSheetLoader("PLIST", std::unique_ptr<PlistSpriteSheetLoader>(new (std::nothrow) PlistSpriteSheetLoader()));

    return true;
}

SpriteFrameCache::~SpriteFrameCache()
{
}

void SpriteFrameCache::addSpriteFramesWithFile(const std::string& spriteSheetFileName, Texture2D* texture, const std::string& format)
{
    auto* loader = getSpriteSheetLoader(format);
    if (loader)
    {
        loader->Load(spriteSheetFileName, texture, *this);
    }
}

void SpriteFrameCache::addSpriteFramesWithFile(const std::string& spriteSheetFileName, const std::string& format)
{
    auto* loader = getSpriteSheetLoader(format);
    if (loader)
    {
        loader->Load(spriteSheetFileName, *this);
    }
}

void SpriteFrameCache::addSpriteFramesWithFileContent(const Data& content, Texture2D* texture, const std::string& format)
{
    auto* loader = getSpriteSheetLoader(format);
    if (loader)
    {
        loader->Load(content, texture, *this);
    }
}


bool SpriteFrameCache::isSpriteFramesWithFileLoaded(const std::string& plist) const
{
    return isSpriteSheetInUse(plist) && isPlistFull(plist);
}

void SpriteFrameCache::addSpriteFrame(SpriteFrame* frame, const std::string& frameName)
{
    CCASSERT(frame, "frame should not be nil");

    const std::string name = "by#addSpriteFrame()";
    auto&& itr = _spriteSheets.find(name);
    if (itr != _spriteSheets.end())
    {
        insertFrame(itr->second, frameName, frame);
    }
    else
    {
        auto spriteSheet = std::make_shared<SpriteSheet>();
        spriteSheet->path = name;
        insertFrame(spriteSheet, frameName, frame);
    }
}

void SpriteFrameCache::removeSpriteFrames()
{
    clear();
}

void SpriteFrameCache::removeUnusedSpriteFrames()
{
    auto removed = false;
    std::vector<std::string> toRemoveFrames;

    const auto frames = getSpriteFrames();
    for (auto& iter : frames)
    {
        auto* spriteFrame = iter.second;
        if (spriteFrame->getReferenceCount() == 1)
        {
            toRemoveFrames.push_back(iter.first);
            spriteFrame->getTexture()->removeSpriteFrameCapInset(spriteFrame);
            CCLOG("cocos2d: SpriteFrameCache: removing unused frame: %s", iter.first.c_str());
            removed = true;
        }
    }

    if (removed)
    {
        eraseFrames(toRemoveFrames);
    }
}


void SpriteFrameCache::removeSpriteFrameByName(const std::string& name)
{
    // explicit nil handling
    if (name.empty())
        return;

    eraseFrame(name);
}

void SpriteFrameCache::removeSpriteFramesFromFile(const std::string& atlasPath)
{
    //const auto fullPath = FileUtils::getInstance()->fullPathForFilename(plist);
    //auto dict = FileUtils::getInstance()->getValueMapFromFile(fullPath);
    //if (dict.empty())
    //{
    //    CCLOG("cocos2d:SpriteFrameCache:removeSpriteFramesFromFile: create dict by %s fail.",plist.c_str());
    //    return;
    //}
    //removeSpriteFramesFromDictionary(dict);

    // remove it from the cache
    removeSpriteSheet(atlasPath);
}

void SpriteFrameCache::removeSpriteFramesFromFileContent(const std::string& plist_content)
{
    auto dict = FileUtils::getInstance()->getValueMapFromData(plist_content.data(), static_cast<int>(plist_content.size()));
    if (dict.empty())
    {
        CCLOG("cocos2d:SpriteFrameCache:removeSpriteFramesFromFileContent: create dict by fail.");
        return;
    }
    removeSpriteFramesFromDictionary(dict);
}

void SpriteFrameCache::removeSpriteFramesFromDictionary(ValueMap& dictionary)
{
    if (dictionary["frames"].getType() != cocos2d::Value::Type::MAP)
        return;

    const auto& framesDict = dictionary["frames"].asValueMap();
    std::vector<std::string> keysToRemove;

    for (const auto& iter : framesDict)
    {
        if (findFrame(iter.first))
        {
            keysToRemove.push_back(iter.first);
        }
    }

    eraseFrames(keysToRemove);
}

void SpriteFrameCache::removeSpriteFramesFromTexture(Texture2D* texture)
{
    std::vector<std::string> keysToRemove;

    for (auto& iter : getSpriteFrames())
    {
        auto key = iter.first;
        auto* frame = findFrame(key);
        if (frame && (frame->getTexture() == texture))
        {
            keysToRemove.push_back(key);
        }
    }

    eraseFrames(keysToRemove);
}

SpriteFrame* SpriteFrameCache::getSpriteFrameByName(const std::string& name)
{
    auto* frame = findFrame(name);
    if (!frame)
    {
        CCLOG("cocos2d: SpriteFrameCache: Frame '%s' isn't found", name.c_str());
    }
    return frame;
}

bool SpriteFrameCache::reloadTexture(const std::string& spriteSheetFileName)
{
    CCASSERT(!spriteSheetFileName.empty(), "plist filename should not be nullptr");

    auto spriteSheetItr = _spriteSheets.find(spriteSheetFileName);
    if (spriteSheetItr == _spriteSheets.end())
    {
        return false; // Sprite sheet wasn't loaded, so don't reload it
    }

    const auto format = spriteSheetItr->second->format;

    if (isSpriteSheetInUse(spriteSheetFileName))
    {
        removeSpriteSheet(spriteSheetFileName);
        spriteSheetItr = _spriteSheets.end(); // we've removed the associated entry, so it's no longer valid
    }
    else
    {
        //If one plist hasn't be loaded, we don't load it here.
        return false;
    }

    auto* loader = getSpriteSheetLoader(format);
    if (loader)
    {
        loader->Reload(spriteSheetFileName, *this);
    }
    return true;
}

void SpriteFrameCache::insertFrame(const std::shared_ptr<SpriteSheet>& spriteSheet, const std::string& frameName, SpriteFrame* spriteFrame)
{
    spriteSheet->frames.insert(frameName);
    _spriteFrames.insert(frameName, spriteFrame);   //add SpriteFrame
    _spriteSheets[spriteSheet->path] = spriteSheet;
    _spriteFrameToSpriteSheetMap[frameName] = spriteSheet; //insert index frameName->plist
}

bool SpriteFrameCache::eraseFrame(const std::string& frameName)
{
    _spriteFrames.erase(frameName); //drop SpriteFrame
    const auto itFrame = _spriteFrameToSpriteSheetMap.find(frameName);
    if (itFrame != _spriteFrameToSpriteSheetMap.end())
    {
        auto& spriteSheet = itFrame->second;
        spriteSheet->full = false;
        spriteSheet->frames.erase(frameName);

        if (spriteSheet->frames.empty())
        {
            _spriteSheets.erase(spriteSheet->path);
        }

        _spriteFrameToSpriteSheetMap.erase(itFrame);    //update index frame->plist

        // erase all sprite sheets if all frames are erased
        //if (_spriteFrameToSpriteSheetMap.empty())
        //{
        //    _spriteSheets.clear();
        //}
        return true;
    }
    return false;
}

bool SpriteFrameCache::eraseFrames(const std::vector<std::string>& frames)
{
    auto ret = false;
    for (const auto& frame : frames)
    {
        ret |= eraseFrame(frame);
    }

    return ret;
}

bool SpriteFrameCache::removeSpriteSheet(const std::string& spriteSheetFileName)
{
    auto it = _spriteSheets.find(spriteSheetFileName);
    if (it == _spriteSheets.end()) return false;

    auto& frames = it->second->frames;
    for (const auto& f : frames)
    {
        // !!do not!! call `_spriteFrames.erase(f);` to erase SpriteFrame
        // only erase index here.  TODO: This makes no sense, why not call _spriteFrames.erase(f) here?
        _spriteFrames.erase(f);
        _spriteFrameToSpriteSheetMap.erase(f);                             //erase plist frame frameName->plist
    }
    _spriteSheets.erase(spriteSheetFileName);                            //update index plist->[frameNames]

    return true;
}

void SpriteFrameCache::clear()
{
    _spriteSheets.clear();
    _spriteFrameToSpriteSheetMap.clear();
    _spriteFrames.clear();
}

bool SpriteFrameCache::hasFrame(const std::string& frame) const
{
    return _spriteFrameToSpriteSheetMap.find(frame) != _spriteFrameToSpriteSheetMap.end();
}

bool SpriteFrameCache::isSpriteSheetInUse(const std::string& spriteSheetFileName) const
{
    const auto spriteSheetItr = _spriteSheets.find(spriteSheetFileName);
    return spriteSheetItr != _spriteSheets.end() && !spriteSheetItr->second->frames.empty();
}

SpriteFrame* SpriteFrameCache::findFrame(const std::string& frame)
{
    return _spriteFrames.at(frame);
}

void SpriteFrameCache::addSpriteFrameCapInset(SpriteFrame* spriteFrame, const Rect& capInsets, Texture2D* texture)
{
    texture->addSpriteFrameCapInset(spriteFrame, capInsets);
}

Map<std::string, SpriteFrame*>& SpriteFrameCache::getSpriteFrames()
{
    return _spriteFrames;
}

void SpriteFrameCache::registerSpriteSheetLoader(std::string formatId, std::unique_ptr<ISpriteSheetLoader> loader)
{
    if (_spriteSheetLoaders.find(formatId) != _spriteSheetLoaders.end())
    {
        return;
    }

    _spriteSheetLoaders.emplace(std::move(formatId), std::move(loader));
}

void SpriteFrameCache::deregisterSpriteSheetLoader(const std::string& formatId)
{
    auto&& itr = _spriteSheetLoaders.find(formatId);
    if (itr != _spriteSheetLoaders.end())
    {
        _spriteSheetLoaders.erase(itr);
    }
}

ISpriteSheetLoader* SpriteFrameCache::getSpriteSheetLoader(const std::string& formatId)
{
    auto&& itr = _spriteSheetLoaders.find(formatId);
    if (itr != _spriteSheetLoaders.end())
    {
        return itr->second.get();
    }

    return nullptr;
}

NS_CC_END
