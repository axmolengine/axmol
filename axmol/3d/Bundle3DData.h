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

#include "axmol/base/Object.h"
#include "axmol/base/Types.h"
#include "axmol/math/Math.h"
#include "axmol/3d/AABB.h"

#include <vector>
#include <map>
#include <string>

#include "axmol/3d/shaderinfos.h"
#include "axmol/rhi/IndexArray.h"

namespace ax
{

using ilist_u16_t = rhi::ilist_u16_t;
using ilist_u32_t = rhi::ilist_u32_t;
using IndexArray  = rhi::IndexArray;

/**mesh vertex attribute
 * @lua NA
 */
struct AX_DLL MeshVertexAttrib
{
    rhi::VertexFormat type;
    shaderinfos::VertexKey vertexAttrib;
    int getAttribSizeBytes() const;
};

/** model node data, since 3.3
 * @lua NA
 */
struct ModelData
{
    std::string subMeshId;
    std::string materialId;
    std::vector<std::string> bones;
    std::vector<Mat4> invBindPose;

    virtual ~ModelData() {}

    virtual void resetData()
    {
        bones.clear();
        invBindPose.clear();
    }
};

/** Node data, since 3.3
 * @lua NA
 */
struct NodeData
{
    std::string id;
    Mat4 transform;
    std::vector<ModelData*> modelNodeDatas;
    std::vector<NodeData*> children;

    virtual ~NodeData() { resetData(); }
    virtual void resetData()
    {
        id.clear();
        transform.setIdentity();
        for (auto&& it : children)
        {
            delete it;
        }
        children.clear();

        for (auto&& modeldata : modelNodeDatas)
        {
            delete modeldata;
        }
        modelNodeDatas.clear();
    }
};

/** node datas, since 3.3
 * @lua NA
 */
struct NodeDatas
{
    std::vector<NodeData*> skeleton;  // skeleton
    std::vector<NodeData*> nodes;     // nodes, CCNode, MeshRenderer or parts of MeshRenderer

    virtual ~NodeDatas() { resetData(); }

    void resetData()
    {
        for (auto&& it : skeleton)
        {
            delete it;
        }
        skeleton.clear();
        for (auto&& it : nodes)
        {
            delete it;
        }
        nodes.clear();
    }
};

/**mesh data
 * @lua NA
 */
struct MeshData
{
    using IndexArray = ::ax::IndexArray;
    std::vector<float> vertex;
    int vertexSizeInFloat;
    std::vector<IndexArray> subMeshIndices;
    std::vector<std::string> subMeshIds;  // subMesh Names (since 3.3)
    std::vector<AABB> subMeshAABB;
    int numIndex;
    tlx::pod_vector<MeshVertexAttrib> attribs;
    int attribCount;

public:
    /**
     * Get per vertex size
     * @return return the sum size of all vertex attributes.
     */
    int getPerVertexSize() const
    {
        int vertexsize = 0;
        for (const auto& attrib : attribs)
        {
            vertexsize += attrib.getAttribSizeBytes();
        }
        return vertexsize;
    }

    /**
     * Reset the data
     */
    void resetData()
    {
        vertex.clear();
        subMeshIndices.clear();
        subMeshAABB.clear();
        attribs.clear();
        vertexSizeInFloat = 0;
        numIndex          = 0;
        attribCount       = 0;
    }
    MeshData() : vertexSizeInFloat(0), numIndex(0), attribCount(0) {}
};

/** mesh datas
 * @lua NA
 */
struct MeshDatas
{
    std::vector<MeshData*> meshDatas;

    void resetData()
    {
        for (auto&& it : meshDatas)
        {
            delete it;
        }
        meshDatas.clear();
    }
    ~MeshDatas() { resetData(); }
};

/**skin data
 * @lua NA
 */
struct SkinData
{
    std::vector<std::string> skinBoneNames;     // skin bones affect skin
    std::vector<std::string> nodeBoneNames;     // node bones don't affect skin, all bones [skinBone, nodeBone]
    std::vector<Mat4> inverseBindPoseMatrices;  // bind pose of skin bone, only for skin bone
    std::vector<Mat4> skinBoneOriginMatrices;   // original bone transform, for skin bone
    std::vector<Mat4> nodeBoneOriginMatrices;   // original bone transform, for node bone

