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
    computeLocations();
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
}

ProgramGL::~ProgramGL()
{
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
    static_cast<ShaderModuleGL*>(_vertexShaderModule)
        ->compileShader(backend::ShaderStage::VERTEX, _vertexShader);
    static_cast<ShaderModuleGL*>(_fragmentShaderModule)
        ->compileShader(backend::ShaderStage::FRAGMENT, _fragmentShader);
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

void ProgramGL::computeLocations()
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

    location = _ubo.getUniformLocationByName(UNIFORM_NAME_MVP_MATRIX.data()).location[0];
    _builtinUniformLocation[Uniform::MVP_MATRIX].location[0] = location;
    _builtinUniformLocation[Uniform::MVP_MATRIX].location[1] =
        _activeUniformInfos[UNIFORM_NAME_MVP_MATRIX].bufferOffset;

    /// u_textColor
    location = _ubo.getUniformLocationByName(UNIFORM_NAME_TEXT_COLOR.data()).location[0];
    _builtinUniformLocation[Uniform::TEXT_COLOR].location[0] = location;
    _builtinUniformLocation[Uniform::TEXT_COLOR].location[1] =
        _activeUniformInfos[UNIFORM_NAME_TEXT_COLOR].bufferOffset;

    /// u_effectColor
    location = _ubo.getUniformLocationByName(UNIFORM_NAME_EFFECT_COLOR.data()).location[0];
    _builtinUniformLocation[Uniform::EFFECT_COLOR].location[0] = location;
    _builtinUniformLocation[Uniform::EFFECT_COLOR].location[1] =
        _activeUniformInfos[UNIFORM_NAME_EFFECT_COLOR].bufferOffset;

    /// u_effectType
    location = _ubo.getUniformLocationByName(UNIFORM_NAME_EFFECT_TYPE.data()).location[0];
    _builtinUniformLocation[Uniform::EFFECT_TYPE].location[0] = location;
    _builtinUniformLocation[Uniform::EFFECT_TYPE].location[1] =
        _activeUniformInfos[UNIFORM_NAME_EFFECT_TYPE].bufferOffset;

    /// u_tex0
    location                                              = glGetUniformLocation(_program, UNIFORM_NAME_TEXTURE.data());
    _builtinUniformLocation[Uniform::TEXTURE].location[0] = location;

    /// u_tex1
    location = glGetUniformLocation(_program, UNIFORM_NAME_TEXTURE1.data());
    _builtinUniformLocation[Uniform::TEXTURE1].location[0] = location;
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

    _ubo.setProgram(_program);
    _ubo.bindUniformBlock(UniformBlockStage::ALL_STAGES);
    _ubo.generateUniformOffsets(UniformBlockStage::ALL_STAGES);
    _ubo.allocateUBOBuffer();

    GLint numOfUniforms = 0;
    glGetProgramiv(_program, GL_ACTIVE_UNIFORMS, &numOfUniforms);
    if (!numOfUniforms)
        return;

    UniformInfo uniform;
    GLint length     = 0;
    _totalFragBufferSize = 0;
    _totalVertBufferSize = 0;
    _maxLocation     = -1;
    _activeUniformInfos.clear();
    GLchar uniformName[MAX_UNIFORM_NAME_LENGTH + 1];
    for (int i = 0; i < numOfUniforms; ++i)
    {
        uniform.isUBO = false;
        uniform.isFragment = false;

        glGetActiveUniform(_program, i, MAX_UNIFORM_NAME_LENGTH, &length, &uniform.count, &uniform.type, uniformName);
        uniformName[length] = '\0';

        if (memcmp(uniformName, "vs_ub.", 5 * sizeof(GLchar)) == 0 ||
            memcmp(uniformName, "fs_ub.", 5 * sizeof(GLchar)) == 0)
        {
            if (uniformName[0] == 'f')
                uniform.isFragment = true;

            while (uniformName[0] != '.')
                for (int i = 0; i < MAX_UNIFORM_NAME_LENGTH; i++)
                    uniformName[i] = uniformName[i + 1];
            if (uniformName[0] == '.')
                for (int i = 0; i < MAX_UNIFORM_NAME_LENGTH; i++)
                    uniformName[i] = uniformName[i + 1];

            uniform.isUBO = true;
        }

        if (length > 3)
        {
            char* c = strrchr(uniformName, '[');
            if (c)
            {
                *c              = '\0';
                uniform.isArray = true;
            }
        }
        uniform.location                 = glGetUniformLocation(_program, uniformName);
        uniform.size                     = UtilsGL::getGLDataTypeSize(uniform.type);
        uniform.bufferOffset             = (uniform.size == 0u) ? 0 : static_cast<unsigned>(uniform.isFragment ? _totalFragBufferSize : _totalVertBufferSize);
        if (_activeUniformInfos.find(uniformName) == _activeUniformInfos.end())
            _activeUniformInfos[uniformName] = uniform;
        (uniform.isFragment ? _totalFragBufferSize : _totalVertBufferSize) += uniform.size * uniform.count;
        _maxLocation = _maxLocation <= uniform.location ? (uniform.location + 1) : _maxLocation;
    }
}

