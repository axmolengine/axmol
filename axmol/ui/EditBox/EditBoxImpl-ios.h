/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2012 James Chen
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS)

#    include "axmol/ui/EditBox/EditBoxImpl-common.h"

@class UIEditBoxImplIOS_objc;
@class UIFont;

namespace ax
{

namespace ui
{

class EditBox;

class EditBoxImplIOS : public EditBoxImplCommon
{
public:
    /**
     */
    EditBoxImplIOS(EditBox* pEditText);
    /**
     * @lua NA
     */
    virtual ~EditBoxImplIOS();

    bool isEditing() override;
    void createNativeControl() override;
    void setNativeFont(std::string_view fontName, int fontSize) override;
    void setNativeFontColor(const Color32& color) override;
    void setNativePlaceholderFont(std::string_view fontName, int fontSize) override;
    void setNativePlaceholderFontColor(const Color32& color) override;
    void setNativeInputMode(EditBox::InputMode inputMode) override;
    void setNativeInputFlag(EditBox::InputFlag inputFlag) override;
    void setNativeReturnType(EditBox::KeyboardReturnType returnType) override;
    void setNativeTextHorizontalAlignment(ax::TextHAlignment alignment) override;
    void setNativeText(std::string_view text) override;
    void setNativePlaceHolder(std::string_view text) override;
    void setNativeVisible(bool visible) override;
    void updateNativeFrame(const Rect& rect) override;
    std::string_view getNativeDefaultFontName() override;
    void nativeOpenKeyboard() override;
    void nativeCloseKeyboard() override;

    // need to remove siri text
    std::string_view getText() override;

    void doAnimationWhenKeyboardMove(float duration, float distance) override;

private:
    UIFont* createNativeFont(std::string_view fontName, int fontSize);

    UIEditBoxImplIOS_objc* _systemControl;
};

}  // namespace ui

}  // namespace ax

#endif /* #if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS) */
