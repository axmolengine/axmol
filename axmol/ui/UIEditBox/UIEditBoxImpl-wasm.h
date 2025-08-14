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

#include "platform/PlatformConfig.h"

#if AX_TARGET_PLATFORM == AX_PLATFORM_WASM
#    include "ui/UIEditBox/UIEditBoxImpl-common.h"

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
    void createNativeControl(const Rect& frame) override;
    void setNativeFont(const char* pFontName, int fontSize) override;
    void setNativeFontColor(const Color32& color) override;
    void setNativePlaceholderFont(const char* pFontName, int fontSize) override;
    void setNativePlaceholderFontColor(const Color32& color) override;
    void setNativeInputMode(EditBox::InputMode inputMode) override;
    void setNativeInputFlag(EditBox::InputFlag inputFlag) override;
    void setNativeReturnType(EditBox::KeyboardReturnType returnType) override;
    void setNativeTextHorizontalAlignment(TextHAlignment alignment) override;
    void setNativeText(const char* pText) override;
    void setNativePlaceHolder(const char* pText) override;
    void setNativeVisible(bool visible) override;
    void updateNativeFrame(const Rect& rect) override;
    const char* getNativeDefaultFontName() override;
    void nativeOpenKeyboard() override;
    void nativeCloseKeyboard() override;
    void setNativeMaxLength(int maxLength) override;
    //EditBoxDelegate::EditBoxEndAction _endAction;
    bool isEditingMode() const { return _editingMode; }
private:
    void createEditCtrl(EditBox::InputMode inputMode);

    static bool s_isInitialized;
    static int s_editboxChildID;
    static void lazyInit();
};

}  // namespace ui

}

#endif /* (AX_TARGET_PLATFORM == AX_PLATFORM_WASM) */

