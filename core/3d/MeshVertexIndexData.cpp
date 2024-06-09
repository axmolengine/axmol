/****************************************************************************
 Copyright (c) 2014-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

#include <list>
#include <fstream>
#include <iostream>
#include <sstream>

#include "3d/MeshVertexIndexData.h"
#include "3d/ObjLoader.h"
#include "3d/MeshMaterial.h"
#include "3d/Mesh.h"
#include "3d/Bundle3D.h"

#include "base/Macros.h"
#include "base/EventCustom.h"
#include "base/EventListenerCustom.h"
#include "base/EventDispatcher.h"
#include "base/EventType.h"
#include "base/Director.h"

#include "renderer/backend/Buffer.h"
#include "renderer/backend/DriverBase.h"

using namespace std;

NS_AX_BEGIN

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
MeshIndexData* MeshIndexData::create(std::string_view id,
                                     MeshVertexData* vertexData,
                                     backend::Buffer* indexbuffer,
                                     const AABB& aabb)
{
    auto meshindex = new MeshIndexData();

    meshindex->_id          = id;
    meshindex->_indexBuffer = indexbuffer;
    meshindex->_vertexData  = vertexData;
    indexbuffer->retain();
    meshindex->_aabb = aabb;

    meshindex->autorelease();
    return meshindex;
}

backend::Buffer* MeshIndexData::getVertexBuffer() const
{
    return _vertexData->getVertexBuffer();
}

MeshIndexData::MeshIndexData()
{
#if AX_ENABLE_CACHE_TEXTURE_DATA
    _backToForegroundListener = EventListenerCustom::create(EVENT_RENDERER_RECREATED, [this](EventCustom*) {
        _indexBuffer->updateData((void*)_indexData.data(), _indexData.bsize());
    });
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_backToForegroundListener, 1);
#endif
}

void MeshIndexData::setIndexData(const ax::MeshData::IndexArray& indexdata)
{
#if AX_ENABLE_CACHE_TEXTURE_DATA
    if (!_indexData.empty())
        return;
    _indexData = indexdata;
#endif
}

MeshIndexData::~MeshIndexData()
{
    AX_SAFE_RELEASE(_indexBuffer);
    _indexData.clear();
#if AX_ENABLE_CACHE_TEXTURE_DATA
    Director::getInstance()->getEventDispatcher()->removeEventListener(_backToForegroundListener);
#endif
}

void MeshVertexData::setVertexData(const std::vector<float>& vertexData)
{
#if AX_ENABLE_CACHE_TEXTURE_DATA
    if (!_vertexData.empty())
        return;
    _vertexData = vertexData;
#endif
}

MeshVertexData* MeshVertexData::create(const MeshData& meshdata, CustomCommand::IndexFormat format)
{
    auto vertexdata           = new MeshVertexData();
    vertexdata->_vertexBuffer = backend::DriverBase::getInstance()->newBuffer(
        meshdata.vertex.size() * sizeof(meshdata.vertex[0]), backend::BufferType::VERTEX, backend::BufferUsage::STATIC);
    // AX_SAFE_RETAIN(vertexdata->_vertexBuffer);

    vertexdata->_sizePerVertex = meshdata.getPerVertexSize();

    vertexdata->_attribs = meshdata.attribs;

    if (vertexdata->_vertexBuffer)
    {
#if AX_ENABLE_CACHE_TEXTURE_DATA
        vertexdata->setVertexData(meshdata.vertex);
        vertexdata->_vertexBuffer->usingDefaultStoredData(false);
#endif
        vertexdata->_vertexBuffer->updateData((void*)&meshdata.vertex[0],
                                              meshdata.vertex.size() * sizeof(meshdata.vertex[0]));
    }

    bool needCalcAABB = (meshdata.subMeshAABB.size() != meshdata.subMeshIndices.size());
    for (size_t i = 0, size = meshdata.subMeshIndices.size(); i < size; ++i)
    {
        auto& indices = meshdata.subMeshIndices[i];
        auto indexBuffer = backend::DriverBase::getInstance()->newBuffer(
            indices.bsize(), backend::BufferType::INDEX, backend::BufferUsage::STATIC);
        indexBuffer->autorelease();
#if AX_ENABLE_CACHE_TEXTURE_DATA
        indexBuffer->usingDefaultStoredData(false);
#endif
        indexBuffer->updateData((void*)indices.data(), indices.bsize());

        std::string id           = (i < meshdata.subMeshIds.size() ? meshdata.subMeshIds[i] : "");
        MeshIndexData* indexdata = nullptr;
        if (needCalcAABB)
        {
            auto aabb = Bundle3D::calculateAABB(meshdata.vertex, meshdata.getPerVertexSize(), indices);
            indexdata = MeshIndexData::create(id, vertexdata, indexBuffer, aabb);
        }
        else
            indexdata = MeshIndexData::create(id, vertexdata, indexBuffer, meshdata.subMeshAABB[i]);
#if AX_ENABLE_CACHE_TEXTURE_DATA
        indexdata->setIndexData(indices);
#endif
        vertexdata->_indices.pushBack(indexdata);
    }

    vertexdata->autorelease();
    return vertexdata;
}

MeshIndexData* MeshVertexData::getMeshIndexDataById(std::string_view id) const
{
    for (auto&& it : _indices)
    {
        if (it->getId() == id)
            return it;
    }
    return nullptr;
}

bool MeshVertexData::hasVertexAttrib(shaderinfos::VertexKey attrib) const
{
    for (const auto& it : _attribs)
    {
        if (it.vertexAttrib == attrib)
            return true;
    }
    return false;
}

MeshVertexData::MeshVertexData()
{
#if AX_ENABLE_CACHE_TEXTURE_DATA
    _backToForegroundListener = EventListenerCustom::create(EVENT_RENDERER_RECREATED, [this](EventCustom*) {
        _vertexBuffer->updateData((void*)_vertexData.data(), _vertexData.size() * sizeof(_vertexData[0]));
    });
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_backToForegroundListener, 1);
#endif
}

MeshVertexData::~MeshVertexData()
{
    AX_SAFE_RELEASE(_vertexBuffer);
    _indices.clear();
    _vertexData.clear();
#if AX_ENABLE_CACHE_TEXTURE_DATA
    Director::getInstance()->getEventDispatcher()->removeEventListener(_backToForegroundListener);
#endif
}

NS_AX_END
