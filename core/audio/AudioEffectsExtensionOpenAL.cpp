/****************************************************************************
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

#include "AudioEffectsExtensionOpenAL.h"

#include "AudioMacros.h"

#if ALC_EXT_EFX == 1
namespace ax
{
EffectsExtensionOpenAL* EffectsExtensionOpenAL::getInstance()
{
    static EffectsExtensionOpenAL instance;
    return &instance;
}

EffectsExtensionOpenAL::EffectsExtensionOpenAL()
{
    if (!alcIsExtensionPresent(alcGetContextsDevice(alcGetCurrentContext()), "ALC_EXT_EFX"))
    {
        return;
    }

    _isAvailable = true;
}

void EffectsExtensionOpenAL::genAuxiliaryEffectSlots(int count, uint32_t& slot)
{
    alGenAuxiliaryEffectSlots(count, &slot);
    CHECK_AL_ERROR_DEBUG();
}

void EffectsExtensionOpenAL::genEffect(uint32_t& effect)
{
    alGenEffects(1, &effect);
    CHECK_AL_ERROR_DEBUG();
}

void EffectsExtensionOpenAL::deleteAuxiliaryEffectSlot(uint32_t slot)
{
    alDeleteAuxiliaryEffectSlots(1, &slot);
}

void EffectsExtensionOpenAL::deleteEffect(uint32_t effect)
{
    alDeleteEffects(1, &effect);
}

void EffectsExtensionOpenAL::bindEffectToAuxiliarySlot(uint32_t slot, uint32_t effect)
{
    alAuxiliaryEffectSloti(slot, (ALenum)al::efx::Effecti::SlotEffect, effect);
    CHECK_AL_ERROR_DEBUG();
}

void EffectsExtensionOpenAL::auxiliaryEffectSlot(uint32_t slot, al::efx::EffectSlotf param, float value)
{
    alAuxiliaryEffectSlotf(slot, (ALenum)param, value);
    CHECK_AL_ERROR_DEBUG();
}

void EffectsExtensionOpenAL::bindSourceToAuxiliarySlot(uint32_t SourceId, uint32_t slot, uint32_t slotnumber, uint32_t filter)
{
    alSource3i(SourceId, (ALenum)al::Sourcei::EfxAuxilarySendFilter, slot, slotnumber, filter);
}

void EffectsExtensionOpenAL::setEffectParamFloat(uint32_t effect, int param, float value)
{
    alEffectf(effect, param, value);
    CHECK_AL_ERROR_DEBUG();
}

void EffectsExtensionOpenAL::setEffectParamFloatArray(uint32_t effect, int param, const float* values)
{
    alEffectfv(effect, param, values);
}

void EffectsExtensionOpenAL::setEffectParamInt(uint32_t effect, int param, int value)
{
    alEffecti(effect, param, value);
    CHECK_AL_ERROR_DEBUG();
}

uint32_t EffectsExtensionOpenAL::genFilter()
{
    uint32_t filter = 0;
    alGenFilters(1, &filter);
    return filter;
}

void EffectsExtensionOpenAL::setFilterParamInt(uint32_t sourceId, int filterParam, int filterType)
{
    alFilteri(sourceId, filterParam, filterType);
}

void EffectsExtensionOpenAL::setFilterParamFloat(uint32_t sourceId, int filterParam, float filterType)
{
    alFilterf(sourceId, filterParam, filterType);
}

void EffectsExtensionOpenAL::bindFilterToSource(uint32_t sourceId, uint32_t filterId)
{
    alSourcei(sourceId, (ALenum)al::Sourcei::EfxDirectFilter, filterId);
}

void EffectsExtensionOpenAL::deleteFilter(uint32_t filterId)
{
    alDeleteFilters(1, &filterId);
}
}
#endif
