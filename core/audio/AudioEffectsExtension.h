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
#pragma once

#include "audio/oal_port.h"

#if AX_USE_ALSOFT

#   include <cstdint>

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

}
#endif
