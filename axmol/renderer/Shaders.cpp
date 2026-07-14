/****************************************************************************
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2012 		cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

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
#include "axmol/renderer/Shaders.h"

namespace ax
{

using namespace std::string_view_literals;

AX_DLL const std::string_view positionColor_vs                   = "positionColor_vs"sv;
AX_DLL const std::string_view positionColor_fs                   = "positionColor_fs"sv;
AX_DLL const std::string_view positionTexture_vs                 = "positionTexture_vs"sv;
AX_DLL const std::string_view positionTexture_fs                 = "positionTexture_fs"sv;
AX_DLL const std::string_view positionTextureColor_vs            = "positionTextureColor_vs"sv;
AX_DLL const std::string_view positionTextureColor_fs            = "positionTextureColor_fs"sv;
AX_DLL const std::string_view positionTextureColorAlphaTest_fs   = "positionTextureColorAlphaTest_fs"sv;
AX_DLL const std::string_view positionTextureGray_fs             = "positionTextureGray_fs"sv;
AX_DLL const std::string_view positionTextureGrayAlpha_fs        = "positionTextureGrayAlpha_fs"sv;
AX_DLL const std::string_view label_normal_fs                    = "label_normal_fs"sv;
AX_DLL const std::string_view label_outline_fs                   = "label_outline_fs"sv;
AX_DLL const std::string_view label_distanceNormal_fs            = "label_distanceNormal_fs"sv;
AX_DLL const std::string_view label_distanceOutline_fs           = "label_distanceOutline_fs"sv;
AX_DLL const std::string_view label_distanceGlow_fs              = "label_distanceGlow_fs"sv;
AX_DLL const std::string_view positionColorLengthTexture_vs      = "positionColorLengthTexture_vs"sv;
AX_DLL const std::string_view positionColorLengthTexture_fs      = "positionColorLengthTexture_fs"sv;
AX_DLL const std::string_view positionColorTextureAsPointsize_vs = "positionColorTextureAsPointsize_vs"sv;
AX_DLL const std::string_view position_vs                        = "position_vs"sv;
AX_DLL const std::string_view layer_radialGradient_fs            = "layer_radialGradient_fs"sv;
AX_DLL const std::string_view grayScale_fs                       = "grayScale_fs"sv;
AX_DLL const std::string_view positionUColor_vs                  = "positionUColor_vs"sv;
AX_DLL const std::string_view dualSampler_fs                     = "dualSampler_fs"sv;
AX_DLL const std::string_view dualSampler_gray_fs                = "dualSampler_gray_fs"sv;
AX_DLL const std::string_view cameraClear_vs                     = "cameraClear_vs"sv;
AX_DLL const std::string_view cameraClear_fs                     = "cameraClear_fs"sv;
AX_DLL const std::string_view quadTexture_fs                     = "quadTexture_fs"sv;
AX_DLL const std::string_view quadTexture_vs                     = "quadTexture_vs"sv;
AX_DLL const std::string_view quadColor_fs                       = "quadColor_fs"sv;
AX_DLL const std::string_view quadColor_vs                       = "quadColor_vs"sv;
AX_DLL const std::string_view hsv_fs                             = "hsv_fs"sv;
AX_DLL const std::string_view dualSampler_hsv_fs                 = "dualSampler_hsv_fs"sv;
AX_DLL const std::string_view videoTextureYUY2_fs                = "videoTextureYUY2_fs"sv;
AX_DLL const std::string_view videoTextureNV12_fs                = "videoTextureNV12_fs"sv;
AX_DLL const std::string_view videoTextureI420_fs                = "videoTextureI420_fs"sv;
AX_DLL const std::string_view posUVColor2D_vs                    = "posUVColor2D_vs"sv;
AX_DLL const std::string_view lineColor_fs                       = "lineColor_fs"sv;
AX_DLL const std::string_view lineColor_vs                       = "lineColor_vs"sv;
AX_DLL const std::string_view color_fs                           = "color_fs"sv;
AX_DLL const std::string_view colorNormal_fs                     = "colorNormal_fs"sv;
AX_DLL const std::string_view colorNormalTexture_fs              = "colorNormalTexture_fs"sv;
AX_DLL const std::string_view colorTexture_fs                    = "colorTexture_fs"sv;
AX_DLL const std::string_view particleTexture_fs                 = "particleTexture_fs"sv;
AX_DLL const std::string_view particleColor_fs                   = "particleColor_fs"sv;
AX_DLL const std::string_view particle_vs                        = "particle_vs"sv;
AX_DLL const std::string_view positionNormalTexture_vs           = "positionNormalTexture_vs"sv;
AX_DLL const std::string_view skinPositionNormalTexture_vs       = "skinPositionNormalTexture_vs"sv;
AX_DLL const std::string_view unlit_vs                           = "unlit_vs"sv;
AX_DLL const std::string_view unlit_instance_vs                  = "unlit_instance_vs"sv;
AX_DLL const std::string_view skinPositionTexture_vs             = "skinPositionTexture_vs"sv;
AX_DLL const std::string_view skybox_fs                          = "skybox_fs"sv;
AX_DLL const std::string_view skybox_vs                          = "skybox_vs"sv;
AX_DLL const std::string_view terrain_fs                         = "terrain_fs"sv;
AX_DLL const std::string_view terrain_vs                         = "terrain_vs"sv;
AX_DLL const std::string_view colorNormalTexture_fs_1            = "colorNormalTexture_fs_1"sv;
AX_DLL const std::string_view positionNormalTexture_vs_1         = "positionNormalTexture_vs_1"sv;
AX_DLL const std::string_view skinPositionNormalTexture_vs_1     = "skinPositionNormalTexture_vs_1"sv;

}  // namespace ax
