/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2012 James Chen
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/platform/PlatformConfig.h"

#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)

#    include "axmol/ui/EditBox/EditBoxImpl-common.h"

namespace ax
{

class Label;

namespace ui
{

class EditBox;

class EditBoxImplAndroid : public EditBoxImplCommon
{
public:
    /**
     */
    EditBoxImplAndroid(EditBox* pEditText);
    /**
     * @lua NA
     */
    virtual ~EditBoxImplAndroid();

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
    int _editBoxIndex;
};

}  // namespace ui

}  // namespace ax

#endif /* #if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID) */
