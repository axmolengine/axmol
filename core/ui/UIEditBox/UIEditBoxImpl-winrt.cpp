///****************************************************************************
//Copyright (c) 2014 cocos2d-x.org
//Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
//Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).
//
//https://axmol.dev/
//
//* Portions Copyright (c) Microsoft Open Technologies, Inc.
//* All Rights Reserved
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.
//****************************************************************************/

#include "platform/PlatformConfig.h"
#if (AX_TARGET_PLATFORM == AX_PLATFORM_WINRT)
#include "ui/UIEditBox/UIEditBoxImpl-winrt.h"
#include "platform/winrt/WinRTUtils.h"
#include "platform/winrt/GLViewImpl-winrt.h"
#include "2d/FontFreeType.h"

#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#if defined(WINAPI_FAMILY) && WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
#define XAML_TOP_PADDING 10.0f
#else
#define XAML_TOP_PADDING 0.0f
#endif

#define EDIT_BOX_PADDING 5.0f

namespace ax
{

namespace ui
{

static const winrt::hstring EDIT_BOX_XAML_NAME = L"axmol_editbox";
static const winrt::hstring CANVAS_XAML_NAME   = L"axmol_canvas";

EditBoxImpl* __createSystemEditBox(EditBox* pEditBox)
{
    return new UIEditBoxImplWinrt(pEditBox);
}

EditBoxWinRT::EditBoxWinRT(
    winrt::delegate<Windows::Foundation::IInspectable const&, winrt::hstring const&> const& beginHandler,
    winrt::delegate<Windows::Foundation::IInspectable const&, winrt::hstring const&> const& changeHandler,
    winrt::delegate<Windows::Foundation::IInspectable const&, ax::EndEventArgs const&> const& endHandler)
    : _beginHandler(beginHandler)
    , _changeHandler(changeHandler)
    , _endHandler(endHandler)
    , _alignment()
    , _initialText(L"")
    , _fontFamily(L"Segoe UI")
    , _fontSize(12)
    , _password(false)
    , _isEditing(false)
    , _multiline(false)
    , _maxLength(0 /* unlimited */)
{
    m_dispatcher = ax::GLViewImpl::sharedGLView()->getDispatcher();
    m_panel      = ax::GLViewImpl::sharedGLView()->getPanel();
}

void EditBoxWinRT::closeKeyboard()
{
    m_dispatcher.get().RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, [strong_this{get_strong()}]() {
        strong_this->removeTextBox();
        strong_this->_textBox = nullptr;
        auto canvas           = findXamlElement(strong_this->m_panel.get(), CANVAS_XAML_NAME).as<Canvas>();
        canvas.Visibility(Visibility::Collapsed);
    });
}

Windows::UI::Xaml::Controls::Control EditBoxWinRT::createPasswordBox()
{
    auto passwordBox = PasswordBox();
    passwordBox.BorderThickness(Windows::UI::Xaml::Thickness{0});
    passwordBox.Name(EDIT_BOX_XAML_NAME);
    passwordBox.Width(_size.Width);
    passwordBox.Height(_size.Height);
    passwordBox.Foreground(Media::SolidColorBrush(_color));
    passwordBox.FontSize(_fontSize);
    passwordBox.FontFamily(Media::FontFamily(_fontFamily));
    passwordBox.MaxLength(_maxLength);
    passwordBox.Password(_initialText);
    _changeToken = passwordBox.PasswordChanged({get_weak(), &ax::ui::EditBoxWinRT::onPasswordChanged});
    return passwordBox;
}

Windows::UI::Xaml::Controls::Control EditBoxWinRT::createTextBox()
{
    auto textBox = TextBox();
    textBox.BorderThickness(Windows::UI::Xaml::Thickness{0});
    textBox.Name(EDIT_BOX_XAML_NAME);
    textBox.Width(_size.Width);
    textBox.Height(_size.Height);
    textBox.Foreground(Media::SolidColorBrush(_color));
    textBox.FontSize(_fontSize);
    textBox.FontFamily(Media::FontFamily(_fontFamily));
    textBox.MaxLength(_maxLength);
    textBox.AcceptsReturn(_multiline);
    textBox.TextWrapping(_multiline ? TextWrapping::Wrap : TextWrapping::NoWrap);
    textBox.Text(_initialText);
    setInputScope(textBox);
    _setTextHorizontalAlignment(textBox);
    _changeToken = textBox.TextChanged({get_weak(), &ax::ui::EditBoxWinRT::onTextChanged});
    return textBox;
}

void EditBoxWinRT::onPasswordChanged(Windows::Foundation::IInspectable const& sender,
                                     Windows::UI::Xaml::RoutedEventArgs const& args)
{
    onTextChanged(sender, nullptr);
}

void EditBoxWinRT::onTextChanged(Windows::Foundation::IInspectable const& sender,
                                 Windows::UI::Xaml::Controls::TextChangedEventArgs const& e)
{
    winrt::hstring text = L"";
    if (_password)
    {
        text = _textBox.as<PasswordBox>().Password();
    }
    else
    {
        text = _textBox.as<TextBox>().Text();
    }
    std::shared_ptr<ax::InputEvent> inputEvent(new UIEditBoxEvent(*this, text, _changeHandler));
    ax::GLViewImpl::sharedGLView()->QueueEvent(inputEvent);
}

void EditBoxWinRT::onKeyDown(Windows::Foundation::IInspectable const& sender,
                             Windows::UI::Xaml::Input::KeyRoutedEventArgs const& args)
{
    if (args.Key() == Windows::System::VirtualKey::Enter && !_multiline)
    {
        onLostFocus(nullptr, args);
    }
    else if (args.Key() == Windows::System::VirtualKey::Tab)
    {
        onLostFocus(nullptr, args);
    }
}

void EditBoxWinRT::onGotFocus(Windows::Foundation::IInspectable const& sender,
                              Windows::UI::Xaml::RoutedEventArgs const& args)
{
    Concurrency::critical_section::scoped_lock lock(_critical_section);
    std::shared_ptr<ax::InputEvent> inputEvent(new UIEditBoxEvent(*this, winrt::hstring{}, _beginHandler));
    ax::GLViewImpl::sharedGLView()->QueueEvent(inputEvent);
    _isEditing = true;
}

void EditBoxWinRT::onLostFocus(Windows::Foundation::IInspectable const& sender,
                               Windows::UI::Xaml::RoutedEventArgs const& args)
{
    EditBoxDelegate::EditBoxEndAction action             = EditBoxDelegate::EditBoxEndAction::UNKNOWN;
    Windows::UI::Xaml::Input::KeyRoutedEventArgs keyArgs = args.try_as<Windows::UI::Xaml::Input::KeyRoutedEventArgs>();
    if (keyArgs)
    {
        if (keyArgs.Key() == Windows::System::VirtualKey::Enter && !_multiline)
        {
            action = EditBoxDelegate::EditBoxEndAction::RETURN;
        }
        else if (keyArgs.Key() == Windows::System::VirtualKey::Tab)
        {
            action = EditBoxDelegate::EditBoxEndAction::TAB_TO_NEXT;
        }
    }

    _isEditing = false;
    Concurrency::critical_section::scoped_lock lock(_critical_section);
    if (!_textBox)
        return;

    winrt::hstring text = L"";
    if (_password)
    {
        text = _textBox.as<PasswordBox>().Password();
        _textBox.as<PasswordBox>().PasswordChanged(_changeToken);
    }
    else
    {
        text = _textBox.as<TextBox>().Text();
        _textBox.as<TextBox>().TextChanged(_changeToken);
    }

    std::shared_ptr<ax::InputEvent> inputEvent(
        new UIEditBoxEndEvent(*this, text, static_cast<int>(action), _endHandler));
    ax::GLViewImpl::sharedGLView()->QueueEvent(inputEvent);

    _textBox.LostFocus(_unfocusToken);
    _textBox.GotFocus(_focusToken);
    _textBox.KeyDown(_keydownToken);
    closeKeyboard();
}

bool EditBoxWinRT::isEditing()
{
    return _isEditing;
}

void EditBoxWinRT::openKeyboard()
{
    m_dispatcher.get().RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, [this]() {
        removeTextBox();
        Canvas canvas = findXamlElement(m_panel.get(), CANVAS_XAML_NAME).as<Canvas>();

        if (_password)
        {
            _textBox = createPasswordBox();
        }
        else
        {
            _textBox = createTextBox();
        }

        // Position the text box
        canvas.SetLeft(_textBox, _rect.X);
        canvas.SetTop(_textBox, _rect.Y - XAML_TOP_PADDING);

        _setTexVerticalAlignment(_textBox);
        _setPadding(_textBox);

        // Finally, insert it into the XAML scene hierarchy and make the containing canvas visible
        canvas.Children().InsertAt(0, _textBox);
        canvas.Background(Media::SolidColorBrush());
        canvas.Visibility(Visibility::Visible);
        _keydownToken = _textBox.KeyDown({get_weak(), &ax::ui::EditBoxWinRT::onKeyDown});
        _focusToken   = _textBox.GotFocus({get_weak(), &ax::ui::EditBoxWinRT::onGotFocus});
        _unfocusToken = _textBox.LostFocus({get_weak(), &ax::ui::EditBoxWinRT::onLostFocus});

        _textBox.Focus(FocusState::Programmatic);
        if (_password)
        {
            _textBox.as<PasswordBox>().SelectAll();
        }
        else
        {
            _textBox.as<TextBox>().Select(_initialText.size(), 0);
        }

        auto inputPane = Windows::UI::ViewManagement::InputPane::GetForCurrentView();
    });
}

