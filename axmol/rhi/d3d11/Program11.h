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
#pragma once

#include "axmol/rhi/Program.h"

#include "axmol/rhi/d3d11/ShaderModule11.h"

/**
 * @addtogroup _d3d11
 * @{
 */

namespace ax::rhi::d3d11
{
/**
 * @brief A D3D11-based ShaderProgram implementation
 *
 */
class ProgramImpl : public Program
{
public:
    /// @name Constructor, Destructor and Initializers
    /**
     * @param vertexShader Specifes the vertex shader source.
     * @param fragmentShader Specifes the fragment shader source.
     */
    ProgramImpl(Data& vsData, Data& fsData);
    ~ProgramImpl() override;

    ID3DBlob* getVSBlob() const;

    inline void apply(ID3D11DeviceContext* context)
    {
        context->VSSetShader(
            static_cast<ID3D11VertexShader*>(static_cast<ShaderModuleImpl*>(_vsModule)->internalHandle()), nullptr, 0);
        context->PSSetShader(
            static_cast<ID3D11PixelShader*>(static_cast<ShaderModuleImpl*>(_fsModule)->internalHandle()), nullptr, 0);
    }

    void bindUniformBuffers(ID3D11DeviceContext*, const uint8_t* buffer, size_t bufferSize);

private:
    // The ubo instances
    UniformBufferVector _uniformBuffers;
};
}  // namespace ax::rhi::d3d11

/** @} */
