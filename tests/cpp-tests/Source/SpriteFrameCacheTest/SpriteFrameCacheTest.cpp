/****************************************************************************
 Copyright (c) 2012 cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
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

#include "SpriteFrameCacheTest.h"

#include <cassert>

#include "NinePatchImageParser.h"

using namespace ax;

SpriteFrameCacheTests::SpriteFrameCacheTests()
{
    ADD_TEST_CASE(SpriteFrameCachePixelFormatTest);
    ADD_TEST_CASE(SpriteFrameCacheLoadMultipleTimes);
    ADD_TEST_CASE(SpriteFrameCacheFullCheck);
    ADD_TEST_CASE(SpriteFrameCacheJsonAtlasTest);
}

SpriteFrameCachePixelFormatTest::SpriteFrameCachePixelFormatTest()
{
    const Size screenSize = Director::getInstance()->getWinSize();

    infoLabel = Label::create();
    infoLabel->setAnchorPoint(Point(0.5f, 1.0f));
    infoLabel->setAlignment(ax::TextHAlignment::CENTER);
    infoLabel->setPosition(screenSize.width * 0.5f, screenSize.height * 0.7f);
    addChild(infoLabel);

    // load atlas definition with specified PixelFormat and check that it matches to expected format
    loadSpriteFrames("Images/sprite_frames_test/test_A8.plist", backend::PixelFormat::R8);
    loadSpriteFrames("Images/sprite_frames_test/test_RGBA8888.plist", backend::PixelFormat::RGBA8);
    loadSpriteFrames("Images/sprite_frames_test/test_AI88.plist", backend::PixelFormat::RG8);
    loadSpriteFrames("Images/sprite_frames_test/test_RGB565.plist", backend::PixelFormat::RGB565);
    loadSpriteFrames("Images/sprite_frames_test/test_RGB888.plist", backend::PixelFormat::RGB8);
    loadSpriteFrames("Images/sprite_frames_test/test_RGBA4444.plist", backend::PixelFormat::RGBA4);
    loadSpriteFrames("Images/sprite_frames_test/test_RGBA5551.plist", backend::PixelFormat::RGB5A1);

    if (Configuration::getInstance()->supportsPVRTC())
    {
        loadSpriteFrames("Images/sprite_frames_test/test_PVRTC2.plist", backend::PixelFormat::PVRTC2A);
        loadSpriteFrames("Images/sprite_frames_test/test_PVRTC4.plist", backend::PixelFormat::PVRTC4A);
        loadSpriteFrames("Images/sprite_frames_test/test_PVRTC2_NOALPHA.plist", backend::PixelFormat::PVRTC2);
    }

    // test loading atlases without PixelFormat specified
    Texture2D::setDefaultAlphaPixelFormat(backend::PixelFormat::RGB5A1);
    loadSpriteFrames("Images/sprite_frames_test/test_NoFormat.plist", backend::PixelFormat::RGB5A1);

    // restore default alpha pixel format
    Texture2D::setDefaultAlphaPixelFormat(backend::PixelFormat::RGBA8);
}

void SpriteFrameCachePixelFormatTest::loadSpriteFrames(std::string_view file,
                                                       ax::backend::PixelFormat expectedFormat)
{
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(file);
    SpriteFrame* spriteFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName("sprite_frames_test/grossini.png");
    Texture2D* texture       = spriteFrame->getTexture();
    const ssize_t bitsPerKB  = 8 * 1024;
    const double memorySize  = 1.0 * texture->getBitsPerPixelForFormat() * texture->getContentSizeInPixels().width *
                              texture->getContentSizeInPixels().height / bitsPerKB;

    const std::string textureInfo = fmt::format("{}{}: {:.2} KB\r\n", infoLabel->getString(),
                                                        texture->getStringForFormat(), memorySize);
    infoLabel->setString(textureInfo);

    SpriteFrameCache::getInstance()->removeSpriteFramesFromFile(file);
    Director::getInstance()->getTextureCache()->removeTexture(texture);
}

SpriteFrameCacheLoadMultipleTimes::SpriteFrameCacheLoadMultipleTimes()
{
    const Size screenSize = Director::getInstance()->getWinSize();

    // load atlas definition with specified PixelFormat and check that it matches to expected format
    loadSpriteFrames("Images/sprite_frames_test/test_RGBA8888.plist", backend::PixelFormat::RGBA8);
    loadSpriteFrames("Images/sprite_frames_test/test_RGBA8888.plist", backend::PixelFormat::RGBA8);
    loadSpriteFrames("Images/sprite_frames_test/test_RGBA8888.plist", backend::PixelFormat::RGBA8);
    loadSpriteFrames("Images/sprite_frames_test/test_RGBA8888.plist", backend::PixelFormat::RGBA8);
}

void SpriteFrameCacheLoadMultipleTimes::loadSpriteFrames(std::string_view file,
                                                         ax::backend::PixelFormat expectedFormat)
{
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(file);
    SpriteFrame* spriteFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName("sprite_frames_test/grossini.png");
    Texture2D* texture       = spriteFrame->getTexture();

    SpriteFrameCache::getInstance()->removeSpriteFrameByName("sprite_frames_test/grossini.png");
    Director::getInstance()->getTextureCache()->removeTexture(texture);
}

SpriteFrameCacheFullCheck::SpriteFrameCacheFullCheck()
{
    const Size screenSize = Director::getInstance()->getWinSize();
    // load atlas definition with specified PixelFormat and check that it matches to expected format
    loadSpriteFrames("Images/test_polygon.plist", backend::PixelFormat::RGBA8);
}

void SpriteFrameCacheFullCheck::loadSpriteFrames(std::string_view file, ax::backend::PixelFormat expectedFormat)
{
    auto cache = SpriteFrameCache::getInstance();

    AXASSERT(cache->isSpriteFramesWithFileLoaded("plist which not exists") == false, "Plist not exists");

    cache->addSpriteFramesWithFile(file);
    AXASSERT(cache->isSpriteFramesWithFileLoaded(file) == true, "Plist should be full after loaded");

    cache->removeSpriteFrameByName("not_exists_grossinis_sister.png");
    AXASSERT(cache->isSpriteFramesWithFileLoaded(file) == true, "Plist should not be still full");

    cache->removeSpriteFrameByName("grossinis_sister1.png");
    AXASSERT(cache->isSpriteFramesWithFileLoaded(file) == false, "Plist should not be full after remove any sprite");

    cache->addSpriteFramesWithFile(file);
    AXASSERT(cache->isSpriteFramesWithFileLoaded(file) == true, "Plist should be full after reloaded");
}

class GenericJsonArraySpriteSheetLoader : public SpriteSheetLoader
{
public:
    static constexpr uint32_t FORMAT = SpriteSheetFormat::CUSTOM + 1;

    uint32_t getFormat() override { return FORMAT; }

    void load(std::string_view filePath, SpriteFrameCache& cache) override
    {
        AXASSERT(!filePath.empty(), "atlas filename should not be nullptr");

        const auto fullPath = FileUtils::getInstance()->fullPathForFilename(filePath);
        if (fullPath.empty())
        {
            // return if plist file doesn't exist
            AXLOGD("GenericJsonArraySpriteSheetLoader: can not find {}", filePath);
            return;
        }

        const auto data = FileUtils::getInstance()->getStringFromFile(fullPath);

        rapidjson::Document jDoc;
        jDoc.Parse(data.c_str());

        std::string texturePath;

        auto&& metaItr = jDoc.FindMember("meta");
        if (metaItr != jDoc.MemberEnd())
        {
            texturePath = metaItr->value["image"].GetString();
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
            if (startPos != std::string::npos)
            {
                texturePath = texturePath.erase(startPos);
            }

            // append .png
            texturePath = texturePath.append(".png");

            AXLOGD("GenericJsonArraySpriteSheetLoader::load: Trying to use file {} as texture", texturePath);
        }

        addSpriteFramesWithJson(jDoc, texturePath, filePath, cache);
    }

    void load(std::string_view filePath, Texture2D* texture, SpriteFrameCache& cache) override
    {
        const auto fullPath = FileUtils::getInstance()->fullPathForFilename(filePath);
        rapidjson::Document jDoc;
        const auto data = FileUtils::getInstance()->getStringFromFile(fullPath);
        jDoc.Parse(data.c_str());

        addSpriteFramesWithJson(jDoc, texture, filePath, cache);
    }

    void load(std::string_view filePath, std::string_view textureFileName, SpriteFrameCache& cache) override
    {
        AXASSERT(!textureFileName.empty(), "texture name should not be null");
        const auto fullPath = FileUtils::getInstance()->fullPathForFilename(filePath);
        rapidjson::Document jDoc;
        const auto data = FileUtils::getInstance()->getStringFromFile(fullPath);
        jDoc.Parse(data.c_str());
        addSpriteFramesWithJson(jDoc, textureFileName, filePath, cache);
    }

    void load(const Data& content, Texture2D* texture, SpriteFrameCache& cache) override
    {
        if (content.isNull())
        {
            return;
        }

        rapidjson::Document jDoc;
        const auto* text = (char*)content.getBytes();
        jDoc.Parse(text);
        addSpriteFramesWithJson(jDoc, texture, "by#addSpriteFramesWithFileContent()", cache);
    }

    void reload(std::string_view filePath, SpriteFrameCache& cache) override
    {
        const auto fullPath = FileUtils::getInstance()->fullPathForFilename(filePath);
        rapidjson::Document doc;
        const auto data = FileUtils::getInstance()->getStringFromFile(fullPath);
        doc.Parse(data.c_str());

        std::string texturePath;

        auto&& metaItr = doc.FindMember("meta");
        if (metaItr != doc.MemberEnd())
        {
            texturePath = metaItr->value["image"].GetString();
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
            if (startPos != std::string::npos)
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
            reloadSpriteFramesWithDictionary(doc, texture, filePath, cache);
        }
        else
        {
            AXLOGD("GenericJsonArraySpriteSheetLoader::reload: Couldn't load texture");
        }
    }

protected:
    void addSpriteFramesWithJson(const rapidjson::Document& doc,
                                 std::string_view texturePath,
                                 std::string_view atlasPath,
                                 SpriteFrameCache& cache)
    {
        std::string pixelFormatName;
        auto&& metaItr = doc.FindMember("meta");
        if (metaItr != doc.MemberEnd())
        {
            auto&& pixelFormatItr = metaItr->value.FindMember("format");
            if (pixelFormatItr != metaItr->value.MemberEnd())
            {
                pixelFormatName = pixelFormatItr->value.GetString();
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
            addSpriteFramesWithJson(doc, texture, atlasPath, cache);
        }
        else
        {
            AXLOGD("GenericJsonArraySpriteSheetLoader::addSpriteFramesWithJson: Couldn't load texture");
        }
    }

    void addSpriteFramesWithJson(const rapidjson::Document& doc,
                                 Texture2D* texture,
                                 std::string_view atlasPath,
                                 SpriteFrameCache& cache)
    {
        auto&& framesItr = doc.FindMember("frames");
        if (framesItr == doc.MemberEnd())
            return;

        const auto spriteSheet = std::make_shared<SpriteSheet>();
        spriteSheet->format    = getFormat();
        spriteSheet->path      = atlasPath;

        const auto textureFileName = Director::getInstance()->getTextureCache()->getTextureFilePath(texture);
        Image* image               = nullptr;
        NinePatchImageParser parser;

        for (auto&& frameItr : framesItr->value.GetArray())
        {
            if (!frameItr.IsObject())
            {
                continue;
            }

            const auto spriteFrameName = frameItr["filename"].GetString();
            auto* spriteFrame          = cache.findFrame(spriteFrameName);
            if (spriteFrame)
            {
                continue;
            }

            const auto frameX      = frameItr["frame"]["x"].GetFloat();
            const auto frameY      = frameItr["frame"]["y"].GetFloat();
            const auto frameW      = frameItr["frame"]["w"].GetFloat();
            const auto frameH      = frameItr["frame"]["h"].GetFloat();
            const auto textureRect = Rect(frameX, frameY, frameW, frameH);

            const auto originalWidth  = frameItr["sourceSize"]["w"].GetInt();
            const auto originalHeight = frameItr["sourceSize"]["w"].GetInt();
            const auto originalSize   = Size(originalWidth, originalHeight);

            const auto trimmedWidth  = frameItr["spriteSourceSize"]["w"].GetInt();
            const auto trimmedHeight = frameItr["spriteSourceSize"]["h"].GetInt();
            const auto trimmedSize   = Size(trimmedWidth, trimmedHeight);

            const auto textureRotated = frameItr["rotated"].GetBool();

            // create frame
            spriteFrame = SpriteFrame::createWithTexture(
                texture, Rect(textureRect.origin.x, textureRect.origin.y, trimmedSize.width, trimmedSize.height),
                textureRotated, Vec2(), originalSize);

            const auto flag = NinePatchImageParser::isNinePatchImage(spriteFrameName);
            if (flag)
            {
                if (image == nullptr)
                {
                    image = new Image();
                    image->initWithImageFile(textureFileName);
                }
                parser.setSpriteFrameInfo(image, spriteFrame->getRectInPixels(), spriteFrame->isRotated());
                cache.addSpriteFrameCapInset(spriteFrame, parser.parseCapInset(), texture);
            }

            // add sprite frame
            cache.insertFrame(spriteSheet, spriteFrameName, spriteFrame);
        }

        spriteSheet->full = true;

        AX_SAFE_DELETE(image);
    }

    void reloadSpriteFramesWithDictionary(const rapidjson::Document& doc,
                                          Texture2D* texture,
                                          std::string_view atlasPath,
                                          SpriteFrameCache& cache)
    {
        auto&& framesItr = doc.FindMember("frames");
        if (framesItr == doc.MemberEnd())
            return;

        const auto spriteSheet = std::make_shared<SpriteSheet>();
        spriteSheet->format    = getFormat();
        spriteSheet->path      = atlasPath;

        for (auto&& frameItr : framesItr->value.GetArray())
        {
            if (!frameItr.IsObject())
            {
                continue;
            }

            const auto spriteFrameName = frameItr["filename"].GetString();
            auto* spriteFrame          = cache.findFrame(spriteFrameName);
            if (spriteFrame)
            {
                continue;
            }

            const auto frameX      = frameItr["frame"]["x"].GetFloat();
            const auto frameY      = frameItr["frame"]["y"].GetFloat();
            const auto frameW      = frameItr["frame"]["w"].GetFloat();
            const auto frameH      = frameItr["frame"]["h"].GetFloat();
            const auto textureRect = Rect(frameX, frameY, frameW, frameH);

            const auto originalWidth  = frameItr["sourceSize"]["w"].GetInt();
            const auto originalHeight = frameItr["sourceSize"]["w"].GetInt();
            const auto originalSize   = Size(originalWidth, originalHeight);

            const auto trimmedWidth  = frameItr["spriteSourceSize"]["w"].GetInt();
            const auto trimmedHeight = frameItr["spriteSourceSize"]["h"].GetInt();
            const auto trimmedSize   = Size(trimmedWidth, trimmedHeight);

            const auto textureRotated = frameItr["rotated"].GetBool();

            // create frame
            spriteFrame = SpriteFrame::createWithTexture(
                texture, Rect(textureRect.origin.x, textureRect.origin.y, trimmedSize.width, trimmedSize.height),
                textureRotated, Vec2(), originalSize);

            // add sprite frame
            cache.insertFrame(spriteSheet, spriteFrameName, spriteFrame);
        }
    }
};

SpriteFrameCacheJsonAtlasTest::SpriteFrameCacheJsonAtlasTest()
{
    auto* cache = SpriteFrameCache::getInstance();
    cache->registerSpriteSheetLoader(std::make_unique<GenericJsonArraySpriteSheetLoader>());

    const Size screenSize = Director::getInstance()->getWinSize();

    infoLabel = Label::create();
    infoLabel->setAnchorPoint(Point(0.5f, 1.0f));
    infoLabel->setAlignment(ax::TextHAlignment::CENTER);
    infoLabel->setPosition(screenSize.width * 0.5f, screenSize.height * 0.7f);
    addChild(infoLabel);

    loadSpriteFrames("Images/sprite_frames_test/test_RGB8888_generic.json"sv, backend::PixelFormat::RGBA8);
}

SpriteFrameCacheJsonAtlasTest::~SpriteFrameCacheJsonAtlasTest()
{
    auto* cache = SpriteFrameCache::getInstance();
    cache->deregisterSpriteSheetLoader(GenericJsonArraySpriteSheetLoader::FORMAT);
}

void SpriteFrameCacheJsonAtlasTest::loadSpriteFrames(std::string_view file,
                                                     ax::backend::PixelFormat expectedFormat)
{
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(file, GenericJsonArraySpriteSheetLoader::FORMAT);
    SpriteFrame* spriteFrame =
        SpriteFrameCache::getInstance()->getSpriteFrameByName("sprite_frames_test/grossini.png"sv);
    Texture2D* texture      = spriteFrame->getTexture();
    const ssize_t bitsPerKB = 8 * 1024;
    const double memorySize = 1.0 * texture->getBitsPerPixelForFormat() * texture->getContentSizeInPixels().width *
                              texture->getContentSizeInPixels().height / bitsPerKB;
    const std::string textureInfo = fmt::format("{}{}: {:.2} KB\r\n", infoLabel->getString(),
                                                        texture->getStringForFormat(), memorySize);
    infoLabel->setString(textureInfo);

    SpriteFrameCache::getInstance()->removeSpriteFramesFromFile(file);
    Director::getInstance()->getTextureCache()->removeTexture(texture);
}
