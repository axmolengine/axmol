/****************************************************************************
 Copyright (c) 2016 Google Inc.
 Copyright (c) 2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/platform/PlatformMacros.h"
#include "axmol/rhi/Buffer.h"

namespace ax
{
inline namespace experimental
{
class Distortion;

class AX_DLL DistortionMesh
{
public:
    ~DistortionMesh();
    DistortionMesh(Distortion* distortion,
                   float screenWidth,
                   float screenHeight,
                   float xEyeOffsetScreen,
                   float yEyeOffsetScreen,
                   float textureWidth,
                   float textureHeight,
                   float xEyeOffsetTexture,
                   float yEyeOffsetTexture,
                   float viewportXTexture,
                   float viewportYTexture,
                   float viewportWidthTexture,
                   float viewportHeightTexture,
                   bool vignetteEnabled);

    int _indices{0};
    rhi::Buffer* _vbo{nullptr};
    rhi::Buffer* _ebo{nullptr};
};
}  // namespace experimental
}  // namespace ax
