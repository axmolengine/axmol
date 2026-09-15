
/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include "axmol/audio/oal_port.h"

namespace ax
{

#if AX_USE_ALSOFT

typedef EFXEAXREVERBPROPERTIES ReverbProperties;

#else

struct ReverbProperties
{
    float flDensity;
    float flDiffusion;
    float flGain;
    float flGainHF;
    float flGainLF;
    float flDecayTime;
    float flDecayHFRatio;
    float flDecayLFRatio;
    float flReflectionsGain;
    float flReflectionsDelay;
    float flReflectionsPan[3];
    float flLateReverbGain;
    float flLateReverbDelay;
    float flLateReverbPan[3];
    float flEchoTime;
    float flEchoDepth;
    float flModulationTime;
    float flModulationDepth;
    float flAirAbsorptionGainHF;
    float flHFReference;
    float flLFReference;
    float flRoomRolloffFactor;
    int iDecayHFLimit;
};

#endif

}  // namespace ax
