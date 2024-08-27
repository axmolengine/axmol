/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.

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

#include "Macros.h"
#include "base/Object.h"
#include "platform/PlatformMacros.h"
#include "Types.h"
#include "ShaderCache.h"

#include <functional>
#include <string>
#include <vector>
#include <unordered_map>

NS_AX_BACKEND_BEGIN

class ShaderModule;
class VertexLayout;
class ProgramManager;

/**
 * @addtogroup _backend
 * @{
 */

enum class VertexLayoutType
{
    Unspec,      // needs binding after program load
    Pos,         // V2F
    Texture,     // T2F
    Sprite,      // V3F_C4B_T2F posTexColor
    DrawNode,    // V2F_C4B_T2F
    DrawNode3D,  // V3F_C4B
    SkyBox,      // V3F
    PU3D,        // V3F_C4B_T2F // same with sprite, TODO: reuse spriete
    posColor,    // V3F_C4B
    Terrain3D,   // V3F_T2F_V3F
    Count
};

/**
 * @addtogroup _backend
 * @{
 */

/**
 * A program.
 */
class AX_DLL Program : public Object
{
public:
    ~Program();
    /**
     * Get engine built-in program.
     * @param type Specifies the built-in program type.
     */
    static Program* getBuiltinProgram(uint32_t type);

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
    virtual UniformLocation getUniformLocation(backend::Uniform name) const = 0;

    /**
     * Get attribute location by attribute name.
     * @param name Specifies the attribute name.
     * @return The attribute location.
     */
    virtual int getAttributeLocation(std::string_view name) const = 0;

    /**
     * Get attribute location by engine built-in attribute enum name.
     * @param name Specifies the engine built-in attribute enum name.
     * @return The attribute location.
     */
    virtual int getAttributeLocation(backend::Attribute name) const = 0;

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
    virtual const hlookup::string_map<AttributeBindInfo>& getActiveAttributes() const = 0;

    /**
     * Get vertex shader.
     * @return Vertex shader.
     */
    std::string_view getVertexShader() const { return _vertexShader; }

    /**
     * Get fragment shader.
     * @ Fragment shader.
     */
    std::string_view getFragmentShader() const { return _fragmentShader; }

    /**
    * Sets the program shared vertex layout type, see: VertexLayoutType
    */
    void setupVertexLayout(VertexLayoutType vlt);

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
    virtual const hlookup::string_map<UniformInfo>& getAllActiveUniformInfo(ShaderStage stage) const = 0;

    inline VertexLayout* getVertexLayout() const { return _vertexLayout; }

protected:

    void setProgramIds(uint32_t progType, uint64_t progId);

    /**
     * @param vs Specifes the vertex shader source.
     * @param fs Specifes the fragment shader source.
     */
    Program(std::string_view vs, std::string_view fs);

#if AX_ENABLE_CACHE_TEXTURE_DATA
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
    friend class ProgramManager;

    std::string _vertexShader;                            ///< Vertex shader.
    std::string _fragmentShader;                          ///< Fragment shader.
    VertexLayout* _vertexLayout = nullptr;
    uint32_t _programType = ProgramType::CUSTOM_PROGRAM;  ///< built-in program type, initial value is CUSTOM_PROGRAM.
    uint64_t _programId   = 0;

    using VERTEX_LAYOUT_SETUP_FUNC = std::function<void(Program*)>;

    static std::function<void(Program*)> s_vertexLayoutSetupList[static_cast<int>(VertexLayoutType::Count)];
};

// end of _backend group
/// @}
NS_AX_BACKEND_END
