#include "GTextInput.h"
#include "UIPackage.h"
#include "GRoot.h"
#include "ui/UIEditBox/UIEditBox.h"
#include "utils/ByteBuffer.h"
#include "utils/UBBParser.h"
#include "utils/ToolSet.h"

NS_FGUI_BEGIN
using namespace ax;

GTextInput::GTextInput()
{
}

GTextInput::~GTextInput()
{
}

void GTextInput::handleInit()
{
    _input = FUIInput::create();
    _input->retain();
    _input->setDelegate(this);

    _displayObject = _input;

    this->addEventListener(UIEventType::TouchEnd, [this](EventContext*) {
        _input->openKeyboard();
    });
}

bool GTextInput::isSingleLine() const
{
    return _input->isSingleLine();
}

void GTextInput::setSingleLine(bool value)
{
    _input->setSingleLine(value);
}

void GTextInput::applyTextFormat()
{
    _input->applyTextFormat();
}

void GTextInput::setPrompt(const std::string & value)
{
    if (value.empty())
        _input->setPlaceHolder(value.c_str());
    else
    {
        UBBParser* parser = UBBParser::getInstance();
        _input->setPlaceHolder(parser->parse(value.c_str(), true).c_str());
        if (!parser->lastColor.empty())
            _input->setPlaceholderFontColor(ToolSet::hexToColor(parser->lastColor.c_str()));
        if (!parser->lastFontSize.empty())
            _input->setPlaceholderFontSize(Value(parser->lastFontSize).asInt());
    }
}

void GTextInput::setPassword(bool value)
{
    _input->setPassword(value);
}

void GTextInput::setKeyboardType(int value)
{
    _input->setKeyboardType(value);
}

void GTextInput::setMaxLength(int value)
{
    _input->setMaxLength(value);
}

void GTextInput::setRestrict(const std::string & value)
{
}

void GTextInput::handleSizeChanged()
{
    _input->setContentSize(_size);
}

void GTextInput::setup_beforeAdd(ByteBuffer* buffer, int beginPos)
{
    GTextField::setup_beforeAdd(buffer, beginPos);

    buffer->seek(beginPos, 4);

    const std::string* str;
    if ((str = buffer->readSP()))
        setPrompt(*str);

    if ((str = buffer->readSP()))
        setRestrict(*str);

    int iv = buffer->readInt();
    if (iv != 0)
        setMaxLength(iv);
    iv = buffer->readInt();
    if (iv != 0)
        setKeyboardType(iv);
    if (buffer->readBool())
        setPassword(true);
}

void GTextInput::setTextFieldText()
{
    if (_templateVars != nullptr)
        _input->setText(parseTemplate(_text.c_str()));
    else
        _input->setText(_text);
}

void GTextInput::editBoxReturn(ax::ui::EditBox * editBox)
{
    //found that this will trigger even when focus is lost
    //if (isSingleLine())
    // dispatchEvent(UIEventType::Submit);
}

void GTextInput::editBoxTextChanged(ax::ui::EditBox* editBox, std::string_view /*text*/)
{
    _text.clear();
    _text.append(_input->getText());
}

NS_FGUI_END
