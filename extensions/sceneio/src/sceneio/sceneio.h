#pragma once

#include "sceneext/sceneext.h"

#include "sceneio/ActionTimeline/ActionTimelineCache.h"
#include "sceneio/ActionTimeline/CSLoader.h"

namespace ax::ext
{
SCNIO_API void initializeSceneIO();
SCNIO_API void cleanupSceneIO();
}  // namespace ax::ext
