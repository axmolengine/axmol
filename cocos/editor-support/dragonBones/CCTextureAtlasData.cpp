#include "CCTextureAtlasData.h"

DRAGONBONES_NAMESPACE_BEGIN
void CCTextureAtlasData::_onClear()
{
    TextureAtlasData::_onClear();

    if (_renderTexture != nullptr)
    {
        _renderTexture->release();
        _renderTexture = nullptr;
    }
}

TextureData* CCTextureAtlasData::createTexture() const
{
    return BaseObject::borrowObject<CCTextureData>();
}

void CCTextureAtlasData::setRenderTexture(cocos2d::Texture2D* value)
{
    if (_renderTexture == value) 
    {
        return;
    }

    _renderTexture = value;

    if (_renderTexture != nullptr) 
    {
        _renderTexture->retain();

        for (const auto& pair : textures) 
        {
            const auto textureData = static_cast<CCTextureData*>(pair.second);

            if (textureData->spriteFrame == nullptr)
            {
                cocos2d::Rect rect(
                    textureData->region.x, textureData->region.y,
                    textureData->rotated ? textureData->region.height : textureData->region.width,
                    textureData->rotated ? textureData->region.width : textureData->region.height
                );
                cocos2d::Vec2 offset(0.0f, 0.0f);
                cocos2d::Size originSize(rect.size.width, rect.size.height);
                textureData->spriteFrame = cocos2d::SpriteFrame::createWithTexture(_renderTexture, rect, textureData->rotated, offset, originSize); // TODO multiply textureAtlas
                textureData->spriteFrame->retain();
            }
        }
    }
    else 
    {
        for (const auto& pair : textures)
        {
            const auto textureData = static_cast<CCTextureData*>(pair.second);

            if (textureData->spriteFrame != nullptr)
            {
                textureData->spriteFrame->release();
                textureData->spriteFrame = nullptr;
            }
        }
    }
}

void CCTextureData::_onClear()
{
    TextureData::_onClear();

    if (spriteFrame != nullptr)
    {
        spriteFrame->release();
        spriteFrame = nullptr;
    }
}
DRAGONBONES_NAMESPACE_END
