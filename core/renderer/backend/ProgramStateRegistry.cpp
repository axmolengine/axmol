#include "ProgramStateRegistry.h"

NS_AX_BACKEND_BEGIN

static ProgramStateRegistry* _sharedStateRegistry = nullptr;
/** returns the shared instance */
ProgramStateRegistry* ProgramStateRegistry::getInstance()
{
    if (_sharedStateRegistry)
        return _sharedStateRegistry;

    _sharedStateRegistry = new ProgramStateRegistry();
    if (!_sharedStateRegistry->init())
    {
        AX_SAFE_DELETE(_sharedStateRegistry);
    }

    return _sharedStateRegistry;
}

/** purges the cache. It releases the retained instance. */
void ProgramStateRegistry::destroyInstance()
{
    AX_SAFE_RELEASE_NULL(_sharedStateRegistry);
}

bool ProgramStateRegistry::init()
{
    return true;
}

void ProgramStateRegistry::registerProgram(uint32_t programType, int textureSamplerFlags, Program* program)
{
    uint32_t key = (static_cast<uint32_t>(programType) << 16) | textureSamplerFlags;
    auto it      = this->_registry.find(key);
    if (it == this->_registry.end())
        this->_registry.emplace(key, program);
    else
        it->second = program;
}

void ProgramStateRegistry::clearPrograms()
{
    this->_registry.clear();
}

ProgramState* ProgramStateRegistry::newProgramState(uint32_t programType, int textureSamplerFlags)
{
    uint32_t key = ((programType) << 16) | textureSamplerFlags;
    auto it      = this->_registry.find(key);
    if (it != this->_registry.end())
    {
        auto fallback = it->second;
        if (fallback)
            return new ProgramState(fallback);
    }

    return new ProgramState(Program::getBuiltinProgram(programType));
}

uint32_t ProgramStateRegistry::getProgramType(uint32_t programType, int textureSamplerFlags)
{
    uint32_t key = (static_cast<uint32_t>(programType) << 16) | textureSamplerFlags;
    auto it      = this->_registry.find(key);
    if (it != this->_registry.end())
    {
        auto fallback = it->second;
        if (fallback)
            return fallback->getProgramType();
    }
    return programType;
}
// end of _backend group
/// @}
NS_AX_BACKEND_END
