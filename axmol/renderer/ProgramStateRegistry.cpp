#include "axmol/renderer/ProgramStateRegistry.h"

namespace ax {

static ProgramStateRegistry* _sharedStateRegistry = nullptr;
/** returns the shared instance */
ProgramStateRegistry* ProgramStateRegistry::getInstance()
{
    if (_sharedStateRegistry)
        return _sharedStateRegistry;

    _sharedStateRegistry = new ProgramStateRegistry();
    return _sharedStateRegistry;
}

/** purges the cache. It releases the retained instance. */
void ProgramStateRegistry::destroyInstance()
{
    AX_SAFE_DELETE(_sharedStateRegistry);
}

void ProgramStateRegistry::registerProgram(uint32_t programType, int textureSamplerFlags, uint32_t builtinProgramType)
{
    uint32_t key = (static_cast<uint32_t>(programType) << 16) | textureSamplerFlags;
    auto it      = this->_registry.find(key);
    if (it == this->_registry.end())
        this->_registry.emplace(key, builtinProgramType);
    else
        it->second = builtinProgramType;
}

void ProgramStateRegistry::clearPrograms()
{
    this->_registry.clear();
}

rhi::ProgramState* ProgramStateRegistry::newProgramState(uint32_t programType, int textureSamplerFlags)
{
    uint32_t key = ((programType) << 16) | textureSamplerFlags;
    auto it      = this->_registry.find(key);
    if (it != this->_registry.end())
    {
        auto fallback = it->second;
        if (fallback)
            return new rhi::ProgramState(axpm->getBuiltinProgram(fallback));
    }

    return new rhi::ProgramState(axpm->getBuiltinProgram(programType));
}

uint32_t ProgramStateRegistry::getProgramType(uint32_t programType, int textureSamplerFlags)
{
    uint32_t key = (static_cast<uint32_t>(programType) << 16) | textureSamplerFlags;
    auto it      = this->_registry.find(key);
    if (it != this->_registry.end())
    {
        auto fallback = it->second;
        if (fallback)
            return fallback;
    }
    return programType;
}
// end of _renderer group
/// @}
}
