
set(lib_name recast)
set(target_name ${lib_name})

project(${lib_name})

set(${target_name}_src
  DebugDraw.cpp
  DetourAlloc.cpp
  DetourAssert.cpp
  DetourCommon.cpp
  DetourCrowd.cpp
  DetourDebugDraw.cpp
  DetourLocalBoundary.cpp
  DetourNavMesh.cpp
  DetourNavMeshBuilder.cpp
  DetourNavMeshQuery.cpp
  DetourNode.cpp
  DetourObstacleAvoidance.cpp
  DetourPathCorridor.cpp
  DetourPathQueue.cpp
  DetourProximityGrid.cpp
  DetourTileCache.cpp
  DetourTileCacheBuilder.cpp
)

add_library(${target_name} STATIC
  ${${target_name}_src}
)

target_include_directories(${target_name} PUBLIC ..)
