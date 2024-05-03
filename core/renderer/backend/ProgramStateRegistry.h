#pragma once
#include "Macros.h"
#include "base/Object.h"
#include "platform/PlatformMacros.h"
#include "Program.h"
#include "ProgramState.h"

#include <string>
#include <unordered_map>

NS_AX_BACKEND_BEGIN

/**
 * @addtogroup _backend
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

    bool init();
    void clearPrograms();

    void registerProgram(uint32_t programType, int textureSamplerFlags, uint32_t builtinProgramType);

    ProgramState* newProgramState(uint32_t programType, int textureSamplerFlags);
    uint32_t getProgramType(uint32_t programType, int textureSamplerFlags);

protected:
    std::unordered_map<uint32_t, uint32_t> _registry;
};

// end of _backend group
/// @}
NS_AX_BACKEND_END