void EditBoxWinRT::setFontColor(Windows::UI::Color const& color)
{
    _color = color;
}

void EditBoxWinRT::setFontFamily(winrt::hstring const& fontFamily)
{
    _fontFamily = fontFamily;
}

void EditBoxWinRT::setFontSize(int fontSize)
{
    _fontSize = fontSize;
}

void EditBoxWinRT::removeTextBox()
{
    auto canvas = findXamlElement(m_panel.get(), CANVAS_XAML_NAME);
    auto box    = findXamlElement(canvas, EDIT_BOX_XAML_NAME);
    removeXamlElement(canvas, box);
    _isEditing = false;
}

void EditBoxWinRT::setInputFlag(int inputFlags)
{
    _password = false;
    switch ((EditBox::InputFlag)inputFlags)
    {
    case EditBox::InputFlag::PASSWORD:
        _password = true;
        break;
    default:
        AXLOGD("Warning: cannot set INITIAL_CAPS_* input flags for WinRT edit boxes");
    }
}

void EditBoxWinRT::setInputMode(int inputMode)
{
    _multiline = (EditBox::InputMode)inputMode == EditBox::InputMode::ANY;
    _inputMode = inputMode;
}

void EditBoxWinRT::setTextHorizontalAlignment(int alignment)
{
    _alignment = alignment;
}

