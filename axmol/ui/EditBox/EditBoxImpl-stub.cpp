/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/ui/EditBox/EditBox.h"

#if (AX_TARGET_PLATFORM != AX_PLATFORM_ANDROID) && (AX_TARGET_PLATFORM != AX_PLATFORM_IOS) && \
    (AX_TARGET_PLATFORM != AX_PLATFORM_WIN32) && (AX_TARGET_PLATFORM != AX_PLATFORM_MAC)

namespace ax
{

namespace ui
{

EditBoxImpl* __createSystemEditBox(EditBox* pEditBox)
{
    return nullptr;
}

}  // namespace ui

}  // namespace ax

#endif /* #if (..) */
