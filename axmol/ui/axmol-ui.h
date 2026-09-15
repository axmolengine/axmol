/****************************************************************************
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

#include "axmol/ui/Widget.h"
#include "axmol/ui/LayoutGroup.h"
#include "axmol/ui/Button.h"
#include "axmol/ui/CheckBox.h"
#include "axmol/ui/RadioButton.h"
#include "axmol/ui/ImageView.h"
#include "axmol/ui/Text.h"
#include "axmol/ui/TextAtlas.h"
#include "axmol/ui/LoadingBar.h"
#include "axmol/ui/ScrollView.h"
#include "axmol/ui/ListView.h"
#include "axmol/ui/Slider.h"
#include "axmol/ui/InputField.h"
#include "axmol/ui/TextBMFont.h"
#include "axmol/ui/PageView.h"
#include "axmol/ui/UIHelper.h"
#include "axmol/ui/RichText.h"
#include "axmol/ui/HBox.h"
#include "axmol/ui/VBox.h"
#include "axmol/ui/RelativeBox.h"
#if defined(AX_ENABLE_VIDEO)
#    include "axmol/ui/VideoPlayer.h"
#endif
#if !defined(_WIN32) || defined(AX_ENABLE_MSEDGE_WEBVIEW2)
#    include "axmol/ui/WebView/WebView.h"
#endif
#include "axmol/ui/GUIExport.h"
#include "axmol/ui/Scale9Sprite.h"
#include "axmol/ui/EditBox/EditBox.h"
#include "axmol/ui/LayoutComponent.h"
#include "axmol/ui/TabView.h"

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

}  // namespace ax
// end of ui group
/// @}
