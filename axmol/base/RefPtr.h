/****************************************************************************
 Copyright (c) 2014      PlayFirst Inc.
 Copyright (c) 2014-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once
/// @cond DO_NOT_SHOW

#include "axmol/base/Object.h"
#include "axmol/base/Macros.h"
#include "axmol/tlx/memory.hpp"
#include <functional>
#include <type_traits>

namespace ax
{

template <typename T>
using retain_ptr = tlx::retain_ptr<T, tlx::retain_traits<Object>>;

template <typename T>
using RefPtr = retain_ptr<T>;

}  // namespace ax

/// @endcond
