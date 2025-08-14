/****************************************************************************
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
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

#include "axmol/ui/UIWidget.h"
#include "axmol/ui/UILayout.h"
#include "axmol/ui/UIButton.h"
#include "axmol/ui/UICheckBox.h"
#include "axmol/ui/UIRadioButton.h"
#include "axmol/ui/UIImageView.h"
#include "axmol/ui/UIText.h"
#include "axmol/ui/UITextAtlas.h"
#include "axmol/ui/UILoadingBar.h"
#include "axmol/ui/UIScrollView.h"
#include "axmol/ui/UIListView.h"
#include "axmol/ui/UISlider.h"
#include "axmol/ui/UITextField.h"
#include "axmol/ui/UITextFieldEx.h"
#include "axmol/ui/UITextBMFont.h"
#include "axmol/ui/UIPageView.h"
#include "axmol/ui/UIHelper.h"
#include "axmol/ui/UIRichText.h"
#include "axmol/ui/UIHBox.h"
#include "axmol/ui/UIVBox.h"
#include "axmol/ui/UIRelativeBox.h"
#if defined(AX_ENABLE_MEDIA)
#    include "axmol/ui/UIMediaPlayer.h"
#endif
#if !defined(_WIN32) || defined(AX_ENABLE_MSEDGE_WEBVIEW2)
#    include "axmol/ui/UIWebView/UIWebView.h"
#endif
#include "axmol/ui/GUIExport.h"
#include "axmol/ui/UIScale9Sprite.h"
#include "axmol/ui/UIEditBox/UIEditBox.h"
#include "axmol/ui/UILayoutComponent.h"
#include "axmol/ui/UITabControl.h"

/**
 * @addtogroup ui
 * @{
 */
namespace ax
{
namespace ui
{

/**
 * Get current cocos GUI module version string.
 *@return A string representation of GUI module version number
 */
AX_GUI_DLL const char* axmolUIVersion();

}  // namespace ui

}
// end of ui group
/// @}

