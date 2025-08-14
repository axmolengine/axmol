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

#pragma once

#include "platform/PlatformConfig.h"
#include "2d/Label.h"
#include "ui/UIEditBox/UIEditBoxImpl-common.h"
#include "ui/UIEditBox/UIEditBoxImpl.h"

namespace ax
{

namespace ui
{

class EditBox;

class AX_GUI_DLL EditBoxImplCommon : public EditBoxImpl
{
public:
    /**
     */
    EditBoxImplCommon(EditBox* pEditText);
    /**
     * @lua NA
     */
    virtual ~EditBoxImplCommon();

    bool initWithSize(const Size& size) override;

    void setFont(const char* pFontName, int fontSize) override;
    void setFontColor(const Color32& color) override;
    void setPlaceholderFont(const char* pFontName, int fontSize) override;
    void setPlaceholderFontColor(const Color32& color) override;
    void setInputMode(EditBox::InputMode inputMode) override;
    void setInputFlag(EditBox::InputFlag inputFlag) override;
    void setReturnType(EditBox::KeyboardReturnType returnType) override;
    void setText(const char* pText) override;
    void setPlaceHolder(const char* pText) override;
    void setVisible(bool visible) override;

    void setMaxLength(int maxLength) override;
    void setTextHorizontalAlignment(TextHAlignment alignment) override;

    int getMaxLength() override { return _maxLength; }
    const char* getText() override { return _text.c_str(); }
    const char* getPlaceHolder() override { return _placeHolder.c_str(); }

    const char* getFontName() override { return _fontName.c_str(); }
    int getFontSize() override { return _fontSize; }
    const Color32& getFontColor() override { return _colText; }

    const char* getPlaceholderFontName() override { return _placeholderFontName.c_str(); }
    int getPlaceholderFontSize() override { return _placeholderFontSize; }
    const Color32& getPlaceholderFontColor() override { return _colPlaceHolder; }

    EditBox::InputMode getInputMode() override { return _editBoxInputMode; }
    EditBox::InputFlag getInputFlag() override { return _editBoxInputFlag; }
    EditBox::KeyboardReturnType getReturnType() override { return _keyboardReturnType; }
    TextHAlignment getTextHorizontalAlignment() override { return _alignment; }

    virtual void refreshInactiveText();

    void setContentSize(const Size& size) override;

    void setAnchorPoint(const Vec2& anchorPoint) override {}
    void setPosition(const Vec2& pos) override {}
    void setGlobalZOrder(float globalZOrder) override;

    /**
     * @lua NA
     */
    void draw(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags) override;
    /**
     * @lua NA
     */
    void onEnter() override;
    void openKeyboard() override;
    void closeKeyboard() override;

    virtual void onEndEditing(std::string_view text);

    void editBoxEditingDidBegin();
    void editBoxEditingChanged(std::string_view text);
    void editBoxEditingDidEnd(std::string_view text,
                              EditBoxDelegate::EditBoxEndAction action = EditBoxDelegate::EditBoxEndAction::UNKNOWN);

    bool isEditing() override                                                = 0;
    virtual void createNativeControl(const Rect& frame)                              = 0;
    virtual void setNativeFont(const char* pFontName, int fontSize)                  = 0;
    virtual void setNativeFontColor(const Color32& color)                            = 0;
    virtual void setNativePlaceholderFont(const char* pFontName, int fontSize)       = 0;
    virtual void setNativePlaceholderFontColor(const Color32& color)                 = 0;
    virtual void setNativeInputMode(EditBox::InputMode inputMode)                    = 0;
    virtual void setNativeInputFlag(EditBox::InputFlag inputFlag)                    = 0;
    virtual void setNativeReturnType(EditBox::KeyboardReturnType returnType)         = 0;
    virtual void setNativeTextHorizontalAlignment(ax::TextHAlignment alignment) = 0;
    virtual void setNativeText(const char* pText)                                    = 0;
    virtual void setNativePlaceHolder(const char* pText)                             = 0;
    virtual void setNativeVisible(bool visible)                                      = 0;
    virtual void updateNativeFrame(const Rect& rect)                                 = 0;
    virtual const char* getNativeDefaultFontName()                                   = 0;
    virtual void nativeOpenKeyboard()                                                = 0;
    virtual void nativeCloseKeyboard()                                               = 0;
    virtual void setNativeMaxLength(int maxLength){};

protected:
    void initInactiveLabels(const Size& size);
    void setInactiveText(const char* pText);
    void refreshLabelAlignment();
    void placeInactiveLabels(const Size& size);
    void doAnimationWhenKeyboardMove(float duration, float distance) override{};

    Label* _label;
    Label* _labelPlaceHolder;
    EditBox::InputMode _editBoxInputMode;
    EditBox::InputFlag _editBoxInputFlag;
    EditBox::KeyboardReturnType _keyboardReturnType;
    TextHAlignment _alignment;

    std::string _text;
    std::string _placeHolder;

    std::string _fontName;
    std::string _placeholderFontName;

    int _fontSize;
    int _placeholderFontSize;

    Color32 _colText;
    Color32 _colPlaceHolder;

    int _maxLength;
    Size _contentSize;
    bool _editingMode;
};

}  // namespace ui

}

