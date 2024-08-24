/****************************************************************************
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2012 		cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

/// @cond DO_NOT_SHOW

#include "platform/PlatformMacros.h"

#include <string_view>

/**
 * @addtogroup renderer
 * @{
 */

namespace ax
{

extern AX_DLL const std::string_view positionColor_vert;
extern AX_DLL const std::string_view positionColor_frag;
extern AX_DLL const std::string_view positionTexture_vert;
extern AX_DLL const std::string_view positionTexture_frag;
extern AX_DLL const std::string_view positionTextureColor_vert;
extern AX_DLL const std::string_view positionTextureColor_frag;
extern AX_DLL const std::string_view positionTextureColorAlphaTest_frag;
extern AX_DLL const std::string_view label_normal_frag;
extern AX_DLL const std::string_view label_outline_frag;
extern AX_DLL const std::string_view label_distanceNormal_frag;
extern AX_DLL const std::string_view label_distanceOutline_frag;
extern AX_DLL const std::string_view label_distanceGlow_frag;
extern AX_DLL const std::string_view positionColorLengthTexture_vert;
extern AX_DLL const std::string_view positionColorLengthTexture_frag;
extern AX_DLL const std::string_view positionColorTextureAsPointsize_vert;
extern AX_DLL const std::string_view position_vert;
extern AX_DLL const std::string_view layer_radialGradient_frag;
extern AX_DLL const std::string_view grayScale_frag;
extern AX_DLL const std::string_view positionUColor_vert;
extern AX_DLL const std::string_view dualSampler_frag;
extern AX_DLL const std::string_view dualSampler_gray_frag;
extern AX_DLL const std::string_view cameraClear_vert;
extern AX_DLL const std::string_view cameraClear_frag;

extern AX_DLL const std::string_view quadTexture_frag;
extern AX_DLL const std::string_view quadTexture_vert;
extern AX_DLL const std::string_view quadColor_frag;
extern AX_DLL const std::string_view quadColor_vert;

extern AX_DLL const std::string_view hsv_frag;
extern AX_DLL const std::string_view dualSampler_hsv_frag;

extern AX_DLL const std::string_view videoTextureYUY2_frag;
extern AX_DLL const std::string_view videoTextureNV12_frag;
extern AX_DLL const std::string_view videoTextureI420_frag;

/* below is 3d shaders */
extern AX_DLL const std::string_view lineColor_frag;
extern AX_DLL const std::string_view lineColor_vert;
extern AX_DLL const std::string_view color_frag;
extern AX_DLL const std::string_view colorNormal_frag;
extern AX_DLL const std::string_view colorNormalTexture_frag;
extern AX_DLL const std::string_view colorTexture_frag;
extern AX_DLL const std::string_view colorTextureInstance_frag;
extern AX_DLL const std::string_view particleTexture_frag;
extern AX_DLL const std::string_view particleColor_frag;
extern AX_DLL const std::string_view particle_vert;
extern AX_DLL const std::string_view positionNormalTexture_vert;
extern AX_DLL const std::string_view skinPositionNormalTexture_vert;
extern AX_DLL const std::string_view positionTexture3D_vert;
extern AX_DLL const std::string_view positionTextureInstance_vert;
extern AX_DLL const std::string_view skinPositionTexture_vert;
extern AX_DLL const std::string_view skybox_frag;
extern AX_DLL const std::string_view skybox_vert;
extern AX_DLL const std::string_view terrain_frag;
extern AX_DLL const std::string_view terrain_vert;


/* blow is with normal map */
extern AX_DLL const std::string_view colorNormalTexture_frag_1;
extern AX_DLL const std::string_view positionNormalTexture_vert_1;
extern AX_DLL const std::string_view skinPositionNormalTexture_vert_1;

}
/**
 end of support group
 @}
 */
/// @endcond
