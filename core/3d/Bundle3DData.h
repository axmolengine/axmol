/****************************************************************************
 Copyright (c) 2014-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmolengine.github.io/

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

#ifndef __AX_BUNDLE_3D_DATA_H__
#define __AX_BUNDLE_3D_DATA_H__

#include "base/Object.h"
#include "base/Types.h"
#include "math/Math.h"
#include "3d/AABB.h"

#include "renderer/backend/Types.h"

#include <vector>
#include <map>
#include <string>

#include "3d/3DProgramInfo.h"

#include "yasio/byte_buffer.hpp"

NS_AX_BEGIN

using ilist_u16_t = std::initializer_list<uint16_t>;
using ilist_u32_t = std::initializer_list<uint32_t>;

// The underlaying GL driver support U_INT8, U_SHORT, U_INT32, but we support U_SHORT and U_INT32 only
template <typename _T>
inline constexpr bool is_index_format_type_v =
    std::is_integral_v<_T> && (std::is_same_v<_T, uint16_t> || std::is_same_v<_T, uint32_t>);

class IndexArray
{
public:
    static constexpr unsigned int formatToStride(backend::IndexFormat format) { return 1 << (int)format; }
    static constexpr backend::IndexFormat strideToFormat(unsigned int stride)
    {
        return (backend::IndexFormat)(stride >> 1);
    }

    IndexArray() : _stride(formatToStride(backend::IndexFormat::U_SHORT)) {}
    IndexArray(backend::IndexFormat indexFormat) : _stride(formatToStride(indexFormat)) {}

    template <typename _Ty = uint16_t, std::enable_if_t<is_index_format_type_v<_Ty>, int> = 0>
    IndexArray(std::initializer_list<_Ty> rhs) : _stride(sizeof(_Ty))
    {
        auto ifirst     = reinterpret_cast<const uint8_t*>(std::addressof(*rhs.begin()));
        auto size_bytes = rhs.size() * sizeof(_Ty);
        _buffer.assign(ifirst, ifirst + size_bytes);
    }

    IndexArray(const IndexArray& rhs) : _stride(rhs._stride), _buffer(rhs._buffer) {}
    IndexArray(IndexArray&& rhs) noexcept : _stride(rhs._stride), _buffer(std::move(rhs._buffer)) {}

    IndexArray& operator=(const IndexArray& rhs)
    {
        _stride = rhs._stride;
        _buffer = rhs._buffer;
        return *this;
    }
    IndexArray& operator=(IndexArray&& rhs) noexcept
    {
        this->swap(rhs);
        return *this;
    }

    void swap(IndexArray& rhs)
    {
        std::swap(_stride, rhs._stride);
        _buffer.swap(rhs._buffer);
    }

    /** Returns the format of the index array. */
    backend::IndexFormat format() const { return strideToFormat(_stride); }

    /** Clears the internal byte buffer. */
    void clear() { _buffer.clear(); }

    /** Clears the internal byte buffer and sets the format specified. */
    void clear(backend::IndexFormat format)
    {
        clear();
        _stride = formatToStride(format);
    }

    /** Pushes back a value. */
    template <typename _Ty = uint16_t, std::enable_if_t<is_index_format_type_v<_Ty>, int> = 0>
    void emplace_back(const _Ty& val)
    {
        assert(_stride == sizeof(_Ty));
        const auto ifirst = reinterpret_cast<const uint8_t*>(&val);
        _buffer.insert(_buffer.end(), ifirst, ifirst + sizeof(val));
    }

    template <typename _Ty = uint16_t, std::enable_if_t<is_index_format_type_v<_Ty>, int> = 0>
    void insert(_Ty* position, _Ty* first, _Ty* last)
    {
        assert(_stride == sizeof(_Ty));
        auto ifirst     = reinterpret_cast<const uint8_t*>(first);
        auto size_bytes = std::distance(first, last) * sizeof(_Ty);
        _buffer.insert((yasio::byte_buffer::iterator)position, ifirst, ifirst + size_bytes);
    }

    template <typename _Ty = uint16_t, std::enable_if_t<is_index_format_type_v<_Ty>, int> = 0>
    const _Ty* begin() const
    {
        return (const _Ty*)_buffer.begin();
    }
    template <typename _Ty = uint16_t, std::enable_if_t<is_index_format_type_v<_Ty>, int> = 0>
    _Ty* begin()
    {
        return (_Ty*)_buffer.begin();
    }

    template <typename _Ty = uint16_t, std::enable_if_t<is_index_format_type_v<_Ty>, int> = 0>
    const _Ty* end() const
    {
        return (const _Ty*)_buffer.end();
    }
    template <typename _Ty = uint16_t, std::enable_if_t<is_index_format_type_v<_Ty>, int> = 0>
    _Ty* end()
    {
        return (_Ty*)_buffer.end();
    }

    template <typename _Ty = uint16_t, std::enable_if_t<is_index_format_type_v<_Ty>, int> = 0>
    _Ty* erase(_Ty* position)
    {
        return (_Ty*)_buffer.erase((uint8_t*)position);
    }

    template <typename _Ty = uint16_t, std::enable_if_t<is_index_format_type_v<_Ty>, int> = 0>
    _Ty* erase(_Ty* first, _Ty* last)
    {
        return (_Ty*)_buffer.erase((uint8_t*)first, (uint8_t*)last);
    }

    template <typename _Ty = uint16_t, std::enable_if_t<is_index_format_type_v<_Ty>, int> = 0>
    _Ty& at(size_t idx)
    {
        assert(sizeof(_Ty) == _stride);
        return (_Ty&)_buffer[idx * sizeof(_Ty)];
    }
    template <typename _Ty = uint16_t, std::enable_if_t<is_index_format_type_v<_Ty>, int> = 0>
    const _Ty& at(size_t idx) const
    {
        assert(sizeof(_Ty) == _stride);
        return (const _Ty&)_buffer[idx * sizeof(_Ty)];
    }

    template <typename _Ty>
    _Ty& operator[](_Ty idx)
    {
        assert(sizeof(_Ty) == _stride);
        return (_Ty&)_buffer[idx * _stride];
    }

    template <typename _Ty>
    const _Ty& operator[](_Ty idx) const
    {
        assert(sizeof(_Ty) == _stride);
        return (const _Ty&)_buffer[idx * _stride];
    }

    uint8_t* data() noexcept { return _buffer.data(); }
    const uint8_t* data() const noexcept { return _buffer.data(); }

    /** Returns the count of indices in the container. */
    size_t size() const { return _buffer.size() / _stride; }
    /** Returns the size of the container in bytes. */
    size_t bsize() const { return _buffer.size(); }

    /** Resizes the count of indices in the container. */
    void resize(size_t size) {_buffer.resize(static_cast<yasio::byte_buffer::size_type>(size * _stride)); }
    /** Resizes the container in bytes. */
    void bresize(size_t size) { _buffer.resize(static_cast<yasio::byte_buffer::size_type>(size)); }

    /** Returns true if the container is empty. Otherwise, false. */
    bool empty() const { return _buffer.empty(); }

    template <typename _Fty>
    void for_each(_Fty cb) const
    {
        assert(_stride == 2 || _stride == 4);
        for (auto it = _buffer.begin(); it != _buffer.end(); it += _stride)
        {
            uint32_t val = 0;
            memcpy(&val, it, _stride);
            cb(val);
        }
    }

