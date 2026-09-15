/****************************************************************************
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2012 		cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/
#pragma once

/// @cond DO_NOT_SHOW

#include "axmol/platform/PlatformMacros.h"

#include <string_view>

/**
 * @addtogroup renderer
 * @{
 */

namespace ax
{

extern AX_DLL const std::string_view positionColor_vs;
extern AX_DLL const std::string_view positionColor_fs;
extern AX_DLL const std::string_view positionTexture_vs;
extern AX_DLL const std::string_view positionTexture_fs;
extern AX_DLL const std::string_view positionTextureColor_vs;
extern AX_DLL const std::string_view positionTextureColor_fs;
extern AX_DLL const std::string_view positionTextureColorAlphaTest_fs;
extern AX_DLL const std::string_view positionTextureGray_fs;
extern AX_DLL const std::string_view positionTextureGrayAlpha_fs;
extern AX_DLL const std::string_view label_normal_fs;
extern AX_DLL const std::string_view label_outline_fs;
extern AX_DLL const std::string_view label_distanceNormal_fs;
extern AX_DLL const std::string_view label_distanceOutline_fs;
extern AX_DLL const std::string_view label_distanceGlow_fs;
extern AX_DLL const std::string_view positionColorLengthTexture_vs;
extern AX_DLL const std::string_view positionColorLengthTexture_fs;
extern AX_DLL const std::string_view positionColorTextureAsPointsize_vs;
extern AX_DLL const std::string_view position_vs;
extern AX_DLL const std::string_view layer_radialGradient_vs;
extern AX_DLL const std::string_view layer_radialGradient_fs;
extern AX_DLL const std::string_view grayScale_fs;
extern AX_DLL const std::string_view positionUColor_vs;
extern AX_DLL const std::string_view dualSampler_fs;
extern AX_DLL const std::string_view dualSampler_gray_fs;
extern AX_DLL const std::string_view cameraClear_vs;
extern AX_DLL const std::string_view cameraClear_fs;

extern AX_DLL const std::string_view quadTexture_fs;
extern AX_DLL const std::string_view quadTexture_vs;
extern AX_DLL const std::string_view quadColor_fs;
extern AX_DLL const std::string_view quadColor_vs;

extern AX_DLL const std::string_view hsv_fs;
extern AX_DLL const std::string_view dualSampler_hsv_fs;

extern AX_DLL const std::string_view videoTextureYUY2_fs;
extern AX_DLL const std::string_view videoTextureNV12_fs;
extern AX_DLL const std::string_view videoTextureI420_fs;

extern AX_DLL const std::string_view posUVColor2D_vs;

/* below is 3d shaders */
extern AX_DLL const std::string_view lineColor_fs;
extern AX_DLL const std::string_view lineColor_vs;
extern AX_DLL const std::string_view color_fs;
extern AX_DLL const std::string_view colorNormal_fs;
extern AX_DLL const std::string_view colorNormalTexture_fs;
extern AX_DLL const std::string_view colorTexture_fs;
extern AX_DLL const std::string_view particleTexture_fs;
extern AX_DLL const std::string_view particleColor_fs;
extern AX_DLL const std::string_view particle_vs;
extern AX_DLL const std::string_view positionNormalTexture_vs;
extern AX_DLL const std::string_view skinPositionNormalTexture_vs;
extern AX_DLL const std::string_view unlit_vs;
extern AX_DLL const std::string_view unlit_instance_vs;
extern AX_DLL const std::string_view skinPositionTexture_vs;
extern AX_DLL const std::string_view skybox_fs;
extern AX_DLL const std::string_view skybox_vs;
extern AX_DLL const std::string_view terrain_fs;
extern AX_DLL const std::string_view terrain_vs;

/* blow is with normal map */
extern AX_DLL const std::string_view colorNormalTexture_fs_1;
extern AX_DLL const std::string_view positionNormalTexture_vs_1;
extern AX_DLL const std::string_view skinPositionNormalTexture_vs_1;

}  // namespace ax
/**
 end of support group
 @}
 */
/// @endcond
