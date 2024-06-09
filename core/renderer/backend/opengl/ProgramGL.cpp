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

#if AX_GLES_PROFILE == 200
#    define DEF_TO_INT(pointer, index) (*((GLint*)(pointer) + index))
#    define DEF_TO_FLOAT(pointer, index) (*((GLfloat*)(pointer) + index))
static void setUniform(bool isArray, GLuint location, unsigned int size, GLenum uniformType, void* data)
{
    GLsizei count = size;
    switch (uniformType)
    {
    case GL_INT:
    case GL_BOOL:
    case GL_SAMPLER_2D:
    case GL_SAMPLER_CUBE:
        if (isArray)
            glUniform1iv(location, count, (GLint*)data);
        else
            glUniform1i(location, DEF_TO_INT(data, 0));
        break;
    case GL_INT_VEC2:
    case GL_BOOL_VEC2:
        if (isArray)
            glUniform2iv(location, count, (GLint*)data);
        else
            glUniform2i(location, DEF_TO_INT(data, 0), DEF_TO_INT(data, 1));
        break;
    case GL_INT_VEC3:
    case GL_BOOL_VEC3:
        if (isArray)
            glUniform3iv(location, count, (GLint*)data);
        else
            glUniform3i(location, DEF_TO_INT(data, 0), DEF_TO_INT(data, 1), DEF_TO_INT(data, 2));
        break;
    case GL_INT_VEC4:
    case GL_BOOL_VEC4:
        if (isArray)
            glUniform4iv(location, count, (GLint*)data);
        else
            glUniform4i(location, DEF_TO_INT(data, 0), DEF_TO_INT(data, 1), DEF_TO_INT(data, 2), DEF_TO_INT(data, 4));
        break;
    case GL_FLOAT:
        if (isArray)
            glUniform1fv(location, count, (GLfloat*)data);
        else
            glUniform1f(location, DEF_TO_FLOAT(data, 0));
        break;
    case GL_FLOAT_VEC2:
        if (isArray)
            glUniform2fv(location, count, (GLfloat*)data);
        else
            glUniform2f(location, DEF_TO_FLOAT(data, 0), DEF_TO_FLOAT(data, 1));
        break;
    case GL_FLOAT_VEC3:
        if (isArray)
            glUniform3fv(location, count, (GLfloat*)data);
        else
            glUniform3f(location, DEF_TO_FLOAT(data, 0), DEF_TO_FLOAT(data, 1), DEF_TO_FLOAT(data, 2));
        break;
    case GL_FLOAT_VEC4:
        if (isArray)
            glUniform4fv(location, count, (GLfloat*)data);
        else
            glUniform4f(location, DEF_TO_FLOAT(data, 0), DEF_TO_FLOAT(data, 1), DEF_TO_FLOAT(data, 2),
                        DEF_TO_FLOAT(data, 3));
        break;
    case GL_FLOAT_MAT2:
        glUniformMatrix2fv(location, count, GL_FALSE, (GLfloat*)data);
        break;
    case GL_FLOAT_MAT3:
        glUniformMatrix3fv(location, count, GL_FALSE, (GLfloat*)data);
        break;
    case GL_FLOAT_MAT4:
        glUniformMatrix4fv(location, count, GL_FALSE, (GLfloat*)data);
        break;
        break;

    default:
        AXASSERT(false, "invalidate Uniform data type");
        break;
    }
}
#endif

ProgramGL::ProgramGL(std::string_view vertexShader, std::string_view fragmentShader)
    : Program(vertexShader, fragmentShader)
{
    _vertexShaderModule   = static_cast<ShaderModuleGL*>(ShaderCache::getInstance()->newVertexShaderModule(_vertexShader));
    _fragmentShaderModule = static_cast<ShaderModuleGL*>(ShaderCache::getInstance()->newFragmentShaderModule(_fragmentShader));

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
            AXLOGE("axmol:ERROR: {}: failed to link program: {} ", __FUNCTION__, errorInfo.get());
        }
        else
            AXLOGE("axmol:ERROR: {}: failed to link program ", __FUNCTION__);
        glDeleteProgram(_program);
        _program = 0;
    }
}

