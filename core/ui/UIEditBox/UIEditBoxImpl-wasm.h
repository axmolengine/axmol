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

#ifndef __UIEditBoxIMPLWasm_H__
#define __UIEditBoxIMPLWasm_H__

#include "platform/PlatformConfig.h"

#if AX_TARGET_PLATFORM == AX_PLATFORM_WASM
#    include "ui/UIEditBox/UIEditBoxImpl-common.h"

NS_AX_BEGIN

namespace ui
{

class EditBox;

class AX_API EditBoxImplWasm : public EditBoxImplCommon
{
public:
    EditBoxImplWasm(EditBox* pEditText);
    virtual ~EditBoxImplWasm();

    virtual bool isEditing() override;
    virtual void createNativeControl(const Rect& frame) override;
    virtual void setNativeFont(const char* pFontName, int fontSize) override;
    virtual void setNativeFontColor(const Color4B& color) override;
    virtual void setNativePlaceholderFont(const char* pFontName, int fontSize) override;
    virtual void setNativePlaceholderFontColor(const Color4B& color) override;
    virtual void setNativeInputMode(EditBox::InputMode inputMode) override;
    virtual void setNativeInputFlag(EditBox::InputFlag inputFlag) override;
    virtual void setNativeReturnType(EditBox::KeyboardReturnType returnType) override;
    virtual void setNativeTextHorizontalAlignment(TextHAlignment alignment) override;
    virtual void setNativeText(const char* pText) override;
    virtual void setNativePlaceHolder(const char* pText) override;
    virtual void setNativeVisible(bool visible) override;
    virtual void updateNativeFrame(const Rect& rect) override;
    virtual const char* getNativeDefaultFontName() override;
    virtual void nativeOpenKeyboard() override;
    virtual void nativeCloseKeyboard() override;
    virtual void setNativeMaxLength(int maxLength) override;
    //EditBoxDelegate::EditBoxEndAction _endAction;
    bool isEditingMode() const { return _editingMode; }
private:
    void createEditCtrl(EditBox::InputMode inputMode);
    
    static bool s_isInitialized;
    static int s_editboxChildID;
    static void lazyInit();
};

}  // namespace ui

NS_AX_END

#endif /* (AX_TARGET_PLATFORM == AX_PLATFORM_WASM) */

#endif /* __UIEditBoxIMPLWasm_H__ */
