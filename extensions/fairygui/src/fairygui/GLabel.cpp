#include "GLabel.h"
#include "GButton.h"
#include "GTextField.h"
#include "GTextInput.h"
#include "PackageItem.h"
#include "utils/ByteBuffer.h"
#include "utils/ToolSet.h"

NS_FGUI_BEGIN
using namespace ax;

GLabel::GLabel() :
    _titleObject(nullptr),
    _iconObject(nullptr)
{
}
GLabel::~GLabel()
{
}

const std::string& GLabel::getText() const
{
    if (_titleObject != nullptr)
        return _titleObject->getText();
    else
        return STD_STRING_EMPTY;
}

void GLabel::setText(const std::string & value)
{
    if (_titleObject != nullptr)
        _titleObject->setText(value);
    updateGear(6);
}

const std::string& GLabel::getIcon() const
{
    if (_iconObject != nullptr)
        return _iconObject->getIcon();
    else
        return STD_STRING_EMPTY;
}

void GLabel::setIcon(const std::string & value)
{
    if (_iconObject != nullptr)
        _iconObject->setIcon(value);
    updateGear(7);
}

ax::Color3B GLabel::getTitleColor() const
{
    GTextField* tf = getTextField();
    if (tf)
        return tf->getColor();
    else
        return Color3B::BLACK;
}

void GLabel::setTitleColor(const ax::Color3B & value)
{
    GTextField* tf = getTextField();
    if (tf)
        tf->setColor(value);
}

int GLabel::getTitleFontSize() const
{
    GTextField* tf = getTextField();
    if (tf)
        return tf->getFontSize();
    else
        return 0;
}

void GLabel::setTitleFontSize(int value)
{
    GTextField* tf = getTextField();
    if (tf)
        tf->setFontSize(value);
}

GTextField * GLabel::getTextField() const
{
    if (dynamic_cast<GTextField*>(_titleObject))
        return dynamic_cast<GTextField*>(_titleObject);
    else if (dynamic_cast<GLabel*>(_titleObject))
        return dynamic_cast<GLabel*>(_titleObject)->getTextField();
    else if (dynamic_cast<GButton*>(_titleObject))
        return dynamic_cast<GButton*>(_titleObject)->getTextField();
    else
        return nullptr;
}

ax::Value GLabel::getProp(ObjectPropID propId)
{
    switch (propId)
    {
    case ObjectPropID::Color:
        return Value(ToolSet::colorToInt(getTitleColor()));
    case ObjectPropID::OutlineColor:
    {
        GTextField* tf = getTextField();
        if (tf != nullptr)
            return Value(ToolSet::colorToInt(tf->getOutlineColor()));
        else
            return Value::Null;
    }
    case ObjectPropID::FontSize:
        return Value(getTitleFontSize());
    default:
        return GComponent::getProp(propId);
    }
}

void GLabel::setProp(ObjectPropID propId, const ax::Value& value)
{
    switch (propId)
    {
    case ObjectPropID::Color:
        setTitleColor(ToolSet::intToColor(value.asUnsignedInt()));
        break;
    case ObjectPropID::OutlineColor:
    {
        GTextField* tf = getTextField();
        if (tf != nullptr)
            tf->setOutlineColor(ToolSet::intToColor(value.asUnsignedInt()));
        break;
    }
    case ObjectPropID::FontSize:
        setTitleFontSize(value.asInt());
        break;
    default:
        GComponent::setProp(propId, value);
        break;
    }
}

void GLabel::constructExtension(ByteBuffer* buffer)
{
    _titleObject = getChild("title");
    _iconObject = getChild("icon");
}

void GLabel::setup_afterAdd(ByteBuffer* buffer, int beginPos)
{
    GComponent::setup_afterAdd(buffer, beginPos);

    if (!buffer->seek(beginPos, 6))
        return;

    if ((ObjectType)buffer->readByte() != _packageItem->objectType)
        return;

    const std::string* str;

    if ((str = buffer->readSP()))
        setTitle(*str);
    if ((str = buffer->readSP()))
        setIcon(*str);
    if (buffer->readBool())
        setTitleColor((Color3B)buffer->readColor());
    int iv = buffer->readInt();
    if (iv != 0)
        setTitleFontSize(iv);

    if (buffer->readBool())
    {
        GTextInput* input = dynamic_cast<GTextInput*>(getTextField());
        if (input)
        {
            if ((str = buffer->readSP()))
                input->setPrompt(*str);
            if ((str = buffer->readSP()))
                input->setRestrict(*str);
            iv = buffer->readInt();
            if (iv != 0)
                input->setMaxLength(iv);
            iv = buffer->readInt();
            if (iv != 0)
                input->setKeyboardType(iv);
            if (buffer->readBool())
                input->setPassword(true);
        }
        else
            buffer->skip(13);
    }
}

NS_FGUI_END