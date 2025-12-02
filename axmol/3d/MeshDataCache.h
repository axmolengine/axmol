/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
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
