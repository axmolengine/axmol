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
#include "axmol/tlx/vector.hpp"
#include "axmol/tlx/byte_buffer.hpp"
#include "axmol/rhi/opengl/UtilsGL.h"
#include "axmol/rhi/opengl/OpenGLState.h"
#include "axmol/rhi/opengl/BufferGL.h"

namespace ax::rhi::gl
{
ProgramImpl::ProgramImpl(Data& vsData, Data& fsData) : Program(vsData, fsData)
{
    compileProgram();
#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    _backToForegroundListener =
        CustomEventListener::create(EVENT_RENDERER_RECREATED, [this](CustomEvent*) { this->reloadProgram(); });
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_backToForegroundListener, -1);
#endif
}

ProgramImpl::ProgramImpl(Data& csData) : Program(csData)
{
    compileProgram();
#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    _backToForegroundListener =
        CustomEventListener::create(EVENT_RENDERER_RECREATED, [this](CustomEvent*) { this->reloadProgram(); });
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_backToForegroundListener, -1);
#endif
}

ProgramImpl::~ProgramImpl()
{
    deleteUniformBuffers();
    if (_program)
        glDeleteProgram(_program);

#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    Director::getInstance()->getEventDispatcher()->removeEventListener(_backToForegroundListener);
#endif
}

#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
void ProgramImpl::reloadProgram()
{
    compileProgram();
}
#endif

bool ProgramImpl::isValid() const
{
    return _program != 0;
}

void ProgramImpl::compileProgram()
{
    GLuint computeShader = 0;
    GLuint vertShader    = 0;
    GLuint fragShader    = 0;
    if (_csModule)
    {
        computeShader = static_cast<ShaderModuleImpl*>(_csModule)->getShader();
        if (computeShader == 0)
            return;
    }
    else
    {
        if (_vsModule == nullptr || _fsModule == nullptr)
            return;

        vertShader = static_cast<ShaderModuleImpl*>(_vsModule)->getShader();
        fragShader = static_cast<ShaderModuleImpl*>(_fsModule)->getShader();
        if (vertShader == 0 || fragShader == 0)
            return;
    }

    _program = glCreateProgram();
    if (!_program)
        return;

    if (computeShader)
        glAttachShader(_program, computeShader);
    else
    {
        glAttachShader(_program, vertShader);
        glAttachShader(_program, fragShader);
    }

    glLinkProgram(_program);

    GLint status = 0;
    glGetProgramiv(_program, GL_LINK_STATUS, &status);
    if (GL_FALSE == status)
    {
        GLint errorInfoLen = 0;
        glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &errorInfoLen);

        std::string_view vsSource = _vsModule ? _vsModule->getCode() : std::string_view{};
        std::string_view fsSource = _fsModule ? _fsModule->getCode() : std::string_view{};
        std::string_view csSource = _csModule ? _csModule->getCode() : std::string_view{};
        if (errorInfoLen > 1)
        {
            auto errorInfo = tlx::make_unique_for_overwrite<char[]>(static_cast<size_t>(errorInfoLen));
            glGetProgramInfoLog(_program, errorInfoLen, NULL, errorInfo.get());
            AXLOGE(
                "axmol:ERROR: {}: failed to link program: {} \n--- vsSource ---\n{}\n --- fsSource ---\n{}\n --- "
                "csSource ---\n{}",
                __FUNCTION__, errorInfo.get(), vsSource, fsSource, csSource);
        }
        else
            AXLOGE("axmol:ERROR: {}: failed to link program", __FUNCTION__);
        glDeleteProgram(_program);
        _program = 0;
        return;
    }

    /// building runtime reflections and ubos

    /// --- update runtime location
    for (auto& entry : _activeTextureInfos)
    {
        entry.second->runtimeLocation = glGetUniformLocation(_program, entry.first.data());
    }

    /// --- create uniform buffers and setup binding point
    auto driver = axdrv;
    deleteUniformBuffers();

#if defined(_WIN32) && AX_GLES_PROFILE
    for (auto& uboInfo : _activeUniformBlockInfos)
    {
        const auto blockIndex = glGetUniformBlockIndex(_program, uboInfo.name.data());
        uboInfo.runtimeIndex  = blockIndex == GL_INVALID_INDEX ? -1 : static_cast<int>(blockIndex);
        if (blockIndex != GL_INVALID_INDEX)
        {
            glUniformBlockBinding(_program, blockIndex, uboInfo.binding);
            CHECK_GL_ERROR_DEBUG();
        }
        else
            AXLOGE("OpenGL uniform block '{}' was not found in linked program", uboInfo.name);
        _uniformBuffers.push_back(driver->createBuffer(uboInfo.sizeBytes, BufferType::UNIFORM, BufferUsage::DYNAMIC));
    }