void ProgramGL::generateUniformBlockBuffers()
{

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
    auto loc = _builtinUniformLocation[name];
    if (loc.location[0] == UNIFORM_BLOCK_IDENTIFIER)
        return _ubo.getUniformLocationByName(mapLocationEnumToUBO(name).data());
    else
        return loc;
}

UniformLocation ProgramGL::getUniformLocation(std::string_view uniform) const
{
    UniformLocation uniformLocation;
    if (_ubo.uniformOffsetsByName.find(uniform.data()) != _ubo.uniformOffsetsByName.end())
    {
        uniformLocation = _ubo.uniformOffsetsByName.at(uniform.data());
    }
    else if (_activeUniformInfos.find(uniform) != _activeUniformInfos.end())
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
    return stage == ShaderStage::VERTEX ? _totalVertBufferSize : _totalFragBufferSize;
}

void UniformBlockDescriptor::bindUniformBlock(GLint _program, UniformBlockStage _stage)
{
    this->stage = _stage;

    const char* blockName = stage == UniformBlockStage::VERTEX ? "vs_ub" : "fs_ub";
    blockIndex            = glGetUniformBlockIndex(_program, blockName);
    if (blockIndex == GL_INVALID_INDEX)
    {
        stage = UniformBlockStage::UBO_NOT_FOUND;
        return;
    }

    bindingPoint = (GLint)stage - 1;
    glUniformBlockBinding(_program, blockIndex, bindingPoint);

    glGetActiveUniformBlockiv(_program, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

    if (blockSize == -1)
    {
        stage = UniformBlockStage::UBO_NOT_FOUND;
        return;
    }

    glGetActiveUniformBlockiv(_program, blockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &numUniforms);

    uniformIndices = new GLint[numUniforms];
    glGetActiveUniformBlockiv(_program, blockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, uniformIndices);

    uniformOffsets = new GLint[numUniforms];
    glGetActiveUniformsiv(_program, numUniforms, reinterpret_cast<GLuint*>(uniformIndices), GL_UNIFORM_OFFSET,
                          uniformOffsets);
}

UniformBlockDescriptor::~UniformBlockDescriptor()
{
    AX_SAFE_DELETE(uniformIndices);
    AX_SAFE_DELETE(uniformOffsets);
}

void UniformBlockHandler::generateUniformOffsets(UniformBlockStage _stage)
{
    if (_stage == UniformBlockStage::ALL_STAGES)
    {
        generateUniformOffsets(UniformBlockStage::VERTEX);
        generateUniformOffsets(UniformBlockStage::FRAGMENT);
    }
    else
    {
        auto&& ubo = _stage == UniformBlockStage::VERTEX ? _vertexUBO : _fragmentUBO;

        if (ubo.stage == UniformBlockStage::UBO_NOT_FOUND)
            return;

        vertBlockSize = _stage == UniformBlockStage::VERTEX ? ubo.blockSize : vertBlockSize;
        fragBlockSize = _stage == UniformBlockStage::FRAGMENT ? ubo.blockSize : fragBlockSize;

        yasio::basic_byte_buffer<GLchar> buffer;
        for (GLint i = 0; i < ubo.numUniforms; ++i)
        {
            GLint uniformOffset = ubo.uniformOffsets[i];

            buffer.resize(MAX_UNIFORM_NAME_LENGTH, std::true_type{});

            GLint index = ubo.uniformIndices[i];

            GLsizei cch{};
            GLenum dataType{};
            GLint arrayCount{};
            glGetActiveUniform(_program, ubo.uniformIndices[i], MAX_UNIFORM_NAME_LENGTH, &cch, &arrayCount,
                               &dataType,
                               buffer.data());
            std::string_view uniformName{buffer.data(), static_cast<size_t>(cch)};

            auto dot = uniformName.find_last_of('.');
            if (dot != std::string::npos)
                uniformName.remove_prefix(dot + 1);

            auto loc        = UniformLocation{};
            loc.location[0] = UNIFORM_BLOCK_IDENTIFIER;
            loc.location[1] = uniformOffset / sizeof(float);
            loc.shaderStage = _stage == UniformBlockStage::VERTEX ? ShaderStage::VERTEX : ShaderStage::FRAGMENT;

            uniformOffsetsByName.emplace(std::string{uniformName}, loc);
        }
    }
}

void UniformBlockHandler::allocateUBOBuffer()
{
    if (_UBOVertBuffer || _UBOFragBuffer)
        return;

    int totalStagesSize = vertBlockSize + fragBlockSize;
    int32_t* dummy      = new int32_t[totalStagesSize];
    memset(dummy, 0, totalStagesSize);

    _UBOVertBuffer =
        (BufferGL*)Device::getInstance()->newBuffer(vertBlockSize, BufferType::UNIFORM, BufferUsage::DYNAMIC);
    _UBOVertBuffer->updateData(dummy, vertBlockSize);

    _UBOFragBuffer =
        (BufferGL*)Device::getInstance()->newBuffer(vertBlockSize, BufferType::UNIFORM, BufferUsage::DYNAMIC);
    _UBOFragBuffer->updateData(dummy, vertBlockSize);
}

UniformLocation UniformBlockHandler::getUniformLocationByName(std::string name) const
{
    auto it = uniformOffsetsByName.find(name);
    if (it != uniformOffsetsByName.end())
        return it->second;
    else
        return {};
}

UniformBlockHandler::~UniformBlockHandler()
{
    AX_SAFE_RELEASE(_UBOVertBuffer);
    AX_SAFE_RELEASE(_UBOFragBuffer);
}

void UniformBlockHandler::bindUniformBlock(UniformBlockStage _stage)
{
    switch (_stage)
    {
    case UniformBlockStage::VERTEX:
        _vertexUBO.bindUniformBlock(_program, UniformBlockStage::VERTEX);
        break;
    case UniformBlockStage::FRAGMENT:
        _fragmentUBO.bindUniformBlock(_program, UniformBlockStage::FRAGMENT);
        break;
    case UniformBlockStage::ALL_STAGES:
        _vertexUBO.bindUniformBlock(_program, UniformBlockStage::VERTEX);
        _fragmentUBO.bindUniformBlock(_program, UniformBlockStage::FRAGMENT);
        break;
    }
}

void UniformBlockHandler::bindUniformBlocks()
{
    bindUniformBlock(UniformBlockStage::ALL_STAGES);
}

NS_AX_BACKEND_END
