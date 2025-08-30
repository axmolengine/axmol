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

#include "axmol/rhi/opengl/ProgramGL.h"
#include "axmol/rhi/opengl/ShaderModuleGL.h"
#include "axmol/rhi/opengl/MacrosGL.h"
#include "axmol/base/Director.h"
#include "axmol/base/EventDispatcher.h"
#include "axmol/base/EventType.h"
#include "axmol/tlx/utility.hpp"
#include "axmol/tlx/pod_vector.hpp"
#include "yasio/byte_buffer.hpp"
#include "axmol/rhi/opengl/UtilsGL.h"
#include "axmol/rhi/opengl/OpenGLState.h"

namespace ax::rhi::gl
{
inline static std::string_view mapLocationEnumToUBO(rhi::Uniform name)
{
    switch (name)
    {
    case Uniform::MVP_MATRIX:
        return UNIFORM_NAME_MVP_MATRIX;
        break;
    case Uniform::TEXTURE:
        return UNIFORM_NAME_TEXTURE;
        break;
    case Uniform::TEXTURE1:
        return UNIFORM_NAME_TEXTURE1;
        break;
    case Uniform::TEXTURE2:
        return UNIFORM_NAME_TEXTURE2;
        break;
    case Uniform::TEXTURE3:
        return UNIFORM_NAME_TEXTURE3;
        break;
    case Uniform::TEXT_COLOR:
        return UNIFORM_NAME_TEXT_COLOR;
        break;
    case Uniform::EFFECT_COLOR:
        return UNIFORM_NAME_EFFECT_COLOR;
        break;
    case Uniform::EFFECT_TYPE:
        return UNIFORM_NAME_EFFECT_TYPE;
        break;
    default:;
    }
    return ""sv;
}

ProgramImpl::ProgramImpl(std::string_view vertexShader, std::string_view fragmentShader)
    : Program(vertexShader, fragmentShader)
{
    _vertexShaderModule =
        static_cast<ShaderModuleImpl*>(ShaderCache::getInstance()->acquireVertexShaderModule(_vsSource));
    _fragmentShaderModule =
        static_cast<ShaderModuleImpl*>(ShaderCache::getInstance()->acquireFragmentShaderModule(_fsSource));

    compileProgram();

    reflectVertexInputs();
    reflectUniformInfos();
#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    for (const auto& uniform : _activeUniformInfos)
    {
        auto location                            = uniform.second.location;
        _originalUniformLocations[uniform.first] = location;
        _mapToCurrentActiveLocation[location]    = location;
        _mapToOriginalLocation[location]         = location;
    }

    _backToForegroundListener =
        EventListenerCustom::create(EVENT_RENDERER_RECREATED, [this](EventCustom*) { this->reloadProgram(); });
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_backToForegroundListener, -1);
#endif

    setBuiltinLocations();
}

ProgramImpl::~ProgramImpl()
{
    clearUniformBuffers();

    AX_SAFE_RELEASE(_vertexShaderModule);
    AX_SAFE_RELEASE(_fragmentShaderModule);
    if (_program)
        glDeleteProgram(_program);

#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    Director::getInstance()->getEventDispatcher()->removeEventListener(_backToForegroundListener);
#endif
}

#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
void ProgramImpl::reloadProgram()
{
    _activeUniformInfos.clear();
    _mapToCurrentActiveLocation.clear();
    _mapToOriginalLocation.clear();
    static_cast<ShaderModuleImpl*>(_vertexShaderModule)->compileShader(rhi::ShaderStage::VERTEX, _vsSource);
    static_cast<ShaderModuleImpl*>(_fragmentShaderModule)->compileShader(rhi::ShaderStage::FRAGMENT, _fsSource);
    compileProgram();
    reflectUniformInfos();

    for (const auto& uniform : _activeUniformInfos)
    {
        auto location                                   = _originalUniformLocations[uniform.first];
        _mapToCurrentActiveLocation[location]           = uniform.second.location;
        _mapToOriginalLocation[uniform.second.location] = location;
    }
}
#endif

