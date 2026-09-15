/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include "axmol/3d/MeshVertexIndexData.h"

namespace ax
{

/**
 * @addtogroup _3d
 * @{
 */

///////////////////////////////////////////////////////
/**
 * @brief MeshDataCache: the cache data of MeshRenderer, used to speed up the creation process of MeshRenderer
 */
class AX_DLL MeshDataCache
{
public:
    struct MeshRenderData
    {
        Vector<MeshVertexData*> meshVertexDatas;
        Vector<rhi::ProgramState*> programStates;
        NodeDatas* nodedatas;
        MaterialDatas* materialdatas;
        ~MeshRenderData()
        {
            if (nodedatas)
                delete nodedatas;
            if (materialdatas)
                delete materialdatas;
            meshVertexDatas.clear();
            programStates.clear();
        }
    };

    static MeshDataCache* getInstance();
    static void destroyInstance();

    /**
     * get a MeshData object by key
     *
     * @lua NA
     */
    MeshRenderData* getMeshRenderData(std::string_view key) const;

    /**
     * add a MeshData object into the MeshRenderer with a specified key
     *
     * @lua NA
     */
    bool addMeshRenderData(std::string_view key, MeshRenderData* meshdata);

    /** remove a MeshData from the MeshRenderer with a specified key */
    void removeMeshRenderData(std::string_view key);

    /** remove all the MeshData objects from the MeshRenderer */
    void removeAllMeshRenderData();

    MeshDataCache();
    ~MeshDataCache();

protected:
    static MeshDataCache* _cacheInstance;
    tlx::string_map<MeshRenderData*> _meshDatas;  // cached mesh data
};

// end of 3d group
/// @}

}  // namespace ax
