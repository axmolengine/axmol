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

namespace ax::rhi
{
class Program;
/**
 * @addtogroup _rhi
 * @{
 */

/**
 * Store vertex attribute layout.
 */
class AX_DLL VertexLayout
{
    friend class DriverBase;

protected:
    VertexLayout()                    = default;
    VertexLayout(const VertexLayout& rhs) = default;

public:
    struct InputBindingDesc
    {
        InputBindingDesc() = default;
        InputBindingDesc(std::string_view _name,
                         int _index,
                         VertexFormat _format,
                         std::size_t _offset,
                         bool needToBeNormallized,
                         uint8_t instanceStepRate)
            : name(_name)
            , format(_format)
            , offset(_offset)
            , index(_index)
            , needToBeNormallized(needToBeNormallized)
            , instanceStepRate(instanceStepRate)
        {}

        std::string name;  ///< name is used in opengl
        VertexFormat format      = VertexFormat::INT3;
        unsigned int offset      = 0;
        int index                = 0;  ///< index is used in metal
        bool needToBeNormallized = false;
        uint8_t instanceStepRate = 0;
    };

    virtual ~VertexLayout() = default;

    /**
     * Set attribute values to name.
     * @param name Specifies the attribute name.
     * @param index Specifies the index of the generic vertex attribute to be modified.
     * @param format Specifies how the vertex attribute data is laid out in memory.
     * @param offset Specifies the byte offset to the first component of the first generic vertex attribute.
     * @param needToBeNormallized Specifies whether fixed-point data values should be normalized (true) or converted
     * directly as fixed-point values (false) when they are accessed.
     */
    void setAttrib(std::string_view name,
                   const VertexInputDesc* desc,
                   VertexFormat format,
                   std::size_t offset,
                   bool needNormalized,
                   uint8_t instanceStepRate = 0);
    /**
     * Set stride of vertices.
     * @param stride Specifies the distance between the data of two vertices, in bytes.
     */
    inline void setStride(std::size_t stride) { _strides[0] = stride; }

    /**
     * Get the distance between the data of two vertices, in bytes.
     * @return The distance between the data of two vertices, in bytes.
     */
    inline std::size_t getStride() const { return _strides[0]; }

    /**
     * Set stride of vertices.
     * @param stride Specifies the distance between the data of two vertices, in bytes.
     */
    inline void setInstanceStride(std::size_t stride) { _strides[1] = stride; }

    /**
     * Get the distance between the data of two vertices, in bytes.
     * @return The distance between the data of two vertices, in bytes.
     */
    inline std::size_t getInstanceStride() const { return _strides[1]; }

    /**
     * Get attribute informations
     * @return Atrribute informations.
     */
    inline const hlookup::string_map<InputBindingDesc>& getBindings() const { return _bindings; }

    inline const hlookup::string_map<InputBindingDesc>& getInstanceBindings() const { return _instanceBindings; }

    /**
     * Get vertex step function. Default value is VERTEX.
     * @return Vertex step function.
     * @note Used in metal.
     */
    inline VertexStepMode getVertexStepMode() const { return _stepMode; }

    /**
     * Check if vertex layout has been set.
     */
    inline bool isValid() const { return _strides[0] != 0; }

    virtual VertexLayout* clone() { return new VertexLayout(*this); }

protected:
    hlookup::string_map<InputBindingDesc> _bindings;
    hlookup::string_map<InputBindingDesc> _instanceBindings;
    uint32_t _strides[2]     = {}; // 0: normal verts, 1: instance verts
    VertexStepMode _stepMode = VertexStepMode::VERTEX;
};

// end of _rhi group
/// @}
}  // namespace ax::rhi