void ProgramImpl::compileProgram()
{
    if (_vertexShaderModule == nullptr || _fragmentShaderModule == nullptr)
        return;

    auto vertShader = _vertexShaderModule->getShader();
    auto fragShader = _fragmentShaderModule->getShader();

    assert(vertShader != 0 && fragShader != 0);
    if (vertShader == 0 || fragShader == 0)
        return;

    _program = glCreateProgram();
    if (!_program)
        return;

    glAttachShader(_program, vertShader);
    glAttachShader(_program, fragShader);

    glLinkProgram(_program);

    GLint status = 0;
    glGetProgramiv(_program, GL_LINK_STATUS, &status);
    if (GL_FALSE == status)
    {
        GLint errorInfoLen = 0;
        glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &errorInfoLen);
        if (errorInfoLen > 1)
        {
            auto errorInfo = axstd::make_unique_for_overwrite<char[]>(static_cast<size_t>(errorInfoLen));
            glGetProgramInfoLog(_program, errorInfoLen, NULL, errorInfo.get());
            AXLOGE("axmol:ERROR: {}: failed to link program: {} ", __FUNCTION__, errorInfo.get());
        }
        else
            AXLOGE("axmol:ERROR: {}: failed to link program ", __FUNCTION__);
        glDeleteProgram(_program);
        _program = 0;
    }
}

void ProgramImpl::setBuiltinLocations()
{
    /*--- Builtin Attribs ---*/

    std::fill(_builtinVertxInputs, _builtinVertxInputs + VertexInputKind::VIK_COUNT, nullptr);

    /// a_position
    _builtinVertxInputs[VertexInputKind::POSITION] = getVertexInputDesc(VERTEX_INPUT_NAME_POSITION);

    /// a_color
    _builtinVertxInputs[VertexInputKind::COLOR] = getVertexInputDesc(VERTEX_INPUT_NAME_COLOR);

    /// a_texCoord
    _builtinVertxInputs[VertexInputKind::TEXCOORD] = getVertexInputDesc(VERTEX_INPUT_NAME_TEXCOORD);

    // a_normal
    _builtinVertxInputs[VertexInputKind::NORMAL] = getVertexInputDesc(VERTEX_INPUT_NAME_NORMAL);

    // a_instance
    _builtinVertxInputs[VertexInputKind::INSTANCE] = getVertexInputDesc(VERTEX_INPUT_NAME_INSTANCE);

    /*--- Builtin Uniforms ---*/

    /// u_MVPMatrix
    _builtinUniformLocation[Uniform::MVP_MATRIX] = getUniformLocation(UNIFORM_NAME_MVP_MATRIX);

    /// u_tex0
    _builtinUniformLocation[Uniform::TEXTURE] = getUniformLocation(UNIFORM_NAME_TEXTURE);

    /// u_tex1
    _builtinUniformLocation[Uniform::TEXTURE1] = getUniformLocation(UNIFORM_NAME_TEXTURE1);

    /// u_textColor
    _builtinUniformLocation[Uniform::TEXT_COLOR] = getUniformLocation(UNIFORM_NAME_TEXT_COLOR);

    /// u_effectColor
    _builtinUniformLocation[Uniform::EFFECT_COLOR] = getUniformLocation(UNIFORM_NAME_EFFECT_COLOR);

    /// u_effectType
    _builtinUniformLocation[Uniform::EFFECT_TYPE] = getUniformLocation(UNIFORM_NAME_EFFECT_TYPE);
}