void EditBoxWinRT::setMaxLength(int maxLength)
{
    _maxLength = maxLength;
}

void EditBoxWinRT::_setTextHorizontalAlignment(TextBox textBox)
{
    switch (_alignment)
    {
    default:
    case 0:
        textBox.TextAlignment(TextAlignment::Left);
        break;
    case 1:
        textBox.TextAlignment(TextAlignment::Center);
        break;
    case 2:
        textBox.TextAlignment(TextAlignment::Right);
        break;
    }
}

void EditBoxWinRT::_setTexVerticalAlignment(Windows::UI::Xaml::Controls::Control textBox)
{
    textBox.VerticalAlignment(_multiline ? VerticalAlignment::Top : VerticalAlignment::Center);
}

void EditBoxWinRT::_setPadding(Windows::UI::Xaml::Controls::Control editBox)
{
    double padding = EDIT_BOX_PADDING * ax::Director::getInstance()->getGLView()->getScaleX();
    if (_multiline)
    {
        editBox.Padding(Thickness{padding, padding, 0.0, 0.0});
    }
    else
    {
        editBox.Padding(Thickness{padding, 0.0, 0.0, 0.0});
    }
}

void EditBoxWinRT::setInputScope(TextBox textBox)
{
    InputScope inputScope = InputScope();
    InputScopeName name   = InputScopeName();

    switch ((EditBox::InputMode)_inputMode)
    {
    case EditBox::InputMode::SINGLE_LINE:
    case EditBox::InputMode::ANY:
        name.NameValue(InputScopeNameValue::Default);
        break;
    case EditBox::InputMode::EMAIL_ADDRESS:
        name.NameValue(InputScopeNameValue::EmailSmtpAddress);
        break;
    case EditBox::InputMode::DECIMAL:
    case EditBox::InputMode::NUMERIC:
        name.NameValue(InputScopeNameValue::Number);
        break;
    case EditBox::InputMode::PHONE_NUMBER:
        name.NameValue(InputScopeNameValue::TelephoneNumber);
        break;
    case EditBox::InputMode::URL:
        name.NameValue(InputScopeNameValue::Url);
        break;
    }

    textBox.InputScope(nullptr);
    inputScope.Names().Append(name);
    textBox.InputScope(inputScope);
}

