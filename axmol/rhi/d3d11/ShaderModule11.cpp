/****************************************************************************
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
// refer: https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-semantics
#include "axmol/rhi/d3d11/ShaderModule11.h"
#include "axmol/platform/win32/ComPtr.h"
#include "axmol/base/Logging.h"

#pragma comment(lib, "d3dcompiler.lib")

namespace ax::rhi::d3d11
{

namespace
{
// ShaderStage -> profile
inline const char* stageToProfile(ShaderStage s)
{
    switch (s)
    {
    case ShaderStage::VERTEX:
        return "vs_5_0";
    case ShaderStage::FRAGMENT:
        return "ps_5_0";
    default:
        return "vs_5_0";
    }
}
}  // namespace

ShaderModuleImpl::ShaderModuleImpl(ID3D11Device* device, ShaderStage stage, Data& data) : ShaderModule(stage, data)
{
    compileShader(device);
}

ShaderModuleImpl::~ShaderModuleImpl()
{
    SafeRelease(_shader);
    SafeRelease(_blob);
}

void ShaderModuleImpl::compileShader(ID3D11Device* device)
{
    ComPtr<ID3DBlob> errorBlob;
    UINT flags = D3DCOMPILE_OPTIMIZATION_LEVEL2 | D3DCOMPILE_ENABLE_STRICTNESS;
#if !defined(NDEBUG)
    flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
    HRESULT hr = D3DCompile(_codeSpan.data(), _codeSpan.size(), nullptr, nullptr, nullptr, "main",
                            stageToProfile(_stage), flags, 0, &_blob, &errorBlob);
    if (FAILED(hr))
    {
        std::string_view errorDetail =
            errorBlob ? std::string_view((const char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize())
                      : "Unknown compile error"sv;
        AXLOGE("axmol:ERROR: Failed to compile shader, hr:{},{}", hr, errorDetail);
        AXASSERT(false, "Shader compile failed!");
        return;
    }

    if (_stage == ShaderStage::VERTEX)
        device->CreateVertexShader(_blob->GetBufferPointer(), _blob->GetBufferSize(), nullptr,
                                   (ID3D11VertexShader**)&_shader);
    else
        device->CreatePixelShader(_blob->GetBufferPointer(), _blob->GetBufferSize(), nullptr,
                                  (ID3D11PixelShader**)&_shader);
}

}  // namespace ax::rhi::d3d11
