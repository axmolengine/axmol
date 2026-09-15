/****************************************************************************
Copyright (c) 2011      Laschweinski
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#include "axmol/platform/PlatformConfig.h"
#if AX_TARGET_PLATFORM == AX_PLATFORM_WASM

#    include "axmol/platform/Common.h"
#    include "axmol/platform/wasm/StdC-wasm.h"
#    include "axmol/base/Logging.h"
#    include <emscripten/emscripten.h>

namespace ax
{

AlertResult showAlert(std::string_view msg, std::string_view title, AlertStyle)
{
    auto pszMsg   = msg.data();
    auto pszTitle = title.data();
    EM_ASM_ARGS({ window.alert(UTF8ToString($0) + ": " + UTF8ToString($1)); }, pszTitle, pszTitle);
    return AlertResult::None;
}

}  // namespace ax

#endif  //  AX_TARGET_PLATFORM == AX_PLATFORM_WASM
