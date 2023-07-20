/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2020 C4games Ltd.

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

#include "ProgramGL.h"
#include "ShaderModuleGL.h"
#include "renderer/backend/Types.h"
#include "renderer/backend/opengl/MacrosGL.h"
#include "base/Director.h"
#include "base/EventDispatcher.h"
#include "base/EventType.h"
#include "base/axstd.h"
#include "yasio/byte_buffer.hpp"
#include "renderer/backend/opengl/UtilsGL.h"
#include "OpenGLState.h"

NS_AX_BACKEND_BEGIN

ProgramGL::ProgramGL(std::string_view vertexShader, std::string_view fragmentShader)
    : Program(vertexShader, fragmentShader)
{
    _vertexShaderModule   = static_cast<ShaderModuleGL*>(ShaderCache::newVertexShaderModule(_vertexShader));
    _fragmentShaderModule = static_cast<ShaderModuleGL*>(ShaderCache::newFragmentShaderModule(_fragmentShader));

    AX_SAFE_RETAIN(_vertexShaderModule);
    AX_SAFE_RETAIN(_fragmentShaderModule);
    compileProgram();
    computeUniformInfos();
#if AX_ENABLE_CACHE_TEXTURE_DATA
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

ProgramGL::~ProgramGL()
{
    clearUniformBuffers();

    AX_SAFE_RELEASE(_vertexShaderModule);
    AX_SAFE_RELEASE(_fragmentShaderModule);
    if (_program)
        glDeleteProgram(_program);

#if AX_ENABLE_CACHE_TEXTURE_DATA
    Director::getInstance()->getEventDispatcher()->removeEventListener(_backToForegroundListener);
#endif
}

#if AX_ENABLE_CACHE_TEXTURE_DATA
void ProgramGL::reloadProgram()
{
    _activeUniformInfos.clear();
    _mapToCurrentActiveLocation.clear();
    _mapToOriginalLocation.clear();
    static_cast<ShaderModuleGL*>(_vertexShaderModule)->compileShader(backend::ShaderStage::VERTEX, _vertexShader);
    static_cast<ShaderModuleGL*>(_fragmentShaderModule)->compileShader(backend::ShaderStage::FRAGMENT, _fragmentShader);
    compileProgram();
    computeUniformInfos();

    for (const auto& uniform : _activeUniformInfos)
    {
        auto location                                   = _originalUniformLocations[uniform.first];
        _mapToCurrentActiveLocation[location]           = uniform.second.location;
        _mapToOriginalLocation[uniform.second.location] = location;
    }
}
#endif

void ProgramGL::compileProgram()
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
            log("cocos2d: ERROR: %s: failed to link program: %s ", __FUNCTION__, errorInfo.get());
        }
        else
            log("cocos2d: ERROR: %s: failed to link program ", __FUNCTION__);
        glDeleteProgram(_program);
        _program = 0;
    }
}

void ProgramGL::setBuiltinLocations()
{
    std::fill(_builtinAttributeLocation, _builtinAttributeLocation + ATTRIBUTE_MAX, -1);
    //    std::fill(_builtinUniformLocation, _builtinUniformLocation + UNIFORM_MAX, -1);

    /// a_position
    auto location                                  = glGetAttribLocation(_program, ATTRIBUTE_NAME_POSITION.data());
    _builtinAttributeLocation[Attribute::POSITION] = location;

    /// a_color
    location                                    = glGetAttribLocation(_program, ATTRIBUTE_NAME_COLOR.data());
    _builtinAttributeLocation[Attribute::COLOR] = location;

    /// a_texCoord
    location                                       = glGetAttribLocation(_program, ATTRIBUTE_NAME_TEXCOORD.data());
    _builtinAttributeLocation[Attribute::TEXCOORD] = location;

    // a_normal
    location                                     = glGetAttribLocation(_program, ATTRIBUTE_NAME_NORMAL.data());
    _builtinAttributeLocation[Attribute::NORMAL] = location;

    /// u_MVPMatrix
    auto uloc                                                = getUniformLocation(UNIFORM_NAME_MVP_MATRIX);
    _builtinUniformLocation[Uniform::MVP_MATRIX].location[0] = uloc.location[0];
    _builtinUniformLocation[Uniform::MVP_MATRIX].location[1] = uloc.location[1];

    /// u_textColor
    uloc                                                     = getUniformLocation(UNIFORM_NAME_TEXT_COLOR);
    _builtinUniformLocation[Uniform::TEXT_COLOR].location[0] = uloc.location[0];
    _builtinUniformLocation[Uniform::TEXT_COLOR].location[1] = uloc.location[1];

    /// u_effectColor
    uloc                                                       = getUniformLocation(UNIFORM_NAME_EFFECT_COLOR);
    _builtinUniformLocation[Uniform::EFFECT_COLOR].location[0] = uloc.location[0];
    _builtinUniformLocation[Uniform::EFFECT_COLOR].location[1] = uloc.location[1];

    /// u_effectType
    uloc                                                      = getUniformLocation(UNIFORM_NAME_EFFECT_TYPE);
    _builtinUniformLocation[Uniform::EFFECT_TYPE].location[0] = uloc.location[0];
    _builtinUniformLocation[Uniform::EFFECT_TYPE].location[1] = uloc.location[1];

    /// u_tex0
    uloc                                                  = getUniformLocation(UNIFORM_NAME_TEXTURE);
    _builtinUniformLocation[Uniform::TEXTURE].location[0] = uloc.location[0];

    /// u_tex1
    uloc                                                   = getUniformLocation(UNIFORM_NAME_TEXTURE1);
    _builtinUniformLocation[Uniform::TEXTURE1].location[0] = uloc.location[0];
}

