/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2012 Jozef Pridavok
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/platform/PlatformConfig.h"

#if (AX_TARGET_PLATFORM == AX_PLATFORM_MAC)

#    include "axmol/ui/EditBox/EditBoxImpl-common.h"

@class UIEditBoxImplMac;
@class NSFont;

namespace ax
{

namespace ui
{

class EditBox;

class EditBoxImplMac : public EditBoxImplCommon
{
public:
    /**
     */
    EditBoxImplMac(EditBox* pEditText);
    /**
     * @lua NA
     */
    virtual ~EditBoxImplMac();

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
    void setNativeMaxLength(int maxLength) override;

private:
    NSFont* createNativeFont(std::string_view fontName, int fontSize);

    UIEditBoxImplMac* _sysEdit;
};

}  // namespace ui

}  // namespace ax

#endif  // #if (AX_TARGET_PLATFORM == AX_PLATFORM_MAC)