void EditBoxWinRT::setPosition(Windows::Foundation::Rect const& rect)
{
    _rect = rect;
}

void EditBoxWinRT::setSize(Windows::Foundation::Size const& size)
{
    _size = size;
}

void EditBoxWinRT::setText(winrt::hstring const& text)
{
    _initialText = text;
    // If already editing
    if (_isEditing)
    {
        m_dispatcher.get().RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, [weak_this{get_weak()}]() {
            auto strong_this{weak_this.get()};
            if (!strong_this)
                return;

            if (!strong_this->_password)
            {
                auto textBox              = strong_this->_textBox.as<TextBox>();
                unsigned int currentStart = textBox.SelectionStart();
                bool cursorAtEnd          = currentStart == textBox.Text().size();
                textBox.Text(strong_this->_initialText);
                if (cursorAtEnd || currentStart > textBox.Text().size())
                {
                    currentStart = textBox.Text().size();
                }
                textBox.Select(currentStart, 0);
            }
        });
    }
}

void EditBoxWinRT::setVisible(bool visible)
{
    _visible = visible;
    // If already editing
    m_dispatcher.get().RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, [weak_this{get_weak()}]() {
        if (auto strong_this{weak_this.get()})
        {
            Canvas canvas = findXamlElement(strong_this->m_panel.get(), CANVAS_XAML_NAME).as<Canvas>();
            canvas.Visibility(strong_this->_visible ? Visibility::Visible : Visibility::Collapsed);
        }
    });
}

UIEditBoxImplWinrt::UIEditBoxImplWinrt(EditBox* pEditText) : EditBoxImplCommon(pEditText)
{
    auto beginHandler  = ([this](Windows::Foundation::IInspectable const& sender, winrt::hstring const& arg) {
        this->editBoxEditingDidBegin();
    });
    auto changeHandler = ([this](Windows::Foundation::IInspectable const& sender, winrt::hstring const& arg) {
        auto text = PlatformStringToString(arg);
        this->editBoxEditingChanged(text);
    });
    auto endHandler    = ([this](Windows::Foundation::IInspectable const& sender, ax::EndEventArgs const& arg) {
        auto text   = PlatformStringToString(arg.GetText());
        auto action = arg.GetAction();
        this->editBoxEditingDidEnd(text, static_cast<ax::ui::EditBoxDelegate::EditBoxEndAction>(action));
        this->onEndEditing(text);
    });

    _system_control_agile = winrt::make_agile(winrt::make<EditBoxWinRT>(beginHandler, changeHandler, endHandler));
    _system_control       = _system_control_agile.get().as<EditBoxWinRT>();
}

void UIEditBoxImplWinrt::setNativeFont(const char* pFontName, int fontSize)
{
    // fontSize
    _fontSize      = fontSize;
    auto transform = _editBox->getNodeToWorldTransform();
    ax::Vec3 scale;
    transform.getScale(&scale);
    _system_control->setFontSize(_fontSize * ax::Director::getInstance()->getGLView()->getScaleY() /** scale.y*/);

    // fontFamily
    auto font = ax::FontFreeType::create(pFontName, fontSize, ax::GlyphCollection::DYNAMIC, ""sv);
    if (font != nullptr)
    {
        std::string fontName = "ms-appx:///Content/" + std::string(pFontName) + '#' + font->getFontFamily();
        _system_control->setFontFamily(PlatformStringFromString(fontName));
    }
}

