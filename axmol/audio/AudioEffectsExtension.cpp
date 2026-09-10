/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/audio/AudioEffectsExtension.h"

#include "AudioMacros.h"

#if AX_USE_ALSOFT
namespace ax
{
AudioEffectsExtension* AudioEffectsExtension::getInstance()
{
    static AudioEffectsExtension instance;
    return &instance;
}

AudioEffectsExtension::AudioEffectsExtension()
{
    if (!alcIsExtensionPresent(alcGetContextsDevice(alcGetCurrentContext()), "ALC_EXT_EFX"))
    {
        return;
    }

    _isAvailable = true;
}

void AudioEffectsExtension::genAuxiliaryEffectSlots(int count, uint32_t& slot)
{
    alGenAuxiliaryEffectSlots(count, &slot);
    CHECK_AL_ERROR_DEBUG();
}

void AudioEffectsExtension::genEffect(uint32_t& effect)
{
    alGenEffects(1, &effect);
    CHECK_AL_ERROR_DEBUG();
}

void AudioEffectsExtension::deleteAuxiliaryEffectSlot(uint32_t slot)
{
    alDeleteAuxiliaryEffectSlots(1, &slot);
}

void AudioEffectsExtension::deleteEffect(uint32_t effect)
{
    alDeleteEffects(1, &effect);
}

void AudioEffectsExtension::bindEffectToAuxiliarySlot(uint32_t slot, uint32_t effect)
{
    alAuxiliaryEffectSloti(slot, AL_EFFECTSLOT_EFFECT, effect);
    CHECK_AL_ERROR_DEBUG();
}

void AudioEffectsExtension::auxiliaryEffectSlot(uint32_t slot, int param, float value)
{
    alAuxiliaryEffectSlotf(slot, param, value);
    CHECK_AL_ERROR_DEBUG();
}

void AudioEffectsExtension::bindSourceToAuxiliarySlot(uint32_t SourceId,
                                                      uint32_t slot,
                                                      uint32_t slotnumber,
                                                      uint32_t filter)
{
    alSource3i(SourceId, AL_AUXILIARY_SEND_FILTER, slot, slotnumber, filter);
}

void AudioEffectsExtension::setEffectParamFloat(uint32_t effect, int param, float value)
{
    alEffectf(effect, param, value);
    CHECK_AL_ERROR_DEBUG();
}

void AudioEffectsExtension::setEffectParamFloatArray(uint32_t effect, int param, const float* values)
{
    alEffectfv(effect, param, values);
}

void AudioEffectsExtension::setEffectParamInt(uint32_t effect, int param, int value)
{
    alEffecti(effect, param, value);
    CHECK_AL_ERROR_DEBUG();
}

uint32_t AudioEffectsExtension::genFilter()
{
    uint32_t filter = 0;
    alGenFilters(1, &filter);
    return filter;
}

void AudioEffectsExtension::setFilterParamInt(uint32_t sourceId, int filterParam, int filterType)
{
    alFilteri(sourceId, filterParam, filterType);
}

void AudioEffectsExtension::setFilterParamFloat(uint32_t sourceId, int filterParam, float filterType)
{
    alFilterf(sourceId, filterParam, filterType);
}

void AudioEffectsExtension::bindFilterToSource(uint32_t sourceId, uint32_t filterId)
{
    alSourcei(sourceId, AL_DIRECT_FILTER, filterId);
}

void AudioEffectsExtension::deleteFilter(uint32_t filterId)
{
    alDeleteFilters(1, &filterId);
}
}  // namespace ax
#endif
