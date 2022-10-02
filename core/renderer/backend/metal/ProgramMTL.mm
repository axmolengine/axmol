/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.

 https://axmolengine.github.io/

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

#include "ProgramMTL.h"
#include "ShaderModuleMTL.h"
#include "base/ccMacros.h"

NS_AX_BACKEND_BEGIN
namespace
{
constexpr std::string_view metalSpecificDefine = "#define METAL\n"sv;
}

ProgramMTL::ProgramMTL(std::string_view vertexShader, std::string_view fragmentShader)
    : Program(vertexShader, fragmentShader)
{
    _vertexShader = static_cast<ShaderModuleMTL*>(ShaderCache::newVertexShaderModule(vertexShader));
    std::string combinedSource{metalSpecificDefine};
    combinedSource += fragmentShader;
    _fragmentShader = static_cast<ShaderModuleMTL*>(ShaderCache::newFragmentShaderModule(std::move(combinedSource)));

    AX_SAFE_RETAIN(_vertexShader);
    AX_SAFE_RETAIN(_fragmentShader);
}

ProgramMTL::~ProgramMTL()
{
    AX_SAFE_RELEASE(_vertexShader);
    AX_SAFE_RELEASE(_fragmentShader);
}

int ProgramMTL::getAttributeLocation(Attribute name) const
{
    return _vertexShader->getAttributeLocation(name);
}

int ProgramMTL::getAttributeLocation(std::string_view name) const
{
    return _vertexShader->getAttributeLocation(name);
}

UniformLocation ProgramMTL::getUniformLocation(backend::Uniform name) const
{
    UniformLocation uniformLocation;
    auto vsLocation = _vertexShader->getUniformLocation(name);
    auto fsLocation = _fragmentShader->getUniformLocation(name);
    if (vsLocation != -1 && fsLocation != -1)
    {
        uniformLocation.shaderStage = ShaderStage::VERTEX_AND_FRAGMENT;
        uniformLocation.location[0] = vsLocation;
        uniformLocation.location[1] = fsLocation;
    }
    else if (vsLocation != -1)
    {
        uniformLocation.shaderStage = ShaderStage::VERTEX;
        uniformLocation.location[0] = vsLocation;
    }
    else
    {
        uniformLocation.shaderStage = ShaderStage::FRAGMENT;
        uniformLocation.location[1] = fsLocation;
    }
    return uniformLocation;
}

UniformLocation ProgramMTL::getUniformLocation(std::string_view uniform) const
{
    UniformLocation uniformLocation;
    auto vsLocation = _vertexShader->getUniformLocation(uniform);
    auto fsLocation = _fragmentShader->getUniformLocation(uniform);
    if (vsLocation != -1 && fsLocation != -1)
    {
        uniformLocation.shaderStage = ShaderStage::VERTEX_AND_FRAGMENT;
        uniformLocation.location[0] = vsLocation;
        uniformLocation.location[1] = fsLocation;
    }
    else if (vsLocation != -1)
    {
        uniformLocation.shaderStage = ShaderStage::VERTEX;
        uniformLocation.location[0] = vsLocation;
    }
    else
    {
        uniformLocation.shaderStage = ShaderStage::FRAGMENT;
        uniformLocation.location[1] = fsLocation;
    }
    return uniformLocation;
}

int ProgramMTL::getMaxVertexLocation() const
{
    return _vertexShader->getMaxLocation();
}

int ProgramMTL::getMaxFragmentLocation() const
{
    return _fragmentShader->getMaxLocation();
}

const hlookup::string_map<AttributeBindInfo>& ProgramMTL::getActiveAttributes() const
{
    return _vertexShader->getAttributeInfo();
}

// const std::vector<char>& ProgramMTL::cloneUniformBuffer(ShaderStage stage) const
//{
//     switch (stage) {
//         case ShaderStage::VERTEX:
//             return _vertexShader->cloneUniformBuffer();
//             break;
//         case ShaderStage::FRAGMENT:
//             return _fragmentShader->cloneUniformBuffer();
//         default:
//             AXASSERT(false, "Invalid shader stage.");
//             break;
//     }
// }

const UniformInfo& ProgramMTL::getActiveUniformInfo(ShaderStage stage, int location) const
{
    switch (stage)
    {
    case ShaderStage::VERTEX:
        return _vertexShader->getActiveUniform(location);
    case ShaderStage::FRAGMENT:
        return _fragmentShader->getActiveUniform(location);
    default:
        AXASSERT(false, "Invalid shader stage.");
        break;
    }
}

std::size_t ProgramMTL::getUniformBufferSize(ShaderStage stage) const
{
    switch (stage)
    {
    case ShaderStage::VERTEX:
        return _vertexShader->getUniformBufferSize();
    case ShaderStage::FRAGMENT:
        return _fragmentShader->getUniformBufferSize();
    default:
        AXASSERT(false, "Invalid shader stage.");
        break;
    }
    return 0;
}

const hlookup::string_map<UniformInfo>& ProgramMTL::getAllActiveUniformInfo(ShaderStage stage) const
{
    switch (stage)
    {
    case ShaderStage::VERTEX:
        return _vertexShader->getAllActiveUniformInfo();
    case ShaderStage::FRAGMENT:
        return _fragmentShader->getAllActiveUniformInfo();
    default:
        AXASSERT(false, "Invalid shader stage.");
    }
}

NS_AX_BACKEND_END
