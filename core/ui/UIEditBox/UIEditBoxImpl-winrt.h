/****************************************************************************
Copyright (c) 2014 cocos2d-x.org
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

#ifndef __UIEditBoxIMPLWINRT_H__
#define __UIEditBoxIMPLWINRT_H__

#include "platform/PlatformConfig.h"
#if AX_TARGET_PLATFORM == AX_PLATFORM_WINRT
#include "ui/UIEditBox/UIEditBoxImpl-common.h"

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;

NS_AX_BEGIN

namespace ui {

  class EditBoxWinRT : public winrt::implements <EditBoxWinRT, Windows::Foundation::IInspectable>
  {
  public:
      EditBoxWinRT(winrt::delegate<Windows::Foundation::IInspectable const&, winrt::hstring const&> const& beginHandler,
          winrt::delegate<Windows::Foundation::IInspectable const&, winrt::hstring const&> const& changeHandler,
          winrt::delegate<Windows::Foundation::IInspectable const&, ax::EndEventArgs const&> const& endHandler);

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
    void setPosition(Windows::Foundation::Rect const& rect);
    void setSize(Windows::Foundation::Size const& size);
    void setText(winrt::hstring const& text);
    void setVisible(bool visible);

  private:

    void onPasswordChanged(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
    void onTextChanged(Windows::Foundation::IInspectable const& sender,
                                       Windows::UI::Xaml::Controls::TextChangedEventArgs const& e);
    void onKeyDown(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs const& args);
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
    Windows::Foundation::Rect _rect;
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

    winrt::agile_ref<Windows::UI::Core::CoreDispatcher> m_dispatcher = nullptr;
    winrt::agile_ref<Windows::UI::Xaml::Controls::Panel> m_panel = nullptr;
    Concurrency::critical_section _critical_section;

    winrt::delegate<Windows::Foundation::IInspectable const&, winrt::hstring const&> _beginHandler = nullptr;
    winrt::delegate<Windows::Foundation::IInspectable const&, winrt::hstring const&> _changeHandler = nullptr;
    winrt::delegate<Windows::Foundation::IInspectable const&, EndEventArgs const&> _endHandler     = nullptr;

    winrt::event_token _unfocusToken;
    winrt::event_token _changeToken;
    winrt::event_token _focusToken;
    winrt::event_token _keydownToken;
  };

  class AX_GUI_DLL UIEditBoxImplWinrt : public EditBoxImplCommon
  {
  public:
    /**
    * @js NA
    */
    UIEditBoxImplWinrt(EditBox* pEditText);

    /**
    * @js NA
    * @lua NA
    */
    virtual ~UIEditBoxImplWinrt() { };

    virtual bool isEditing() override { return _system_control.get()->isEditing(); }
    virtual void createNativeControl(const Rect& frame) override {  }
    virtual void setNativeFont(const char* pFontName, int fontSize) override;
    virtual void setNativeFontColor(const Color4B& color) override;
    virtual void setNativePlaceholderFont(const char* pFontName, int fontSize) override { AXLOGD("Warning! You can't change WinRT placeholder font"); }
    virtual void setNativePlaceholderFontColor(const Color4B& color) override { AXLOGD("Warning! You can't change WinRT placeholder font color"); }
    virtual void setNativeInputMode(EditBox::InputMode inputMode) override;
    virtual void setNativeInputFlag(EditBox::InputFlag inputFlag) override;
    virtual void setNativeReturnType(EditBox::KeyboardReturnType returnType) override { AXLOGD("Warning! You can't change WinRT return type"); }
    virtual void setNativeTextHorizontalAlignment(ax::TextHAlignment alignment);
    virtual void setNativeText(const char* pText) override;
    virtual void setNativePlaceHolder(const char* pText) override { AXLOGD("Warning! You can't change WinRT placeholder text"); }
    virtual void setNativeVisible(bool visible) override;
    virtual void updateNativeFrame(const Rect& rect) override; // TODO
    virtual const char* getNativeDefaultFontName() override { return "Segoe UI"; }
    virtual void nativeOpenKeyboard();
    virtual void nativeCloseKeyboard() override;
    virtual void setNativeMaxLength(int maxLength) override;

  private:
    ax::Vec2 convertDesignCoordToXamlCoord(const ax::Vec2& designCoord);
    virtual void doAnimationWhenKeyboardMove(float duration, float distance) override { AXLOGD("Warning! doAnimationWhenKeyboardMove not supported on WinRT"); }

    winrt::agile_ref<Windows::Foundation::IInspectable> _system_control_agile;
    winrt::com_ptr<EditBoxWinRT> _system_control{};
    int _fontSize;
  };

} // namespace ui

NS_AX_END

#endif // CC_PLATFORM_WINRT

#endif // #ifdef __UIEditBoxIMPLWINRT_H__

