/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
// refer: https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-semantics
#include "axmol/rhi/d3d12/ShaderModule12.h"
#include "axmol/rhi/d3d12/GraphicsDevice12.h"
#include "axmol/platform/msw/ComPtr.h"
#include "axmol/base/Logging.h"

namespace ax::rhi::d3d12
{
ShaderModuleImpl::ShaderModuleImpl(GraphicsDeviceImpl* driver, ShaderStage stage, Data& chunk)
    : ShaderModule(stage, chunk)
{
    if (_precompiled)
    {
        _blob     = _codeSpan;
        _compiled = true;
    }
    else
    {
        _nativeBlob = driver->compileShader(_codeSpan, stage, _blob);
        _compiled   = _nativeBlob != nullptr;
    }
}

ShaderModuleImpl::~ShaderModuleImpl() {}

}  // namespace ax::rhi::d3d12