#else
    if (!_activeUniformBlockInfos.empty())
    {
        // Desktop GL or Android GLES, should use block size from driver
        // see issue: https://github.com/axmolengine/axmol/issues/2987
        tlx::hash_map<int, UniformBlockInfo*> blockInfoMap;
        blockInfoMap.reserve(_activeUniformBlockInfos.size());
        uint32_t cpuOffset = 0;
        for (auto& uboInfo : _activeUniformBlockInfos)
        {
            const auto blockIndex = glGetUniformBlockIndex(_program, uboInfo.name.data());
            uboInfo.runtimeIndex  = blockIndex == GL_INVALID_INDEX ? -1 : static_cast<int>(blockIndex);
            if (blockIndex == GL_INVALID_INDEX)
            {
                AXLOGE("OpenGL uniform block '{}' was not found in linked program", uboInfo.name);
                _uniformBuffers.push_back(
                    driver->createBuffer(uboInfo.sizeBytes, BufferType::UNIFORM, BufferUsage::DYNAMIC));
                continue;
            }
            glUniformBlockBinding(_program, blockIndex, uboInfo.binding);
            CHECK_GL_ERROR_DEBUG();

            GLint blockSize{0};
            glGetActiveUniformBlockiv(_program, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
            if (uboInfo.sizeBytes != blockSize)
                uboInfo.sizeBytes = blockSize;
            if (uboInfo.cpuOffset != cpuOffset)
                uboInfo.cpuOffset = cpuOffset;
            cpuOffset += blockSize;

            blockInfoMap.emplace(blockIndex, &uboInfo);

            _uniformBuffers.push_back(
                driver->createBuffer(uboInfo.sizeBytes, BufferType::UNIFORM, BufferUsage::DYNAMIC));
        }
        if (_uniformBufferSize != cpuOffset)
            _uniformBufferSize = cpuOffset;

        /*
         * Adjust offset, cpuOffset, elementSize from Graphics API
         */
        GLchar nameBuffer[UNIFORM_NAME_BUFFER_SIZE];
        GLint nameLen       = 0;
        GLint numOfUniforms = 0;
        glGetProgramiv(_program, GL_ACTIVE_UNIFORMS, &numOfUniforms);
        for (GLint i = 0; i < numOfUniforms; ++i)
        {
            GLint count{0};
            GLenum uniformType{0};
            glGetActiveUniform(_program, i, UNIFORM_NAME_BUFFER_SIZE, &nameLen, &count, &uniformType, nameBuffer);

            std::string_view uniformFullName{nameBuffer, static_cast<size_t>(nameLen)};

            // Trim name vs_ub.xxx[0] --> vs_ub.xxx
            const auto pos = uniformFullName.find('[');
            if (pos != std::string_view::npos)
                uniformFullName = uniformFullName.substr(0, pos);

            GLint blockIndex{-1};
            glGetActiveUniformsiv(_program, 1, reinterpret_cast<const GLuint*>(&i), GL_UNIFORM_BLOCK_INDEX,
                                  &blockIndex);
            if (blockIndex != -1)
            {  // member of uniform block
                const auto blockInfoIt = blockInfoMap.find(blockIndex);
                if (blockInfoIt == blockInfoMap.end() || blockInfoIt->second == nullptr)
                    continue;

                auto* blockInfo = blockInfoIt->second;
                GLint uniformOffset{-1};
                glGetActiveUniformsiv(_program, 1, reinterpret_cast<const GLuint*>(&i), GL_UNIFORM_OFFSET,
                                      &uniformOffset);
                auto elementSize = UtilsGL::getGLDataTypeSize(uniformType);

                const auto uniformId = makeUniformNameKey(uniformFullName);
                const auto uniformIt = _activeUniformInfos.find(uniformId);
                if (uniformIt == _activeUniformInfos.end())
                    continue;

                // A desktop GL driver may expose a linked UBO member with a
                // name that is absent from the archive reflection. There is
                // no CPU-side UniformInfo to adjust in that case.
                auto& uniformInfo = uniformIt->second;
                if (uniformInfo.cpuOffset != blockInfo->cpuOffset)
                    uniformInfo.cpuOffset = blockInfo->cpuOffset;

                if (uniformInfo.offset != uniformOffset)
                    uniformInfo.offset = uniformOffset;

                const auto sizeBytes = elementSize * uniformInfo.count;
                if (uniformInfo.sizeBytes != sizeBytes)
                    uniformInfo.sizeBytes = sizeBytes;
            }
        }
    }
#endif
}

void ProgramImpl::bindUniformBuffers(const uint8_t* buffer, size_t bufferSize)
{
    const auto uboCount = _activeUniformBlockInfos.size();
    for (size_t i = 0; i < uboCount; ++i)
    {
        auto& info = _activeUniformBlockInfos[i];
        auto ubo   = static_cast<BufferImpl*>(_uniformBuffers[i]);
        ubo->updateData(buffer + info.cpuOffset, info.sizeBytes);
        __state->bindUniformBufferBase(info.binding, ubo->internalHandle());
    }

    CHECK_GL_ERROR_DEBUG();
}

void ProgramImpl::deleteUniformBuffers()
{
    if (_uniformBuffers.empty())
        return;
    for (auto ubo : _uniformBuffers)
        delete ubo;
    _uniformBuffers.clear();
}

}  // namespace ax::rhi::gl
