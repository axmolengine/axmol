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

#include "axmol/rhi/ShaderModule.h"
#include "axmol/rhi/axslc-spec.h"
#include "yasio/ibstream.hpp"

namespace ax::rhi
{
using namespace ::axslc;
ShaderStage ShaderModule::getShaderStage() const
{
    return _stage;
}

ShaderModule::ShaderModule(ShaderStage stage, Data& data) : _stage(stage)
{
    _chunkData = std::move(data);
    parseShaderCode();
}

ShaderModule::~ShaderModule() {}

void ShaderModule::parseShaderCode(void)
{
    yasio::fast_ibstream_view ibs(_chunkData.data(), _chunkData.size());
    uint32_t fourccId = ibs.read<uint32_t>();
    if (fourccId != SC_CHUNK)
    {
        assert(false && "axmol: Not valid axslcc shader chunk");
        return;
    }
    // since 3.3.0, it should be match the whole shader data size
    auto sc_size = ibs.read<uint32_t>();
    struct sc_chunk chunk;
    ibs.read_bytes(&chunk, static_cast<int>(sizeof(chunk)));
    if (chunk.major < 3 || chunk.minor < 3)
    {
        AXLOGE(
            "The axslcc shader chunk version too old: found {}.{}, required >= 3.3, "
            "Please update/recompile the shader.",
            chunk.major, chunk.minor);
        assert(false && "axmol: Shader version too old");
    }

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

    assert(ref_stage == _stage && "Shader stage mismatch in axslc chunk");

    fourccId = ibs.read<uint32_t>();
    if (fourccId == SC_CHUNK_CODE || fourccId == SC_CHUNK_DATA)
    {
        // Expecting SPIR-V binary blob from axslc, not text
        const int codeLen           = ibs.read<int>();
        std::string_view shaderCode = ibs.read_bytes(codeLen);
        _codeSpan                   = std::span{(uint8_t*)shaderCode.data(), shaderCode.size()};
    }
    else
    {
        AXLOGE("axmol: No code/data chunk (SC_CHUNK_CODE/SC_CHUNK_DATA) found for shader stage.");
        assert(false);
    }
}

}  // namespace ax::rhi
