/****************************************************************************
Copyright (c) 2014 cocos2d-x.org
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

#include "axmol/platform/PlatformConfig.h"
#if AX_TARGET_PLATFORM == AX_PLATFORM_WINRT
#    include "axmol/ui/EditBox/EditBoxImpl-common.h"

#    include <winrt/Windows.Foundation.h>
#    include <winrt/Windows.UI.Xaml.h>
#    include <winrt/Windows.UI.Xaml.Controls.h>

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;

namespace ax
{

namespace ui
{

class EditBoxWinRT : public winrt::implements<EditBoxWinRT, Windows::Foundation::IInspectable>
{
public:
    EditBoxWinRT(winrt::delegate<Windows::Foundation::IInspectable const&, winrt::hstring const&> const& beginHandler,
                 winrt::delegate<Windows::Foundation::IInspectable const&, winrt::hstring const&> const& changeHandler,
                 winrt::delegate<Windows::Foundation::IInspectable const&,
                                 winrt::hstring const&,
                                 EditBoxDelegate::EditBoxEndAction> const& endHandler);

    void closeKeyboard();
    bool isEditing();
    void openKeyboard();
    void setFontColor(Windows::UI::Color const& color);
    void setFontFamily(winrt::hstring const& fontFamily);
    void setFontSize(int fontSize);
    void setInputFlag(int inputFlags);
    void setInputMode(int inputMode);
    void setTextHorizontalAlignment(int alignment);
    void setMaxLength(int maxLength);
    void setPosition(float x, float y);
    void setSize(float width, float height);
    void setText(winrt::hstring const& text);
    void setVisible(bool visible);

    void applyRect();

private:
    void onPasswordChanged(Windows::Foundation::IInspectable const& sender,
                           Windows::UI::Xaml::RoutedEventArgs const& args);
    void onTextChanged(Windows::Foundation::IInspectable const& sender,
                       Windows::UI::Xaml::Controls::TextChangedEventArgs const& e);
    void onKeyDown(Windows::Foundation::IInspectable const& sender,
                   Windows::UI::Xaml::Input::KeyRoutedEventArgs const& args);
    void onGotFocus(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
    void onLostFocus(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);

    Windows::UI::Xaml::Controls::Control createPasswordBox();
    Windows::UI::Xaml::Controls::Control createTextBox();
    void removeTextBox();
    void setInputScope(TextBox textBox);
    void _setTextHorizontalAlignment(TextBox textBox);
    void _setTexVerticalAlignment(Windows::UI::Xaml::Controls::Control textBox);
    void _setPadding(Windows::UI::Xaml::Controls::Control editBox);

    // Properties
    Windows::Foundation::Point _position;
    Windows::Foundation::Size _size;
    Windows::UI::Color _color{0xFF, 0xFF, 0xFF, 0xFF};
    winrt::hstring _initialText;
    int _fontSize;
    winrt::hstring _fontFamily;
    int _alignment;
    int _inputMode;
    int _inputFlag;
    bool _password;
    bool _isEditing;
    bool _visible;
    bool _multiline;
    int _maxLength;

    // The actual edit box, however, could be a TextBox, PasswordBox, or a SearchBox (not yet implemented)
    Windows::UI::Xaml::Controls::Control _textBox = nullptr;

    winrt::agile_ref<Windows::UI::Core::CoreDispatcher> m_dispatcher      = nullptr;
    winrt::agile_ref<Windows::UI::Xaml::Controls::SwapChainPanel> m_panel = nullptr;
    Concurrency::critical_section _critical_section;

    winrt::delegate<Windows::Foundation::IInspectable const&, winrt::hstring const&> _beginHandler  = nullptr;
    winrt::delegate<Windows::Foundation::IInspectable const&, winrt::hstring const&> _changeHandler = nullptr;
    winrt::delegate<Windows::Foundation::IInspectable const&, winrt::hstring const&, EditBoxDelegate::EditBoxEndAction>
        _endHandler = nullptr;

    winrt::event_token _unfocusToken;
    winrt::event_token _changeToken;
    winrt::event_token _focusToken;
    winrt::event_token _keydownToken;
};

class AX_GUI_DLL UIEditBoxImplWinrt : public EditBoxImplCommon
{
public:
    /**
     */
    UIEditBoxImplWinrt(EditBox* pEditText);

    /**
     * @lua NA
     */
    virtual ~UIEditBoxImplWinrt() {};

    bool isEditing() override { return _system_control.get()->isEditing(); }
    void createNativeControl() override {}
    void setNativeFont(std::string_view fontName, int fontSize) override;
    void setNativeFontColor(const Color32& color) override;
    void setNativePlaceholderFont(std::string_view fontName, int fontSize) override
    {
        AXLOGD("Warning! You can't change WinRT placeholder font");
    }
    void setNativePlaceholderFontColor(const Color32& color) override
    {
        AXLOGD("Warning! You can't change WinRT placeholder font color");
    }
    void setNativeInputMode(EditBox::InputMode inputMode) override;
    void setNativeInputFlag(EditBox::InputFlag inputFlag) override;
    void setNativeReturnType(EditBox::KeyboardReturnType returnType) override
    {
        AXLOGD("Warning! You can't change WinRT return type");
    }
    virtual void setNativeTextHorizontalAlignment(ax::TextHAlignment alignment);
    void setNativeText(std::string_view text) override;
    void setNativePlaceHolder(std::string_view text) override
    {
        AXLOGD("Warning! You can't change WinRT placeholder text");
    }
    void setNativeVisible(bool visible) override;
    void updateNativeFrame(const Rect& rect) override;  // TODO
    std::string_view getNativeDefaultFontName() override { return "Segoe UI"; }
    virtual void nativeOpenKeyboard();
    void nativeCloseKeyboard() override;
    void setNativeMaxLength(int maxLength) override;

private:
    ax::Vec2 convertDesignCoordToXamlCoord(const ax::Vec2& designCoord);
    void doAnimationWhenKeyboardMove(float duration, float distance) override
    {
        AXLOGD("Warning! doAnimationWhenKeyboardMove not supported on WinRT");
    }

    winrt::agile_ref<Windows::Foundation::IInspectable> _system_control_agile;
    winrt::com_ptr<EditBoxWinRT> _system_control{};
    int _fontSize;
};

}  // namespace ui

}  // namespace ax

#endif  // AX_PLATFORM_WINRT