    // bone child info, both skinbone and node bone
    std::map<int, std::vector<int>> boneChild;  // key parent, value child
    int rootBoneIndex;
    void resetData()
    {
        skinBoneNames.clear();
        nodeBoneNames.clear();
        inverseBindPoseMatrices.clear();
        skinBoneOriginMatrices.clear();
        nodeBoneOriginMatrices.clear();
        boneChild.clear();
        rootBoneIndex = -1;
    }

    void addSkinBoneNames(std::string_view name)
    {
        auto it = std::find(skinBoneNames.begin(), skinBoneNames.end(), name);
        if (it == skinBoneNames.end())
            skinBoneNames.emplace_back(std::string{name});
    }

    void addNodeBoneNames(std::string_view name)
    {
        auto it = std::find(nodeBoneNames.begin(), nodeBoneNames.end(), name);
        if (it == nodeBoneNames.end())
            nodeBoneNames.emplace_back(std::string{name});
    }

    int getSkinBoneNameIndex(std::string_view name) const
    {
        int i = 0;
        for (const auto& iter : skinBoneNames)
        {
            if ((iter) == name)
                return i;
            i++;
        }
        return -1;
    }

    int getBoneNameIndex(std::string_view name) const
    {
        int i = 0;
        for (const auto& iter : skinBoneNames)
        {
            if ((iter) == name)
                return i;
            i++;
        }
        for (const auto& iter : nodeBoneNames)
        {
            if (iter == name)
                return i;
            i++;
        }
        return -1;
    }
};

/**material data,
 * @lua NA
 */
struct MaterialData
{
    std::map<int, std::string> texturePaths;  // submesh id, texture path
    void resetData() { texturePaths.clear(); }
};

/**new material, since 3.3
 * @lua NA
 */
struct NTextureData
{
    enum class Usage
    {
        Unknown      = 0,
        None         = 1,
        Diffuse      = 2,
        Emissive     = 3,
        Ambient      = 4,
        Specular     = 5,
        Shininess    = 6,
        Normal       = 7,
        Bump         = 8,
        Transparency = 9,
        Reflection   = 10
    };
    std::string id;
    std::string filename;
    Usage type;
    rhi::SamplerAddressMode wrapS;
    rhi::SamplerAddressMode wrapT;
};
struct NMaterialData
{
    std::vector<NTextureData> textures;
    std::string id;
    const NTextureData* getTextureData(const NTextureData::Usage& type) const
    {
        for (const auto& it : textures)
        {
            if (it.type == type)
                return &it;
        }
        return nullptr;
    }
};
/** material datas, since 3.3
 * @lua NA
 */
struct MaterialDatas
{
    std::vector<NMaterialData> materials;
    void resetData() { materials.clear(); }
    const NMaterialData* getMaterialData(std::string_view materialid) const
    {
        for (const auto& it : materials)
        {
            if (it.id == materialid)
                return &it;
        }
        return nullptr;
    }
};
/**animation data
 * @lua NA
 */
struct Animation3DData
{
public:
    struct Vec3Key
    {
        Vec3Key() : _time(0) {}

        Vec3Key(float time, const Vec3& v) : _time(time), _key(v) {}

        float _time;
        Vec3 _key;
    };

    struct QuatKey
    {
        QuatKey() : _time(0), _key(Quaternion::identity()) {}

        QuatKey(float time, const Quaternion& quat) : _time(time), _key(quat) {}

        float _time;
        Quaternion _key;
    };

public:
    tlx::string_map<std::vector<Vec3Key>> _translationKeys;
    tlx::string_map<std::vector<QuatKey>> _rotationKeys;
    tlx::string_map<std::vector<Vec3Key>> _scaleKeys;

    float _totalTime;

public:
    Animation3DData() : _totalTime(0) {}

    Animation3DData(const Animation3DData& other)
        : _translationKeys(other._translationKeys)
        , _rotationKeys(other._rotationKeys)
        , _scaleKeys(other._scaleKeys)
        , _totalTime(other._totalTime)
    {}

    void resetData()
    {
        _totalTime = 0;
        _translationKeys.clear();
        _rotationKeys.clear();
        _scaleKeys.clear();
    }
};

/**reference data
 * @lua NA
 */
struct Reference
{
    std::string id;
    unsigned int type;
    unsigned int offset;
};

}  // namespace ax
