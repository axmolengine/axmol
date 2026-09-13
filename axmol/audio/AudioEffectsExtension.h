/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include "axmol/audio/oal_port.h"

#if AX_USE_ALSOFT

#    include <cstdint>

namespace ax
{

class AudioEffectsExtension
{
public:
    static AudioEffectsExtension* getInstance();

    bool isAvailable() const { return _isAvailable; }

    void genAuxiliaryEffectSlots(int count, uint32_t& slot);
    void genEffect(uint32_t& effect);
    void deleteAuxiliaryEffectSlot(uint32_t slot);
    void deleteEffect(uint32_t effect);
    void bindEffectToAuxiliarySlot(uint32_t slot, uint32_t effect);
    void auxiliaryEffectSlot(uint32_t slot, int param, float value);
    void bindSourceToAuxiliarySlot(uint32_t SourceId, uint32_t slot, uint32_t slotnumber, uint32_t filter);
    void setEffectParamFloat(uint32_t effect, int param, float value);
    void setEffectParamFloatArray(uint32_t effect, int param, const float* values);
    void setEffectParamInt(uint32_t effect, int param, int value);
    uint32_t genFilter();
    void setFilterParamInt(uint32_t sourceId, int filterParam, int filterType);
    void setFilterParamFloat(uint32_t sourceId, int filterParam, float EfxFilterType);
    void bindFilterToSource(uint32_t sourceId, uint32_t filterId);
    void deleteFilter(uint32_t filterId);

private:
    AudioEffectsExtension();

    bool _isAvailable{};
};

}  // namespace ax
#endif