void UIEditBoxImplWinrt::setNativeFontColor(const Color4B& color)
{
    Windows::UI::Color win_color = {0xFF, color.r, color.g, color.b};
    _system_control->setFontColor(win_color);
}

void UIEditBoxImplWinrt::setNativeInputMode(EditBox::InputMode inputMode)
{
    _system_control->setInputMode((int)inputMode);
}

void UIEditBoxImplWinrt::setNativeInputFlag(EditBox::InputFlag inputFlag)
{
    _system_control->setInputFlag((int)inputFlag);
}

void UIEditBoxImplWinrt::setNativeTextHorizontalAlignment(ax::TextHAlignment alignment)
{
    _system_control->setTextHorizontalAlignment((int)alignment);
}

void UIEditBoxImplWinrt::setNativeText(const char* pText)
{
    _system_control->setText(PlatformStringFromString(pText));
}

void UIEditBoxImplWinrt::setNativeVisible(bool visible)
{
    _system_control->setVisible(visible);
}

void UIEditBoxImplWinrt::updateNativeFrame(const Rect& rect) {}

void UIEditBoxImplWinrt::nativeOpenKeyboard()
{
    // Update the text
    _system_control->setText(PlatformStringFromString(getText()));
    // Size
    auto glView    = ax::Director::getInstance()->getGLView();
    auto transform = _editBox->getNodeToWorldTransform();
    ax::Vec3 scale;
    transform.getScale(&scale);
    Windows::Foundation::Size xamlSize = {_editBox->getContentSize().width * glView->getScaleX() * scale.x,
                                          _editBox->getContentSize().height * glView->getScaleY() * scale.y};
    _system_control->setSize(xamlSize);
    _system_control->setFontSize(_fontSize * ax::Director::getInstance()->getGLView()->getScaleY() /** scale.y*/);
    // Position
    auto directorInstance = ax::Director::getInstance();
    auto frameSize        = glView->getFrameSize();
    auto winSize          = directorInstance->getWinSize();
    auto leftBottom       = _editBox->convertToWorldSpace(ax::Point::ZERO);
    auto rightTop =
        _editBox->convertToWorldSpace(ax::Point(_editBox->getContentSize().width, _editBox->getContentSize().height));
    Windows::Foundation::Rect rect;
    rect.X      = frameSize.width / 2 + (leftBottom.x - winSize.width / 2) * glView->getScaleX();
    rect.Y      = frameSize.height / 2 - (rightTop.y - winSize.height / 2) * glView->getScaleY();
    rect.Width  = (rightTop.x - leftBottom.x) * glView->getScaleX();
    rect.Height = (rightTop.y - leftBottom.y) * glView->getScaleY();
    _system_control->setPosition(rect);
    // .. and open
    _system_control->openKeyboard();
}

void UIEditBoxImplWinrt::nativeCloseKeyboard()
{
    _system_control->closeKeyboard();
}

void UIEditBoxImplWinrt::setNativeMaxLength(int maxLength)
{
    _system_control->setMaxLength(maxLength);
}

ax::Vec2 UIEditBoxImplWinrt::convertDesignCoordToXamlCoord(const ax::Vec2& designCoord)
{
    auto glView      = ax::Director::getInstance()->getGLView();
    float viewH      = glView->getFrameSize().height;
    Vec2 visiblePos  = Vec2(designCoord.x * glView->getScaleX(), designCoord.y * glView->getScaleY());
    Vec2 screenGLPos = visiblePos + glView->getViewPortRect().origin;
    Vec2 xamlPos(screenGLPos.x, viewH - screenGLPos.y);
    return xamlPos;
}

}  // namespace ui

}  // namespace ax

#endif // (CC_TARGET_PLATFORM == CC_PLATFORM_WP8 || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
