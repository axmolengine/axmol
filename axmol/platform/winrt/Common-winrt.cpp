/****************************************************************************
Copyright (c) 2010 cocos2d-x.org
Copyright (c) Microsoft Open Technologies, Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/
#include "axmol/platform/Common.h"
#include "axmol/platform/StdC.h"
#include "axmol/platform/winrt/RenderView-winrt.h"
#include "axmol/platform/winrt/WinRTUtils.h"

#if defined(VLD_DEBUG_MEMORY)
#    include <vld.h>
#endif

namespace ax
{

AlertResult showAlert(std::string_view msg, std::string_view title, AlertStyle style)
{
    // Create the message dialog and set its content
    auto hmsg   = PlatformStringFromString(msg);
    auto htitle = PlatformStringFromString(title);
    return RenderView::sharedRenderView()->showAlertDialog(hmsg, htitle, style);
}

}  // namespace ax