void ProgramGL::setBuiltinLocations()
{
    /*--- Builtin Attribs ---*/

    std::fill(_builtinAttributeLocation, _builtinAttributeLocation + Attribute::ATTRIBUTE_MAX, -1);

    /// a_position
    _builtinAttributeLocation[Attribute::POSITION] = getAttributeLocation(ATTRIBUTE_NAME_POSITION);

    /// a_color
    _builtinAttributeLocation[Attribute::COLOR] = getAttributeLocation(ATTRIBUTE_NAME_COLOR);

    /// a_texCoord
    _builtinAttributeLocation[Attribute::TEXCOORD] = getAttributeLocation(ATTRIBUTE_NAME_TEXCOORD);

    // a_normal
    _builtinAttributeLocation[Attribute::NORMAL] = getAttributeLocation(ATTRIBUTE_NAME_NORMAL);

    // a_instance
    _builtinAttributeLocation[Attribute::INSTANCE] = getAttributeLocation(ATTRIBUTE_NAME_INSTANCE);

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
        std::string_view name{attrName.get(), static_cast<size_t>(attrNameLen)};
        info.location = glGetAttribLocation(_program, name.data());
        info.type     = attrType;
        info.size     = UtilsGL::getGLDataTypeSize(attrType) * attrSize;
        CHECK_GL_ERROR_DEBUG();
        _activeAttribs[name] = info;
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

    // OpenGL UBO: uloc[0]: block_offset, uloc[1]: offset in block

    auto driver = DriverBase::getInstance();
    /* Query uniform blocks */
    clearUniformBuffers();

#if AX_GLES_PROFILE != 200
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
            static_cast<BufferGL*>(driver->newBuffer(blockSize, BufferType::UNIFORM, BufferUsage::DYNAMIC)),
            static_cast<int>(_totalBufferSize), blockSize);
        desc._ubo->updateData(nullptr, blockSize);  // ubo data can be nullptr

        CHECK_GL_ERROR_DEBUG();

        // increase _totalBufferSize
        _totalBufferSize += blockSize;
    }
#endif

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

#if AX_GLES_PROFILE != 200
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
        {  // must be samper: sampler2D or samplerCube
            assert(uniform.type == GL_SAMPLER_2D || uniform.type == GL_SAMPLER_CUBE);
            uniform.location     = glGetUniformLocation(_program, uniformName.data());
            uniform.bufferOffset = -1;
        }
#else
        if (uniform.type == GL_SAMPLER_2D || uniform.type == GL_SAMPLER_CUBE)
        {
            uniform.location     = glGetUniformLocation(_program, uniformName.data());
            uniform.bufferOffset = -1;
        }
        else
        {  // GLES2.0: GLSL100
            uniform.location     = glGetUniformLocation(_program, uniformFullName.data());
            uniform.bufferOffset = (uniform.size == 0) ? 0 : _totalBufferSize;
            _totalBufferSize += uniform.size * uniform.count;
        }
#endif

        _activeUniformInfos[uniformName] = uniform;

        _maxLocation = _maxLocation <= uniform.location ? (uniform.location + 1) : _maxLocation;
    }
}

void ProgramGL::bindUniformBuffers(const char* buffer, size_t bufferSize)
{
#if AX_GLES_PROFILE != 200
    for (GLuint blockIdx = 0; blockIdx < static_cast<GLuint>(_uniformBuffers.size()); ++blockIdx)
    {
        auto& desc = _uniformBuffers[blockIdx];
        desc._ubo->updateData(buffer + desc._location, desc._size);
        __gl->bindUniformBufferBase(blockIdx, desc._ubo->getHandler());
    }
#else
    for (auto&& iter : _activeUniformInfos)
    {
        auto& uniformInfo = iter.second;
        if (uniformInfo.size <= 0)
            continue;

        int elementCount = uniformInfo.count;
        setUniform(uniformInfo.count > 1, uniformInfo.location, elementCount, uniformInfo.type,
                   (void*)(buffer + uniformInfo.bufferOffset));
    }
#endif

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
    auto iter = _activeUniformInfos.find(uniform);
    if (iter != _activeUniformInfos.end())
    {
        const auto& uniformInfo = iter->second;
#if AX_ENABLE_CACHE_TEXTURE_DATA
        uniformLocation.vertStage.location = _mapToOriginalLocation.at(uniformInfo.location);
#else
        uniformLocation.vertStage.location = uniformInfo.location;
#endif
        uniformLocation.vertStage.offset = uniformInfo.bufferOffset;
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

const hlookup::string_map<UniformInfo>& ProgramGL::getAllActiveUniformInfo(ShaderStage stage) const
{
    return _activeUniformInfos;
}

std::size_t ProgramGL::getUniformBufferSize(ShaderStage stage) const
{
    return _totalBufferSize;
}

NS_AX_BACKEND_END
