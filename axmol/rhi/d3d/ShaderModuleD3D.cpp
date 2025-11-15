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
// refer: https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-semantics
#include "axmol/rhi/d3d/ShaderModuleD3D.h"
#include "axmol/platform/win32/ComPtr.h"
#include "axmol/base/Logging.h"
#include "yasio/ibstream.hpp"
#include "axmol/rhi/axslc-spec.h"

#pragma comment(lib, "d3dcompiler.lib")
// #pragma comment(lib, "dxguid.lib")

namespace ax::rhi::d3d
{

using namespace ::axslc;

namespace
{
// ShaderStage -> profile
inline const char* stageToProfile(ShaderStage s)
{
    switch (s)
    {
    case ShaderStage::VERTEX:
        return "vs_5_0";
    case ShaderStage::FRAGMENT:
        return "ps_5_0";
    default:
        return "vs_5_0";
    }
}

static inline std::string_view _sc_read_name(yasio::fast_ibstream_view* ibs)
{
    // view bytes without copy
    std::string_view name = ibs->read_bytes(sizeof(sc_refl_input::name));
    auto len              = name.find_last_not_of('\0');
    assert(len != std::string::npos);  // name must not empty
    name.remove_suffix(name.length() - len - 1);
    return name;
}

}  // namespace

struct SLCReflectContext
{
    sc_chunk_refl* refl;
    yasio::fast_ibstream_view* ibs;
};

ShaderModuleImpl::ShaderModuleImpl(ID3D11Device* device, ShaderStage stage, std::string_view source)
    : ShaderModule(stage)
{
    compileShader(device, stage, source);
}

ShaderModuleImpl::~ShaderModuleImpl()
{
    releaseShader();
}

void ShaderModuleImpl::releaseShader()
{
    SafeRelease(_shader);
    SafeRelease(_blob);
}

void ShaderModuleImpl::compileShader(ID3D11Device* device, ShaderStage stage, std::string_view source)
{
    std::fill(std::begin(_builtinVertexInputs), std::end(_builtinVertexInputs), nullptr);

    _maxLocation       = -1;
    _uniformBufferSize = 0;
    for (int i = 0; i < UNIFORM_COUNT; ++i)
        _builtinUniforms[i] = {};

    yasio::fast_ibstream_view ibs(source.data(), source.length());
    uint32_t fourccId = ibs.read<uint32_t>();
    if (fourccId != SC_CHUNK)
    {
        assert(false);
        return;
    }
    auto sc_size = ibs.read<uint32_t>();  // always 0, doesn't matter
    struct sc_chunk chunk;
    ibs.read_bytes(&chunk, static_cast<int>(sizeof(chunk)));

    std::string_view shaderSource;

    do
    {
        fourccId = ibs.read<uint32_t>();
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

        assert(ref_stage == stage);

        fourccId = ibs.read<uint32_t>();
        if (fourccId == SC_CHUNK_CODE)
        {
            shaderSource = ibs.read_bytes(ibs.read<int>());
        }
        else if (fourccId == SC_CHUNK_DATA)
        {
            shaderSource = ibs.read_bytes(ibs.read<int>());
        }
        else
        {
            // no text or binary code chunk
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
                sc_chunk_refl refl;
                ibs.advance(sizeof(refl.name));
                refl.num_inputs          = ibs.read<uint32_t>();
                refl.num_textures        = ibs.read<uint32_t>();
                refl.num_uniform_buffers = ibs.read<uint32_t>();
                refl.num_storage_images  = ibs.read<uint32_t>();
                refl.num_storage_buffers = ibs.read<uint32_t>();

                // skip infos we don't need
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

                assert(ibs.tell() - refl_data_offset == refl_size);
            }
            else
            {
                assert(false);
                return;
            }
        }

        assert(ibs.eof());
    } while (false);  // iterator stages, current only 1 stage

