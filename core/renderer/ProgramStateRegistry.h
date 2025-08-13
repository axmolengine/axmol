#pragma once

#include "platform/PlatformMacros.h"
#include "rhi/Program.h"
#include "rhi/ProgramState.h"

#include <string>
#include <unordered_map>

namespace ax {

/**
 * @addtogroup _renderer
 * @{
 * #todo: Rename to ProgramStateRegistry
 */
class AX_DLL ProgramStateRegistry
{
public:
    /** returns the shared instance */
    static ProgramStateRegistry* getInstance();

    /** purges the cache. It releases the retained instance. */
    static void destroyInstance();

    void clearPrograms();

    void registerProgram(uint32_t programType, int textureSamplerFlags, uint32_t builtinProgramType);

    rhi::ProgramState* newProgramState(uint32_t programType, int textureSamplerFlags);
    uint32_t getProgramType(uint32_t programType, int textureSamplerFlags);

protected:
    std::unordered_map<uint32_t, uint32_t> _registry;
};

// end of _renderer group
/// @}
}
