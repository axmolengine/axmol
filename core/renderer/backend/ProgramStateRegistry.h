#pragma once
#include "Macros.h"
#include "base/CCRef.h"
#include "platform/CCPlatformMacros.h"
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
class AX_DLL ProgramStateRegistry : public Ref
{
public:
    /** returns the shared instance */
    static ProgramStateRegistry* getInstance();

    /** purges the cache. It releases the retained instance. */
    static void destroyInstance();

    bool init();
    void clearPrograms();

    void registerProgram(uint32_t programType, int textureSamplerFlags, Program*);

    ProgramState* newProgramState(uint32_t programType, int textureSamplerFlags);
    uint32_t getProgramType(uint32_t programType, int textureSamplerFlags);

protected:
    std::unordered_map<uint32_t, Program*> _registry;
};

// end of _backend group
/// @}
NS_AX_BACKEND_END
