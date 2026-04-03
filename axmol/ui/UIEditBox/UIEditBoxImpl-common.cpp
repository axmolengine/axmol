/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2012 James Chen
 Copyright (c) 2013-2015 zilongshanren
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
#include "axmol/ui/UIEditBox/UIEditBoxImpl-common.h"

#define kLabelZOrder 9999

#include "axmol/ui/UIEditBox/UIEditBox.h"
#include "axmol/base/Director.h"
#include "axmol/2d/Label.h"
#include "axmol/ui/UIHelper.h"

static const int AX_EDIT_BOX_PADDING = 5;

#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32)
#    define PASSWORD_CHAR "*"
#else
#    define PASSWORD_CHAR "\u25CF"
#endif

namespace ax
{

static Vec2 applyPadding(const Vec2& sizeToCorrect)
{
    return Vec2(sizeToCorrect.width - AX_EDIT_BOX_PADDING * 2, sizeToCorrect.height);
}

namespace ui
{

EditBoxImplCommon::EditBoxImplCommon(EditBox* pEditText)
    : EditBoxImpl(pEditText)
    , _label(nullptr)
    , _labelPlaceHolder(nullptr)
    , _editBoxInputMode(EditBox::InputMode::SINGLE_LINE)
    , _editBoxInputFlag(EditBox::InputFlag::INITIAL_CAPS_ALL_CHARACTERS)
    , _keyboardReturnType(EditBox::KeyboardReturnType::DEFAULT)
    , _alignment(TextHAlignment::LEFT)
    , _fontSize(-1)
    , _placeholderFontSize(-1)
    , _colText(Color32::WHITE)
    , _colPlaceHolder(Color32::GRAY)
    , _maxLength(-1)
    , _editingMode(false)
{}

EditBoxImplCommon::~EditBoxImplCommon() {}

bool EditBoxImplCommon::initWithSize(const Vec2& size)
{
    do
    {
        Rect rect = Rect(0, 0, size.width, size.height);

        this->createNativeControl(rect);

        initInactiveLabels(size);
        setContentSize(size);

        return true;
    } while (0);

    return false;
}

void EditBoxImplCommon::initInactiveLabels(const Vec2& size)
{
    std::string_view defaultFontName = this->getNativeDefaultFontName();

    _label = Label::create();
    _label->setAnchorPoint(Vec2(0.0f, 1.0f));
    _label->setOverflow(Label::Overflow::CLAMP);
    _label->setVisible(false);
    _label->setGlobalZOrder(_editBox->getGlobalZOrder());
    _editBox->addChild(_label, kLabelZOrder);

    _labelPlaceHolder = Label::create();
    _labelPlaceHolder->setAnchorPoint(Vec2(0.0f, 1.0f));
    _labelPlaceHolder->setTextColor(Color32::GRAY);
    _labelPlaceHolder->enableWrap(false);
    _labelPlaceHolder->setGlobalZOrder(_editBox->getGlobalZOrder());
    _editBox->addChild(_labelPlaceHolder, kLabelZOrder);

    setFont(defaultFontName, size.height * 2 / 3);
    setPlaceholderFont(defaultFontName, size.height * 2 / 3);
}

void EditBoxImplCommon::placeInactiveLabels(const Vec2& size)
{
    _label->setDimensions(size.width, size.height);

    auto placeholderSize = _labelPlaceHolder->getContentSize();

    if (_editBoxInputMode == EditBox::InputMode::ANY)
    {
        _label->setPosition(Vec2((float)AX_EDIT_BOX_PADDING, size.height - AX_EDIT_BOX_PADDING));
        _label->setVerticalAlignment(TextVAlignment::TOP);
        _label->enableWrap(true);

        _labelPlaceHolder->setPosition(Vec2((float)AX_EDIT_BOX_PADDING, size.height - AX_EDIT_BOX_PADDING));
        _labelPlaceHolder->setVerticalAlignment(TextVAlignment::TOP);
    }
    else
    {
        _label->enableWrap(false);
        _label->setPosition(Vec2((float)AX_EDIT_BOX_PADDING, size.height));
        _label->setVerticalAlignment(TextVAlignment::CENTER);

        _labelPlaceHolder->setPosition(Vec2((float)AX_EDIT_BOX_PADDING, (size.height + placeholderSize.height) / 2));
        _labelPlaceHolder->setVerticalAlignment(TextVAlignment::CENTER);
    }
}

void EditBoxImplCommon::setInactiveText(std::string_view text)
{
    if (EditBox::InputFlag::PASSWORD == _editBoxInputFlag)
    {
        std::string passwordString;
        for (size_t i = 0, len = text.length(); i < len; ++i)
            passwordString.append(PASSWORD_CHAR);
        _label->setString(passwordString);
    }
    else
    {
        _label->setString(text);
    }
    // Clip the text width to fit to the text box
    const auto maxSize = applyPadding(_editBox->getContentSize());
    Vec2 labelSize     = _label->getContentSize();
    if (labelSize.width > maxSize.width || labelSize.height > maxSize.height)
    {
        _label->setDimensions(maxSize.width, maxSize.height);
    }
}

void EditBoxImplCommon::setFont(std::string_view fontName, int fontSize)
{
    _fontName = fontName;
    _fontSize = fontSize;
    this->setNativeFont(fontName, fontSize * _label->getNodeToWorldAffineTransform().a);

    if (FileUtils::getInstance()->isFileExist(fontName))
    {
        TTFConfig ttfConfig(fontName, fontSize);
        _label->setTTFConfig(ttfConfig);
    }
    else
    {
        if (!_fontName.empty())
        {
            _label->setSystemFontName(fontName);
        }
        if (fontSize > 0)
        {
            _label->setSystemFontSize(fontSize);
        }
    }
}

void EditBoxImplCommon::setFontColor(const Color32& color)
{
    _colText = color;
    this->setNativeFontColor(color);
    _label->setTextColor(color);
}

void EditBoxImplCommon::setPlaceholderFont(std::string_view fontName, int fontSize)
{
    _placeholderFontName = fontName;
    _placeholderFontSize = fontSize;
    this->setNativePlaceholderFont(fontName, fontSize * _labelPlaceHolder->getNodeToWorldAffineTransform().a);

    if (FileUtils::getInstance()->isFileExist(fontName))
    {
        TTFConfig ttfConfig(fontName, fontSize);
        _labelPlaceHolder->setTTFConfig(ttfConfig);
    }
    else
    {
        if (!_placeholderFontName.empty())
        {
            _labelPlaceHolder->setSystemFontName(fontName);
        }
        if (fontSize > 0)
        {
            _labelPlaceHolder->setSystemFontSize(fontSize);
        }
    }
}

void EditBoxImplCommon::setPlaceholderFontColor(const Color32& color)
{
    _colPlaceHolder = color;
    this->setNativePlaceholderFontColor(color);
    _labelPlaceHolder->setTextColor(color);
}

void EditBoxImplCommon::setInputMode(EditBox::InputMode inputMode)
{
    _editBoxInputMode = inputMode;
    this->setNativeInputMode(inputMode);
    this->placeInactiveLabels(applyPadding(_editBox->getContentSize()));
}

void EditBoxImplCommon::setMaxLength(int maxLength)
{
    _maxLength = maxLength;
    this->setNativeMaxLength(maxLength);
}

void EditBoxImplCommon::setTextHorizontalAlignment(ax::TextHAlignment alignment)
{
    _alignment = alignment;
    this->setNativeTextHorizontalAlignment(alignment);
    refreshLabelAlignment();
}

void EditBoxImplCommon::setInputFlag(EditBox::InputFlag inputFlag)
{
    _editBoxInputFlag = inputFlag;
    this->setNativeInputFlag(inputFlag);
}

void EditBoxImplCommon::setReturnType(EditBox::KeyboardReturnType returnType)
{
    _keyboardReturnType = returnType;
    this->setNativeReturnType(returnType);
}

void EditBoxImplCommon::refreshInactiveText()
{
    setInactiveText(_text);

    refreshLabelAlignment();
    if (!_editingMode)
    {
        if (_text.empty())
        {
            _label->setVisible(false);
            _labelPlaceHolder->setVisible(true);
        }
        else
        {
            _label->setVisible(true);
            _labelPlaceHolder->setVisible(false);
        }
    }
}

void EditBoxImplCommon::refreshLabelAlignment()
{
    _label->setHorizontalAlignment(_alignment);
    _labelPlaceHolder->setHorizontalAlignment(_alignment);
}

void EditBoxImplCommon::setText(std::string_view text)
{
    this->setNativeText(text);
    _text = text;
    refreshInactiveText();
}

void EditBoxImplCommon::setPlaceHolder(std::string_view text)
{
    _placeHolder = text;
    this->setNativePlaceHolder(text);
    _labelPlaceHolder->setString(_placeHolder);
}

void EditBoxImplCommon::setVisible(bool visible)
{
    if (visible)
    {
        refreshInactiveText();
    }
    else
    {
        this->setNativeVisible(visible);
        _label->setVisible(visible);
        _labelPlaceHolder->setVisible(visible);
    }
}

void EditBoxImplCommon::setContentSize(const Vec2& size)
{
    _contentSize = applyPadding(size);
    AXLOGD("[Edit text] content size = ({}, {})", _contentSize.width, _contentSize.height);
    placeInactiveLabels(_contentSize);
}

void EditBoxImplCommon::setGlobalZOrder(float globalZOrder)
{
    if (_label)
    {
        _label->setGlobalZOrder(globalZOrder);
    }

    if (_labelPlaceHolder)
    {
        _labelPlaceHolder->setGlobalZOrder(globalZOrder);
    }
}

void EditBoxImplCommon::draw(Renderer* /*renderer*/, const Mat4& /*transform*/, uint32_t flags)
{
    if (flags)
    {
        auto rect = ui::Helper::convertBoundingBoxToScreen(_editBox);
        this->updateNativeFrame(rect);
    }
}

void EditBoxImplCommon::onEnter(void)
{
    std::string_view text = getText();
    setInactiveText(text);
}

void EditBoxImplCommon::openKeyboard()
{
    _editBox->setBrightStyle(Widget::BrightStyle::HIGHLIGHT);
    _label->setVisible(false);
    _labelPlaceHolder->setVisible(false);
    _editingMode = true;
    this->setNativeVisible(true);
    this->nativeOpenKeyboard();
}

void EditBoxImplCommon::closeKeyboard()
{
    this->nativeCloseKeyboard();
    _editingMode = false;
}

void EditBoxImplCommon::onEndEditing(std::string_view /*text*/)
{
    _editBox->setBrightStyle(Widget::BrightStyle::NORMAL);
    _editingMode = false;
    this->setNativeVisible(false);
    refreshInactiveText();
}

void EditBoxImplCommon::editBoxEditingDidBegin()
{
    // LOGD("textFieldShouldBeginEditing...");
    ax::ui::EditBoxDelegate* pDelegate = _editBox->getDelegate();

    if (pDelegate != nullptr)
    {
        pDelegate->editBoxEditingDidBegin(_editBox);
    }

#if AX_ENABLE_SCRIPT_BINDING
    if (NULL != _editBox && 0 != _editBox->getScriptEditBoxHandler())
    {
        ax::CommonScriptData data(_editBox->getScriptEditBoxHandler(), "began"sv, _editBox);
        ax::ScriptEvent event(ax::kCommonEvent, (void*)&data);
        ax::ScriptEngineManager::sendEventToLua(event);
    }
#endif
}

void EditBoxImplCommon::editBoxEditingDidEnd(std::string_view text, EditBoxDelegate::EditBoxEndAction action)
{
    // LOGD("textFieldShouldEndEditing...");
    _text = text;

    ax::ui::EditBoxDelegate* pDelegate = _editBox->getDelegate();
    if (pDelegate != nullptr)
    {
        pDelegate->editBoxEditingDidEndWithAction(_editBox, action);
        pDelegate->editBoxReturn(_editBox);
    }

#if AX_ENABLE_SCRIPT_BINDING
    if (_editBox != nullptr && 0 != _editBox->getScriptEditBoxHandler())
    {
        ax::CommonScriptData data(_editBox->getScriptEditBoxHandler(), "ended"sv, _editBox);
        ax::ScriptEvent event(ax::kCommonEvent, (void*)&data);
        ax::ScriptEngineManager::sendEventToLua(event);
        tlx::strlcpy(data.eventName, "return"sv);
        event.data = (void*)&data;
        ax::ScriptEngineManager::sendEventToLua(event);
    }
#endif

    if (_editBox != nullptr)
    {
        this->onEndEditing(_text);
    }
}

void EditBoxImplCommon::editBoxEditingChanged(std::string_view text)
{
    // LOGD("editBoxTextChanged...");
    ax::ui::EditBoxDelegate* pDelegate = _editBox->getDelegate();
    _text                              = text;
    if (pDelegate != nullptr)
    {
        pDelegate->editBoxTextChanged(_editBox, text);
    }

#if AX_ENABLE_SCRIPT_BINDING
    if (NULL != _editBox && 0 != _editBox->getScriptEditBoxHandler())
    {
        ax::CommonScriptData data(_editBox->getScriptEditBoxHandler(), "changed"sv, _editBox);
        ax::ScriptEvent event(ax::kCommonEvent, (void*)&data);
        ax::ScriptEngineManager::sendEventToLua(event);
    }
#endif
}

}  // namespace ui

}  // namespace ax
