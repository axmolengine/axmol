/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2015 hanxi
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/platform/PlatformConfig.h"

#if (AX_TARGET_PLATFORM == AX_PLATFORM_LINUX)

#    include "axmol/ui/EditBox/EditBoxImpl-common.h"

namespace ax
{

class Label;

namespace ui
{

class EditBox;

class EditBoxImplLinux : public EditBoxImplCommon
{
public:
    /**
     */
    EditBoxImplLinux(EditBox* pEditText);
    /**
     * @lua NA
     */
    virtual ~EditBoxImplLinux();

    bool isEditing() override;
    void createNativeControl() override {};
    void setNativeFont(std::string_view fontName, int fontSize) override {};
    void setNativeFontColor(const Color32& color) override {};
    void setNativePlaceholderFont(std::string_view fontName, int fontSize) override {};
    void setNativePlaceholderFontColor(const Color32& color) override {};
    void setNativeInputMode(EditBox::InputMode inputMode) override {};
    void setNativeInputFlag(EditBox::InputFlag inputFlag) override {};
    void setNativeReturnType(EditBox::KeyboardReturnType returnType) override {};
    virtual void setNativeTextHorizontalAlignment(ax::TextHAlignment alignment) {};
    void setNativeText(std::string_view text) override {};
    void setNativePlaceHolder(std::string_view text) override {};
    void setNativeVisible(bool visible) override {};
    void updateNativeFrame(const Rect& rect) override;
    void pollEvents() override;
    std::string_view getNativeDefaultFontName() override { return ""sv; };
    void nativeOpenKeyboard() override;
    void nativeCloseKeyboard() override {};
    void setNativeMaxLength(int maxLength) override {};

    bool isDialogRunning(void* dlg) { return dlg != nullptr && dlg == _inputDialog; }

    void nativeDalogShow(void* dlg, void* entry);
    void nativeDialogEnd();

    void* getNativeInputEntry() const { return _inputEntry; }
    std::string_view getInputInitialValue() const { return _inputInitialValue; }

private:
    void doAnimationWhenKeyboardMove(float duration, float distance) override {}
    void* _inputDialog{nullptr};  // The GTKDialog
    void* _inputEntry{nullptr};   // the GTKEntry
    std::string _inputInitialValue;
};

}  // namespace ui

}  // namespace ax

#endif /* #if (AX_TARGET_PLATFORM == AX_PLATFORM_LINUX) */
