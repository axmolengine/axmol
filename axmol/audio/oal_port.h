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
