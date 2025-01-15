/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
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
#include "2d/LabelAtlas.h"
#include "renderer/TextureAtlas.h"
#include "platform/FileUtils.h"
#include "base/Director.h"
#include "renderer/TextureCache.h"

#if AX_LABELATLAS_DEBUG_DRAW
#    include "renderer/Renderer.h"
#endif

namespace ax
{

// CCLabelAtlas - Creation & Init

LabelAtlas* LabelAtlas::create(std::string_view string,
                               std::string_view charMapFile,
                               int itemWidth,
                               int itemHeight,
                               int startCharMap)
{
    LabelAtlas* ret = new LabelAtlas();
    if (ret->initWithString(string, charMapFile, itemWidth, itemHeight, startCharMap))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

bool LabelAtlas::initWithString(std::string_view string,
                                std::string_view charMapFile,
                                int itemWidth,
                                int itemHeight,
                                int startCharMap)
{
    Texture2D* texture = _director->getTextureCache()->addImage(charMapFile);
    return initWithString(string, texture, itemWidth, itemHeight, startCharMap);
}

bool LabelAtlas::initWithString(std::string_view string,
                                Texture2D* texture,
                                int itemWidth,
                                int itemHeight,
                                int startCharMap)
{
    if (AtlasNode::initWithTexture(texture, itemWidth, itemHeight, static_cast<int>(string.size())))
    {
        _mapStartChar = startCharMap;
        this->setString(string);
        return true;
    }
    return false;
}

LabelAtlas* LabelAtlas::create(std::string_view string, std::string_view fntFile)
{
    LabelAtlas* ret = new LabelAtlas();

    if (ret->initWithString(string, fntFile))
    {
        ret->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ret);
    }

    return ret;
}

LabelAtlas* LabelAtlas::create(std::string_view string,
                               Texture2D* texture,
                               int itemWidth,
                               int itemHeight,
                               int startCharMap)
{
    LabelAtlas* ret = new LabelAtlas();

    if (ret->initWithString(string, texture, itemWidth, itemHeight, startCharMap))
    {
        ret->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ret);
    }

    return ret;
}

bool LabelAtlas::initWithString(std::string_view theString, std::string_view fntFile)
{
    std::string pathStr    = FileUtils::getInstance()->fullPathForFilename(fntFile);
    std::string relPathStr = pathStr.substr(0, pathStr.find_last_of('/')) + "/";

    ValueMap dict = FileUtils::getInstance()->getValueMapFromFile(pathStr);

    AXASSERT(dict["version"].asInt() == 1, "Unsupported version. Upgrade cocos2d version");

    std::string textureFilename = relPathStr + dict["textureFilename"].asString();

    unsigned int width     = static_cast<unsigned int>(dict["itemWidth"].asInt() / AX_CONTENT_SCALE_FACTOR());
    unsigned int height    = static_cast<unsigned int>(dict["itemHeight"].asInt() / AX_CONTENT_SCALE_FACTOR());
    unsigned int startChar = dict["firstChar"].asInt();

    this->initWithString(theString, textureFilename, width, height, startChar);

    return true;
}

