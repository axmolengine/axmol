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

#include "ShaderModuleMTL.h"
#include "DriverMTL.h"

#include "yasio/ibstream.hpp"
#include "yasio/sz.hpp"

#include "glslcc/sgs-spec.h"

NS_AX_BACKEND_BEGIN

struct SLCReflectContext
{
    sgs_chunk_refl* refl;
    yasio::fast_ibstream_view* ibs;
};

static inline std::string_view _sgs_read_name(yasio::fast_ibstream_view* ibs) {
    // view bytes without copy
    std::string_view name = ibs->read_bytes(sizeof(sgs_refl_input::name));
    auto len = name.find_last_not_of('\0');
    assert(len != std::string::npos); // name must not empty
    name.remove_suffix(name.length() - len - 1);
    return name;
}

ShaderModuleMTL::ShaderModuleMTL(id<MTLDevice> mtlDevice, ShaderStage stage, std::string_view source)
    : ShaderModule(stage)
{
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

    std::string_view mslCode;

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

        int code_size = 0;
        fourccId      = ibs.read<uint32_t>();
        if (fourccId == SGS_CHUNK_CODE)
        {
            code_size = ibs.read<int>();
            mslCode   = ibs.read_bytes(code_size);
        }
        else if (fourccId == SGS_CHUNK_DATA)
        {
            code_size = ibs.read<int>();
            mslCode   = ibs.read_bytes(code_size);
        }
        else
        {
            // no text or binary code chunk
            assert(false);
        }

        size_t refl_size = 0;
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
                const auto refl_size = ibs.read<uint32_t>();
                const auto refl_data_offset = ibs.tell();
                sgs_chunk_refl refl;
                ibs.advance(sizeof(refl.name));
                refl.num_inputs = ibs.read<uint32_t>();
                refl.num_textures = ibs.read<uint32_t>();
                refl.num_uniform_buffers = ibs.read<uint32_t>();
                refl.num_storage_images = ibs.read<uint32_t>();
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

    auto metalShader = mslCode.data();
    NSString* shader = [NSString stringWithUTF8String:metalShader];
    NSError* error;
    id<MTLLibrary> library = [mtlDevice newLibraryWithSource:shader options:nil error:&error];
    if (!library)
    {
        NSLog(@"Can not compile metal shader: %@", error);
        NSLog(@"%s", metalShader);
        assert(false);
        return;
    }

    _mtlFunction = [library newFunctionWithName:@"main0"];

    if (!_mtlFunction)
    {
        NSLog(@"metal shader is ---------------");
        NSLog(@"%s", metalShader);
        assert(false);
    }

    setBuiltinLocations();

    [library release];
}

ShaderModuleMTL::~ShaderModuleMTL()
{
    [_mtlFunction release];
}

void ShaderModuleMTL::parseAttibute(SLCReflectContext* context)
{
    auto ibs = context->ibs;

    for (int i = 0; i < context->refl->num_inputs; ++i)
    {
        std::string_view name = _sgs_read_name(ibs);
        auto loc = ibs->read<int32_t>();

        ibs->advance(sizeof(sgs_refl_input) - offsetof(sgs_refl_input, semantic));

        AttributeBindInfo attributeInfo;
        attributeInfo.location = loc;
        _attributeInfo[name]   = attributeInfo;
    }
}

void ShaderModuleMTL::parseUniform(SLCReflectContext* context)
{
    _uniformBufferSize = 0;
    auto ibs = context->ibs;
    for (int i = 0; i < context->refl->num_uniform_buffers; ++i)
    {
        ibs->advance(sizeof(sgs_refl_ub::name));
        auto ub_binding = ibs->read<int32_t>();
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

            uniform.count               = array_size;
            uniform.location            = ub_binding;
            uniform.size                = size_bytes;
            uniform.bufferOffset        = offset;
            uniform.type                = format;
            _activeUniformInfos[name]   = uniform;

            if (_maxLocation < i)
                _maxLocation = (i + 1);
        }
        _uniformBufferSize = ub_size_bytes;
        // current: only support 1 uniform block for metal
        break;
    }
}

void ShaderModuleMTL::parseTexture(SLCReflectContext* context)
{
    auto ibs = context->ibs;
    for (int i = 0; i < context->refl->num_textures; ++i)
    {
        std::string_view name = _sgs_read_name(ibs);
        auto binding = ibs->read<int32_t>();

        ibs->advance(sizeof(sgs_refl_texture) - offsetof(sgs_refl_texture, image_dim));

        UniformInfo uniform;
        uniform.location          = binding;
        uniform.bufferOffset      = -1;
        _activeUniformInfos[name] = uniform;
    }
}

const UniformInfo& ShaderModuleMTL::getUniformInfo(Uniform name) const
{
    return _builtinUniforms[name];
}

const UniformInfo& ShaderModuleMTL::getUniformInfo(std::string_view name) const
{
    static UniformInfo none;
    auto iter = _activeUniformInfos.find(name);
    if (iter != _activeUniformInfos.end())
        return iter->second;
    return none;
}

void ShaderModuleMTL::setBuiltinLocations()
{
    /*--- Builtin Attribs ---*/

    /// a_position
    _attributeLocation[Attribute::POSITION] = getAttributeLocation(ATTRIBUTE_NAME_POSITION);

    /// a_color
    _attributeLocation[Attribute::COLOR] = getAttributeLocation(ATTRIBUTE_NAME_COLOR);

    /// a_texCoord
    _attributeLocation[Attribute::TEXCOORD] = getAttributeLocation(ATTRIBUTE_NAME_TEXCOORD);

    // a_normal
    _attributeLocation[Attribute::NORMAL] = getAttributeLocation(ATTRIBUTE_NAME_NORMAL);

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

int ShaderModuleMTL::getAttributeLocation(Attribute name) const
{
    return _attributeLocation[name];
}

int ShaderModuleMTL::getAttributeLocation(std::string_view name)
{
    auto iter = _attributeInfo.find(name);
    if (iter != _attributeInfo.end())
        return _attributeInfo[name].location;
    else
        return -1;
}

NS_AX_BACKEND_END
