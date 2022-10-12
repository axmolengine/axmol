/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2022 Bytedance Inc.

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
    static void setupDummy(Program*);
    static void setupPos(Program*);
    static void setupTexture(Program*);
    static void setupSprite(Program*);
    static void setupDrawNode(Program*);
    static void setupDrawNode3D(Program*);
    static void setupSkyBox(Program*);
    static void setupPU3D(Program*);
    static void setupPosColor(Program*);
    static void setupTerrain3D(Program*);
};

/**
 * Cache and reuse program object.
 */
class AX_DLL ProgramManager : public Ref
{
public:
    /** new progrma with vertexLayout setup support, user should use this API */
    static Program* newProgram(std::string_view vertShaderSource,
                                        std::string_view fragShaderSource,
        std::function<void(Program*)> fnSetupLayout = VertexLayoutHelper::setupDummy);

    /** returns the shared instance */
    static ProgramManager* getInstance();

    /** purges the cache. It releases the retained instance. */
    static void destroyInstance();

    /// get built-in program
    Program* getBuiltinProgram(uint32_t type) const;

    // get custom program, should call registerCustomProgramFactory first
    Program* getCustomProgram(uint32_t type) const;

    // register custom program create factory
    void registerCustomProgramFactory(uint32_t type,
                                      std::string vertShaderSource,
                                      std::string fragShaderSource,
        std::function<void(Program*)> fnSetupLayout = VertexLayoutHelper::setupDummy);

    /**
     * Remove a program object from cache.
     * @param program Specifies the program object to move.
     */
    void removeProgram(Program* program);

    /**
     * Remove all unused program objects from cache.
     */
    void removeUnusedProgram();

    /**
     * Remove all program objects from cache.
     */
    void removeAllPrograms();

protected:
    ProgramManager() = default;
    virtual ~ProgramManager();

    /**
     * Pre-load programs into cache.
     */
    bool init();

    void registerProgramFactory(uint32_t internalType,
                                std::string&& vertShaderSource,
                                std::string&& fragShaderSource,
                                std::function<void(Program*)> fnSetupLayout);
    Program* addProgram(uint32_t internalType) const;

    std::function<Program*()> _builtinFactories[(int)backend::ProgramType::BUILTIN_COUNT];
    std::unordered_map<uint32_t, std::function<Program*()>> _customFactories;

    mutable std::unordered_map<uint32_t, Program*> _cachedPrograms;  ///< The cached program object.
    static ProgramManager* _sharedProgramManager;  ///< A shared instance of the program cache.
};

using ProgramCache = ProgramManager; // for compatible

// end of _backend group
/// @}
NS_AX_BACKEND_END

/**
* @alias some feq use types to namespace ax
*/

NS_AX_BEGIN

using ProgramType = ::ax::backend::ProgramType;
using Program = ::ax::backend::Program;
using VertexLayout = ::ax::backend::VertexLayout;
using VertexLayoutHelper = ::ax::backend::VertexLayoutHelper;
using ProgramManager = ::ax::backend::ProgramManager;
using ProgramRegistry = ::ax::backend::ProgramStateRegistry;

NS_AX_END
