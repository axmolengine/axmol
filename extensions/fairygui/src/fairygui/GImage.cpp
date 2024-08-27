/****************************************************************************
 Copyright (c) 2015 fairygui.com
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

The MIT License (MIT)

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

#include "GImage.h"
#include "PackageItem.h"
#include "display/FUISprite.h"
#include "utils/ByteBuffer.h"
#include "utils/ToolSet.h"

NS_FGUI_BEGIN
using namespace ax;

GImage::GImage() : _content(nullptr)
{
    _touchDisabled = true;
}

GImage::~GImage()
{
}

void GImage::handleInit()
{
    _content = FUISprite::create();
    _content->retain();

    _displayObject = _content;
}

FlipType GImage::getFlip() const
{
    if (_content->isFlippedX() && _content->isFlippedY())
        return FlipType::BOTH;
    else if (_content->isFlippedX())
        return FlipType::HORIZONTAL;
    else if (_content->isFlippedY())
        return FlipType::VERTICAL;
    else
        return FlipType::NONE;
}

void GImage::setFlip(FlipType value)
{
    _content->setFlippedX(value == FlipType::HORIZONTAL || value == FlipType::BOTH);
    _content->setFlippedY(value == FlipType::VERTICAL || value == FlipType::BOTH);
}

void GImage::handleGrayedChanged()
{
    GObject::handleGrayedChanged();

    ((FUISprite*)_content)->setGrayed(_finalGrayed);
}

ax::Color3B GImage::getColor() const
{
    return _content->getColor();
}

void GImage::setColor(const ax::Color3B& value)
{
    _content->setColor(value);
}

FillMethod GImage::getFillMethod() const
{
    return _content->getFillMethod();
}

void GImage::setFillMethod(FillMethod value)
{
    _content->setFillMethod(value);
}

FillOrigin GImage::getFillOrigin() const
{
    return _content->getFillOrigin();
}

void GImage::setFillOrigin(FillOrigin value)
{
    _content->setFillOrigin(value);
}

bool GImage::isFillClockwise() const
{
    return _content->isFillClockwise();
}

void GImage::setFillClockwise(bool value)
{
    _content->setFillClockwise(value);
}

float GImage::getFillAmount() const
{
    return _content->getFillAmount();
}

void GImage::setFillAmount(float value)
{
    _content->setFillAmount(value);
}

ax::Value GImage::getProp(ObjectPropID propId)
{
    switch (propId)
    {
    case ObjectPropID::Color:
        return Value(ToolSet::colorToInt(getColor()));
    default:
        return GObject::getProp(propId);
    }
}

void GImage::setProp(ObjectPropID propId, const ax::Value& value)
{
    switch (propId)
    {
    case ObjectPropID::Color:
        setColor(ToolSet::intToColor(value.asUnsignedInt()));
        break;
    default:
        GObject::setProp(propId, value);
        break;
    }
}

void GImage::constructFromResource()
{
    PackageItem* contentItem = _packageItem->getBranch();
    sourceSize.width         = contentItem->width;
    sourceSize.height        = contentItem->height;
    initSize                 = sourceSize;

    contentItem = contentItem->getHighResolution();
    contentItem->load();

    _content->setSpriteFrame(contentItem->spriteFrame);
    if (contentItem->scale9Grid)
    {
        const auto contentScaleFactor = AX_CONTENT_SCALE_FACTOR();

        auto scaledInsets = *contentItem->scale9Grid;

        scaledInsets.setRect(scaledInsets.origin.x / contentScaleFactor, scaledInsets.origin.y / contentScaleFactor,
                             scaledInsets.size.x / contentScaleFactor, scaledInsets.size.y / contentScaleFactor);

        ((FUISprite*)_content)->setScale9Grid(&scaledInsets);
    }
    else if (contentItem->scaleByTile)
        ((FUISprite*)_content)->setScaleByTile(true);

    setSize(sourceSize.width, sourceSize.height);
}

void GImage::setup_beforeAdd(ByteBuffer* buffer, int beginPos)
{
    GObject::setup_beforeAdd(buffer, beginPos);

    buffer->seek(beginPos, 5);

    if (buffer->readBool())
        setColor((Color3B)buffer->readColor());
    setFlip((FlipType)buffer->readByte());
    int fillMethod = buffer->readByte();
    if (fillMethod != 0)
    {
        _content->setFillMethod((FillMethod)fillMethod);
        _content->setFillOrigin((FillOrigin)buffer->readByte());
        _content->setFillClockwise(buffer->readBool());
        _content->setFillAmount(buffer->readFloat());
    }
}

NS_FGUI_END
