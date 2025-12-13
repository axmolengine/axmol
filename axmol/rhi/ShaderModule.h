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
#include "axmol/base/Data.h"

#include <span>
#include <string>

namespace ax::rhi
{

/**
 * @addtogroup _rhi
 * @{
 */

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

    inline const Data& getChunkData() const { return _chunkData; }

protected:
    ShaderModule(ShaderStage stage, Data& data);
    virtual ~ShaderModule();
    void setHashValue(uint64_t hash) { _hash = hash; }

    virtual void recompileShader() {};

    void parseShaderCode();

    friend class ShaderCache;
    ShaderStage _stage = ShaderStage::VERTEX;
    uint64_t _hash     = 0;

    Data _chunkData;               // owns the axslcc chunk
    std::span<uint8_t> _codeSpan;  // view into parsed shader code
};

// end of _rhi group
/// @}
}  // namespace ax::rhi
