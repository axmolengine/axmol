/****************************************************************************
 Copyright (c) 2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

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
#pragma once

#include "Macros.h"
#include "base/Object.h"
#include "platform/PlatformMacros.h"
#include "renderer/backend/ShaderModule.h"

#include <string>
#include <unordered_map>

NS_AX_BACKEND_BEGIN
/**
 * @addtogroup _backend
 * @{
 */

/**
 * Create and reuse shader module.
 */
class AX_DLL ShaderCache
{
public:
    static ShaderCache* getInstance();
    static void destroyInstance();

    ~ShaderCache();

    /** purges the cache. It releases the retained instance. */
    void purge();

    /**
     * Create a vertex shader module and add it to cache.
     * If it is created before, then just return the cached shader module.
     * @param shaderSource The source code of the shader.
     */
    backend::ShaderModule* newVertexShaderModule(std::string_view shaderSource);

    /**
     * Create a fragment shader module.
     * If it is created before, then just return the cached shader module.
     * @param shaderSource The source code of the shader.
     */
    backend::ShaderModule* newFragmentShaderModule(std::string_view shaderSource);

    /**
     * Remove all unused shaders.
     */
    void removeUnusedShader();

protected:
    /**
     * New a shaderModule.
     * If it was created before, then just return the cached shader module.
     * Otherwise add it to cache and return the object.
     * @param stage Specifies whether is vertex shader or fragment shader.
     * @param source Specifies shader source.
     * @return A ShaderModule object.
     */
    backend::ShaderModule* newShaderModule(backend::ShaderStage stage, std::string_view shaderSource);

    std::unordered_map<std::size_t, backend::ShaderModule*> _cachedShaders;
};

// end of _backend group
/// @}
NS_AX_BACKEND_END
