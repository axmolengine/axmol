/****************************************************************************
Copyright (c) 2009      Valentin Milea
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

#include "axmol/base/Macros.h"

/**
 * @addtogroup base
 * @{
 */

namespace ax
{

struct AffineTransform;
/**@{
 Conversion between mat4*4 and AffineTransform.
 @param m The Mat4*4 pointer.
 @param t Affine transform.
 */
AX_DLL void CGAffineToGL(const AffineTransform& t, float* m);
AX_DLL void GLToCGAffine(const float* m, AffineTransform* t);
/**@}*/
}  // namespace ax
/**
 end of base group
 @}
 */
