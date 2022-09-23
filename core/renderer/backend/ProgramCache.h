/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.

 https://axys1.github.io/

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
#include "base/CCRef.h"
#include "platform/CCPlatformMacros.h"
#include "Program.h"

#include <string>
#include <unordered_map>

#include "ProgramStateRegistry.h"

NS_AX_BACKEND_BEGIN
/**
 * @addtogroup _backend
 * @{
 */

struct AX_DLL VertexLayoutHelper
{
    static void setupForDummy(Program*);
    static void setupForPos(Program*);
    static void setupForTexture(Program*);
    static void setupForSprite(Program*);
    static void setupForDrawNode(Program*);
    static void setupForDrawNode3D(Program*);
    static void setupForSkyBox(Program*);
    static void setupForPU3D(Program*);
    static void setupForPosColor(Program*);
    static void setupForTerrain3D(Program*);
};

/**
 * Cache and reuse program object.
 */
class ProgramCache : public Ref
{
public:
    /** returns the shared instance */
    static ProgramCache* getInstance();

    /** purges the cache. It releases the retained instance. */
    static void destroyInstance();

    /// get built-in program
    backend::Program* getBuiltinProgram(uint32_t type) const;

    // get custom program, should call registerCustomProgramFactory first
    backend::Program* getCustomProgram(uint32_t type) const;

    // register custom program create factory
    void registerCustomProgramFactory(uint32_t type,
                                      std::string vertShaderSource,
                                      std::string fragShaderSource,
                                      std::function<void(Program*)> cbSetupLayout = VertexLayoutHelper::setupForDummy);

    /**
     * Remove a program object from cache.
     * @param program Specifies the program object to move.
     */
    void removeProgram(backend::Program* program);

    /**
     * Remove all unused program objects from cache.
     */
    void removeUnusedProgram();

    /**
     * Remove all program objects from cache.
     */
    void removeAllPrograms();

protected:
    ProgramCache() = default;
    virtual ~ProgramCache();

    /**
     * Pre-load programs into cache.
     */
    bool init();

    void registerProgramFactory(uint32_t internalType,
                                std::string&& vertShaderSource,
                                std::string&& fragShaderSource,
                                std::function<void(Program*)> cbSetupLayout);
    Program* addProgram(uint32_t internalType) const;

    std::function<Program*()> _builtinFactories[(int)ProgramType::BUILTIN_COUNT];
    std::unordered_map<uint32_t, std::function<Program*()>> _customFactories;

    mutable std::unordered_map<uint32_t, backend::Program*> _cachedPrograms;  ///< The cached program object.
    static ProgramCache* _sharedProgramCache;  ///< A shared instance of the program cache.
};

// end of _backend group
/// @}
NS_AX_BACKEND_END
