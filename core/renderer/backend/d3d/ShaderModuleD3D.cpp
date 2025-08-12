// refer: https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-semantics
#include "ShaderModuleD3D.h"
#include <wrl/client.h>
#include "base/Logging.h"
#include "yasio/ibstream.hpp"
#include "axslcc/sgs-spec.h"

#pragma comment(lib, "d3dcompiler.lib")
// #pragma comment(lib, "dxguid.lib")

namespace ax::backend::d3d
{

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

static inline std::string_view _sgs_read_name(yasio::fast_ibstream_view* ibs)
{
    // view bytes without copy
    std::string_view name = ibs->read_bytes(sizeof(sgs_refl_input::name));
    auto len              = name.find_last_not_of('\0');
    assert(len != std::string::npos);  // name must not empty
    name.remove_suffix(name.length() - len - 1);
    return name;
}

}  // namespace

struct SLCReflectContext
{
    sgs_chunk_refl* refl;
    yasio::fast_ibstream_view* ibs;
};

ShaderModuleImpl::ShaderModuleImpl(ShaderStage stage, std::string_view source, ID3D11Device* device)
    : ShaderModule(stage)
{
    compileShader(stage, source, device);
}

ShaderModuleImpl::~ShaderModuleImpl()
{
    releaseShader();
}

void ShaderModuleImpl::releaseShader()
{
    if (_shader)
        _shader->Release();
    if (_blob)
        _blob->Release();

    _shader = nullptr;
    _blob   = nullptr;
}

