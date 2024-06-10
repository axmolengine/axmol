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

#pragma once

#include "Macros.h"
#include "base/Object.h"
#include "platform/PlatformMacros.h"
#include "Program.h"

#include <string>
#include <unordered_map>
#include <string_view>
#include "ProgramStateRegistry.h"

struct XXH64_state_s;

NS_AX_BACKEND_BEGIN
/**
 * @addtogroup _backend
 * @{
 */

/**
 * Cache and reuse program object.
 */
class AX_DLL ProgramManager
{
public:
    /** returns the shared instance */
    static ProgramManager* getInstance();

    /** purges the cache. It releases the retained instance. */
    static void destroyInstance();

    /// get built-in program
    Program* getBuiltinProgram(uint32_t type);

    /**
     * register a custom program
     * @return
     *   the id of custom program, 0: fail, the id can use by loadProgram
     */
    uint64_t registerCustomProgram(std::string_view vsName,
                                   std::string_view fsName,
                                   VertexLayoutType vlt = VertexLayoutType::Unspec,
                                   bool force           = false);

    /**
     * load a builtin/or custom program:
     * @param id: the id of program to load, the id value returned by registerCustomProgram
     *            or builtin programType, whe the id < ProgramType:BUILTIN_COUNT, this function
     *            is identical to 'getBuiltinProgram'
     */
    Program* loadProgram(uint64_t progId);

    /**
    * @brief Load a program with vsName, fsName as CUSTOM immediately without register
    * @param vsName the vertex shader name: custom/xxx_vs
    * @param fsName the fragment shader name: custom/xxx_vs
    * @param vlt the builtin vertex layout type used for loading program
    * @return Program* (nullable)
    * @remark the returend program type always ProgramType::CUSTOM_PROGRAM
    */
    Program* loadProgram(std::string_view vsName,
                               std::string_view fsName,
                               VertexLayoutType vlt = VertexLayoutType::Unspec);

     /**
     * Unload a program object from cache.
     * @param program Specifies the program object to move.
     */
    void unloadProgram(Program* program);

    /**
     * Unload all unused program objects from cache.
     */
    void unloadUnusedPrograms();

    /**
     * Unload all program objects from cache.
     */
    void unloadAllPrograms();

    /**
     * Remove a program object from cache.
     * @param program Specifies the program object to move.
     */
    AX_DEPRECATED_ATTRIBUTE void removeProgram(Program* prog) { unloadProgram(prog); }

    /**
     * Remove all unused program objects from cache.
     */
    AX_DEPRECATED_ATTRIBUTE void removeUnusedProgram() { unloadUnusedPrograms(); }

    /**
     * Remove all program objects from cache.
     */
    AX_DEPRECATED_ATTRIBUTE void removeAllPrograms() { unloadAllPrograms(); }

protected:
    ProgramManager();
    virtual ~ProgramManager();

    /**
     * Pre-load programs into cache.
     */
    bool init();

    /**
     * register a program
     */
    uint64_t registerProgram(uint32_t progType,
                             std::string_view vsName,
                             std::string_view fsName,
                             VertexLayoutType vlt,
                             bool force = false);

    /**
     * load a custom program by vsName, fsName, vertexLayout
     */
    Program* loadProgram(std::string_view vsName,
                         std::string_view fsName,
                         uint32_t progType,
                         uint64_t progId,
                         VertexLayoutType vlt);

    uint64_t computeProgramId(std::string_view vsName, std::string_view fsName);

    struct BuiltinRegInfo
    {  // builtin shader name is literal string, so use std::string_view ok
        std::string_view vsName;
        std::string_view fsName;
        VertexLayoutType vlt;
    };

    BuiltinRegInfo _builtinRegistry[(int)backend::ProgramType::BUILTIN_COUNT];
    std::unordered_map<int64_t, BuiltinRegInfo> _customRegistry;

    std::unordered_map<int64_t, Program*> _cachedPrograms;  ///< The cached program object.

    XXH64_state_s* _programIdGen;

    static ProgramManager* _sharedProgramManager;  ///< A shared instance of the program cache.
};

using ProgramCache = ProgramManager;  // for compatible

// end of _backend group
/// @}
NS_AX_BACKEND_END

/**
 * @alias some feq use types to namespace ax
 */

NS_AX_BEGIN

using ProgramType      = ::ax::backend::ProgramType;
using ProgramState     = ::ax::backend::ProgramState;
using Program          = ::ax::backend::Program;
using VertexLayout     = ::ax::backend::VertexLayout;
using VertexLayoutType = ::ax::backend::VertexLayoutType;
using ProgramManager   = ::ax::backend::ProgramManager;
using ProgramRegistry  = ::ax::backend::ProgramStateRegistry;

NS_AX_END
