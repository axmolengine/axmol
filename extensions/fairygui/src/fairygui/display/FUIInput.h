#ifndef __FUIINPUT_H__
#define __FUIINPUT_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "ui/UIEditBox/UIEditBox.h"
#include "TextFormat.h"

NS_FGUI_BEGIN

class FUIInput : public ax::ui::EditBox
{
public:
    static FUIInput* create();

    FUIInput();
    virtual ~FUIInput();
    
    std::string getText() const;
    void setText(const std::string& value);

    TextFormat* getTextFormat() const { return _textFormat; }
    void applyTextFormat();

    bool isSingleLine() const;
    void setSingleLine(bool value);

    bool isPassword() const { return _password; }
    void setPassword(bool value);

    int keyboardType() const { return _keyboardType;  }
    void setKeyboardType(int value);

    void openKeyboard();

private:
    void continueInit();

    void _touchDownAction(ax::Object *sender, ax::ui::Widget::TouchEventType controlEvent);

    TextFormat* _textFormat;
    bool _password;
    int _keyboardType;
};

NS_FGUI_END

#endif
