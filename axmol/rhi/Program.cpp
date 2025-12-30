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

#include "axmol/rhi/Program.h"
#include "axmol/renderer/VertexLayoutManager.h"
#include "axmol/rhi/axslc-spec.h"
#include "axmol/tlx/hash.hpp"
#include "yasio/ibstream.hpp"

namespace ax::rhi
{

using namespace ::axslc;

namespace
{
static inline std::string_view _sc_read_name(yasio::fast_ibstream_view* ibs)
{
    // view bytes without copy
    std::string_view name = ibs->read_bytes(SC_NAME_LEN);
    auto len              = name.find_last_not_of('\0');
    assert(len != std::string::npos);  // name must not empty
    name.remove_suffix(name.length() - len - 1);
    return name;
}

}  // namespace

struct SLCReflectContext
{
    uint32_t _cpuOffset{0};
    yasio::fast_ibstream_view* ibs{nullptr};
    sc_chunk_refl* refl{nullptr};
    ShaderStage stage{ShaderStage::UNKNOWN};
};

Program::Program(Data& vsData, Data& fsData)
{
    auto shaderCache = ShaderCache::getInstance();
    _vsModule        = shaderCache->acquireVertexShaderModule(vsData);
    _fsModule        = shaderCache->acquireFragmentShaderModule(fsData);

    SLCReflectContext context{};
    parseStageReflection(ShaderStage::VERTEX, &context);
    parseStageReflection(ShaderStage::FRAGMENT, &context);

    resolveBuiltinBindings();

#if AX_RENDER_API == AX_RENDER_API_D3D12
    if (_activeUniformBlockInfos.size() > 1)
    {
        std::sort(_activeUniformBlockInfos.begin(), _activeUniformBlockInfos.end(),
                  [](auto& a, auto& b) { return a.binding < b.binding; });
    }
#endif
}

Program::~Program()
{
    AX_SAFE_RELEASE(_vsModule);
    AX_SAFE_RELEASE(_fsModule);
    AX_SAFE_RELEASE(_vertexLayout);
}

void Program::setProgramIds(uint32_t progType, uint64_t progId)
{
    _programType = progType;
    _programId   = progId;
}

void Program::setVertexLayout(VertexLayout* layout)
{
    Object::assign(_vertexLayout, layout);
}

const VertexInputDesc* Program::getVertexInputDesc(std::string_view name) const
{
    auto it = _activeVertexInputs.find(name);
    if (it != _activeVertexInputs.end())
        return &it->second;
    else
        return nullptr;
}

UniformLocation Program::getUniformLocation(rhi::Uniform kind) const
{
    auto info = _builtinUniforms[kind];
    if (info)
        return UniformLocation{.location        = info->location,
                               .runtimeLocation = info->runtimeLocation,
                               .offset          = info->offset,
                               .cpuOffset       = info->cpuOffset};

    return UniformLocation{};
}

UniformLocation Program::getUniformLocation(std::string_view name) const
{
    auto it = _shortNameToIds.find(name);
    if (it != _shortNameToIds.end())
    {
        assert(!it->second.empty());
        auto& uniformInfo = _activeUniformInfos.at(it->second[0]);
        return UniformLocation{.location        = uniformInfo.location,
                               .runtimeLocation = uniformInfo.runtimeLocation,
                               .offset          = uniformInfo.offset,
                               .cpuOffset       = uniformInfo.cpuOffset};
    }

    return UniformLocation{};
}

void Program::getUniformLocations(std::string_view name, UniformLocationVector& out) const
{
    auto it = _shortNameToIds.find(name);
    if (it == _shortNameToIds.end())
        return;

    out.clear();

    auto& reflectedIds = it->second;
    for (auto reflectedId : reflectedIds)
    {
        const auto& info = _activeUniformInfos.at(reflectedId);
        out.push_back(rhi::UniformLocation{
            .location        = info.location,
            .runtimeLocation = info.runtimeLocation,
            .offset          = info.offset,
            .cpuOffset       = info.cpuOffset,
        });
    }
}

const UniformInfo* Program::getFirstUniformInfo(std::string_view name) const
{
    auto it = _shortNameToIds.find(name);
    if (it != _shortNameToIds.end())
    {
        assert(!it->second.empty());
        return &_activeUniformInfos.at(it->second[0]);
    }
    return nullptr;
}

UniformInfo& Program::getUniformInfo(uint64_t id)
{
    return _activeUniformInfos.at(id);
}

std::size_t Program::getUniformBufferSize() const
{
    return _uniformBufferSize;
}

void Program::parseStageReflection(ShaderStage stage, SLCReflectContext* context)
{
    const auto& shaderData = stage == ShaderStage::VERTEX ? _vsModule->getChunkData() : _fsModule->getChunkData();
    yasio::fast_ibstream_view ibs(shaderData.data(), shaderData.size());
    context->ibs   = &ibs;
    context->stage = stage;
    // shader module already verify shader source, just advance
    ibs.advance(sizeof(uint32_t));  // skip fourcc
    // since 3.3.0, it should be match the whole shader data size
    ibs.advance(sizeof(uint32_t));  // skip sc_size
    struct sc_chunk chunk;
    ibs.advance(sizeof(sc_chunk));  // skip header

    auto fourccId = ibs.read<uint32_t>();
    if (fourccId != SC_CHUNK_STAG)
    {
        assert(false);
        return;  // error
    }
    auto stage_size       = ibs.read<uint32_t>();  // stage_size
    auto stage_id         = ibs.read<uint32_t>();  // stage_id
    ShaderStage ref_stage = (ShaderStage)-1;
    if (stage_id == SC_STAGE_VERTEX)
        ref_stage = ShaderStage::VERTEX;
    else if (stage_id == SC_STAGE_FRAGMENT)
        ref_stage = ShaderStage::FRAGMENT;

    assert(ref_stage == stage && "Shader stage mismatch in axslc chunk");

    fourccId = ibs.read<uint32_t>();
    if (fourccId == SC_CHUNK_CODE || fourccId == SC_CHUNK_DATA)
    {
        // Expecting SPIR-V binary blob from axslc, not text
        const int codeLen = ibs.read<int>();
        ibs.advance(codeLen);  // skip shader code
    }
    else
    {
        AXLOGE("axmol: No code/data chunk (SC_CHUNK_CODE/SC_CHUNK_DATA) found for shader stage.");
        assert(false);
    }

    if (!ibs.eof())
    {  // try read reflect info
        fourccId = ibs.read<uint32_t>();
        if (fourccId == SC_CHUNK_REFL)
        {
            /*
             REFL: Reflection data for the shader stage
             struct sc_chunk_refl: reflection data header
             struct sc_refl_input[]: array of vertex-shader input attributes (see sc_chunk_refl for number of
             inputs) struct sc_refl_uniformbuffer[]: array of uniform buffer objects (see sc_chunk_refl for number
             of uniform buffers) struct sc_refl_texture[]: array of texture objects (see sc_chunk_refl for number
             of textures) struct sc_refl_texture[]: array of storage image objects (see sc_chunk_refl for number
             of storage images) struct sc_refl_buffer[]: array of storage buffer objects (see sc_chunk_refl for
             number of storage buffers)
             */
            const auto refl_size        = ibs.read<uint32_t>();
            const auto refl_data_offset = ibs.tell();
            sc_chunk_refl refl{};
            ibs.advance(sizeof(refl.name));
            refl.num_inputs          = ibs.read<uint32_t>();
            refl.num_textures        = ibs.read<uint32_t>();
            refl.num_uniform_buffers = ibs.read<uint32_t>();
            refl.num_storage_images  = ibs.read<uint32_t>();
            refl.num_storage_buffers = ibs.read<uint32_t>();

            // skip infos we don't need
            ibs.advance(sizeof(sc_chunk_refl) - offsetof(sc_chunk_refl, flatten_ubo));

            // SLCReflectContext context{&refl, &ibs};
            // context
            context->refl = &refl;

            // refl_inputs
            reflectVertexInputs(context);

            // refl_uniformbuffers
            reflectUniforms(context);

            // refl_textures
            reflectSamplers(context);

            // refl_storage_images: ignore
            ibs.advance(refl.num_storage_images * sizeof(sc_refl_texture));

            // refl_storage_buffers: ignore
            ibs.advance(refl.num_storage_buffers * sizeof(sc_refl_buffer));

            assert(ibs.tell() - refl_data_offset == refl_size && "Reflection chunk size mismatch");
        }
        else
        {
            AXLOGE("axmol: Missing reflection chunk (SC_CHUNK_REFL).");
            assert(false);
            return;
        }
    }

    assert(ibs.eof());
}

void Program::reflectVertexInputs(SLCReflectContext* context)
{
    auto ibs = context->ibs;

    for (int i = 0; i < context->refl->num_inputs; ++i)
    {
        std::string_view name     = _sc_read_name(ibs);
        std::string_view semantic = _sc_read_name(ibs);
        auto location             = ibs->read<int32_t>();
        auto semantic_index       = ibs->read<uint16_t>();
        auto var_type             = ibs->read<uint16_t>();

        VertexInputDesc desc;
        desc.semantic = semantic;
#if AX_RENDER_API == AX_RENDER_API_D3D11 || AX_RENDER_API == AX_RENDER_API_D3D12
        desc.location = semantic_index;
#else
        desc.location = location;
#endif
        desc.varType = var_type;
        _activeVertexInputs.emplace(name, desc);
    }
}

void Program::reflectUniforms(SLCReflectContext* context)
{
    auto ibs = context->ibs;
    char nameBuffer[UNIFORM_NAME_BUFFER_SIZE];
    for (int i = 0; i < context->refl->num_uniform_buffers; ++i)
    {
        auto ubName      = _sc_read_name(ibs);
        auto ubBinding   = ibs->read<int32_t>();  // descriptor set/binding index
        auto ubSizeBytes = ibs->read<uint32_t>();
        ibs->advance(sizeof(sc_refl_ub::array_size));
        auto ubNumOfMembers = ibs->read<uint16_t>();

        const auto cpuOffset = context->_cpuOffset;
        context->_cpuOffset += ubSizeBytes;
        _activeUniformBlockInfos.push_back(UniformBlockInfo{.binding    = ubBinding,
                                                            .cpuOffset  = cpuOffset,
                                                            .sizeBytes  = ubSizeBytes,
                                                            .numMembers = ubNumOfMembers,
                                                            .stage      = context->stage,
                                                            .name       = ubName.data()});

        size_t ubNameLen      = tlx::strlcpy(nameBuffer, sizeof(nameBuffer), ubName);
        nameBuffer[ubNameLen] = '.';
        for (int k = 0; k < ubNumOfMembers; ++k)
        {
            UniformInfo uniform;
            auto name = _sc_read_name(ibs);

            auto shortNameLen = tlx::strlcpy(nameBuffer + ubNameLen + 1, sizeof(nameBuffer) - (ubNameLen + 1), name);

            const std::string_view uniformFullName(nameBuffer, ubNameLen + shortNameLen + 1);
            auto offset    = ibs->read<int32_t>();
            auto sizeBytes = static_cast<uint16_t>(ibs->read<uint32_t>());
            auto arraySize = ibs->read<uint16_t>();
            auto varType   = ibs->read<uint16_t>();

            uniform.count           = arraySize;
            uniform.location        = ubBinding;
            uniform.runtimeLocation = ubBinding;
            uniform.sizeBytes       = sizeBytes;
            uniform.offset          = offset;
            uniform.cpuOffset       = cpuOffset;
            uniform.varType         = varType;

            const auto reflectedId = makeUniformNameKey(uniformFullName);
            auto ret               = _activeUniformInfos.emplace(reflectedId, uniform);
            assert(ret.second);

            addShortNameMapping(name, reflectedId);
        }
        _uniformBufferSize += ubSizeBytes;
    }
}

void Program::reflectSamplers(SLCReflectContext* context)
{
    const auto samplerCount = context->refl->num_textures;
    if (samplerCount <= 0)
        return;
    auto ibs = context->ibs;
    _activeTextureInfos.reserve(samplerCount);
    for (int i = 0; i < samplerCount; ++i)
    {
        UniformInfo uniform{};

        std::string_view name   = _sc_read_name(ibs);
        uniform.location        = ibs->read<int32_t>();  // sampler binding index
        uniform.runtimeLocation = uniform.location;
        const auto imageDim     = ibs->read<uint8_t>();
        uniform.varType         = SC_TYPE_HALF + imageDim;
        ibs->advance(static_cast<ptrdiff_t>(sizeof(uint8_t)));  // skip bits: multisample, arrayed, reserved
        uniform.count       = (std::max)(1, static_cast<int>(ibs->read<uint8_t>()));
        uniform.samplerSlot = ibs->read<uint8_t>();

        const auto reflectedId = makeTextureNameKey(name);
        auto ret               = _activeUniformInfos.emplace(reflectedId, uniform);
        assert(ret.second);
        addShortNameMapping(name, reflectedId);

        _activeTextureInfos.emplace_back(name, &ret.first->second);
    }
#if AX_RENDER_API == AX_RENDER_API_D3D12
    if (_activeTextureInfos.size() > 1)
    {
        // Important:
        // In D3D11/D3D12, the order in which descriptor ranges are declared
        // determines their linear layout inside the root signature's descriptor table.
        // The GPU does not automatically reorder them by register number (t#).
        // If the ranges are added in a different order than the shader's register
        // declarations, the linear indices will not match the expected t# slots,
        // causing resources to be bound incorrectly (e.g. a texture ending up in
        // another array slot).
        // To avoid this, we sort by 'location' (the register index) so that the
        // SRV ranges are declared in ascending order, matching the shader layout.
        std::sort(_activeTextureInfos.begin(), _activeTextureInfos.end(),
                  [](auto& a, auto& b) { return a.second->location < b.second->location; });
    }
#endif
}

void Program::resolveBuiltinBindings()
{
    std::fill(std::begin(_builtinVertexInputs), std::end(_builtinVertexInputs), nullptr);
    for (int i = 0; i < UNIFORM_COUNT; ++i)
        _builtinUniforms[i] = {};

    /*--- Builtin Attribs ---*/

    _builtinVertexInputs.resize(VertexInputKind::VIK_COUNT, nullptr);

    /// a_position
    _builtinVertexInputs[VertexInputKind::POSITION] = getVertexInputDesc(VERTEX_INPUT_NAME_POSITION);

    /// a_color
    _builtinVertexInputs[VertexInputKind::COLOR] = getVertexInputDesc(VERTEX_INPUT_NAME_COLOR);

    /// a_texCoord
    _builtinVertexInputs[VertexInputKind::TEXCOORD] = getVertexInputDesc(VERTEX_INPUT_NAME_TEXCOORD);

    // a_normal
    _builtinVertexInputs[VertexInputKind::NORMAL] = getVertexInputDesc(VERTEX_INPUT_NAME_NORMAL);

    // a_instance, metal use SSUBO(Shader Storage Uniform Buffer) before axmol-3.0.0
    _builtinVertexInputs[VertexInputKind::INSTANCE] = getVertexInputDesc(VERTEX_INPUT_NAME_INSTANCE);

    /*--- Builtin Uniforms ---*/

    /// u_MVPMatrix
    _builtinUniforms[Uniform::MVP_MATRIX] = getFirstUniformInfo(UNIFORM_NAME_MVP_MATRIX);

    /// u_tex0
    _builtinUniforms[Uniform::TEXTURE] = getFirstUniformInfo(UNIFORM_NAME_TEXTURE);

    /// u_tex1
    _builtinUniforms[Uniform::TEXTURE1] = getFirstUniformInfo(UNIFORM_NAME_TEXTURE1);

    /// u_textColor
    _builtinUniforms[Uniform::TEXT_COLOR] = getFirstUniformInfo(UNIFORM_NAME_TEXT_COLOR);

    /// u_effectColor
    _builtinUniforms[Uniform::EFFECT_COLOR] = getFirstUniformInfo(UNIFORM_NAME_EFFECT_COLOR);

    /// u_effectWidth
    _builtinUniforms[Uniform::EFFECT_WIDTH] = getFirstUniformInfo(UNIFORM_NAME_EFFECT_WIDTH);

    /// u_labelPass
    _builtinUniforms[Uniform::LABEL_PASS] = getFirstUniformInfo(UNIFORM_NAME_LABEL_PASS);
}

void Program::addShortNameMapping(std::string_view shortName, uint64_t reflectedId)
{
    auto it = _shortNameToIds.find(shortName);
    if (it == _shortNameToIds.end()) [[likely]]
    {
        _shortNameToIds.emplace(shortName, tlx::pod_vector<uint64_t>{reflectedId});
    }
    else
    {
        it->second.push_back(reflectedId);
    }
}

uint64_t Program::makeUniformNameKey(std::string_view name)
{
    return tlx::hash64_str(name, 0);
}

uint64_t Program::makeTextureNameKey(std::string_view name)
{
    return tlx::hash64_str(name, 1);
}

}  // namespace ax::rhi