protected:
    unsigned char _stride;
    yasio::byte_buffer _buffer;
};

/**mesh vertex attribute
 * @js NA
 * @lua NA
 */
struct AX_DLL MeshVertexAttrib
{
    backend::VertexFormat type;
    shaderinfos::VertexKey vertexAttrib;
    int getAttribSizeBytes() const;
};

/** model node data, since 3.3
 * @js NA
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
 * @js NA
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
 * @js NA
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
 * @js NA
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
    std::vector<MeshVertexAttrib> attribs;
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
 * @js NA
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
 * @js NA
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
 * @js NA
 * @lua NA
 */
struct MaterialData
{
    std::map<int, std::string> texturePaths;  // submesh id, texture path
    void resetData() { texturePaths.clear(); }
};

/**new material, since 3.3
 * @js NA
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
    backend::SamplerAddressMode wrapS;
    backend::SamplerAddressMode wrapT;
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
 * @js NA
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
 * @js NA
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
    std::map<std::string, std::vector<Vec3Key>> _translationKeys;
    std::map<std::string, std::vector<QuatKey>> _rotationKeys;
    std::map<std::string, std::vector<Vec3Key>> _scaleKeys;

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
 * @js NA
 * @lua NA
 */
struct Reference
{
    std::string id;
    unsigned int type;
    unsigned int offset;
};

NS_AX_END

#endif  //__AX_BUNDLE_3D_DATA_H__
