#include "GTextField.h"
#include "utils/ByteBuffer.h"
#include "utils/ToolSet.h"

NS_FGUI_BEGIN
using namespace ax;

GTextField::GTextField()
    : _templateVars(nullptr),
      _ubbEnabled(false),
      _autoSize(AutoSizeType::BOTH)
{
}

GTextField::~GTextField()
{
    AX_SAFE_DELETE(_templateVars);
}

void GTextField::setText(const std::string& value)
{
    _text = value;
    setTextFieldText();
    updateGear(6);
    updateSize();
}

void GTextField::setUBBEnabled(bool value)
{
    if (_ubbEnabled != value)
    {
        _ubbEnabled = value;
        setTextFieldText();
        updateSize();
    }
}

void GTextField::setColor(const ax::Color3B& value)
{
    TextFormat* tf = getTextFormat();
    if (tf->color != value)
    {
        tf->color = value;
        applyTextFormat();
    }
}

void GTextField::setFontSize(float value)
{
    TextFormat* tf = getTextFormat();
    if (tf->fontSize != value)
    {
        tf->fontSize = value;
        applyTextFormat();
    }
}

void GTextField::setOutlineColor(const ax::Color3B& value)
{
    TextFormat* tf = getTextFormat();
    if (tf->outlineColor != value)
    {
        tf->outlineColor = value;
        applyTextFormat();
    }
}

void GTextField::setTemplateVars(ax::ValueMap* value)
{
    if (_templateVars == nullptr && value == nullptr)
        return;

    if (value == nullptr)
        AX_SAFE_DELETE(_templateVars);
    else
    {
        if (_templateVars == nullptr)
            _templateVars = new ax::ValueMap();
        *_templateVars = *value;
    }

    flushVars();
}

GTextField* GTextField::setVar(const std::string& name, const ax::Value& value)
{
    if (_templateVars == nullptr)
        _templateVars = new ax::ValueMap();

    (*_templateVars)[name] = value;

    return this;
}

void GTextField::flushVars()
{
    setTextFieldText();
    updateSize();
}

void GTextField::updateSize()
{
}

ax::Value GTextField::getProp(ObjectPropID propId)
{
    switch (propId)
    {
    case ObjectPropID::Color:
        return Value(ToolSet::colorToInt(getColor()));
    case ObjectPropID::OutlineColor:
        return Value(ToolSet::colorToInt(getOutlineColor()));
    case ObjectPropID::FontSize:
        return Value(getFontSize());
    default:
        return GObject::getProp(propId);
    }
}

void GTextField::setProp(ObjectPropID propId, const ax::Value& value)
{
    switch (propId)
    {
    case ObjectPropID::Color:
        setColor(ToolSet::intToColor(value.asUnsignedInt()));
        break;
    case ObjectPropID::OutlineColor:
        setOutlineColor(ToolSet::intToColor(value.asUnsignedInt()));
        break;
    case ObjectPropID::FontSize:
        setFontSize(value.asInt());
        break;
    default:
        GObject::setProp(propId, value);
        break;
    }
}

void GTextField::setup_beforeAdd(ByteBuffer* buffer, int beginPos)
{
    GObject::setup_beforeAdd(buffer, beginPos);

    buffer->seek(beginPos, 5);

    TextFormat* tf = getTextFormat();

    tf->face = buffer->readS();
    tf->fontSize = buffer->readShort();
    tf->color = (Color3B)buffer->readColor();
    tf->align = (TextHAlignment)buffer->readByte();
    tf->verticalAlign = (TextVAlignment)buffer->readByte();
    tf->lineSpacing = buffer->readShort();
    tf->letterSpacing = buffer->readShort();
    _ubbEnabled = buffer->readBool();
    setAutoSize((AutoSizeType)buffer->readByte());
    tf->underline = buffer->readBool();
    tf->italics = buffer->readBool();
    tf->bold = buffer->readBool();
    if (buffer->readBool())
        setSingleLine(true);
    if (buffer->readBool())
    {
        tf->outlineColor = (Color3B)buffer->readColor();
        tf->outlineSize = buffer->readFloat();
        tf->enableEffect(TextFormat::OUTLINE);
    }

    if (buffer->readBool())
    {
        tf->shadowColor = (Color3B)buffer->readColor();
        float f1 = buffer->readFloat();
        float f2 = buffer->readFloat();
        tf->shadowOffset = Vec2(f1, -f2);
        tf->enableEffect(TextFormat::SHADOW);
    }

    if (buffer->readBool())
        _templateVars = new ax::ValueMap();
}

