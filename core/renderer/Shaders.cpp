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
#include "renderer/Shaders.h"

namespace ax
{

using namespace std::string_view_literals;

AX_DLL const std::string_view positionColor_vert                   = "positionColor_vs"sv;
AX_DLL const std::string_view positionColor_frag                   = "positionColor_fs"sv;
AX_DLL const std::string_view positionTexture_vert                 = "positionTexture_vs"sv;
AX_DLL const std::string_view positionTexture_frag                 = "positionTexture_fs"sv;
AX_DLL const std::string_view positionTextureColor_vert            = "positionTextureColor_vs"sv;
AX_DLL const std::string_view positionTextureColor_frag            = "positionTextureColor_fs"sv;
AX_DLL const std::string_view positionTextureColorAlphaTest_frag   = "positionTextureColorAlphaTest_fs"sv;
AX_DLL const std::string_view label_normal_frag                    = "label_normal_fs"sv;
AX_DLL const std::string_view label_outline_frag                   = "label_outline_fs"sv;
AX_DLL const std::string_view label_distanceNormal_frag            = "label_distanceNormal_fs"sv;
AX_DLL const std::string_view label_distanceOutline_frag           = "label_distanceOutline_fs"sv;
AX_DLL const std::string_view label_distanceGlow_frag              = "label_distanceGlow_fs"sv;
AX_DLL const std::string_view positionColorLengthTexture_vert      = "positionColorLengthTexture_vs"sv;
AX_DLL const std::string_view positionColorLengthTexture_frag      = "positionColorLengthTexture_fs"sv;
AX_DLL const std::string_view positionColorTextureAsPointsize_vert = "positionColorTextureAsPointsize_vs"sv;
AX_DLL const std::string_view position_vert                        = "position_vs"sv;
AX_DLL const std::string_view layer_radialGradient_frag            = "layer_radialGradient_fs"sv;
AX_DLL const std::string_view grayScale_frag                       = "grayScale_fs"sv;
AX_DLL const std::string_view positionUColor_vert                  = "positionUColor_vs"sv;
AX_DLL const std::string_view dualSampler_frag                     = "dualSampler_fs"sv;
AX_DLL const std::string_view dualSampler_gray_frag                = "dualSampler_gray_fs"sv;
AX_DLL const std::string_view cameraClear_vert                     = "cameraClear_vs"sv;
AX_DLL const std::string_view cameraClear_frag                     = "cameraClear_fs"sv;
AX_DLL const std::string_view quadTexture_frag                     = "quadTexture_fs"sv;
AX_DLL const std::string_view quadTexture_vert                     = "quadTexture_vs"sv;
AX_DLL const std::string_view quadColor_frag                       = "quadColor_fs"sv;
AX_DLL const std::string_view quadColor_vert                       = "quadColor_vs"sv;
AX_DLL const std::string_view hsv_frag                             = "hsv_fs"sv;
AX_DLL const std::string_view dualSampler_hsv_frag                 = "dualSampler_hsv_fs"sv;
AX_DLL const std::string_view videoTextureYUY2_frag                = "videoTextureYUY2_fs"sv;
AX_DLL const std::string_view videoTextureNV12_frag                = "videoTextureNV12_fs"sv;
AX_DLL const std::string_view videoTextureI420_frag                = "videoTextureI420_fs"sv;
AX_DLL const std::string_view lineColor_frag                       = "lineColor_fs"sv;
AX_DLL const std::string_view lineColor_vert                       = "lineColor_vs"sv;
AX_DLL const std::string_view color_frag                           = "color_fs"sv;
AX_DLL const std::string_view colorNormal_frag                     = "colorNormal_fs"sv;
AX_DLL const std::string_view colorNormalTexture_frag              = "colorNormalTexture_fs"sv;
AX_DLL const std::string_view colorTexture_frag                    = "colorTexture_fs"sv;
AX_DLL const std::string_view particleTexture_frag                 = "particleTexture_fs"sv;
AX_DLL const std::string_view particleColor_frag                   = "particleColor_fs"sv;
AX_DLL const std::string_view particle_vert                        = "particle_vs"sv;
AX_DLL const std::string_view positionNormalTexture_vert           = "positionNormalTexture_vs"sv;
AX_DLL const std::string_view skinPositionNormalTexture_vert       = "skinPositionNormalTexture_vs"sv;
AX_DLL const std::string_view positionTexture3D_vert               = "positionTexture3D_vs"sv;
AX_DLL const std::string_view positionTextureInstance_vert         = "positionTextureInstance_vs"sv;
AX_DLL const std::string_view skinPositionTexture_vert             = "skinPositionTexture_vs"sv;
AX_DLL const std::string_view skybox_frag                          = "skybox_fs"sv;
AX_DLL const std::string_view skybox_vert                          = "skybox_vs"sv;
AX_DLL const std::string_view terrain_frag                         = "terrain_fs"sv;
AX_DLL const std::string_view terrain_vert                         = "terrain_vs"sv;
AX_DLL const std::string_view colorNormalTexture_frag_1            = "colorNormalTexture_fs_1"sv;
AX_DLL const std::string_view positionNormalTexture_vert_1         = "positionNormalTexture_vs_1"sv;
AX_DLL const std::string_view skinPositionNormalTexture_vert_1     = "skinPositionNormalTexture_vs_1"sv;

}
