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

#include "axmol/rhi/d3d/ShaderModuleD3D.h"

/**
 * @addtogroup _d3d
 * @{
 */

namespace ax::rhi::d3d
{
class BufferImpl;

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
    ProgramImpl(std::string_view vertexShader, std::string_view fragmentShader);
    ~ProgramImpl() override;

    /**
     * Get uniform location by name.
     * @param uniform Specifies the uniform name.
     * @return The uniform location.
     */
    UniformLocation getUniformLocation(std::string_view uniform) const override;

    /**
     * Get uniform location by engine built-in uniform enum name.
     * @param name Specifies the engine built-in uniform enum name.
     * @return The uniform location.
     */
    UniformLocation getUniformLocation(rhi::Uniform name) const override;

    /**
     * Get attribute location by attribute name.
     * @param name Specifies the attribute name.
     * @return The attribute location.
     */
    const VertexInputDesc* getVertexInputDesc(std::string_view name) const override;

    /**
     * Get attribute location by engine built-in attribute enum name.
     * @param name Specifies the engine built-in attribute enum name.
     * @return The attribute location.
     */
    const VertexInputDesc* getVertexInputDesc(VertexInputKind name) const override;

    /**
     * Get active vertex attributes.
     * @return Active vertex attributes. key is active attribute name, Value is corresponding attribute info.
     */
    const axstd::string_map<VertexInputDesc>& getActiveVertexInputs() const override;

    /**
     * Get vertex shader module.
     * @return Vertex shader module.
     */
    ShaderModuleImpl* getVertexShader() const { return _vertexShader; }

    /**
     * Get fragment shader module.
     * @ Fragment shader module.
     */
    ShaderModuleImpl* getFragmentShader() const { return _fragmentShader; }

    /**
     * Get maximum vertex location.
     * @return Maximum vertex locaiton.
     */
    int getMaxVertexLocation() const override;

    /**
     * Get maximum fragment location.
     * @return Maximum fragment location.
     */
    int getMaxFragmentLocation() const override;

    /**
     * Get uniform buffer size in bytes that can hold all the uniforms.
     * @param stage Specifies the shader stage. The symbolic constant can be either VERTEX or FRAGMENT.
     * @return The uniform buffer size in bytes.
     */
    std::size_t getUniformBufferSize(ShaderStage stage) const override;

    /**
     * Get all uniformInfos.
     * @return The uniformInfos.
     */
    const axstd::string_map<UniformInfo>& getActiveUniformInfos(ShaderStage stage) const override;

    ID3DBlob* getVSBlob() const;

    void bindVertexUniformBuffer(ID3D11DeviceContext*, const char* data, size_t size, UINT bindingIndex);
    void bindFragmentUniformBuffer(ID3D11DeviceContext*, const char* data, size_t size, UINT bindingIndex);

private:
    ShaderModuleImpl* _vertexShader   = nullptr;
    ShaderModuleImpl* _fragmentShader = nullptr;

    BufferImpl* _vsUniformBuffer{nullptr};
    BufferImpl* _fsUniformBuffer{nullptr};
};
}  // namespace ax::rhi::d3d

/** @} */
