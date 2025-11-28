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
#include "axmol/platform/PlatformMacros.h"
#include "axmol/rhi/ShaderCache.h"
#include "axmol/tlx/hlookup.hpp"

#include <functional>
#include <vector>

namespace ax
{
class ProgramManager;

namespace rhi
{

class ShaderModule;
class VertexLayout;

/**
 * @addtogroup _rhi
 * @{
 */

/**
 * A program.
 */
class AX_DLL Program : public Object
{
    friend class ::ax::ProgramManager;

public:
    ~Program();

    /**
     * Get uniform location by name.
     * @param uniform Specifies the uniform name.
     * @return The uniform location.
     */
    virtual UniformLocation getUniformLocation(std::string_view uniform) const = 0;

    /**
     * Get uniform location by engine built-in uniform enum name.
     * @param name Specifies the engine built-in uniform enum name.
     * @return The uniform location.
     */
    virtual UniformLocation getUniformLocation(rhi::Uniform name) const = 0;

    /**
     * Get attribute location by attribute name.
     * @param name Specifies the attribute name.
     * @return The attribute location.
     */
    virtual const VertexInputDesc* getVertexInputDesc(std::string_view name) const = 0;

    /**
     * Get attribute location by engine built-in attribute enum name.
     * @param name Specifies the engine built-in attribute enum name.
     * @return The attribute location.
     */
    virtual const VertexInputDesc* getVertexInputDesc(rhi::VertexInputKind name) const = 0;

    /**
     * Get maximum vertex location.
     * @return Maximum vertex locaiton.
     */
    virtual int getMaxVertexLocation() const = 0;

    /**
     * Get maximum fragment location.
     * @return Maximum fragment location.
     */
    virtual int getMaxFragmentLocation() const = 0;

    /**
     * Get active vertex attributes.
     * @return Active vertex attributes. key is active attribute name, Value is corresponding attribute info.
     */
    virtual const axstd::string_map<VertexInputDesc>& getActiveVertexInputs() const = 0;

    /**
     * Get vertex shader.
     * @return Vertex shader.
     */
    std::string_view getVertexShaderSource() const { return _vsSource; }

    /**
     * Get fragment shader.
     * @ Fragment shader.
     */
    std::string_view getFragmentShaderSource() const { return _fsSource; }

    /**
     * Get engine built-in program type.
     * @return The built-in program type.
     */
    uint32_t getProgramType() const { return _programType; }

    /**
     * Get program id.
     * @return The program id.
     */
    uint64_t getProgramId() const { return _programId; }

    /**
     * Get uniform buffer size in bytes that can hold all the uniforms.
     * @param stage Specifies the shader stage. The symbolic constant can be either VERTEX or FRAGMENT.
     * @return The uniform buffer size in bytes.
     */
    virtual std::size_t getUniformBufferSize(ShaderStage stage) const = 0;

    /**
     * Get all uniformInfos.
     * @return The uniformInfos.
     */
    virtual const axstd::string_map<UniformInfo>& getActiveUniformInfos(ShaderStage stage) const = 0;

    VertexLayout* getVertexLayout() const { return _vertexLayout; }

protected:
    void setVertexLayout(VertexLayout* layout);

    void setProgramIds(uint32_t progType, uint64_t progId);

    /**
     * @param vs Specifes the vertex shader source.
     * @param fs Specifes the fragment shader source.
     */
    Program(std::string_view vs, std::string_view fs);

#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    /**
     * In case of EGL context lost, the engine will reload shaders. Thus location of uniform may changed.
     * The engine will maintain the relationship between the original uniform location and the current active uniform
     * location.
     * @param location Specifies original location.
     * @return Current active uniform location.
     * @see `int getOriginalLocation(int location) const`
     */
    virtual int getMappedLocation(int location) const = 0;

    /**
     * In case of EGL context lost, the engine will reload shaders. Thus location of uniform may changed.
     * The engine will maintain the relationship between the original uniform location and the current active uniform
     * location.
     * @param location Specifies the current active uniform location.
     * @return The original uniform location.
     * @see `int getMappedLocation(int location) const`
     */
    virtual int getOriginalLocation(int location) const = 0;

    /**
     * Get all uniform locations.
     * @return All uniform locations.
     */
    virtual const std::unordered_map<std::string, int> getAllUniformsLocation() const = 0;
    friend class ProgramState;
#endif

    std::string _vsSource;  ///< Vertex shader.
    std::string _fsSource;  ///< Fragment shader.
    VertexLayout* _vertexLayout{nullptr};
    uint32_t _programType = ProgramType::CUSTOM_PROGRAM;  ///< built-in program type, initial value is CUSTOM_PROGRAM.
    uint64_t _programId   = 0;
};

// end of _rhi group
/// @}
}  // namespace rhi
}  // namespace ax