void ProgramImpl::reflectVertexInputs()
{
    if (!_program)
        return;

    GLint numOfActiveInputs = 0;
    glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTES, &numOfActiveInputs);

    if (numOfActiveInputs <= 0)
        return;

    _activeVertexInputs.reserve(numOfActiveInputs);

    constexpr int MAX_VERTEX_INPUT_NAME_LENGTH = 255;
    auto attrName = axstd::make_unique_for_overwrite<char[]>(MAX_VERTEX_INPUT_NAME_LENGTH + 1);

    GLint attrNameLen = 0;
    GLenum attrType;
    GLint attrSize;

    for (int i = 0; i < numOfActiveInputs; i++)
    {
        glGetActiveAttrib(_program, i, MAX_VERTEX_INPUT_NAME_LENGTH, &attrNameLen, &attrSize, &attrType,
                          attrName.get());
        CHECK_GL_ERROR_DEBUG();
        std::string_view name{attrName.get(), static_cast<size_t>(attrNameLen)};

        rhi::VertexInputDesc info;
        info.location = glGetAttribLocation(_program, name.data());
        info.format   = attrType;
        info.count    = UtilsGL::getGLDataTypeSize(attrType) * attrSize;
        CHECK_GL_ERROR_DEBUG();
        _activeVertexInputs.emplace(name, std::move(info));
    }
}

