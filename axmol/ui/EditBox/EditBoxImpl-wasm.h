/****************************************************************************

 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

#include "axmol/platform/PlatformConfig.h"

#if AX_TARGET_PLATFORM == AX_PLATFORM_WASM
#    include "axmol/ui/EditBox/EditBoxImpl-common.h"

namespace ax
{

namespace ui
{

class EditBox;

class AX_API EditBoxImplWasm : public EditBoxImplCommon
{
public:
    EditBoxImplWasm(EditBox* pEditText);
    virtual ~EditBoxImplWasm();

    bool isEditing() override;
    void createNativeControl() override;
    void setNativeFont(std::string_view fontName, int fontSize) override;
    void setNativeFontColor(const Color32& color) override;
    void setNativePlaceholderFont(std::string_view fontName, int fontSize) override;
    void setNativePlaceholderFontColor(const Color32& color) override;
    void setNativeInputMode(EditBox::InputMode inputMode) override;
    void setNativeInputFlag(EditBox::InputFlag inputFlag) override;
    void setNativeReturnType(EditBox::KeyboardReturnType returnType) override;
    void setNativeTextHorizontalAlignment(TextHAlignment alignment) override;
    void setNativeText(std::string_view text) override;
    void setNativePlaceHolder(std::string_view text) override;
    void setNativeVisible(bool visible) override;
    void updateNativeFrame(const Rect& rect) override;
    std::string_view getNativeDefaultFontName() override;
    void nativeOpenKeyboard() override;
    void nativeCloseKeyboard() override;
    void setNativeMaxLength(int maxLength) override;
    // EditBoxDelegate::EditBoxEndAction _endAction;
    bool isEditingMode() const { return _editingMode; }

private:
    void createEditCtrl(EditBox::InputMode inputMode);

    static bool s_isInitialized;
    static int s_editboxChildID;
    static void lazyInit();
};

}  // namespace ui

}  // namespace ax

#endif /* (AX_TARGET_PLATFORM == AX_PLATFORM_WASM) */
