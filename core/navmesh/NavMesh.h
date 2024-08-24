/****************************************************************************
 Copyright (c) 2015-2016 Chukong Technologies Inc.
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

#ifndef __CCNAV_MESH_H__
#define __CCNAV_MESH_H__

#include "base/Config.h"
#if defined(AX_ENABLE_NAVMESH)

#    include "base/Object.h"
#    include "math/Vec3.h"
#    include "recast/DetourNavMesh.h"
#    include "recast/DetourNavMeshQuery.h"
#    include "recast/DetourCrowd.h"
#    include "recast/DetourTileCache.h"
#    include <string>
#    include <vector>

#    include "navmesh/NavMeshAgent.h"
#    include "navmesh/NavMeshDebugDraw.h"
#    include "navmesh/NavMeshObstacle.h"
#    include "navmesh/NavMeshUtils.h"

namespace ax
{

/**
 * @addtogroup 3d
 * @{
 */
class Renderer;
/** @brief NavMesh: The NavMesh information container, include mesh, tileCache, and so on. */
class AX_DLL NavMesh : public Object
{
public:
    /**
    Create navmesh

    @param navFilePath The NavMesh File path.
    @param geomFilePath The geometry File Path,include offmesh information,etc.
    */
    static NavMesh* create(std::string_view navFilePath, std::string_view geomFilePath);

    /** update navmesh. */
    void update(float dt);

    /** Internal method, the updater of debug drawing, need called each frame. */
    void debugDraw(Renderer* renderer);

    /** Enable debug draw or disable. */
    void setDebugDrawEnable(bool enable);

    /** Check enabled debug draw. */
    bool isDebugDrawEnabled() const;

    /** add a agent to navmesh. */
    void addNavMeshAgent(NavMeshAgent* agent);

    /** remove a agent from navmesh. */
    void removeNavMeshAgent(NavMeshAgent* agent);

    /** add a obstacle to navmesh. */
    void addNavMeshObstacle(NavMeshObstacle* obstacle);

    /** remove a obstacle from navmesh. */
    void removeNavMeshObstacle(NavMeshObstacle* obstacle);

    /**
    find a path on navmesh

    @param start The start search position in world coordinate system.
    @param end The end search position in world coordinate system.
    @param pathPoints the key points of path.
    */
    void findPath(const Vec3& start, const Vec3& end, std::vector<Vec3>& pathPoints);

    NavMesh();
    virtual ~NavMesh();

protected:
    bool initWithFilePath(std::string_view navFilePath, std::string_view geomFilePath);
    bool read();
    bool loadNavMeshFile();
    bool loadGeomFile();
    void dtDraw();
    void drawAgents();
    void drawObstacles();
    void drawOffMeshConnections();

protected:
    dtNavMesh* _navMesh;
    dtNavMeshQuery* _navMeshQuery;
    dtCrowd* _crowed;
    dtTileCache* _tileCache;
    LinearAllocator* _allocator;
    dtTileCacheCompressor* _compressor;
    MeshProcess* _meshProcess;
    GeomData* _geomData;

    std::vector<NavMeshAgent*> _agentList;
    std::vector<NavMeshObstacle*> _obstacleList;
    NavMeshDebugDraw _debugDraw;
    std::string _navFilePath;
    std::string _geomFilePath;
    bool _isDebugDrawEnabled;
};

/** @} */

}

#endif  // AX_ENABLE_NAVMESH

#endif  // __CCNAV_MESH_H__