void GTextField::setup_afterAdd(ByteBuffer* buffer, int beginPos)
{
    GObject::setup_afterAdd(buffer, beginPos);

    applyTextFormat();

    buffer->seek(beginPos, 6);

    const std::string& str = buffer->readS();
    if (!str.empty())
        setText(str);
}

std::string GTextField::parseTemplate(const char* text)
{
    const char* pString = text;

    ssize_t pos;
    ssize_t pos2;
    std::string tag, attr;
    std::string repl;
    std::string out;

    while (*pString != '\0')
    {
        const char* p = strchr(pString, '{');
        if (!p)
        {
            out.append(pString);
            break;
        }

        pos = p - pString;
        if (pos > 0 && *(p - 1) == '\\')
        {
            out.append(pString, pos - 1);
            out.append("{");
            pString += pos + 1;
            continue;
        }

        out.append(pString, pos);
        pString += pos;

        p = strchr(pString, '}');
        if (!p)
        {
            out.append(pString);
            break;
        }

        pos = p - pString;
        if (pos == 1)
        {
            out.append(pString, 0, 2);
            pString += 2;
            continue;
        }

        tag.assign(pString + 1, pos - 1);

        attr.clear();
        repl.clear();
        pos2 = tag.find('=');
        if (pos2 != -1)
        {
            auto it = _templateVars->find(tag.substr(0, pos2));
            if (it != _templateVars->end())
                out.append(it->second.asString());
            else
                out.append(tag.substr(pos2 + 1));
        }
        else
        {
            auto it = _templateVars->find(tag);
            if (it != _templateVars->end())
                out.append(it->second.asString());
        }
        pString += pos + 1;
    }
    return out;
}

//---------------------------

GBasicTextField::GBasicTextField() : _label(nullptr),
                                     _updatingSize(false)
{
    _touchDisabled = true;
}

GBasicTextField::~GBasicTextField()
{
}

void GBasicTextField::handleInit()
{
    _label = FUILabel::create();
    _label->retain();

    _displayObject = _label;
}

void GBasicTextField::applyTextFormat()
{
    _label->applyTextFormat();
    updateGear(4);
    if (!_underConstruct)
        updateSize();
}

void GBasicTextField::setAutoSize(AutoSizeType value)
{
    _autoSize = value;
    switch (value)
    {
    case AutoSizeType::NONE:
        _label->setOverflow(Label::Overflow::CLAMP);
        break;
    case AutoSizeType::BOTH:
        _label->setOverflow(Label::Overflow::NONE);
        break;
    case AutoSizeType::HEIGHT:
        _label->setOverflow(Label::Overflow::RESIZE_HEIGHT);
        break;
    case AutoSizeType::SHRINK:
        _label->setOverflow(Label::Overflow::SHRINK);
        break;
    }

    if (_autoSize == AutoSizeType::BOTH)
        _label->setDimensions(0, 0);
    else if (_autoSize == AutoSizeType::HEIGHT)
        _label->setDimensions(_size.width, 0);
    else
        _label->setDimensions(_size.width, _size.height);

    if (!_underConstruct)
        updateSize();
}

void GBasicTextField::setSingleLine(bool value)
{
    _label->enableWrap(!value);
    if (!_underConstruct)
        updateSize();
}

void GBasicTextField::setTextFieldText()
{
    if (_templateVars != nullptr)
        _label->setText(parseTemplate(_text.c_str()));
    else
        _label->setText(_text);
}

void GBasicTextField::updateSize()
{
    if (_updatingSize)
        return;

    _updatingSize = true;

    Size sz = _label->getContentSize();
    if (_autoSize == AutoSizeType::BOTH)
        setSize(sz.width, sz.height);
    else if (_autoSize == AutoSizeType::HEIGHT)
        setHeight(sz.height);

    _updatingSize = false;
}

void GBasicTextField::handleSizeChanged()
{
    if (_updatingSize)
        return;

    if (_autoSize != AutoSizeType::BOTH)
    {
        _label->setDimensions(_size.width, _size.height);

        if (_autoSize == AutoSizeType::HEIGHT)
        {
            if (!_text.empty())
                setSizeDirectly(_size.width, _label->getContentSize().height);
        }
    }
}

void GBasicTextField::handleGrayedChanged()
{
    GObject::handleGrayedChanged();

    _label->setGrayed(_finalGrayed);
}

NS_FGUI_END
