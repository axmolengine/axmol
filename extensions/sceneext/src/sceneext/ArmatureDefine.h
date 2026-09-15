/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

#define VERSION_COMBINED              0.30f
#define VERSION_CHANGE_ROTATION_RANGE 1.0f
#define VERSION_COLOR_READING         1.1f

#ifndef AUTO_ADD_SPRITE_FRAME_NAME_PREFIX
#    define AUTO_ADD_SPRITE_FRAME_NAME_PREFIX 0
#endif  // !AUTO_ADD_SPRITE_FRAME_NAME_PREFIX

#ifndef ENABLE_PHYSICS_BOX2D_DETECT
#    define ENABLE_PHYSICS_BOX2D_DETECT 1
#endif
#ifndef ENABLE_PHYSICS_SAVE_CALCULATED_VERTEX
#    define ENABLE_PHYSICS_SAVE_CALCULATED_VERTEX 1
#endif

#define CS_RETURN_IF(cond) \
    if (cond)              \
    return
#define CS_RETURN_NULL_IF(cond) \
    if (cond)                   \
        return nullptr;

namespace ax::ext
{

const char* armatureVersion();

}
