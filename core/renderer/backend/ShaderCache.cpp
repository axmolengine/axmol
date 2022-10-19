/****************************************************************************
 Copyright (c) 2018 Xiamen Yaji Software Co., Ltd.

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

#include "ShaderCache.h"
#include "renderer/backend/Device.h"

NS_AX_BACKEND_BEGIN

std::unordered_map<std::size_t, backend::ShaderModule*> ShaderCache::_cachedShaders;
ShaderCache* ShaderCache::_sharedShaderCache = nullptr;

ShaderCache* ShaderCache::getInstance()
{
    if (!_sharedShaderCache)
    {
        _sharedShaderCache = new ShaderCache();
        if (!_sharedShaderCache->init())
        {
            AX_SAFE_DELETE(_sharedShaderCache);
        }
    }
    return _sharedShaderCache;
}

void ShaderCache::destroyInstance()
{
    AX_SAFE_RELEASE_NULL(_sharedShaderCache);
}

ShaderCache::~ShaderCache()
{
    for (auto&& shaderModule : _cachedShaders)
    {
        AX_SAFE_RELEASE(shaderModule.second);
    }
    AXLOGINFO("deallocing ShaderCache: %p", this);
}

bool ShaderCache::init()
{
    return true;
}

backend::ShaderModule* ShaderCache::newVertexShaderModule(std::string_view shaderSource)
{
    auto vertexShaderModule = newShaderModule(backend::ShaderStage::VERTEX, shaderSource);
    return vertexShaderModule;
}

backend::ShaderModule* ShaderCache::newFragmentShaderModule(std::string_view shaderSource)
{
    auto fragmenShaderModule = newShaderModule(backend::ShaderStage::FRAGMENT, shaderSource);
    return fragmenShaderModule;
}

backend::ShaderModule* ShaderCache::newShaderModule(backend::ShaderStage stage, std::string_view shaderSource)
{
    std::size_t key = std::hash<std::string_view>{}(shaderSource);
    auto iter       = _cachedShaders.find(key);
    if (_cachedShaders.end() != iter)
        return iter->second;

    auto shader = backend::Device::getInstance()->newShaderModule(stage, shaderSource);
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
