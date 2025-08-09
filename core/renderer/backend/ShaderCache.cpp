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

#include "xxhash.h"

namespace ax::backend {

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
    AXLOGV("purging ShaderCache");
}

backend::ShaderModule* ShaderCache::acquireVertexShaderModule(std::string_view shaderSource)
{
    return acquireShaderModule(backend::ShaderStage::VERTEX, shaderSource);
}

backend::ShaderModule* ShaderCache::acquireFragmentShaderModule(std::string_view shaderSource)
{
    return acquireShaderModule(backend::ShaderStage::FRAGMENT, shaderSource);
}

backend::ShaderModule* ShaderCache::acquireShaderModule(backend::ShaderStage stage, std::string_view shaderSource)
{
    const auto key = XXH64(shaderSource.data(), shaderSource.size(), 0);
    const auto iter       = _cachedShaders.find(key);

    if (_cachedShaders.end() != iter)
    {
        AX_SAFE_RETAIN(iter->second);
        return iter->second;
    }

    ShaderModule* const shader = backend::DriverBase::getInstance()->createShaderModule(stage, shaderSource);

    shader->setHashValue(key);
    _cachedShaders.emplace(key, shader);

    AX_SAFE_RETAIN(shader);

    return shader;
}

void ShaderCache::removeUnusedShader()
{
    for (auto iter = _cachedShaders.cbegin(); iter != _cachedShaders.cend();)
    {
        auto shaderModule = iter->second;
        if (shaderModule->getReferenceCount() == 1)
        {
            //            AXLOGD("TextureCache: removing unused program");
            shaderModule->release();
            iter = _cachedShaders.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

}
