/**
 Copyright 2013 BlackBerry Inc.
 Copyright (c) 2015-2017 Chukong Technologies
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 Original file from GamePlay3D: http://gameplay3d.org

 This file was modified to fit the cocos2d-x project
 */

#include "axmol/rhi/Program.h"
#include "axmol/renderer/Pass.h"
#include "axmol/base/Configuration.h"
#include "axmol/3d/MeshVertexIndexData.h"
#include "axmol/3d/shaderinfos.h"
#include "axmol/3d/VertexInputBinding.h"

namespace ax
{

static std::vector<VertexInputBinding*> __vertexInputBindingCache;

VertexInputBinding::VertexInputBinding() : _meshIndexData(nullptr), _programState(nullptr) {}

VertexInputBinding::~VertexInputBinding()
{
    // Delete from the vertex attribute binding cache.
    std::vector<VertexInputBinding*>::iterator itr =
        std::find(__vertexInputBindingCache.begin(), __vertexInputBindingCache.end(), this);
    if (itr != __vertexInputBindingCache.end())
    {
        __vertexInputBindingCache.erase(itr);
    }

    AX_SAFE_RELEASE(_meshIndexData);
    AX_SAFE_RELEASE(_programState);
}

VertexInputBinding* VertexInputBinding::create(MeshIndexData* meshIndexData, Pass* pass, MeshCommand* command)
{
    AXASSERT(meshIndexData && pass && pass->getProgramState(), "Invalid MeshIndexData and/or programState");

    // Search for an existing vertex attribute binding that can be used.
    VertexInputBinding* b;
    for (size_t i = 0, count = __vertexInputBindingCache.size(); i < count; ++i)
    {
        b = __vertexInputBindingCache[i];
        AX_ASSERT(b);
        if (b->_meshIndexData == meshIndexData && b->_programState == pass->getProgramState())
        {
            // Found a match!
            return b;
        }
    }

    b = new VertexInputBinding();
    if (b->init(meshIndexData, pass, command))
    {
        b->autorelease();
        __vertexInputBindingCache.emplace_back(b);
    }

    return b;
}

bool VertexInputBinding::init(MeshIndexData* meshIndexData, Pass* pass, MeshCommand* command)
{

    AXASSERT(meshIndexData && pass && pass->getProgramState(), "Invalid arguments");

    _meshIndexData = meshIndexData;
    _meshIndexData->retain();
    _programState = pass->getProgramState();
    _programState->retain();

    auto meshVertexData = meshIndexData->getMeshVertexData();
    auto attributeCount = meshVertexData->getMeshVertexAttribCount();

    // Parse and set attributes
    parseAttributes();
    int offset = 0;
    auto vertexLayout = _programState->getMutableVertexLayout();
    for (auto k = 0; k < attributeCount; k++)
    {
        auto meshattribute = meshVertexData->getMeshVertexAttrib(k);
        setVertexInputPointer(vertexLayout, shaderinfos::getAttributeName(meshattribute.vertexAttrib),
                               meshattribute.type, false,
                               offset, 1 << k);
        offset += meshattribute.getAttribSizeBytes();
    }

    vertexLayout->setStride(offset);

    AXASSERT(offset == meshVertexData->getSizePerVertex(), "vertex layout mismatch!");

    return true;
}

uint32_t VertexInputBinding::getVertexAttribsFlags() const
{
    return _vertexAttribsFlags;
}

void VertexInputBinding::parseAttributes()
{
    AXASSERT(_programState, "invalid glprogram");

    _vertexAttribsFlags = 0;
}

bool VertexInputBinding::hasAttribute(const shaderinfos::VertexKey& key) const
{
    auto& name = shaderinfos::getAttributeName(key);
    auto& vertexInputs = _programState->getProgram()->getActiveVertexInputs();
    return vertexInputs.find(name) != vertexInputs.end();
}

const rhi::VertexInputDesc* VertexInputBinding::getVertexInputDesc(std::string_view name)
{
    return _programState->getProgram()->getVertexInputDesc(name);
}

void VertexInputBinding::setVertexInputPointer(VertexLayout* vertexLayout,
                                                 std::string_view name,
                                                 rhi::VertexFormat type,
                                                 bool normalized,
                                                 int offset,
                                                 int flag)
{
    auto v = getVertexInputDesc(name);
    if (v)
    {
        // AXLOGD("VertexInputBinding: set attribute '{}' location: {}, offset: {}", name, v->location, offset);
        vertexLayout->setAttrib(name, v, type, offset, normalized);
        _vertexAttribsFlags |= flag;
    }
    else
    {
        AXLOGW("VertexInputBinding: warning: Attribute not found: {}", name);
    }
}

}