bool ProgramGL::getAttributeLocation(std::string_view attributeName, unsigned int& location) const
{
    GLint loc = glGetAttribLocation(_program, attributeName.data());
    if (-1 == loc)
    {
        AXLOG("axmol: %s: can not find vertex attribute of %s", __FUNCTION__, attributeName.data());
        return false;
    }

    location = GLuint(loc);
    return true;
}

const hlookup::string_map<AttributeBindInfo>& ProgramGL::getActiveAttributes() const
{
    if (!_program || !_activeAttribs.empty())
        return _activeAttribs;

    GLint numOfActiveAttributes = 0;
    glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTES, &numOfActiveAttributes);

    if (numOfActiveAttributes <= 0)
        return _activeAttribs;

    _activeAttribs.reserve(numOfActiveAttributes);

    int MAX_ATTRIBUTE_NAME_LENGTH = 255;
    auto attrName                 = axstd::make_unique_for_overwrite<char[]>(MAX_ATTRIBUTE_NAME_LENGTH + 1);

    GLint attrNameLen = 0;
    GLenum attrType;
    GLint attrSize;
    backend::AttributeBindInfo info;

    for (int i = 0; i < numOfActiveAttributes; i++)
    {
        glGetActiveAttrib(_program, i, MAX_ATTRIBUTE_NAME_LENGTH, &attrNameLen, &attrSize, &attrType, attrName.get());
        CHECK_GL_ERROR_DEBUG();
        info.attributeName = std::string(attrName.get(), attrName.get() + attrNameLen);
        info.location      = glGetAttribLocation(_program, info.attributeName.c_str());
        info.type          = attrType;
        info.size          = UtilsGL::getGLDataTypeSize(attrType) * attrSize;
        CHECK_GL_ERROR_DEBUG();
        _activeAttribs[info.attributeName] = info;
    }

    return _activeAttribs;
}

