/****************************************************************************
Copyright (c) 2011      Laschweinski
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/
#include "axmol/platform/Common.h"
#include "axmol/platform/linux/StdC-linux.h"
#include "axmol/base/Logging.h"

namespace ax
{

AlertResult showAlert(std::string_view msg, std::string_view title, AlertStyle)
{
    AXLOGI("{}: {}", title, msg);
    return AlertResult::None;
}

}  // namespace ax