    ComPtr<ID3DBlob> errorBlob;
    UINT flags = D3DCOMPILE_OPTIMIZATION_LEVEL2 | D3DCOMPILE_ENABLE_STRICTNESS;
#if !defined(NDEBUG)
    flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
    HRESULT hr = D3DCompile(shaderSource.data(), shaderSource.size(), nullptr, nullptr, nullptr, "main",
                            stageToProfile(stage), flags, 0, &_blob, &errorBlob);
    if (FAILED(hr))
    {
        std::string_view errorDetail =
            errorBlob ? std::string_view((const char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize())
                      : "Unknown compile error"sv;
        AXLOGE("axmol:ERROR: Failed to compile shader, hr:{},{}", hr, errorDetail);
        AXASSERT(false, "Shader compile failed!");
        return;
    }

    if (stage == ShaderStage::VERTEX)
        device->CreateVertexShader(_blob->GetBufferPointer(), _blob->GetBufferSize(), nullptr,
                                   (ID3D11VertexShader**)&_shader);
    else
        device->CreatePixelShader(_blob->GetBufferPointer(), _blob->GetBufferSize(), nullptr,
                                  (ID3D11PixelShader**)&_shader);

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
        auto loc                  = ibs->read<int32_t>();
        std::string_view semantic = _sc_read_name(ibs);
        auto semantic_index       = ibs->read<uint32_t>();
        auto format               = ibs->read<uint32_t>();

        VertexInputDesc desc;
        desc.semantic = semantic;
        desc.location = semantic_index;
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
        ibs->advance(sizeof(sc_refl_ub::name));
        auto ub_binding    = ibs->read<int32_t>();
        auto ub_size_bytes = ibs->read<uint32_t>();
        ibs->advance(sizeof(sc_refl_ub::array_size));
        auto ub_num_members = ibs->read<uint16_t>();

        for (int k = 0; k < ub_num_members; ++k)
        {
            UniformInfo uniform;
            auto name       = _sc_read_name(ibs);
            auto offset     = ibs->read<int32_t>();
            auto format     = ibs->read<uint32_t>();
            auto size_bytes = ibs->read<uint32_t>();
            auto array_size = ibs->read<uint16_t>();

            uniform.count             = array_size;
            uniform.location          = ub_binding;
            uniform.size              = size_bytes;
            uniform.bufferOffset      = offset;
            uniform.type              = format;
            _activeUniformInfos[name] = uniform;

            if (_maxLocation < i)
                _maxLocation = (i + 1);
        }
        _uniformBufferSize = ub_size_bytes;
        // current: only support 1 uniform block for d3d11
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
    for (int i = 0; i < context->refl->num_textures; ++i)
    {
        UniformInfo uniform{};
        std::string_view name = _sc_read_name(ibs);
        uniform.location      = ibs->read<int32_t>();  // sampler binding index
        ibs->advance(skip_fields_bytes);
        uniform.count = (std::max)(1, static_cast<int>(ibs->read<uint16_t>()));

        _activeUniformInfos[name] = uniform;
    }
}

void ShaderModuleImpl::setBuiltinLocations()
{
    /*--- Builtin Attribs ---*/

    /// a_position
    _builtinVertexInputs[VertexInputKind::POSITION] = getVertexInputDesc(VERTEX_INPUT_NAME_POSITION);

    /// a_color
    _builtinVertexInputs[VertexInputKind::COLOR] = getVertexInputDesc(VERTEX_INPUT_NAME_COLOR);

    /// a_texCoord
    _builtinVertexInputs[VertexInputKind::TEXCOORD] = getVertexInputDesc(VERTEX_INPUT_NAME_TEXCOORD);

    // a_normal
    _builtinVertexInputs[VertexInputKind::NORMAL] = getVertexInputDesc(VERTEX_INPUT_NAME_NORMAL);

    // a_instance
    _builtinVertexInputs[VertexInputKind::INSTANCE] = getVertexInputDesc(VERTEX_INPUT_NAME_INSTANCE);

    /*--- Builtin Uniforms ---*/

    /// u_MVPMatrix
    _builtinUniforms[Uniform::MVP_MATRIX] = getUniformInfo(UNIFORM_NAME_MVP_MATRIX);

    /// u_tex0
    _builtinUniforms[Uniform::TEXTURE] = getUniformInfo(UNIFORM_NAME_TEXTURE);

    /// u_tex1
    _builtinUniforms[Uniform::TEXTURE1] = getUniformInfo(UNIFORM_NAME_TEXTURE1);

    /// u_textColor
    _builtinUniforms[Uniform::TEXT_COLOR] = getUniformInfo(UNIFORM_NAME_TEXT_COLOR);

    /// u_effectColor
    _builtinUniforms[Uniform::EFFECT_COLOR] = getUniformInfo(UNIFORM_NAME_EFFECT_COLOR);

    /// u_effectWidth
    _builtinUniforms[Uniform::EFFECT_WIDTH] = getUniformInfo(UNIFORM_NAME_EFFECT_WIDTH);

    /// u_labelPass
    _builtinUniforms[Uniform::LABEL_PASS] = getUniformInfo(UNIFORM_NAME_LABEL_PASS);
}
}  // namespace ax::rhi::d3d
