/****************************************************************************
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

/**
 * Tracy profiling backend.
 *
 * Maps Axmol profiling macros to the native Tracy API.
 */

#include <tracy/Tracy.hpp>

#define AX_PROFILER_ZONE_SCOPED         ZoneScoped
#define AX_PROFILER_ZONE_SCOPED_N(name) ZoneScopedN(name)
#define AX_PROFILER_FRAME_MARK          FrameMark
#define AX_PROFILER_THREAD_NAME(name)   tracy::SetThreadName(name)