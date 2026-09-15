/****************************************************************************
 Copyright (c) 2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include "axmol/rhi/RHITypes.h"
#include "axmol/base/Object.h"
#include "axmol/platform/PlatformMacros.h"
#include "axmol/rhi/ShaderModule.h"

#include <string>
#include <unordered_map>

namespace ax::rhi
{
/**
 * @addtogroup _rhi
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
    void removeAllShaders();

    /**
     * Create a vertex shader module and add it to cache.
     * If it is created before, then just return the cached shader module.
     * @param payload The shader payload with reflected information.
     */
    rhi::ShaderModule* acquireVertexShaderModule(Data& payload);

    /**
     * Create a fragment shader module.
     * If it is created before, then just return the cached shader module.
     * @param payload The shader payload with reflected information.
     */
    rhi::ShaderModule* acquireFragmentShaderModule(Data& payload);

    rhi::ShaderModule* acquireComputeShaderModule(Data& payload);

    /**
     * Remove all unused shaders.
     */
    void removeUnusedShader();

#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    void recompileAll();
#endif

protected:
    /**
     * New a shaderModule.
     * If it was created before, then just return the cached shader module.
     * Otherwise add it to cache and return the object.
     * @param stage Specifies whether is vertex shader or fragment shader.
     * @param source Specifies shader source.
     * @return A ShaderModule object.
     */
    rhi::ShaderModule* acquireShaderModule(rhi::ShaderStage stage, Data& payload);

    std::unordered_map<uint64_t, rhi::ShaderModule*> _cachedShaders;
};

// end of _rhi group
/// @}
}  // namespace ax::rhi
