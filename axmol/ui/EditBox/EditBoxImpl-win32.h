/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013 Jozef Pridavok
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

#include "axmol/platform/PlatformConfig.h"

#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32)
#    include "axmol/ui/EditBox/EditBoxImpl-common.h"

namespace ax
{

namespace ui
{

class EditBox;

class AX_GUI_DLL EditBoxImplWin : public EditBoxImplCommon
{
public:
    EditBoxImplWin(EditBox* pEditText);
    virtual ~EditBoxImplWin();

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

private:
    void createEditCtrl(bool singleLine);
    void cleanupEditCtrl();
    void cleanupFont();
    std::string getNativeText() const;
    void _WindowProc(HWND, UINT, WPARAM, LPARAM);

    WNDPROC _prevWndProc;

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK hookGLFWWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    bool _initialFocus{true};
    bool _changedTextManually{false};
    bool _hasFocus{false};
    HWND _hwndEdit{nullptr};
    HFONT _hEditFont{nullptr};
    EditBoxDelegate::EditBoxEndAction _endAction;
    static WNDPROC s_prevCocosWndProc;

    static HINSTANCE s_hInstance;
    static HWND s_hwndCocos;
    static HWND s_previousFocusWnd;
    static bool s_isInitialized;
    static HMENU s_editboxChildID;
    static void lazyInit();
};

}  // namespace ui

}  // namespace ax

#endif /* (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32) */
