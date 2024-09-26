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

#include "../ShaderModule.h"
#include "../Types.h"
#include <stdint.h>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#import <Metal/Metal.h>

NS_AX_BACKEND_BEGIN
/**
 * @addtogroup _metal
 * @{
 */

struct SLCReflectContext;

/**
 * To Create a vertex or fragment shader.
 */
class ShaderModuleMTL : public ShaderModule
{
public:
    /**
     * @param mtlDevice The device for which MTLFunction object was created.
     * @param stage Specify what kinds of shader to be created.
     * @param source Specify the shader source.
     */
    ShaderModuleMTL(id<MTLDevice> mtlDevice, ShaderStage stage, std::string_view source);
    ~ShaderModuleMTL();

    /**
     * Get MTLFunction object.
     * @return A MTLFunction object.
     */
    inline id<MTLFunction> getMTLFunction() const { return _mtlFunction; }

    /**
     * Get all uniformInfos.
     * @return The uniformInfos.
     */
    inline const hlookup::string_map<UniformInfo>& getAllActiveUniformInfo() const { return _activeUniformInfos; }

    /**
     * Get maximum uniform location.
     * @return Maximum uniform location.
     */
    inline const int getMaxLocation() const { return _maxLocation; }

    /**
     * Get active attribute informations.
     * @return Active attribute informations. key is attribute name and Value is corresponding attribute info.
     */
    inline const hlookup::string_map<AttributeBindInfo>& getAttributeInfo() const { return _attributeInfo; }

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
    int getAttributeLocation(Attribute name) const;

    /**
     * Get attribute location by attribute name.
     * @param name Specifies the attribute name.
     * @return The attribute location.
     */
    int getAttributeLocation(std::string_view name);

    /**
     * Get uniform buffer size in bytes that holds all the uniforms.
     * @return The uniform buffer size.
     */
    inline std::size_t getUniformBufferSize() const { return _uniformBufferSize; }

private:
    void parseAttibute(SLCReflectContext* context);
    void parseUniform(SLCReflectContext* context);
    void parseTexture(SLCReflectContext* context);
    void setBuiltinLocations();

    id<MTLFunction> _mtlFunction = nil;

    hlookup::string_map<UniformInfo> _activeUniformInfos;
    hlookup::string_map<AttributeBindInfo> _attributeInfo;

    int _attributeLocation[ATTRIBUTE_MAX];

    int _maxLocation = -1;
    UniformInfo _builtinUniforms[UNIFORM_MAX];

    std::size_t _uniformBufferSize = 0;
};

// end of _metal group
/// @}
NS_AX_BACKEND_END
