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
#include "axmol/rhi/vulkan/ShaderModuleVK.h"
#include "axmol/rhi/vulkan/DriverVK.h"
#include "axmol/base/Logging.h"
#include "yasio/ibstream.hpp"
#include "axmol/rhi/axslc-spec.h"
#include <cassert>
#include <algorithm>

namespace ax::rhi::vk
{

using namespace ::axslc;

namespace
{
static inline std::string_view _sc_read_name(yasio::fast_ibstream_view* ibs)
{
    // View bytes without copy
    std::string_view name = ibs->read_bytes(sizeof(sc_refl_input::name));
    auto len              = name.find_last_not_of('\0');
    assert(len != std::string::npos);  // name must not be empty
    name.remove_suffix(name.length() - len - 1);
    return name;
}
}  // namespace

struct SLCReflectContext
{
    sc_chunk_refl* refl;
    yasio::fast_ibstream_view* ibs;
};

ShaderModuleImpl::ShaderModuleImpl(VkDevice device, ShaderStage stage, std::string_view source) : ShaderModule(stage)
{
    compileShader(device, stage, source);
}

ShaderModuleImpl::~ShaderModuleImpl()
{
    releaseShader();
}

void ShaderModuleImpl::releaseShader()
{
    if (_shader)
    {
        // Device should be accessible from a driver; here we assume destruction is handled externally.
        // If needed, store VkDevice in this class like other implementations.
        // Intentionally left minimal: the lifetime is managed by the owning renderer/driver.
        auto device = static_cast<DriverImpl*>(axdrv)->getDevice();
        vkDestroyShaderModule(device, _shader, nullptr);
    }
}

void ShaderModuleImpl::compileShader(VkDevice device, ShaderStage stage, std::string_view source)
{
    std::fill(std::begin(_builtinVertexInputs), std::end(_builtinVertexInputs), nullptr);

    _maxLocation       = -1;
    _uniformBufferSize = 0;
    for (int i = 0; i < UNIFORM_COUNT; ++i)
        _builtinUniforms[i] = {};

    _samplerCount = 0;
    _activeVertexInputs.clear();
    _activeUniformInfos.clear();
    _activeUniformBlockInfos.clear();
    _activeSamplerInfos.clear();

    yasio::fast_ibstream_view ibs(source.data(), source.length());
    uint32_t fourccId = ibs.read<uint32_t>();
    if (fourccId != SC_CHUNK)
    {
        AXLOGE("axmol: Shader chunk header mismatch (expected SC_CHUNK).");
        assert(false);
        return;
    }

    const auto sc_size = ibs.read<uint32_t>();  // always 0 per spec, doesn't matter
    sc_chunk chunk{};
    ibs.read_bytes(&chunk, static_cast<int>(sizeof(chunk)));

    std::string_view shaderCodeBytes;

    do
    {
        fourccId = ibs.read<uint32_t>();
        if (fourccId != SC_CHUNK_STAG)
        {
            AXLOGE("axmol: Shader stage chunk missing (expected SC_CHUNK_STAG).");
            assert(false);
            return;
        }

        const auto stage_size = ibs.read<uint32_t>();
        const auto stage_id   = ibs.read<uint32_t>();

        ShaderStage ref_stage = static_cast<ShaderStage>(-1);
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
            shaderCodeBytes   = ibs.read_bytes(codeLen);
        }
        else
        {
            AXLOGE("axmol: No code/data chunk (SC_CHUNK_CODE/SC_CHUNK_DATA) found for shader stage.");
            assert(false);
        }

        if (!ibs.eof())
        {
            // Try to read reflection info
            fourccId = ibs.read<uint32_t>();
            if (fourccId == SC_CHUNK_REFL)
            {
                const auto refl_size        = ibs.read<uint32_t>();
                const auto refl_data_offset = ibs.tell();

                sc_chunk_refl refl{};
                ibs.advance(sizeof(refl.name));  // skip human-readable name
                refl.num_inputs          = ibs.read<uint32_t>();
                refl.num_textures        = ibs.read<uint32_t>();
                refl.num_uniform_buffers = ibs.read<uint32_t>();
                refl.num_storage_images  = ibs.read<uint32_t>();
                refl.num_storage_buffers = ibs.read<uint32_t>();

                // Skip fields we don't need beyond flatten_ubo start
                ibs.advance(sizeof(sc_chunk_refl) - offsetof(sc_chunk_refl, flatten_ubo));

                SLCReflectContext context{&refl, &ibs};

                // refl_inputs
                reflectVertexInputs(&context);

                // refl_uniformbuffers
                reflectUniforms(&context);

                // refl_textures
                reflectSamplers(&context);

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
    } while (false);

    // Create VkShaderModule from SPIR-V bytes
    if (shaderCodeBytes.empty())
    {
        AXLOGE("axmol: Shader code blob is empty.");
        assert(false);
        return;
    }

    VkShaderModuleCreateInfo smci{};
    smci.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    smci.codeSize = shaderCodeBytes.size();
    smci.pCode    = reinterpret_cast<const uint32_t*>(shaderCodeBytes.data());

    VkResult vr = vkCreateShaderModule(device, &smci, nullptr, &_shader);
    if (vr != VK_SUCCESS || _shader == VK_NULL_HANDLE)
    {
        AXLOGE("axmol: Failed to create VkShaderModule (VkResult={}).", (int)vr);
        assert(false);
        return;
    }

    setBuiltinLocations();
}

const UniformInfo& ShaderModuleImpl::getUniformInfo(Uniform name) const
{
    return _builtinUniforms[name];
}

const UniformInfo& ShaderModuleImpl::getUniformInfo(std::string_view name) const
{
    static UniformInfo none;
    auto iter = _activeUniformInfos.find(name);
    if (iter != _activeUniformInfos.end())
        return iter->second;
    return none;
}

const VertexInputDesc* ShaderModuleImpl::getVertexInputDesc(VertexInputKind name) const
{
    return _builtinVertexInputs[name];
}

const VertexInputDesc* ShaderModuleImpl::getVertexInputDesc(std::string_view name) const
{
    auto it = _activeVertexInputs.find(name);
    if (it != _activeVertexInputs.end())
        return &it->second;
    else
        return nullptr;
}

void ShaderModuleImpl::reflectVertexInputs(SLCReflectContext* context)
{
    auto ibs = context->ibs;

    for (int i = 0; i < context->refl->num_inputs; ++i)
    {
        std::string_view name     = _sc_read_name(ibs);
        auto loc                  = ibs->read<int32_t>();   // location in shader
        std::string_view semantic = _sc_read_name(ibs);     // semantic (engine-level)
        auto semantic_index       = ibs->read<uint32_t>();  // semantic index
        auto format               = ibs->read<uint32_t>();  // engine-level vertex format enum

        VertexInputDesc desc;
        desc.semantic = semantic;
        desc.location = loc;  // use explicit location
        desc.format   = format;
        _activeVertexInputs.emplace(name, desc);
    }
}

void ShaderModuleImpl::reflectUniforms(SLCReflectContext* context)
{
    _uniformBufferSize = 0;
    auto ibs           = context->ibs;

    for (int i = 0; i < context->refl->num_uniform_buffers; ++i)
    {
        auto ub_name       = ibs->read_bytes(sizeof(sc_refl_ub::name));
        auto ub_binding    = ibs->read<int32_t>();  // descriptor set/binding index
        auto ub_size_bytes = ibs->read<uint32_t>();
        ibs->advance(sizeof(sc_refl_ub::array_size));
        auto ub_num_members = ibs->read<uint16_t>();

        _activeUniformBlockInfos.push_back(UniformBlockInfo{
            .binding = ub_binding, .sizeBytes = ub_size_bytes, .numMembers = ub_num_members, .name = ub_name.data()});

        for (int k = 0; k < ub_num_members; ++k)
        {
            UniformInfo uniform;
            auto name       = _sc_read_name(ibs);
            auto offset     = ibs->read<int32_t>();
            auto format     = ibs->read<uint32_t>();  // uniform type
            auto size_bytes = ibs->read<uint32_t>();
            auto array_size = ibs->read<uint16_t>();

            uniform.count        = array_size;
            uniform.location     = ub_binding;  // binding index for Vulkan
            uniform.size         = size_bytes;
            uniform.bufferOffset = offset;
            uniform.type         = format;

            _activeUniformInfos[name] = uniform;

            if (_maxLocation < i)
                _maxLocation = (i + 1);
        }

        _uniformBufferSize = ub_size_bytes;

        // Current: we support single uniform block per stage in the legacy path; break for parity.
        break;
    }
}

void ShaderModuleImpl::reflectSamplers(SLCReflectContext* context)
{
    constexpr auto skip_fields_bytes = static_cast<ptrdiff_t>(sizeof(sc_refl_texture::image_dim) + sizeof(uint8_t));
    const auto samplerCount          = context->refl->num_textures;
    if (samplerCount <= 0)
        return;
    auto ibs = context->ibs;
    _activeSamplerInfos.reserve(samplerCount);
    for (int i = 0; i < samplerCount; ++i)
    {
        UniformInfo uniform{};

        std::string_view name = _sc_read_name(ibs);
        uniform.location      = ibs->read<int32_t>();  // sampler binding index
        ibs->advance(skip_fields_bytes);
        uniform.count = (std::max)(1, static_cast<int>(ibs->read<uint8_t>()));
        ibs->advance(sizeof(uint8_t));  // skip sampler_slot temporary

        auto ret = _activeUniformInfos.emplace(name, uniform);
        assert(ret.second);
        _activeSamplerInfos.push_back(uniform);
    }
}

void ShaderModuleImpl::setBuiltinLocations()
{
    /*--- Builtin Attributes ---*/

    // a_position
    _builtinVertexInputs[VertexInputKind::POSITION] = getVertexInputDesc(VERTEX_INPUT_NAME_POSITION);

    // a_color
    _builtinVertexInputs[VertexInputKind::COLOR] = getVertexInputDesc(VERTEX_INPUT_NAME_COLOR);

    // a_texCoord
    _builtinVertexInputs[VertexInputKind::TEXCOORD] = getVertexInputDesc(VERTEX_INPUT_NAME_TEXCOORD);

    // a_normal
    _builtinVertexInputs[VertexInputKind::NORMAL] = getVertexInputDesc(VERTEX_INPUT_NAME_NORMAL);

    // a_instance
    _builtinVertexInputs[VertexInputKind::INSTANCE] = getVertexInputDesc(VERTEX_INPUT_NAME_INSTANCE);

    /*--- Builtin Uniforms ---*/

    // u_MVPMatrix
    _builtinUniforms[Uniform::MVP_MATRIX] = getUniformInfo(UNIFORM_NAME_MVP_MATRIX);

    // u_tex0
    _builtinUniforms[Uniform::TEXTURE] = getUniformInfo(UNIFORM_NAME_TEXTURE);

    // u_tex1
    _builtinUniforms[Uniform::TEXTURE1] = getUniformInfo(UNIFORM_NAME_TEXTURE1);

    // u_textColor
    _builtinUniforms[Uniform::TEXT_COLOR] = getUniformInfo(UNIFORM_NAME_TEXT_COLOR);

    // u_effectColor
    _builtinUniforms[Uniform::EFFECT_COLOR] = getUniformInfo(UNIFORM_NAME_EFFECT_COLOR);

    // u_effectWidth
    _builtinUniforms[Uniform::EFFECT_WIDTH] = getUniformInfo(UNIFORM_NAME_EFFECT_WIDTH);

    // u_labelPass
    _builtinUniforms[Uniform::LABEL_PASS] = getUniformInfo(UNIFORM_NAME_LABEL_PASS);
}

}  // namespace ax::rhi::vk
