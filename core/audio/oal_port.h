/****************************************************************************
 Copyright (c) 2014-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
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

#if !defined(__APPLE__) && !defined(__EMSCRIPTEN__)
#    if !defined(AX_USE_ALSOFT)
#        define AX_USE_ALSOFT 1
#    endif
#else
#    if !defined(AX_USE_ALSOFT)
#        define AX_USE_ALSOFT 0
#    endif
#endif

// Use the OpenAL Soft headers bundled with Axmol instead of system-installed AL/xxx.h.
// This ensures consistent behavior across platforms and prevents Emscripten from
// accidentally including system headers.
// Define AL_STOP_SOURCES_ON_DISCONNECT_SOFT here to avoid pulling in the internal
// header "alc/inprogext.h".
#if AX_USE_ALSOFT
#    define AL_ALEXT_PROTOTYPES 1
#    include "3rdparty/openal/include/AL/al.h"
#    include "3rdparty/openal/include/AL/alc.h"
#    include "3rdparty/openal/include/AL/alext.h"
#    include "3rdparty/openal/include/AL/efx.h"
#    include "3rdparty/openal/include/AL/efx-presets.h"

#    ifndef AL_STOP_SOURCES_ON_DISCONNECT_SOFT
#        define AL_STOP_SOURCES_ON_DISCONNECT_SOFT 0x19AB
#    endif
#else
#    if defined(__APPLE__)
#        import <OpenAL/al.h>
#        import <OpenAL/alc.h>
#    elif defined(__EMSCRIPTEN__)
#        include "AL/al.h"
#        include "AL/alc.h"
#        include "AL/alext.h"
#    else
#        error "OpenAL Soft is required on non-(Apple/Emscripten) platforms"
#    endif

#endif

#ifndef MAX_AUDIOINSTANCES
#    define MAX_AUDIOINSTANCES 128
#endif

#if !AX_USE_ALSOFT

// define dummy efx macros and consts used in axmol audio effect implementation
// ...
#    define AL_EAXREVERB_DENSITY               0x0001
#    define AL_EAXREVERB_DIFFUSION             0x0002
#    define AL_EAXREVERB_GAIN                  0x0003
#    define AL_EAXREVERB_GAINHF                0x0004
#    define AL_EAXREVERB_GAINLF                0x0005
#    define AL_EAXREVERB_DECAY_TIME            0x0006
#    define AL_EAXREVERB_DECAY_HFRATIO         0x0007
#    define AL_EAXREVERB_DECAY_LFRATIO         0x0008
#    define AL_EAXREVERB_REFLECTIONS_GAIN      0x0009
#    define AL_EAXREVERB_REFLECTIONS_DELAY     0x000A
#    define AL_EAXREVERB_REFLECTIONS_PAN       0x000B
#    define AL_EAXREVERB_LATE_REVERB_GAIN      0x000C
#    define AL_EAXREVERB_LATE_REVERB_DELAY     0x000D
#    define AL_EAXREVERB_LATE_REVERB_PAN       0x000E
#    define AL_EAXREVERB_ECHO_TIME             0x000F
#    define AL_EAXREVERB_ECHO_DEPTH            0x0010
#    define AL_EAXREVERB_MODULATION_TIME       0x0011
#    define AL_EAXREVERB_MODULATION_DEPTH      0x0012
#    define AL_EAXREVERB_AIR_ABSORPTION_GAINHF 0x0013
#    define AL_EAXREVERB_HFREFERENCE           0x0014
#    define AL_EAXREVERB_LFREFERENCE           0x0015
#    define AL_EAXREVERB_ROOM_ROLLOFF_FACTOR   0x0016
#    define AL_EAXREVERB_DECAY_HFLIMIT         0x0017

/* Reverb effect parameters */
#    define AL_REVERB_DENSITY               0x0001
#    define AL_REVERB_DIFFUSION             0x0002
#    define AL_REVERB_GAIN                  0x0003
#    define AL_REVERB_GAINHF                0x0004
#    define AL_REVERB_DECAY_TIME            0x0005
#    define AL_REVERB_DECAY_HFRATIO         0x0006
#    define AL_REVERB_REFLECTIONS_GAIN      0x0007
#    define AL_REVERB_REFLECTIONS_DELAY     0x0008
#    define AL_REVERB_LATE_REVERB_GAIN      0x0009
#    define AL_REVERB_LATE_REVERB_DELAY     0x000A
#    define AL_REVERB_AIR_ABSORPTION_GAINHF 0x000B
#    define AL_REVERB_ROOM_ROLLOFF_FACTOR   0x000C
#    define AL_REVERB_DECAY_HFLIMIT         0x000D

#    define AL_EFFECT_NULL                  0x0000
#    define AL_EFFECT_REVERB                0x0001
#    define AL_EFFECT_CHORUS                0x0002
#    define AL_EFFECT_DISTORTION            0x0003
#    define AL_EFFECT_ECHO                  0x0004
#    define AL_EFFECT_FLANGER               0x0005
#    define AL_EFFECT_FREQUENCY_SHIFTER     0x0006
#    define AL_EFFECT_VOCAL_MORPHER         0x0007
#    define AL_EFFECT_PITCH_SHIFTER         0x0008
#    define AL_EFFECT_RING_MODULATOR        0x0009
#    define AL_EFFECT_AUTOWAH               0x000A
#    define AL_EFFECT_COMPRESSOR            0x000B
#    define AL_EFFECT_EQUALIZER             0x000C
#    define AL_EFFECT_EAXREVERB             0x8000

/* Source properties. */
#    define AL_DIRECT_FILTER                     0x20005
#    define AL_AUXILIARY_SEND_FILTER             0x20006
#    define AL_AIR_ABSORPTION_FACTOR             0x20007
#    define AL_ROOM_ROLLOFF_FACTOR               0x20008
#    define AL_CONE_OUTER_GAINHF                 0x20009
#    define AL_DIRECT_FILTER_GAINHF_AUTO         0x2000A
#    define AL_AUXILIARY_SEND_FILTER_GAIN_AUTO   0x2000B
#    define AL_AUXILIARY_SEND_FILTER_GAINHF_AUTO 0x2000C

#    define AL_STEREO_ANGLES                     0x1030

/* Auxiliary Effect Slot properties. */
#    define AL_EFFECTSLOT_EFFECT              0x0001
#    define AL_EFFECTSLOT_GAIN                0x0002
#    define AL_EFFECTSLOT_AUXILIARY_SEND_AUTO 0x0003

#endif
