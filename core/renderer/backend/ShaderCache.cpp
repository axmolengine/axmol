/****************************************************************************
 Copyright (c) 2018 Xiamen Yaji Software Co., Ltd.
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

#include "ShaderCache.h"
#include "renderer/backend/DriverBase.h"

NS_AX_BACKEND_BEGIN

static ShaderCache* s_instance;

ShaderCache* ShaderCache::getInstance()
{
    if (s_instance)
        return s_instance;
    return (s_instance = new ShaderCache());
}

void ShaderCache::destroyInstance()
{
    AX_SAFE_DELETE(s_instance);
}

ShaderCache::~ShaderCache()
{
    purge();
}

void ShaderCache::purge()
{
    for (auto& [_, shaderModule] : _cachedShaders)
    {
        AX_SAFE_RELEASE(shaderModule);
    }
    AXLOGINFO("purging ShaderCache");
}

backend::ShaderModule* ShaderCache::newVertexShaderModule(std::string_view shaderSource)
{
    return newShaderModule(backend::ShaderStage::VERTEX, shaderSource);
}

backend::ShaderModule* ShaderCache::newFragmentShaderModule(std::string_view shaderSource)
{
    return newShaderModule(backend::ShaderStage::FRAGMENT, shaderSource);
}

backend::ShaderModule* ShaderCache::newShaderModule(backend::ShaderStage stage, std::string_view shaderSource)
{
    const std::size_t key = std::hash<std::string_view>{}(shaderSource);
    const auto iter       = _cachedShaders.find(key);

    if (_cachedShaders.end() != iter)
        return iter->second;

    ShaderModule* const shader = backend::DriverBase::getInstance()->newShaderModule(stage, shaderSource);

    shader->setHashValue(key);
    _cachedShaders.emplace(key, shader);

    return shader;
}

void ShaderCache::removeUnusedShader()
{
    for (auto iter = _cachedShaders.cbegin(); iter != _cachedShaders.cend();)
    {
        auto shaderModule = iter->second;
        if (shaderModule->getReferenceCount() == 1)
        {
            //            AXLOG("axmol: TextureCache: removing unused program");
            shaderModule->release();
            iter = _cachedShaders.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

NS_AX_BACKEND_END
