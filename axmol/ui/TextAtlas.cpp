/****************************************************************************
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#include "axmol/ui/TextAtlas.h"
#include "axmol/2d/Label.h"

namespace ax
{

namespace ui
{

static const int LABELATLAS_RENDERER_Z = (-1);

IMPLEMENT_CLASS_GUI_INFO(TextAtlas)

TextAtlas::TextAtlas()
    : _labelAtlasRenderer(nullptr)
    , _stringValue("")
    , _charMapFileName("")
    , _itemWidth(0)
    , _itemHeight(0)
    , _startCharMap("")
    , _labelAtlasRendererAdaptDirty(true)
{}

TextAtlas::~TextAtlas() {}

TextAtlas* TextAtlas::create()
{
    TextAtlas* widget = new TextAtlas();
    if (widget->init())
    {
        widget->autorelease();
        return widget;
    }
    AX_SAFE_DELETE(widget);
    return nullptr;
}

void TextAtlas::initRenderNode()
{
    _labelAtlasRenderer = Label::create();
    _labelAtlasRenderer->setAnchorPoint(Anchors::center);
    addProtectedChild(_labelAtlasRenderer, LABELATLAS_RENDERER_Z, -1);
}

TextAtlas* TextAtlas::create(std::string_view stringValue,
                             std::string_view charMapFile,
                             int itemWidth,
                             int itemHeight,
                             std::string_view startCharMap)
{
    TextAtlas* widget = new TextAtlas();
    if (widget->init())
    {
        widget->autorelease();
        widget->setProperty(stringValue, charMapFile, itemWidth, itemHeight, startCharMap);
        return widget;
    }
    AX_SAFE_DELETE(widget);
    return nullptr;
}

void TextAtlas::setProperty(std::string_view stringValue,
                            std::string_view charMapFile,
                            int itemWidth,
                            int itemHeight,
                            std::string_view startCharMap)
{
    _stringValue     = stringValue;
    _charMapFileName = charMapFile;
    _itemWidth       = itemWidth;
    _itemHeight      = itemHeight;
    _startCharMap    = startCharMap;

    _labelAtlasRenderer->setCharMap(_charMapFileName, _itemWidth, _itemHeight, (int)(_startCharMap[0]));
    _labelAtlasRenderer->setString(stringValue);

    updateContentSize();
    _labelAtlasRendererAdaptDirty = true;
    //    AXLOGD("cs w {}, h {}", _contentSize.width, _contentSize.height);
}

void TextAtlas::setString(std::string_view value)
{
    if (value == _labelAtlasRenderer->getString())
    {
        return;
    }
    _stringValue = value;
    _labelAtlasRenderer->setString(value);
    updateContentSize();
    _labelAtlasRendererAdaptDirty = true;
    //    AXLOGD("cssss w {}, h {}", _contentSize.width, _contentSize.height);
}

std::string_view TextAtlas::getString() const
{
    return _labelAtlasRenderer->getString();
}

ssize_t TextAtlas::getCharCount() const
{
    return _labelAtlasRenderer->getCharCount();
}

void TextAtlas::onSizeChanged()
{
    Widget::onSizeChanged();
    _labelAtlasRendererAdaptDirty = true;
}

void TextAtlas::updateLayout()
{
    if (_labelAtlasRendererAdaptDirty)
    {
        labelAtlasScaleChangedWithSize();
        _labelAtlasRendererAdaptDirty = false;
    }
}

Vec2 TextAtlas::resolvePreferredSize(const Vec2& /*sizeHint*/) const
{
    return _labelAtlasRenderer->getContentSize();
}

Node* TextAtlas::getRenderNode()
{
    return _labelAtlasRenderer;
}

void TextAtlas::labelAtlasScaleChangedWithSize()
{
    if (_autoSize)
    {
        _labelAtlasRenderer->setScale(1.0f);
    }
    else
    {
        Vec2 textureSize = _labelAtlasRenderer->getContentSize();
        if (textureSize.width <= 0.0f || textureSize.height <= 0.0f)
        {
            _labelAtlasRenderer->setScale(1.0f);
            return;
        }
        float scaleX = _contentSize.width / textureSize.width;
        float scaleY = _contentSize.height / textureSize.height;
        _labelAtlasRenderer->setScaleX(scaleX);
        _labelAtlasRenderer->setScaleY(scaleY);
    }
    _labelAtlasRenderer->setPosition(_contentSize.width / 2.0f, _contentSize.height / 2.0f);
}

std::string TextAtlas::getDescription() const
{
    return "TextAtlas";
}

Widget* TextAtlas::createCloneInstance()
{
    return TextAtlas::create();
}

void TextAtlas::copySpecialProperties(Widget* widget)
{
    TextAtlas* labelAtlas = dynamic_cast<TextAtlas*>(widget);
    if (labelAtlas)
    {
        setProperty(labelAtlas->_stringValue, labelAtlas->_charMapFileName, labelAtlas->_itemWidth,
                    labelAtlas->_itemHeight, labelAtlas->_startCharMap);
    }
}

ResourceData TextAtlas::getRenderFile()
{
    ResourceData rData;
    rData.type = 0;
    rData.file = _charMapFileName;
    return rData;
}

}  // namespace ui
}  // namespace ax
