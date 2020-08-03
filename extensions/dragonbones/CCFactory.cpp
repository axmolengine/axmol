#include "CCFactory.h"
#include "CCTextureAtlasData.h"
#include "CCArmatureDisplay.h"
#include "CCSlot.h"

DRAGONBONES_NAMESPACE_BEGIN

DragonBones* CCFactory::_dragonBonesInstance = nullptr;
CCFactory* CCFactory::_factory = nullptr;

TextureAtlasData* CCFactory::_buildTextureAtlasData(TextureAtlasData* textureAtlasData, void* textureAtlas) const
{
    if (textureAtlasData != nullptr)
    {
        const auto pos = _prevPath.find_last_of("/");
        if (pos != std::string::npos)
        {
            const auto basePath = _prevPath.substr(0, pos + 1);
            textureAtlasData->imagePath = basePath + textureAtlasData->imagePath;
        }

        if (textureAtlas != nullptr)
        {
            static_cast<CCTextureAtlasData*>(textureAtlasData)->setRenderTexture(static_cast<cocos2d::Texture2D*>(textureAtlas));
        }
        else 
        {
            const auto textureCache = cocos2d::Director::getInstance()->getTextureCache();
            auto texture = textureCache->getTextureForKey(textureAtlasData->imagePath);
            if (texture == nullptr)
            {
                const auto defaultPixelFormat = cocos2d::Texture2D::getDefaultAlphaPixelFormat();
                auto pixelFormat = defaultPixelFormat;
                switch (textureAtlasData->format)
                {
                    case TextureFormat::RGBA8888:
                        pixelFormat = cocos2d::Texture2D::PixelFormat::RGBA8888;
                        break;

                    case TextureFormat::BGRA8888:
                        pixelFormat = cocos2d::Texture2D::PixelFormat::BGRA8888;
                        break;

                    case TextureFormat::RGBA4444:
                        pixelFormat = cocos2d::Texture2D::PixelFormat::RGBA4444;
                        break;

                    case TextureFormat::RGB888:
                        pixelFormat = cocos2d::Texture2D::PixelFormat::RGB888;
                        break;

                    case TextureFormat::RGB565:
                        pixelFormat = cocos2d::Texture2D::PixelFormat::RGB565;
                        break;

                    case TextureFormat::RGBA5551:
                        pixelFormat = cocos2d::Texture2D::PixelFormat::RGB5A1;
                        break;

                    case TextureFormat::DEFAULT:
                    default:
                        break;
                }

                cocos2d::Texture2D::setDefaultAlphaPixelFormat(pixelFormat);
                texture = textureCache->addImage(textureAtlasData->imagePath);
                if (texture != nullptr)
                {
                    cocos2d::Texture2D::setDefaultAlphaPixelFormat(defaultPixelFormat);
                }
            }

            static_cast<CCTextureAtlasData*>(textureAtlasData)->setRenderTexture(texture);
        }
    }
    else
    {
        textureAtlasData = BaseObject::borrowObject<CCTextureAtlasData>();
    }

    return textureAtlasData;
}

Armature* CCFactory::_buildArmature(const BuildArmaturePackage& dataPackage) const
{
    const auto armature = BaseObject::borrowObject<Armature>();
    const auto armatureDisplay = CCArmatureDisplay::create();

    armatureDisplay->retain();
    armatureDisplay->setCascadeOpacityEnabled(true);
    armatureDisplay->setCascadeColorEnabled(true);

    armature->init(
        dataPackage.armature,
        armatureDisplay, armatureDisplay, _dragonBones
    );

    return armature;
}

Slot* CCFactory::_buildSlot(const BuildArmaturePackage& dataPackage, const SlotData* slotData, Armature* armature) const
{
    const auto slot = BaseObject::borrowObject<CCSlot>();
    const auto rawDisplay = DBCCSprite::create();

    rawDisplay->setCascadeOpacityEnabled(true);
    rawDisplay->setCascadeColorEnabled(true);
    rawDisplay->setAnchorPoint(cocos2d::Vec2::ZERO);
    rawDisplay->setLocalZOrder(slotData->zOrder);

    slot->init(
        slotData, armature,
        rawDisplay, rawDisplay
    );

    return slot;
}

DragonBonesData* CCFactory::loadDragonBonesData(const std::string& filePath, const std::string& name, float scale)
{
    if (!name.empty())
    {
        const auto existedData = getDragonBonesData(name);
        if (existedData)
        {
            return existedData;
        }
    }

    const auto fullpath = cocos2d::FileUtils::getInstance()->fullPathForFilename(filePath);
    if (cocos2d::FileUtils::getInstance()->isFileExist(filePath)) 
    {
        const auto pos = fullpath.find(".json");

        if (pos != std::string::npos)
        {
            const auto data = cocos2d::FileUtils::getInstance()->getStringFromFile(filePath);

            return parseDragonBonesData(data.c_str(), name, scale);
        }
        else
        {
#if COCOS2D_VERSION >= 0x00031200
            cocos2d::Data cocos2dData;
            cocos2d::FileUtils::getInstance()->getContents(fullpath, &cocos2dData);
#else
            const auto cocos2dData = cocos2d::FileUtils::getInstance()->getDataFromFile(fullpath);
#endif
            const auto binary = (unsigned char*)malloc(sizeof(unsigned char)* cocos2dData.getSize());
            memcpy(binary, cocos2dData.getBytes(), cocos2dData.getSize());
            const auto data = parseDragonBonesData((char*)binary, name, scale);

            return data;
        }
    }

    return nullptr;
}

TextureAtlasData* CCFactory::loadTextureAtlasData(const std::string& filePath, const std::string& name, float scale)
{
    _prevPath = cocos2d::FileUtils::getInstance()->fullPathForFilename(filePath);
    const auto data = cocos2d::FileUtils::getInstance()->getStringFromFile(_prevPath);
    if (data.empty())
    {
        return nullptr;
    }

    return static_cast<CCTextureAtlasData*>(BaseFactory::parseTextureAtlasData(data.c_str(), nullptr, name, scale));
}

CCArmatureDisplay* CCFactory::buildArmatureDisplay(const std::string& armatureName, const std::string& dragonBonesName, const std::string& skinName, const std::string& textureAtlasName) const
{
    const auto armature = buildArmature(armatureName, dragonBonesName, skinName, textureAtlasName);
    if (armature != nullptr)
    {
        _dragonBones->getClock()->add(armature);

        return static_cast<CCArmatureDisplay*>(armature->getDisplay());
    }

    return nullptr;
}

cocos2d::Sprite* CCFactory::getTextureDisplay(const std::string& textureName, const std::string& dragonBonesName) const
{
    const auto textureData = static_cast<CCTextureData*>(_getTextureData(dragonBonesName, textureName));
    if (textureData != nullptr && textureData->spriteFrame != nullptr)
    {
        const auto display = cocos2d::Sprite::createWithSpriteFrame(textureData->spriteFrame);
        return display;
    }

    return nullptr;
}

DRAGONBONES_NAMESPACE_END
