/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
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

    /**
     * @param csData Specifies the compute shader source.
     */
    explicit ProgramImpl(Data& csData);

    ~ProgramImpl() override;

    std::span<uint8_t> getVSBlob() const;

    inline void apply(ID3D11DeviceContext* context)
    {
        context->VSSetShader(
            static_cast<ID3D11VertexShader*>(static_cast<ShaderModuleImpl*>(_vsModule)->internalHandle()), nullptr, 0);
        context->PSSetShader(
            static_cast<ID3D11PixelShader*>(static_cast<ShaderModuleImpl*>(_fsModule)->internalHandle()), nullptr, 0);
    }

    inline void applyCompute(ID3D11DeviceContext* context)
    {
        context->CSSetShader(
            static_cast<ID3D11ComputeShader*>(static_cast<ShaderModuleImpl*>(_csModule)->internalHandle()), nullptr, 0);
    }

    void bindUniformBuffers(ID3D11DeviceContext*, const uint8_t* buffer, size_t bufferSize);

private:
    // The ubo instances
    UniformBufferVector _uniformBuffers;
};
}  // namespace ax::rhi::d3d11

/** @} */
