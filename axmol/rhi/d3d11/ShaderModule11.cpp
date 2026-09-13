/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
// refer: https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-semantics
#include "axmol/rhi/d3d11/ShaderModule11.h"
#include "axmol/rhi/d3d11/GraphicsDevice11.h"
#include "axmol/platform/msw/ComPtr.h"
#include "axmol/base/Logging.h"

#pragma comment(lib, "d3dcompiler.lib")

namespace ax::rhi::d3d11
{
ShaderModuleImpl::ShaderModuleImpl(GraphicsDeviceImpl* driver, ShaderStage stage, Data& data)
    : ShaderModule(stage, data)
{
    if (_precompiled)
    {
        _blob     = _codeSpan;
        _shader   = driver->createShaderFromBytecode(_codeSpan, stage);
        _compiled = _shader != nullptr;
    }
    else
    {
        ID3DBlob* shaderBlob{nullptr};
        _shader   = driver->compileShader(_codeSpan, stage, shaderBlob);
        _compiled = shaderBlob != nullptr;
        if (_compiled)
        {
            _blob       = {static_cast<uint8_t*>(shaderBlob->GetBufferPointer()), shaderBlob->GetBufferSize()};
            _nativeBlob = shaderBlob;
        }
    }
}

ShaderModuleImpl::~ShaderModuleImpl()
{
    SafeRelease(_shader);
}

}  // namespace ax::rhi::d3d11
