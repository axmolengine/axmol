/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/rhi/opengl/ShaderModuleGL.h"

#include "axmol/platform/PlatformMacros.h"
#include "axmol/base/Macros.h"
#include "axmol/tlx/utility.hpp"
#include "axmol/rhi/opengl/OpenGLState.h"

namespace ax::rhi::gl
{

ShaderModuleImpl::ShaderModuleImpl(ShaderStage stage, Data& data) : ShaderModule(stage, data)
{
    compileShader();
}

ShaderModuleImpl::~ShaderModuleImpl()
{
    deleteShader();
}

void ShaderModuleImpl::recompileShader()
{
    compileShader();
}

void ShaderModuleImpl::compileShader()
{
    GLenum shaderType = _stage == ShaderStage::VERTEX     ? GL_VERTEX_SHADER
                        : _stage == ShaderStage::FRAGMENT ? GL_FRAGMENT_SHADER
                                                          : GL_COMPUTE_SHADER;
    _shader           = glCreateShader(shaderType);
    if (!_shader)
        return;

    const GLchar* sourcePtr = reinterpret_cast<const GLchar*>(_codeSpan.data());
    GLint sourceLen         = static_cast<GLint>(_codeSpan.size());
    glShaderSource(_shader, 1, &sourcePtr, &sourceLen);
    glCompileShader(_shader);

    GLint status = 0;
    glGetShaderiv(_shader, GL_COMPILE_STATUS, &status);

    _compiled = !!status;
    if (!_compiled)
    {
        GLint logLength = 0;
        glGetShaderiv(_shader, GL_INFO_LOG_LENGTH, &logLength);

        if (logLength > 1)
        {
            auto errorLog = tlx::make_unique_for_overwrite<char[]>(static_cast<size_t>(logLength));
            glGetShaderInfoLog(_shader, logLength, nullptr, (GLchar*)errorLog.get());
            AXLOGE("axmol:ERROR: Failed to compile shader, detail: {}\n{}", errorLog.get(),
                   std::string_view{reinterpret_cast<char*>(_codeSpan.data()), _codeSpan.size()});
        }
        else
        {
            AXLOGE("axmol:ERROR: Failed to compile shader without errors.");
        }

        deleteShader();
    }
}

void ShaderModuleImpl::deleteShader()
{
    if (_shader)
    {
        glDeleteShader(_shader);
        _shader = 0;
    }
}

}  // namespace ax::rhi::gl
