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
#include "axmol/rhi/Buffer.h"
#include "axmol/tlx/hlookup.hpp"
#include "axmol/tlx/inlined_vector.hpp"

#include <functional>
#include <vector>

namespace ax
{
class ProgramManager;

namespace rhi
{

class ShaderModule;
class VertexLayout;

enum Uniform : uint32_t
{
    MVP_MATRIX,
    TEXTURE,
    TEXTURE1,
    TEXTURE2,
    TEXTURE3,
    TEXT_COLOR,
    EFFECT_COLOR,
    EFFECT_WIDTH,
    LABEL_PASS,
    UNIFORM_COUNT  // Maximum uniforms
};

enum VertexInputKind : uint32_t
{
    POSITION,
    COLOR,
    TEXCOORD,
    TEXCOORD1,
    TEXCOORD2,
    TEXCOORD3,
    NORMAL,
    INSTANCE,
    VIK_COUNT  //
};

struct UniformBlockInfo
{
    int binding;          // Vulkan binding index
    uint32_t cpuOffset;   // offset in CPU buffer
    uint32_t sizeBytes;   // total size of the UBO
    uint16_t numMembers;  // number of uniforms in this block
    ShaderStage stage;
    std::string_view name;  // block name
};

struct SLCReflectContext;

/**
 * @addtogroup _rhi
 * @{
 */

using UniformLocationVector = tlx::inlined_vector<rhi::UniformLocation, 2>;
using UniformBufferVector   = tlx::inlined_vector<rhi::Buffer*, 2>;

inline constexpr int UNIFORM_NAME_BUFFER_SIZE = 128;

/**
 * A program.
 */
class AX_DLL Program : public Object
{
    friend class ::ax::ProgramManager;

protected:
    Program(Data& vsData, Data& fsData);

public:
    using UniformMap          = std::map<uint64_t, UniformInfo>;
    using TextureUniformEntry = std::pair<std::string_view, UniformInfo*>;

    virtual ~Program();

    /**
     * Get uniform location by name.
     * @param uniform Specifies the uniform name.
     * @return The uniform location.
     */
    UniformLocation getUniformLocation(std::string_view name) const;

    /**
     * Get uniform location by engine built-in uniform enum name.
     * @param name Specifies the engine built-in uniform enum name.
     * @return The uniform location.
     */
    UniformLocation getUniformLocation(rhi::Uniform kind) const;

    /**
     * @brief Retrieves all uniform locations associated with the given uniform name.
     *
     * This function resolves the uniform name to all reflected uniform entries
     * across all shader stages (e.g., vertex, fragment). A uniform name may map to
     * multiple locations when:
     *   - the same uniform appears in multiple stages,
     *   - the uniform is an array,
     *   - the uniform expands to multiple backend binding slots (e.g., samplers).
     *
     * The resolved locations are appended to the user‑provided @p out vector.
     * No memory is allocated internally; callers may reuse the same vector to
     * minimize allocations and improve cache locality.
     *
     * @param name The short uniform name (e.g., "u_color").
     * @param out  The output vector to which all matching uniform locations
     *             will be appended.
     *
     * @note This API complements the legacy getUniformLocation() method, which
     *       returns only the first matching location for backward compatibility.
     */
    void getUniformLocations(std::string_view name, UniformLocationVector& out) const;

    /**
     * Get uniform buffer size in bytes that can hold all the uniforms.
     * @param stage Specifies the shader stage. The symbolic constant can be either VERTEX or FRAGMENT.
     * @return The uniform buffer size in bytes.
     */
    std::size_t getUniformBufferSize() const;

    /**
     * Get attribute location by attribute name.
     * @param name Specifies the attribute name.
     * @return The attribute location.
     */
    const VertexInputDesc* getVertexInputDesc(std::string_view name) const;

    /**
     * Get attribute location by engine built-in attribute enum name.
     * @param name Specifies the engine built-in attribute enum name.
     * @return The attribute location.
     */
    const VertexInputDesc* getVertexInputDesc(rhi::VertexInputKind name) const
    {
        return _builtinVertexInputs[(int)name];
    }

    /**
     * Get active vertex attributes.
     * @return Active vertex attributes. key is active attribute name, Value is corresponding attribute info.
     */
    const tlx::string_map<VertexInputDesc>& getActiveVertexInputs() const { return _activeVertexInputs; }

    const std::vector<TextureUniformEntry>& getActiveTextureInfos() const { return _activeTextureInfos; };

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
     * Get all uniformInfos.
     * @return The uniformInfos.
     */
    const UniformMap& getActiveUniformInfos() const { return _activeUniformInfos; }

    const std::vector<UniformBlockInfo>& getActiveUniformBlockInfos() const { return _activeUniformBlockInfos; }

    VertexLayout* getVertexLayout() const { return _vertexLayout; }

    ShaderModule* getVSModule() const { return _vsModule; }
    ShaderModule* getFSModule() const { return _fsModule; }

protected:
    static uint64_t makeUniformNameKey(std::string_view name);
    static uint64_t makeTextureNameKey(std::string_view name);

    void setVertexLayout(VertexLayout* layout);

    void setProgramIds(uint32_t progType, uint64_t progId);

    const UniformInfo* getFirstUniformInfo(std::string_view name) const;

    UniformInfo& getUniformInfo(uint64_t id);

    void parseStageReflection(ShaderStage stage, SLCReflectContext* context);
    void resolveBuiltinBindings();

    void reflectVertexInputs(SLCReflectContext* context);
    void reflectUniforms(SLCReflectContext* context);
    void reflectSamplers(SLCReflectContext* context);

    void addShortNameMapping(std::string_view shortName, uint64_t reflectedId);

    VertexLayout* _vertexLayout{nullptr};
    uint32_t _programType = ProgramType::CUSTOM_PROGRAM;  ///< built-in program type, initial value is CUSTOM_PROGRAM.
    uint64_t _programId   = 0;

    ShaderModule* _vsModule = nullptr;
    ShaderModule* _fsModule = nullptr;

    // vertex inputs
    tlx::string_map<VertexInputDesc> _activeVertexInputs;

    // the active uniform infos
    UniformMap _activeUniformInfos;

    tlx::hash_map<std::string_view, tlx::pod_vector<uint64_t>> _shortNameToIds;

    // unstable textures reflection, we need copy and update runtimeLocation after link program every time
    std::vector<TextureUniformEntry> _activeTextureInfos;

    // Populated once from ShaderModule reflection at program creation.
    // Contains stable, backend‑independent metadata for all active uniform blocks.
    std::vector<UniformBlockInfo> _activeUniformBlockInfos;

    const UniformInfo* _builtinUniforms[UNIFORM_COUNT];
    tlx::pod_vector<const VertexInputDesc*> _builtinVertexInputs;

    // The total uniform buffer size
    std::size_t _uniformBufferSize{0};
};

// end of _rhi group
/// @}
}  // namespace rhi
}  // namespace ax
