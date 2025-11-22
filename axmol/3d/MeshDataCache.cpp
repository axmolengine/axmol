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
