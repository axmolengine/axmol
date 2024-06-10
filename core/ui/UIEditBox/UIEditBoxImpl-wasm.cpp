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

// Implemented features:
//  [X] single line only based on html <input />

****************************************************************************/

#include "ui/UIEditBox/UIEditBoxImpl-wasm.h"

#if AX_TARGET_PLATFORM == AX_PLATFORM_WASM
#    include "ui/UIHelper.h"
#    include <emscripten/emscripten.h>
NS_AX_BEGIN

namespace ui
{
EditBoxImplWasm* _activeEditBox = nullptr;
extern "C" {
EMSCRIPTEN_KEEPALIVE
void getInputOver(unsigned char* dataPtr, int dataLength)
{
    AXLOG("text {} ", dataPtr);
    if (_activeEditBox)
    {
        if (_activeEditBox->isEditingMode())
            _activeEditBox->editBoxEditingDidEnd(std::string_view{(char*)dataPtr},
                                                 EditBoxDelegate::EditBoxEndAction::RETURN);
        _activeEditBox = nullptr;
    }
    free(dataPtr);
}

EMSCRIPTEN_KEEPALIVE

void getInputChange(unsigned char* dataPtr, int dataLength)
{
    AXLOG("text {} ", dataPtr);
    if (_activeEditBox && _activeEditBox->isEditingMode())
    {
        _activeEditBox->editBoxEditingChanged(std::string_view{(char*)dataPtr});
    }
    free(dataPtr);
}
}

bool EditBoxImplWasm::s_isInitialized = false;
int EditBoxImplWasm::s_editboxChildID = 100;

EditBoxImpl* __createSystemEditBox(EditBox* pEditBox)
{
    return new EditBoxImplWasm(pEditBox);
}

EditBoxImplWasm::EditBoxImplWasm(EditBox* pEditText) : EditBoxImplCommon(pEditText)
{
    if (!s_isInitialized)
    {
        lazyInit();
    }
    s_editboxChildID++;
}

EditBoxImplWasm::~EditBoxImplWasm()
{
    // this->cleanupEditCtrl();
}

bool EditBoxImplWasm::isEditing()
{
    return false;
}

void EditBoxImplWasm::createNativeControl(const Rect& frame)
{
    this->createEditCtrl(ax::ui::EditBox::InputMode::ANY);
}

void EditBoxImplWasm::setNativeFont(const char* pFontName, int fontSize)
{
    // 设置编辑框文本大小
    EM_ASM(
        {
            var input = Module.axmolSharedInput = Module.axmolSharedInput || document.createElement("input");
            input.style.fontSize = $0 + "px";
        },
        fontSize);
}

void EditBoxImplWasm::setNativeFontColor(const Color4B& color)
{
    // not implemented yet
}

void EditBoxImplWasm::setNativePlaceholderFont(const char* pFontName, int fontSize)
{
    // not implemented yet
}

void EditBoxImplWasm::setNativePlaceholderFontColor(const Color4B& color)
{
    // not implemented yet
}

void EditBoxImplWasm::setNativeInputMode(EditBox::InputMode inputMode)
{
    this->createEditCtrl(inputMode);
}
void EditBoxImplWasm::setNativeInputFlag(EditBox::InputFlag inputFlag) {}

void EditBoxImplWasm::setNativeReturnType(EditBox::KeyboardReturnType returnType)
{
    // not implemented yet
}

void EditBoxImplWasm::setNativeTextHorizontalAlignment(TextHAlignment alignment) {}

void EditBoxImplWasm::setNativeText(const char* pText)
{
    EM_ASM({
        var input = Module.axmolSharedInput = Module.axmolSharedInput || document.createElement("input");
        input.value = UTF8ToString($0);
    }, pText);
}

void EditBoxImplWasm::setNativePlaceHolder(const char* pText)
{
    // not implemented yet
}

void EditBoxImplWasm::setNativeVisible(bool visible)
{
    EM_ASM({
        var input = Module.axmolSharedInput = Module.axmolSharedInput || document.createElement("input");
        
        if ($0 == 0)
            input.style.display = "none";
        else
        {
            var inputMode = $1;
            var inputFlag = $2;
            // set input type
            switch(inputMode) {
            case 2: // NUMERIC
            case 3: // PHONE_NUMBER
                input.type = 'number';
            default:
                if (inputFlag != 0) {
                    input.type = 'text';
                } else {
                    input.type = 'password';
                }
            }
            
            input.style.display = "";
            var canvas = document.getElementById('canvas');
            var inputParent = input.parentNode;
            var canvasParent = canvas.parentNode;
            if(inputParent != canvasParent) {
                if (inputParent != null) {
                    inputParent.removeChild(input);
                }
                canvasParent.insertBefore(input, canvas);
            }
        }
    }, (int)visible, (int)_editBoxInputMode, (int)_editBoxInputFlag);
}

void EditBoxImplWasm::updateNativeFrame(const Rect& rect)
{
    EM_ASM({
        var input = Module.axmolSharedInput = Module.axmolSharedInput || document.createElement("input");
        var canvas                          = Module["canvas"];
        // set input style
        input.style.position = "absolute";
        input.style.left     = canvas.offsetLeft + $0 + "px";
        input.style.top      = canvas.offsetTop + $1 + "px";
        input.style.width    = $2 + "px";
        input.style.height   = $3 + "px";
    },
    rect.origin.x, rect.origin.y, rect.size.x, rect.size.y);
}

const char* EditBoxImplWasm::getNativeDefaultFontName()
{
    return "Arial";
}

void EditBoxImplWasm::nativeOpenKeyboard()
{
    _activeEditBox = this;

    this->editBoxEditingDidBegin();
    
    EM_ASM({
        var input = Module.axmolSharedInput = Module.axmolSharedInput || document.createElement("input");
        // sync input value from native and focus
        input.value = UTF8ToString($0);
        input.maxlength = $1 != -1 ? $1 : undefined;
        input.focus();
    }, this->getText(), (int)_maxLength);

    auto rect = ui::Helper::convertBoundingBoxToScreen(_editBox);
    this->updateNativeFrame(rect);
}

void EditBoxImplWasm::nativeCloseKeyboard()
{
    // don't need to implement
}

void EditBoxImplWasm::setNativeMaxLength(int /*maxLength*/)
{
    // since we use shared inputbox, we sync maxlength when open inputbox for current editbox
}

void EditBoxImplWasm::lazyInit()
{
    // clang-format off
    EM_ASM({
        var input = Module.axmolSharedInput = Module.axmolSharedInput || document.createElement("input");
        // set input type
        input.type = "text";
        // set input style
        input.style.position = "absolute";
        input.style.left     = "0px";
        input.style.top      = "0px";
        input.style.width    = "200px";
        input.style.height   = "30px";
        // input.style.border = "1px solid black";
        input.style.padding              = " 0 0 0 0px";
        // document.body.appendChild(input);
        input.addEventListener(
            "keydown", function(event) {
                if (event.key === "Backspace")
                {
                    // allow delete chars by key  backward
                    var cursorPosition = input.selectionStart;
                    if (cursorPosition > 0)
                    {
                        var value = input.value;
                        // del last char
                        // const newValue = value.slice(0, -1);
                        var newValue = value.slice(0, cursorPosition - 1) + value.slice(cursorPosition);
                        // update input value
                        input.value = newValue;
                        input.setSelectionRange(cursorPosition - 1, cursorPosition - 1);
                        // input.selectionStart = ;
                    }
                    return;
                }

                if (input.maxlength !== undefined && input.value.length >= input.maxlength)
                {
                    // prevent max chars
                    event.preventDefault();
                }
            });
        input.addEventListener(
            'change', function() {
                // handle focus lost
                var input = Module.axmolSharedInput = Module.axmolSharedInput || document.createElement("input");
                var value                           = input.value;
                var lengthBytes                     = lengthBytesUTF8(value) + 1;
                var stringOnWasmHeap                = _malloc(lengthBytes);
                stringToUTF8(value, stringOnWasmHeap, lengthBytes);
                _getInputChange(stringOnWasmHeap, lengthBytes);
            });
        input.addEventListener(
            'blur', function() {
                // handle focus lost
                var input = Module.axmolSharedInput = Module.axmolSharedInput || document.createElement("input");
                input.style.display                 = "none";
                var value                           = input.value;
                var lengthBytes                     = lengthBytesUTF8(value) + 1;
                var stringOnWasmHeap                = _malloc(lengthBytes);
                stringToUTF8(value, stringOnWasmHeap, lengthBytes);
                _getInputOver(stringOnWasmHeap, lengthBytes);
            });
    });
    // clang-format on
    s_isInitialized = true;
}

void EditBoxImplWasm::createEditCtrl(EditBox::InputMode inputMode)
{
    EM_ASM({
            Module.axmolSharedInput = Module.axmolSharedInput || document.createElement("input");
        });
    this->setNativeFont(this->getNativeDefaultFontName(), this->_fontSize);
    this->setNativeText(this->_text.c_str());
}

}  // namespace ui

NS_AX_END

#endif