// CCLabelAtlas - Atlas generation
void LabelAtlas::updateAtlasValues()
{
    if (_itemsPerRow == 0)
    {
        return;
    }

    ssize_t n = _string.length();

    const unsigned char* s = (unsigned char*)_string.c_str();

    Texture2D* texture       = _textureAtlas->getTexture();
    float textureWide        = (float)texture->getPixelsWide();
    float textureHigh        = (float)texture->getPixelsHigh();
    float itemWidthInPixels  = _itemWidth * AX_CONTENT_SCALE_FACTOR();
    float itemHeightInPixels = _itemHeight * AX_CONTENT_SCALE_FACTOR();
    if (_ignoreContentScaleFactor)
    {
        itemWidthInPixels  = static_cast<float>(_itemWidth);
        itemHeightInPixels = static_cast<float>(_itemHeight);
    }

    AXASSERT(n <= _textureAtlas->getCapacity(), "updateAtlasValues: Invalid String length");
    auto quads = _textureAtlas->getQuads();
    for (ssize_t i = 0; i < n; i++)
    {

        unsigned char a = s[i] - _mapStartChar;
        float row       = (float)(a % _itemsPerRow);
        float col       = (float)(a / _itemsPerRow);

#if AX_FIX_ARTIFACTS_BY_STRECHING_TEXEL
        // Issue #938. Don't use texStepX & texStepY
        float left   = (2 * row * itemWidthInPixels + 1) / (2 * textureWide);
        float right  = left + (itemWidthInPixels * 2 - 2) / (2 * textureWide);
        float top    = (2 * col * itemHeightInPixels + 1) / (2 * textureHigh);
        float bottom = top + (itemHeightInPixels * 2 - 2) / (2 * textureHigh);
#else
        float left   = row * itemWidthInPixels / textureWide;
        float right  = left + itemWidthInPixels / textureWide;
        float top    = col * itemHeightInPixels / textureHigh;
        float bottom = top + itemHeightInPixels / textureHigh;
#endif  // ! AX_FIX_ARTIFACTS_BY_STRECHING_TEXEL

        quads[i].tl.texCoord.u = left;
        quads[i].tl.texCoord.v = top;
        quads[i].tr.texCoord.u = right;
        quads[i].tr.texCoord.v = top;
        quads[i].bl.texCoord.u = left;
        quads[i].bl.texCoord.v = bottom;
        quads[i].br.texCoord.u = right;
        quads[i].br.texCoord.v = bottom;

        quads[i].bl.position.x = (float)(i * _itemWidth);
        quads[i].bl.position.y = 0;
        quads[i].bl.position.z = 0.0f;
        quads[i].br.position.x = (float)(i * _itemWidth + _itemWidth);
        quads[i].br.position.y = 0;
        quads[i].br.position.z = 0.0f;
        quads[i].tl.position.x = (float)(i * _itemWidth);
        quads[i].tl.position.y = (float)(_itemHeight);
        quads[i].tl.position.z = 0.0f;
        quads[i].tr.position.x = (float)(i * _itemWidth + _itemWidth);
        quads[i].tr.position.y = (float)(_itemHeight);
        quads[i].tr.position.z = 0.0f;
        Color32 c(_displayedColor, _displayedOpacity);
        quads[i].tl.color = c;
        quads[i].tr.color = c;
        quads[i].bl.color = c;
        quads[i].br.color = c;
    }
    if (n > 0)
    {
        _textureAtlas->setDirty(true);
        ssize_t totalQuads = _textureAtlas->getTotalQuads();
        if (n > totalQuads)
        {
            _textureAtlas->increaseTotalQuadsWith(static_cast<int>(n - totalQuads));
        }
    }
}

// CCLabelAtlas - LabelProtocol
void LabelAtlas::setString(std::string_view label)
{
    ssize_t len = label.size();
    if (len > _textureAtlas->getTotalQuads())
    {
        _textureAtlas->resizeCapacity(len);
    }
    _string.clear();
    _string = label;
    this->updateAtlasValues();

    Vec2 s = Vec2(len * _itemWidth, _itemHeight);

    this->setContentSize(s);

    _quadsToDraw = len;
}

std::string_view LabelAtlas::getString() const
{
    return _string;
}

void LabelAtlas::updateColor()
{
    if (_textureAtlas)
    {
        Color color(_displayedColor, _displayedOpacity / 255.0f);
        if (_isOpacityModifyRGB)
        {
            color.r *= color.a;
            color.g *= color.a;
            color.b *= color.a;
        }
        auto quads     = _textureAtlas->getQuads();
        ssize_t length = _string.length();
        for (int index = 0; index < length; index++)
        {
            quads[index].bl.color = color;
            quads[index].br.color = color;
            quads[index].tl.color = color;
            quads[index].tr.color = color;
            _textureAtlas->updateQuad(quads[index], index);
        }
    }
}

// CCLabelAtlas - draw
#if AX_LABELATLAS_DEBUG_DRAW
void LabelAtlas::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
    AtlasNode::draw(renderer, transform, _transformUpdated);

    _debugDrawNode->clear();
    auto size        = getContentSize();
    Vec2 vertices[4] = {Vec2::ZERO, Vec2(size.width, 0), Vec2(size.width, size.height), Vec2(0.0f, size.height)};
    _debugDrawNode->drawPoly(vertices, 4, true, Color32::WHITE);
}
#endif

std::string LabelAtlas::getDescription() const
{
    return fmt::format("<LabelAtlas | Tag = {}, Label = '{}'>", _tag, _string);
}

}