void ProgramGL::computeUniformInfos()
{
    if (!_program)
        return;

    _totalBufferSize = 0;
    _maxLocation     = -1;
    _activeUniformInfos.clear();

    yasio::basic_byte_buffer<GLchar> buffer;  // buffer for name

    // OpenGL UBO: uloc[0]: block_index, uloc[1]: offset in block
    axstd::pod_vector<GLint> uniformOffsets, uniformIndices;
    std::map<GLuint, std::pair<int, int>> uniformIndexMap;

    auto gpuDevice = Device::getInstance();
    /* Query uniform blocks */
    clearUniformBuffers();

    // GLint numAttrs{0};
    // glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTES, &numAttrs); // works

    GLint numblocks{0};
    glGetProgramiv(_program, GL_ACTIVE_UNIFORM_BLOCKS, &numblocks);
    for (int blockIndex = 0; blockIndex < numblocks; ++blockIndex)
    {
        GLint blockSize{0};
        glGetActiveUniformBlockiv(_program, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
        CHECK_GL_ERROR_DEBUG();

        assert(blockSize > 0);  // empty block not allow by GLSL/ESSL

        GLint memberCount{0};
        glGetActiveUniformBlockiv(_program, blockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &memberCount);
        assert(memberCount > 0);

        // buffer.resize_fit(MAX_UNIFORM_NAME_LENGTH + 1);
        // GLsizei length{0};
        // glGetActiveUniformBlockName(_program, blockIndex, buffer.size(), &length, buffer.data());
        //
        // ax::print("### ub: %s", buffer.data());

        uniformIndices.resize(memberCount);
        glGetActiveUniformBlockiv(_program, blockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, uniformIndices.data());

        uniformOffsets.resize(memberCount);
        glGetActiveUniformsiv(_program, memberCount, reinterpret_cast<const GLuint*>(uniformIndices.data()),
                              GL_UNIFORM_OFFSET, uniformOffsets.data());

        // set bindingIndex at CPU
        glUniformBlockBinding(_program, blockIndex, blockIndex);

        // create uniform buffer object
        auto& desc = _uniformBuffers.emplace_back(
            static_cast<BufferGL*>(gpuDevice->newBuffer(blockSize, BufferType::UNIFORM, BufferUsage::DYNAMIC)),
            static_cast<int>(_totalBufferSize), blockSize);
        desc._ubo->updateData(nullptr, blockSize);  // ubo data can be nullptr

        CHECK_GL_ERROR_DEBUG();

        for (GLint i = 0; i < memberCount; ++i)
            uniformIndexMap.emplace(uniformIndices[i], std::make_pair(static_cast<int>(desc._location),
                                                                      static_cast<int>(uniformOffsets[i])));
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
        buffer.resize_fit(MAX_UNIFORM_NAME_LENGTH + 1);
        glGetActiveUniform(_program, i, static_cast<GLint>(buffer.size()), &nameLen, &uniform.count, &uniform.type,
                           buffer.data());
        std::string_view uniformName{buffer.data(), static_cast<size_t>(nameLen)};
        // ax::print("--------- uniform fullName: %s", uniformName.data());

        auto it = uniformIndexMap.find(i);
        if (it != uniformIndexMap.end())
        { // member of uniform block
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

            uniform.location     = it->second.first;
            uniform.bufferOffset = it->second.second;
        }
        else
        {// must be samper: sampler2D or samplerCube
            assert(uniform.type == GL_SAMPLER_2D || uniform.type == GL_SAMPLER_CUBE);
            uniform.location     = glGetUniformLocation(_program, uniformName.data());
            uniform.bufferOffset = 0;
        }

        uniform.size                    = UtilsGL::getGLDataTypeSize(uniform.type);
        _activeUniformInfos[uniformName] = uniform;

        _maxLocation = _maxLocation <= uniform.location ? (uniform.location + 1) : _maxLocation;
    }
}

void ProgramGL::bindUniformBuffers(const char* buffer, size_t bufferSize, uint32_t hashOfUniforms)
{
    if (_hashOfUniforms != hashOfUniforms)
    {
        _hashOfUniforms = hashOfUniforms;

        for (GLuint blockIdx = 0; blockIdx < static_cast<GLuint>(_uniformBuffers.size()); ++blockIdx)
        {
            auto& desc = _uniformBuffers[blockIdx];
            desc._ubo->updateData(buffer + desc._location, desc._size);
        }

        CHECK_GL_ERROR_DEBUG();
    }

    for (GLuint blockIdx = 0; blockIdx < static_cast<GLuint>(_uniformBuffers.size()); ++blockIdx)
    {
        auto& desc = _uniformBuffers[blockIdx];
        __gl->bindUniformBufferBase(blockIdx, desc._ubo->getHandler());
    }

    CHECK_GL_ERROR_DEBUG();
}

void ProgramGL::clearUniformBuffers()
{
    if (_uniformBuffers.empty())
        return;
    for (auto& desc : _uniformBuffers)
        delete desc._ubo;
    _uniformBuffers.clear();
}

int ProgramGL::getAttributeLocation(Attribute name) const
{
    return _builtinAttributeLocation[name];
}

int ProgramGL::getAttributeLocation(std::string_view name) const
{
    return glGetAttribLocation(_program, name.data());
}

inline std::string_view mapLocationEnumToUBO(backend::Uniform name)
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
    }
    return ""sv;
}

UniformLocation ProgramGL::getUniformLocation(backend::Uniform name) const
{
    return _builtinUniformLocation[name];
}

UniformLocation ProgramGL::getUniformLocation(std::string_view uniform) const
{
    UniformLocation uniformLocation;
    if (_activeUniformInfos.find(uniform) != _activeUniformInfos.end())
    {
        const auto& uniformInfo = _activeUniformInfos.at(uniform);
#if AX_ENABLE_CACHE_TEXTURE_DATA
        uniformLocation.location[0] = _mapToOriginalLocation.at(uniformInfo.location);
#else
        uniformLocation.location[0] = uniformInfo.location;
#endif
        uniformLocation.location[1] = uniformInfo.bufferOffset;
    }

    return uniformLocation;
}

int ProgramGL::getMaxVertexLocation() const
{
    return _maxLocation;
}
int ProgramGL::getMaxFragmentLocation() const
{
    return _maxLocation;
}

#if AX_ENABLE_CACHE_TEXTURE_DATA
int ProgramGL::getMappedLocation(int location) const
{
    if (_mapToCurrentActiveLocation.find(location) != _mapToCurrentActiveLocation.end())
        return _mapToCurrentActiveLocation.at(location);
    else
        return -1;
}

int ProgramGL::getOriginalLocation(int location) const
{
    if (_mapToOriginalLocation.find(location) != _mapToOriginalLocation.end())
        return _mapToOriginalLocation.at(location);
    else
        return -1;
}
#endif

const UniformInfo& ProgramGL::getActiveUniformInfo(ShaderStage stage, int location) const
{
    static const UniformInfo s_emptyInfo{};
    return s_emptyInfo;
}

const hlookup::string_map<UniformInfo>& ProgramGL::getAllActiveUniformInfo(ShaderStage stage) const
{
    return _activeUniformInfos;
}

std::size_t ProgramGL::getUniformBufferSize(ShaderStage stage) const
{
    return _totalBufferSize;
}

NS_AX_BACKEND_END
