/****************************************************************************
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

/**
 * No-op profiling backend.
 *
 * Maps Axmol profiling macros to no-op implementations.
 */

#define AX_PROFILER_ZONE_SCOPED
#define AX_PROFILER_ZONE_SCOPED_N(...)
#define AX_PROFILER_FRAME_MARK
#define AX_PROFILER_THREAD_NAME(...)