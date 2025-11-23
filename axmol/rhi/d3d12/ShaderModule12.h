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
#include "axmol/rhi/ShaderModule.h"
#include <d3d12.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <span>

namespace ax::rhi::d3d12
{
/**
 * @addtogroup _d3d12
 * @{
 */

class DriverImpl;

struct SLCReflectContext;

struct D3D12BlobHandle
{
    Microsoft::WRL::ComPtr<IUnknown> blob;
    std::span<uint8_t> view;
};

struct UniformBlockInfo
{
    int binding;          // Vulkan binding index
    uint32_t sizeBytes;   // total size of the UBO
    uint16_t numMembers;  // number of uniforms in this block
    std::string name;     // block name
};

/**
 * @brief A D3D12-based ShaderModule implementation
 *
 */
class ShaderModuleImpl : public ShaderModule
{
public:
    ShaderModuleImpl(DriverImpl* driver, ShaderStage stage, std::string_view source);
    ~ShaderModuleImpl();

    const D3D12BlobHandle& internalHandle() const { return _nativeHandle; }

    /**
     * Get uniform info by engine built-in uniform enum name.
     * @param name Specifies the engine built-in uniform enum name.
     * @return The uniform location.
     */
    const UniformInfo& getUniformInfo(Uniform name) const;

    /**
     * Get uniform info by name.
     * @param uniform Specifies the uniform name.
     * @return The uniform location.
     */
    const UniformInfo& getUniformInfo(std::string_view name) const;

    /**
     * Get attribute location by engine built-in attribute enum name.
     * @param name Specifies the engine built-in attribute enum name.
     * @return The attribute location.
     */
    const VertexInputDesc* getVertexInputDesc(VertexInputKind name) const;

    /**
     * Get attribute location by attribute name.
     * @param name Specifies the attribute name.
     * @return The attribute location.
     */
    const VertexInputDesc* getVertexInputDesc(std::string_view name) const;

    /**
     * Get active attribute informations.
     * @return Active attribute informations. key is attribute name and Value is corresponding attribute info.
     */
    inline const axstd::string_map<VertexInputDesc>& getActiveVertexInputs() const { return _activeVertexInputs; }

    /**
     * Get all uniformInfos.
     * @return The uniformInfos.
     */
    inline const axstd::string_map<UniformInfo>& getActiveUniformInfos() const { return _activeUniformInfos; }

    inline const std::vector<UniformBlockInfo>& getActiveUniformBlockInfos() const { return _activeUniformBlockInfos; }
    inline const std::vector<UniformInfo>& getActiveSamplerInfos() const { return _activeSamplerInfos; }

    /**
     * Get maximum uniform location.
     * @return Maximum uniform location.
     */
    inline const int getMaxLocation() const { return _maxLocation; }

    /**
     * Get uniform buffer size in bytes that holds all the uniforms.
     * @return The uniform buffer size.
     */
    inline std::size_t getUniformBufferSize() const { return _uniformBufferSize; }

private:
    void compileShader(DriverImpl* driver, ShaderStage stage, std::string_view source);
    void releaseShader();

    void reflectVertexInputs(SLCReflectContext* context);
    void reflectUniforms(SLCReflectContext* context);
    void reflectSamplers(SLCReflectContext* context);

    void setBuiltinLocations();

    D3D12BlobHandle _nativeHandle;

    axstd::string_map<VertexInputDesc> _activeVertexInputs;
    axstd::string_map<UniformInfo> _activeUniformInfos;

    std::vector<UniformBlockInfo> _activeUniformBlockInfos;
    std::vector<UniformInfo> _activeSamplerInfos;
    int _samplerCount{0};

    const VertexInputDesc* _builtinVertexInputs[VIK_COUNT];

    int _maxLocation = -1;
    UniformInfo _builtinUniforms[UNIFORM_COUNT];

    std::size_t _uniformBufferSize = 0;
};

/** @} */

}  // namespace ax::rhi::d3d12
