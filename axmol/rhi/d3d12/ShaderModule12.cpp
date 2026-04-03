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
#include "axmol/rhi/d3d12/ShaderModule12.h"
#include "axmol/rhi/d3d12/Driver12.h"
#include "axmol/platform/win32/ComPtr.h"
#include "axmol/base/Logging.h"

namespace ax::rhi::d3d12
{
ShaderModuleImpl::ShaderModuleImpl(DriverImpl* driver, ShaderStage stage, Data& chunk) : ShaderModule(stage, chunk)
{
    driver->compileShader(_codeSpan, stage, _nativeHandle);
}

ShaderModuleImpl::~ShaderModuleImpl()
{
    _nativeHandle.blob.Reset();
    _nativeHandle.view = {};
}

}  // namespace ax::rhi::d3d12
