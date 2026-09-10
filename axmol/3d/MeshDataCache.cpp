/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#include "axmol/3d/MeshDataCache.h"

namespace ax
{
MeshDataCache* MeshDataCache::_cacheInstance = nullptr;
MeshDataCache* MeshDataCache::getInstance()
{
    if (_cacheInstance == nullptr)
        _cacheInstance = new MeshDataCache();
    return _cacheInstance;
}
void MeshDataCache::destroyInstance()
{
    if (_cacheInstance)
    {
        delete _cacheInstance;
        _cacheInstance = nullptr;
    }
}

MeshDataCache::MeshRenderData* MeshDataCache::getMeshRenderData(std::string_view key) const
{
    auto it = _meshDatas.find(key);
    if (it != _meshDatas.end())
        return it->second;
    return nullptr;
}

bool MeshDataCache::addMeshRenderData(std::string_view key, MeshDataCache::MeshRenderData* meshdata)
{
    auto it = _meshDatas.find(key);
    if (it == _meshDatas.end())
    {
        _meshDatas.emplace(key, meshdata);
        return true;
    }
    return false;
}

void MeshDataCache::removeMeshRenderData(std::string_view key)
{
    auto it = _meshDatas.find(key);
    if (it != _meshDatas.end())
    {
        delete it->second;
        _meshDatas.erase(it);
    }
}

void MeshDataCache::removeAllMeshRenderData()
{
    for (auto&& it : _meshDatas)
    {
        delete it.second;
    }
    _meshDatas.clear();
}

MeshDataCache::MeshDataCache() {}
MeshDataCache::~MeshDataCache()
{
    removeAllMeshRenderData();
}

}  // namespace ax