void ShaderModuleImpl::compileShader(ShaderStage stage, std::string_view source, ID3D11Device* device)
{
    _activeUniformInfos.clear();
    _allVertexInputs.clear();
    std::fill(std::begin(_builtinVertexInputs), std::end(_builtinVertexInputs), nullptr);

    _maxLocation       = -1;
    _uniformBufferSize = 0;
    for (int i = 0; i < UNIFORM_MAX; ++i)
        _builtinUniforms[i] = {};

    yasio::fast_ibstream_view ibs(source.data(), source.length());
    uint32_t fourccId = ibs.read<uint32_t>();
    if (fourccId != SGS_CHUNK)
    {
        assert(false);
        return;
    }
    auto sgs_size = ibs.read<uint32_t>();  // always 0, doesn't matter
    struct sgs_chunk chunk;
    ibs.read_bytes(&chunk, static_cast<int>(sizeof(chunk)));

    std::string_view hlslCode;

    do
    {
        fourccId = ibs.read<uint32_t>();
        if (fourccId != SGS_CHUNK_STAG)
        {
            assert(false);
            return;  // error
        }
        auto stage_size       = ibs.read<uint32_t>();  // stage_size
        auto stage_id         = ibs.read<uint32_t>();  // stage_id
        ShaderStage ref_stage = (ShaderStage)-1;
        if (stage_id == SGS_STAGE_VERTEX)
            ref_stage = ShaderStage::VERTEX;
        else if (stage_id == SGS_STAGE_FRAGMENT)
            ref_stage = ShaderStage::FRAGMENT;

        assert(ref_stage == stage);

        fourccId      = ibs.read<uint32_t>();
        if (fourccId == SGS_CHUNK_CODE)
        {
            hlslCode  = ibs.read_bytes(ibs.read<int>());
        }
        else if (fourccId == SGS_CHUNK_DATA)
        {
            hlslCode  = ibs.read_bytes(ibs.read<int>());
        }
        else
        {
            // no text or binary code chunk
            assert(false);
        }

        if (!ibs.eof())
        {  // try read reflect info
            fourccId = ibs.read<uint32_t>();
            if (fourccId == SGS_CHUNK_REFL)
            {
                /*
                 REFL: Reflection data for the shader stage
                 struct sgs_chunk_refl: reflection data header
                 struct sgs_refl_input[]: array of vertex-shader input attributes (see sgs_chunk_refl for number of
                 inputs) struct sgs_refl_uniformbuffer[]: array of uniform buffer objects (see sgs_chunk_refl for number
                 of uniform buffers) struct sgs_refl_texture[]: array of texture objects (see sgs_chunk_refl for number
                 of textures) struct sgs_refl_texture[]: array of storage image objects (see sgs_chunk_refl for number
                 of storage images) struct sgs_refl_buffer[]: array of storage buffer objects (see sgs_chunk_refl for
                 number of storage buffers)
                 */
                const auto refl_size        = ibs.read<uint32_t>();
                const auto refl_data_offset = ibs.tell();
                sgs_chunk_refl refl;
                ibs.advance(sizeof(refl.name));
                refl.num_inputs          = ibs.read<uint32_t>();
                refl.num_textures        = ibs.read<uint32_t>();
                refl.num_uniform_buffers = ibs.read<uint32_t>();
                refl.num_storage_images  = ibs.read<uint32_t>();
                refl.num_storage_buffers = ibs.read<uint32_t>();

                // skip infos we don't need
                ibs.advance(sizeof(sgs_chunk_refl) - offsetof(sgs_chunk_refl, flatten_ubos));

                SLCReflectContext context{&refl, &ibs};

                // refl_inputs
                parseAttibute(&context);

                // refl_uniformbuffers
                parseUniform(&context);

                // refl_textures
                parseTexture(&context);

                // refl_storage_images: ignore
                ibs.advance(refl.num_storage_images * sizeof(sgs_refl_texture));

                // refl_storage_buffers: ignore
                ibs.advance(refl.num_storage_buffers * sizeof(sgs_refl_buffer));

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

    // 编译 HLSL
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(_DEBUG)
    flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
    HRESULT hr = D3DCompile(hlslCode.data(), hlslCode.size(), nullptr, nullptr, nullptr, "main", stageToProfile(stage),
                            flags, 0, &_blob, &errorBlob);
    if (FAILED(hr))
    {
        std::string_view errorDetail = errorBlob
                              ? std::string_view((const char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize())
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
    auto it = _allVertexInputs.find(name);
    if (it != _allVertexInputs.end())
        return &it->second;
    else
        return nullptr;
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

    /// u_effectType
    _builtinUniforms[Uniform::EFFECT_TYPE] = getUniformInfo(UNIFORM_NAME_EFFECT_TYPE);
}

void ShaderModuleImpl::parseAttibute(SLCReflectContext* context)
{
    auto ibs = context->ibs;

    for (int i = 0; i < context->refl->num_inputs; ++i)
    {
        std::string_view name     = _sgs_read_name(ibs);
        auto loc                  = ibs->read<int32_t>();
        std::string_view semantic = _sgs_read_name(ibs);
        auto semantic_index       = ibs->read<uint32_t>();
        auto format               = ibs->read<uint32_t>();

        VertexInputDesc desc;
        // desc.location = loc;
        desc.semantic = semantic;
        desc.location = semantic_index;
        desc.format   = format;
        // desc.count =
        _allVertexInputs.emplace(name, desc);
    }
}

void ShaderModuleImpl::parseUniform(SLCReflectContext* context)
{
    _uniformBufferSize = 0;
    auto ibs           = context->ibs;
    for (int i = 0; i < context->refl->num_uniform_buffers; ++i)
    {
        ibs->advance(sizeof(sgs_refl_ub::name));
        auto ub_binding    = ibs->read<int32_t>();
        auto ub_size_bytes = ibs->read<uint32_t>();
        ibs->advance(sizeof(sgs_refl_ub::array_size));
        auto ub_num_members = ibs->read<uint16_t>();

        for (int k = 0; k < ub_num_members; ++k)
        {
            UniformInfo uniform;
            auto name       = _sgs_read_name(ibs);
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

void ShaderModuleImpl::parseTexture(SLCReflectContext* context)
{
    auto ibs = context->ibs;
    for (int i = 0; i < context->refl->num_textures; ++i)
    {
        std::string_view name = _sgs_read_name(ibs);
        auto binding          = ibs->read<int32_t>();

        ibs->advance(sizeof(sgs_refl_texture) - offsetof(sgs_refl_texture, image_dim));

        UniformInfo uniform;
        uniform.location          = binding;
        uniform.bufferOffset      = -1;
        _activeUniformInfos[name] = uniform;
    }
}

}  // namespace ax::backend::d3d
