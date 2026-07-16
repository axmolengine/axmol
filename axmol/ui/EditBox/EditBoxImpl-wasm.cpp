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

#include "axmol/ui/EditBox/EditBoxImpl-wasm.h"

#if AX_TARGET_PLATFORM == AX_PLATFORM_WASM
#    include "axmol/ui/UIHelper.h"
#    include <emscripten/emscripten.h>
namespace ax
{

namespace ui
{
EditBoxImplWasm* _activeEditBox = nullptr;
extern "C" {
EMSCRIPTEN_KEEPALIVE void axmol_editbox_endediting(const char* pszText, int length)
{
    std::string_view text{pszText, static_cast<size_t>(length)};
    AXLOGD("text {} ", text);
    if (_activeEditBox)
    {
        if (_activeEditBox->isEditingMode())
            _activeEditBox->editBoxEditingDidEnd(text, EditBoxDelegate::EditBoxEndAction::RETURN);
        _activeEditBox = nullptr;
    }
}

EMSCRIPTEN_KEEPALIVE void axmol_editbox_textchange(const char* pszText, int length)
{
    std::string_view text{pszText, static_cast<size_t>(length)};
    AXLOGD("text {} ", text);
    if (_activeEditBox && _activeEditBox->isEditingMode())
    {
        _activeEditBox->editBoxEditingChanged(text);
    }
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

void EditBoxImplWasm::createNativeControl()
{
    this->createEditCtrl(ax::ui::EditBox::InputMode::ANY);
}

void EditBoxImplWasm::setNativeFont(std::string_view /*fontName*/, int fontSize)
{
    // clang-format off
    EM_ASM({
        var input = Module.axmol_editbox_input = Module.axmol_editbox_input || document.createElement("input");
        input.style.fontSize                   = $0 + "px";
    },
    fontSize);
    // clang-format on
}

void EditBoxImplWasm::setNativeFontColor(const Color32& /*color*/)
{
    // not implemented yet
}

void EditBoxImplWasm::setNativePlaceholderFont(std::string_view /*fontName*/, int /*fontSize*/)
{
    // not implemented yet
}

void EditBoxImplWasm::setNativePlaceholderFontColor(const Color32& /*color*/)
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

void EditBoxImplWasm::setNativeText(std::string_view text)
{
    // clang-format off
    EM_ASM({
        var input = Module.axmol_editbox_input = Module.axmol_editbox_input || document.createElement("input");
        input.value                            = UTF8ToString($0, $1);
    },
    text.data(), static_cast<int>(text.size()));
    // clang-format off
}

void EditBoxImplWasm::setNativeVisible(bool visible)
{
    // clang-format off
    EM_ASM({
        var input = Module.axmol_editbox_input = Module.axmol_editbox_input || document.createElement("input");
        if ($0 == 0)
            input.style.display = "none";
        else
        {
            var inputMode = $1;
            var inputFlag = $2;
            // set input type
            switch (inputMode)
            {
            case 2:  // NUMERIC
            case 3:  // PHONE_NUMBER
                input.type = 'number';
            default:
                if (inputFlag != 0)
                {
                    input.type = 'text';
                }
                else
                {
                    input.type = 'password';
                }
            }

            input.style.display = "";
            var canvas          = document.getElementById('canvas');
            var inputParent     = input.parentNode;
            var canvasParent    = canvas.parentNode;
            if (inputParent != canvasParent)
            {
                if (inputParent != null)
                {
                    inputParent.removeChild(input);
                }
                canvasParent.insertBefore(input, canvas);
            }
        }
    },
    (int)visible, (int)_editBoxInputMode, (int)_editBoxInputFlag);
    // clang-format on
}

void EditBoxImplWasm::updateNativeFrame(const Rect& rect)
{
    // clang-format off
    EM_ASM({
        var input = Module.axmol_editbox_input = Module.axmol_editbox_input || document.createElement("input");
        var canvas                             = Module["canvas"];
        // set input style
        input.style.position = "absolute";
        input.style.left     = canvas.offsetLeft + $0 + "px";
        input.style.top      = canvas.offsetTop + $1 + "px";
        input.style.width    = $2 + "px";
        input.style.height   = $3 + "px";
    },
    rect.origin.x, rect.origin.y, rect.size.x, rect.size.y);
    // clang-format on
}

void EditBoxImplWasm::nativeOpenKeyboard()
{
    _activeEditBox = this;

    this->editBoxEditingDidBegin();

    auto text = this->getText();

    // clang-format off
    EM_ASM({
        var input = Module.axmol_editbox_input = Module.axmol_editbox_input || document.createElement("input");
        // sync input value from native and focus
        input.value     = UTF8ToString($0, $1);
        input.maxlength = $2 != -1 ? $2 : undefined;
        input.focus();
    },
    text.data(), (int)text.size(), (int)_maxLength);
    // clang-format on

    auto rect = ui::Helper::getNodeNativeWindowRect(_editBox);
    this->updateNativeFrame(rect);
}

void EditBoxImplWasm::lazyInit()
{
    // clang-format off
    EM_ASM({
        var input = Module.axmol_editbox_input = Module.axmol_editbox_input || document.createElement("input");
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
                var input = Module.axmol_editbox_input = Module.axmol_editbox_input || document.createElement("input");
                var result = Module.stringToUTF8WithLen(input.value);
                _axmol_editbox_textchange(result.ptr, result.length);
            });
        input.addEventListener(
            'blur', function() {
                // handle focus lost
                var input = Module.axmol_editbox_input = Module.axmol_editbox_input || document.createElement("input");
                input.style.display                 = "none";

                var result = Module.stringToUTF8WithLen(input.value);
                _axmol_editbox_endediting(result.ptr, result.length);
                _free(result.ptr)
            });
    });
    // clang-format on
    s_isInitialized = true;
}

void EditBoxImplWasm::createEditCtrl(EditBox::InputMode inputMode)
{
    EM_ASM({ Module.axmol_editbox_input = Module.axmol_editbox_input || document.createElement("input"); });
    this->setNativeFont(this->getNativeDefaultFontName(), this->_fontSize);
    this->setNativeText(this->_text);
}

void EditBoxImplWasm::setNativePlaceHolder(std::string_view text)
{
    EM_ASM(
        {
            var input = Module.axmol_editbox_input = Module.axmol_editbox_input || document.createElement("input");
            // sync input value from native and focus
            input.placeholder = UTF8ToString($0, $1);
            input.focus();
        },
        !text.empty() ? text.data() : "", (int)text.size());
}

std::string_view EditBoxImplWasm::getNativeDefaultFontName()
{
    return "Arial"sv;
}

void EditBoxImplWasm::nativeCloseKeyboard()
{
    // don't need to implement
}

void EditBoxImplWasm::setNativeMaxLength(int /*maxLength*/)
{
    // since we use shared inputbox, we sync maxlength when open inputbox for current editbox
}

}  // namespace ui

}  // namespace ax

#endif