void ProgramImpl::reflectUniformInfos()
{
    if (!_program)
        return;

    _totalBufferSize = 0;
    _maxLocation     = -1;
    _activeUniformInfos.clear();

    yasio::basic_byte_buffer<GLchar> buffer;  // buffer for name

    // OpenGL UBO: uloc[0]: block_offset, uloc[1]: offset in block

    auto driver = DriverBase::getInstance();
    /* Query uniform blocks */
    clearUniformBuffers();

    GLint numblocks{0};
    glGetProgramiv(_program, GL_ACTIVE_UNIFORM_BLOCKS, &numblocks);

    axstd::pod_vector<GLint> uniformBlcokOffsets(numblocks);
    for (int blockIndex = 0; blockIndex < numblocks; ++blockIndex)
    {
        GLint blockSize{0};
        glGetActiveUniformBlockiv(_program, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
        CHECK_GL_ERROR_DEBUG();

        assert(blockSize > 0);  // empty block not allow by GLSL/ESSL

        GLint memberCount{0};
        glGetActiveUniformBlockiv(_program, blockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &memberCount);
        assert(memberCount > 0);

        // set bindingIndex at CPU
        glUniformBlockBinding(_program, blockIndex, blockIndex);

        // create uniform buffer object
        auto& desc = _uniformBuffers.emplace_back(
            static_cast<BufferImpl*>(driver->createBuffer(blockSize, BufferType::UNIFORM, BufferUsage::DYNAMIC)),
            static_cast<int>(_totalBufferSize), blockSize);
        desc._ubo->updateData(nullptr, blockSize);  // ubo data can be nullptr

        CHECK_GL_ERROR_DEBUG();

        // increase _totalBufferSize
        _totalBufferSize += blockSize;
    }

    /*
     * construct _activeUniformInfos: uniformName-->UniformInfo
     */
    UniformInfo uniform;
    GLint nameLen       = 0;
    GLint numOfUniforms = 0;
    glGetProgramiv(_program, GL_ACTIVE_UNIFORMS, &numOfUniforms);
    for (GLint i = 0; i < numOfUniforms; ++i)
    {
        buffer.resize(MAX_UNIFORM_NAME_LENGTH + 1);
        glGetActiveUniform(_program, i, static_cast<GLint>(buffer.size()), &nameLen, &uniform.count, &uniform.type,
                           buffer.data());

        uniform.size = UtilsGL::getGLDataTypeSize(uniform.type);
        std::string_view uniformFullName{buffer.data(), static_cast<size_t>(nameLen)};
        std::string_view uniformName{uniformFullName};

        // Try trim uniform name
        // trim name vs_ub.xxx[0] --> xxx
        auto bracket = uniformName.find_last_of('[');
        if (bracket != std::string_view::npos)
        {
            buffer[bracket] = '\0';
            uniformName     = uniformName.substr(0, bracket);
        }
        auto dot = uniformName.find_last_of('.');
        if (dot != std::string::npos)
            uniformName.remove_prefix(dot + 1);  // trim uniformName

        GLint blockIndex{-1};
        glGetActiveUniformsiv(_program, 1, reinterpret_cast<const GLuint*>(&i), GL_UNIFORM_BLOCK_INDEX, &blockIndex);
        if (blockIndex != -1)
        {  // member of uniform block
            auto& blockDesc = _uniformBuffers[blockIndex];
            GLint uniformOffset{-1};
            glGetActiveUniformsiv(_program, 1, reinterpret_cast<const GLuint*>(&i), GL_UNIFORM_OFFSET, &uniformOffset);
            uniform.location     = blockDesc._location;
            uniform.bufferOffset = uniformOffset;
        }
        else
        {  // must be samper: sampler2D, sampler2DArray, samplerCube
            assert(uniform.type == GL_SAMPLER_2D || uniform.type == GL_SAMPLER_CUBE ||
                   uniform.type == GL_SAMPLER_2D_ARRAY);
            uniform.location     = glGetUniformLocation(_program, uniformName.data());
            uniform.bufferOffset = -1;
        }

        _activeUniformInfos[uniformName] = uniform;

        _maxLocation = _maxLocation <= uniform.location ? (uniform.location + 1) : _maxLocation;
    }
}

void ProgramImpl::bindUniformBuffers(const char* buffer, size_t bufferSize)
{
    for (GLuint blockIdx = 0; blockIdx < static_cast<GLuint>(_uniformBuffers.size()); ++blockIdx)
    {
        auto& desc = _uniformBuffers[blockIdx];
        desc._ubo->updateData(buffer + desc._location, desc._size);
        __state->bindUniformBufferBase(blockIdx, desc._ubo->internalHandle());
    }

    CHECK_GL_ERROR_DEBUG();
}

void ProgramImpl::clearUniformBuffers()
{
    if (_uniformBuffers.empty())
        return;
    for (auto& desc : _uniformBuffers)
        delete desc._ubo;
    _uniformBuffers.clear();
}

const VertexInputDesc* ProgramImpl::getVertexInputDesc(VertexInputKind name) const
{
    return _builtinVertxInputs[name];
}

const VertexInputDesc* ProgramImpl::getVertexInputDesc(std::string_view name) const
{
    auto it = _activeVertexInputs.find(name);
    return it != _activeVertexInputs.end() ? &it->second : nullptr;
}

UniformLocation ProgramImpl::getUniformLocation(rhi::Uniform name) const
{
    return _builtinUniformLocation[name];
}

UniformLocation ProgramImpl::getUniformLocation(std::string_view uniform) const
{
    UniformLocation uniformLocation;
    auto iter = _activeUniformInfos.find(uniform);
    if (iter != _activeUniformInfos.end())
    {
        auto& uniformInfo = iter->second;
#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
        uniformLocation.stages[0].location = _mapToOriginalLocation.at(uniformInfo.location);
#else
        uniformLocation.stages[0].location = uniformInfo.location;
#endif
        uniformLocation.stages[0].offset = uniformInfo.bufferOffset;
    }

    return uniformLocation;
}

int ProgramImpl::getMaxVertexLocation() const
{
    return _maxLocation;
}
int ProgramImpl::getMaxFragmentLocation() const
{
    return _maxLocation;
}

#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
int ProgramImpl::getMappedLocation(int location) const
{
    if (_mapToCurrentActiveLocation.find(location) != _mapToCurrentActiveLocation.end())
        return _mapToCurrentActiveLocation.at(location);
    else
        return -1;
}

int ProgramImpl::getOriginalLocation(int location) const
{
    if (_mapToOriginalLocation.find(location) != _mapToOriginalLocation.end())
        return _mapToOriginalLocation.at(location);
    else
        return -1;
}
#endif

const axstd::string_map<VertexInputDesc>& ProgramImpl::getActiveVertexInputs() const
{
    return _activeVertexInputs;
}

const axstd::string_map<UniformInfo>& ProgramImpl::getActiveUniformInfos(ShaderStage /*stage*/) const
{
    return _activeUniformInfos;
}

std::size_t ProgramImpl::getUniformBufferSize(ShaderStage /*stage*/) const
{
    return _totalBufferSize;
}

}  // namespace ax::rhi::gl
