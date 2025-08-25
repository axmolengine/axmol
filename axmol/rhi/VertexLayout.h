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

#include <string>
#include <cstdint>
#include <vector>
#include <unordered_map>

namespace ax
{
class VertexLayoutManager;
}

namespace ax::rhi
{
class Program;
class VertexLayout;

enum class VertexLayoutKind
{
    Invalid = -1,
    Pos,         // V2F
    Texture,     // T2F
    PosUvColor,  // V3F_T2F_C4F
    Sprite,      // V3F_T2F_C4B
    Sprite2D,    // V2F_T2F_C4B
    DrawNode,    // V2F_T2F_C4F
    DrawNode3D,  // V3F_C4F
    SkyBox,      // V3F
    posColor,    // V3F_C4F
    Terrain3D,   // V3F_T2F_V3F
    Instanced,   // builtin instanced vertex format for 3D transform
    Count,
};

/**
 * @addtogroup _rhi
 * @{
 */

struct AX_DLL InputBindingDesc
{
    InputBindingDesc() = default;
    InputBindingDesc(std::string_view _semantic,
                     int _index,
                     VertexFormat _format,
                     unsigned int _offset,
                     bool needToBeNormallized,
                     uint8_t instanceStepRate);

    char semantic[32]        = {};  ///< semantic is used in d3d11
    unsigned int offset      = 0;
    int index                = 0;  ///< index is used in metal
    VertexFormat format      = VertexFormat::INT3;
    bool needToBeNormallized = false;
    uint8_t instanceStepRate = 0;
};

struct AX_DLL VertexLayoutDesc
{
    friend class VertexLayout;

    /**
     * Start to define vertex layout.
    */
    void startLayout(size_t capacity);

    /**
     * Set attribute values to name.
     * @param name Specifies the attribute name.
     * @param index Specifies the index of the generic vertex attribute to be modified.
     * @param format Specifies how the vertex attribute data is laid out in memory.
     * @param offset Specifies the byte offset to the first component of the first generic vertex attribute.
     * @param needToBeNormallized Specifies whether fixed-point data values should be normalized (true) or converted
     * directly as fixed-point values (false) when they are accessed.
     */
    void addAttrib(std::string_view name,
                   const VertexInputDesc* desc,
                   VertexFormat format,
                   std::size_t offset,
                   bool needNormalized,
                   uint8_t instanceStepRate = 0);

    /*
     * End to define vertex layout and compute hash value.
    */
    void endLayout(int stride = -1);

    /*
    * compute hash
    */
    uint32_t getHash() const { return _hash; }

    uint32_t getStride() const { return _strides[0]; }

    uint32_t getInstanceStride() const { return _strides[1]; }

    bool isValid() const { return !!_strides[0]; }

    const std::vector<InputBindingDesc>& getBindings() const { return _bindings; }

    void clear();

private:
    std::vector<InputBindingDesc> _bindings;
    uint32_t _strides[2] = {};  // 0: normal verts, 1: instance verts
    uint32_t _hash       = -1;
};

/**
 * Store vertex attribute layout.
 */
class AX_DLL VertexLayout : public Object
{
    friend class DriverBase;
    friend class ::ax::VertexLayoutManager;

protected:
    VertexLayout()                    = default;
    explicit VertexLayout(VertexLayoutDesc&& desc) noexcept : _desc(std::move(desc)) {}
    VertexLayout(const VertexLayout& rhs) = delete;
    void setBuiltinId(int id) { _builtinId = id; }

public:

    virtual ~VertexLayout() = default;

    const VertexLayoutDesc& getDesc() const { return _desc; }
    
    uint32_t getStride() const { return _desc.getStride(); }
    uint32_t getInstanceStride() const { return _desc.getInstanceStride(); }

    /**
     * Get attribute informations
     * @return Atrribute informations.
     */
    const std::vector<InputBindingDesc>& getBindings() const { return _desc.getBindings(); }

    uint32_t getHash() const { return _desc.getHash(); }

    /**
     * Get vertex step function. Default value is VERTEX.
     * @return Vertex step function.
     * @note Used in metal.
     */
    VertexStepMode getVertexStepMode() const { return _stepMode; }

    /**
     *  Get built-in vertex layout id, -1 means not built-in
    */
    int getBuiltinId() const { return _builtinId; }

protected:
    VertexLayoutDesc _desc{};
    int _builtinId{-1};
    VertexStepMode _stepMode = VertexStepMode::VERTEX;
};

// end of _rhi group
/// @}
}  // namespace ax::rhi

namespace ax
{
using VertexLayout     = rhi::VertexLayout;
using VertexLayoutDesc = rhi::VertexLayoutDesc;
using VertexLayoutKind = rhi::VertexLayoutKind;
}  // namespace ax
