/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
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

#include "axmol/rhi/RHITypes.h"
#include "axmol/base/Object.h"
#include "axmol/base/EventListenerCustom.h"
#include "axmol/platform/GL.h"
#include "axmol/rhi/Program.h"
#include "axmol/rhi/DriverBase.h"
#include "axmol/rhi/opengl/BufferGL.h"

#include <string>
#include <vector>
#include <unordered_map>

#include "axmol/tlx/pod_vector.hpp"

namespace ax::rhi::gl
{

class ShaderModuleImpl;

/**
 * Store attribute information.
 */
struct AttributeInfo
{
    unsigned int location         = 0;
    unsigned int size             = 0;
    GLenum type                   = GL_BYTE;
    GLsizei stride                = 0;
    unsigned int offset           = 0;
    GLboolean needToBeNormallized = GL_FALSE;
    std::string name;
};

/**
 * @addtogroup _opengl
 * @{
 */

#define MAX_UNIFORM_NAME_LENGTH 256

struct UniformBlockDesc
{
    UniformBlockDesc(BufferImpl* ubo, int loc, int size) : _ubo(ubo), _location(loc), _size(size) {}
    BufferImpl* _ubo;
    int _location;
    int _size;
#if !AX_64BITS
    int __padding;
#endif
};

/**
 * An OpenGL program.
 */
class ProgramImpl : public Program
{
public:
    /**
     * @param vertexShader Specifes the vertex shader source.
     * @param fragmentShader Specifes the fragment shader source.
     */
    ProgramImpl(std::string_view vertexShader, std::string_view fragmentShader);

    ~ProgramImpl();

    /**
     * Get attribute informations.
     * @return Attribute informations.
     */
    inline const std::vector<AttributeInfo>& getAttributeInfos() const { return _attributeInfos; }

    /**
     * Get program object.
     * @return Program object.
     */
    inline GLuint internalHandle() const { return _program; }

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
     * Get active vertex attributes.
     * @return Active vertex attributes. key is active attribute name, Value is corresponding attribute info.
     */
    const axstd::string_map<VertexInputDesc>& getActiveVertexInputs() const override;

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

    void bindUniformBuffers(const char* buffer, size_t bufferSize);

private:
    void compileProgram();

    void reflectVertexInputs();
    void reflectUniformInfos();

    void setBuiltinLocations();

    void clearUniformBuffers();

#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    void reloadProgram();
    int getMappedLocation(int location) const override;
    int getOriginalLocation(int location) const override;
    const std::unordered_map<std::string, int> getAllUniformsLocation() const override
    {
        return _originalUniformLocations;
    }
#endif

    GLuint _program                         = 0;
    ShaderModuleImpl* _vertexShaderModule   = nullptr;
    ShaderModuleImpl* _fragmentShaderModule = nullptr;

    axstd::pod_vector<UniformBlockDesc> _uniformBuffers;

    std::vector<AttributeInfo> _attributeInfos;

    axstd::string_map<VertexInputDesc> _activeVertexInputs;
    axstd::string_map<UniformInfo> _activeUniformInfos;

#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    std::unordered_map<std::string, int>
        _originalUniformLocations;  ///< record the uniform location when shader was first created.
    std::unordered_map<int, int> _mapToCurrentActiveLocation;  ///<
    std::unordered_map<int, int> _mapToOriginalLocation;       ///<
    EventListenerCustom* _backToForegroundListener = nullptr;
#endif

    std::size_t _totalBufferSize = 0;  // total uniform buffer size (all blocks)

    int _maxLocation = -1;

    const VertexInputDesc* _builtinVertxInputs[VertexInputKind::VIK_COUNT];
    UniformLocation _builtinUniformLocation[UNIFORM_COUNT];
};
// end of _opengl group
/// @}
}  // namespace ax::rhi::gl
