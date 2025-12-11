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

#include "axmol/rhi/RHITypes.h"
#include "axmol/base/Object.h"
#include "axmol/tlx/hlookup.hpp"
#include "axmol/tlx/vector.hpp"

#include <string>

namespace ax::rhi
{

/**
 * @addtogroup _rhi
 * @{
 */

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
    uint32_t sizeBytes;   // total size of the UBO
    uint16_t numMembers;  // number of uniforms in this block
    std::string name;     // block name
};

struct SLCReflectContext;

/**
 * Create shader.
 */
class AX_DLL ShaderModule : public ax::Object
{
public:
    /**
     * Get shader stage. The symbolic constant can be either VERTEX or FRAGMENT.
     * @return Shader stage.
     */
    ShaderStage getShaderStage() const;

    uint64_t getHashValue() const { return _hash; }

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
    inline const tlx::string_map<VertexInputDesc>& getActiveVertexInputs() const { return _activeVertexInputs; }

    /**
     * Get all uniformInfos.
     * @return The uniformInfos.
     */
    inline const tlx::string_map<UniformInfo>& getActiveUniformInfos() const { return _activeUniformInfos; }

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

protected:
    ShaderModule(ShaderStage stage);
    virtual ~ShaderModule();
    void setHashValue(uint64_t hash) { _hash = hash; }

    std::string_view parseReflection(std::string_view source);

    void reflectVertexInputs(SLCReflectContext* context);
    void reflectUniforms(SLCReflectContext* context);
    void reflectSamplers(SLCReflectContext* context);

    void setBuiltinLocations();

    friend class ShaderCache;
    ShaderStage _stage = ShaderStage::VERTEX;
    uint64_t _hash     = 0;

    tlx::string_map<VertexInputDesc> _activeVertexInputs;
    tlx::string_map<UniformInfo> _activeUniformInfos;
    std::vector<UniformBlockInfo> _activeUniformBlockInfos;
    std::vector<UniformInfo> _activeSamplerInfos;
    UniformInfo _builtinUniforms[UNIFORM_COUNT];
    tlx::pod_vector<const VertexInputDesc*> _builtinVertexInputs;
    int _maxLocation{-1};
    std::size_t _uniformBufferSize{0};
};

// end of _rhi group
/// @}
}  // namespace ax::rhi
