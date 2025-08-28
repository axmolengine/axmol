/****************************************************************************
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

#pragma once

#include "axmol/ui/UIWidget.h"
#include "axmol/base/IMEDelegate.h"
#include "axmol/base/SimpleTimer.h"
#include "axmol/2d/Label.h"
#include "axmol/base/EventListenerKeyboard.h"

namespace ax
{

namespace ui
{

/**
@brief  The ui::TextFieldEx, better design, better cursor support than ui::TextField
will replace ui::TextField, currently, ui::TextField, 2d/TextFieldTTF were maked as deprecated
*/
class AX_DLL TextFieldEx : public Widget, public IMEDelegate
{
public:
    /**
     */
    TextFieldEx();
    /**
     * @lua NA
     */
    virtual ~TextFieldEx();

    static TextFieldEx* create(std::string_view placeholder,
                               std::string_view fontName,
                               float fontSize,
                               float cursorWidth    = 2,
                               const Color32& color = Color32::WHITE);

    bool initWithPlaceHolder(std::string_view placeholder,
                             std::string_view fontName,
                             float fontSize,
                             float cursorWidth    = 2,
                             const Color32& color = Color32::WHITE);

    void enableIME(Node* control);
    void disableIME(void);

    Label* getRenderLabel();

    inline int getCharCount() const { return static_cast<int>(_charCount); };

    virtual void setPlaceholderColor(const Color32& color);
    virtual const Color32& getPlaceholderColor() const;

    virtual void setTextColor(const Color32& textColor);
    virtual const Color32& getTextColor(void) const;

    void setCursorColor(const Color32& color);
    const Color32& getCursorColor(void) const;

    // input text property
    virtual void setString(std::string_view text);
    virtual std::string_view getString() const;

    // Continuous touch event trigger support.
    void setContinuousTouchDelayTime(float delay) { _continuousTouchDelayTime = delay; }
    float getContinuousTouchDelayTime() const { return _continuousTouchDelayTime; }
    void setContinuousTouchCallback(std::function<void(const Point& worldPoint)> callback)
    {
        _continuousTouchCallback = std::move(callback);
    }

    // place holder text property
    // place holder text displayed when there is no text in the text field.
    virtual void setPlaceholderText(std::string_view text);
    virtual std::string_view getPlaceholderText(void) const;

    virtual void setPasswordEnabled(bool value);
    virtual bool isPasswordEnabled() const;

    bool empty(void) const { return _charCount == 0 || _inputText.empty(); }

    void setEnabled(bool bEnabled) override;

    void setEditable(bool bEditable) { _editable = bEditable; }
    bool isEditable(void) const { return _editable; }

    void setMaxLength(int maxLength) { setCharLimit(maxLength); }

    int getFontType() const;

    /// fonts
    void setTextFontSize(float size);
    float getTextFontSize() const;
    void setTextFontName(std::string_view fontName);
    std::string_view getTextFontName() const;

    AX_SYNTHESIZE(size_t, _charLimit, CharLimit);

    bool isSystemFont(void) const { return _systemFontUsed; }

public:
    std::function<void(void)> onTextModify;
    std::function<void(void)> onOpenIME;
    std::function<void(void)> onCloseIME;
    // IMEDelegate interface
    //////////////////////////////////////////////////////////////////////////
    void openIME(void);
    void closeIME(void);

    void insertText(const char* text, size_t len) override;

protected:
    //////////////////////////////////////////////////////////////////////////

    bool canAttachWithIME() override;
    bool canDetachWithIME() override;

    void deleteBackward(size_t numChars) override;
    std::string_view getContentText() override;

    void handleDeleteKeyEvent();

    /**
    @brief    Open keyboard and receive input text.
    */
    bool attachWithIME() override;

    /**
    @brief    End text input and close keyboard.
    */
    bool detachWithIME() override;

    void keyboardDidShow(IMEKeyboardNotificationInfo& /*info*/) override;
    void keyboardDidHide(IMEKeyboardNotificationInfo& /*info*/) override;

    void updateContentSize(void);

    void __initCursor(int height, int width = 6, const Color32& color = Color32::WHITE);
    void __showCursor(void);
    void __hideCursor(void);
    void __updateCursorPosition(void);

    void __moveCursor(int direction);

    void __moveCursorTo(float x);

protected:
    bool _systemFontUsed;
    std::string _fontName;
    float _fontSize;

    bool _editable;

    Label* _renderLabel;

    size_t _charCount;
    std::string _inputText;

    std::string _placeHolder;
    Color32 _colorSpaceHolder;
    Color32 _colorText;

    bool _secureTextEntry;

    Sprite* _cursor;
    bool _cursorVisible;

    int _insertPosUtf8;
    int _insertPos;  // The actual input content insertPos, step: bytes
    int _cursorPos;  // The cursor normalzed pos,

    EventListenerTouchOneByOne* _touchListener;
    EventListenerKeyboard* _kbdListener;

    bool _touchCursorControlEnabled;
    float _asteriskWidth;

    int _fontType;

    ax::stimer::TIMER_ID _continuousTouchDelayTimerID;
    float _continuousTouchDelayTime;
    std::function<void(const Point& worldPoint)> _continuousTouchCallback;

    static bool s_keyboardVisible;
};

// end of input group
/// @}

};  // namespace ui

}  // namespace ax
