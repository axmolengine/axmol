/**
 Copyright 2013 BlackBerry Inc.
 Copyright (c) 2015-2017 Chukong Technologies
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

#include "renderer/backend/Program.h"
#include "renderer/Pass.h"
#include "base/Configuration.h"
#include "3d/MeshVertexIndexData.h"
#include "3d/3DProgramInfo.h"
#include "3d/VertexAttribBinding.h"

NS_AX_BEGIN

static std::vector<VertexAttribBinding*> __vertexAttribBindingCache;

VertexAttribBinding::VertexAttribBinding() : _meshIndexData(nullptr), _programState(nullptr) {}

VertexAttribBinding::~VertexAttribBinding()
{
    // Delete from the vertex attribute binding cache.
    std::vector<VertexAttribBinding*>::iterator itr =
        std::find(__vertexAttribBindingCache.begin(), __vertexAttribBindingCache.end(), this);
    if (itr != __vertexAttribBindingCache.end())
    {
        __vertexAttribBindingCache.erase(itr);
    }

    AX_SAFE_RELEASE(_meshIndexData);
    AX_SAFE_RELEASE(_programState);
}

VertexAttribBinding* VertexAttribBinding::create(MeshIndexData* meshIndexData, Pass* pass, MeshCommand* command)
{
    AXASSERT(meshIndexData && pass && pass->getProgramState(), "Invalid MeshIndexData and/or programState");

    // Search for an existing vertex attribute binding that can be used.
    VertexAttribBinding* b;
    for (size_t i = 0, count = __vertexAttribBindingCache.size(); i < count; ++i)
    {
        b = __vertexAttribBindingCache[i];
        AX_ASSERT(b);
        if (b->_meshIndexData == meshIndexData && b->_programState == pass->getProgramState())
        {
            // Found a match!
            return b;
        }
    }

    b = new VertexAttribBinding();
    if (b->init(meshIndexData, pass, command))
    {
        b->autorelease();
        __vertexAttribBindingCache.emplace_back(b);
    }

    return b;
}

bool VertexAttribBinding::init(MeshIndexData* meshIndexData, Pass* pass, MeshCommand* command)
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
        setVertexAttribPointer(vertexLayout, shaderinfos::getAttributeName(meshattribute.vertexAttrib),
                               meshattribute.type, false,
                               offset, 1 << k);
        offset += meshattribute.getAttribSizeBytes();
    }

    vertexLayout->setStride(offset);

    AXASSERT(offset == meshVertexData->getSizePerVertex(), "vertex layout mismatch!");

    return true;
}

uint32_t VertexAttribBinding::getVertexAttribsFlags() const
{
    return _vertexAttribsFlags;
}

void VertexAttribBinding::parseAttributes()
{
    AXASSERT(_programState, "invalid glprogram");

    _vertexAttribsFlags = 0;
}

bool VertexAttribBinding::hasAttribute(const shaderinfos::VertexKey& key) const
{
    auto& name = shaderinfos::getAttributeName(key);
    auto& attributes = _programState->getProgram()->getActiveAttributes();
    return attributes.find(name) != attributes.end();
}

const backend::AttributeBindInfo* VertexAttribBinding::getVertexAttribValue(std::string_view name)
{
    auto& attributes = _programState->getProgram()->getActiveAttributes();
    const auto itr = attributes.find(name);
    if (itr != attributes.end())
        return &itr->second;
    return nullptr;
}

void VertexAttribBinding::setVertexAttribPointer(VertexLayout* vertexLayout,
                                                 std::string_view name,
                                                 backend::VertexFormat type,
                                                 bool normalized,
                                                 int offset,
                                                 int flag)
{
    auto v = getVertexAttribValue(name);
    if (v)
    {
        // AXLOGD("set attribute '{}' location: {}, offset: {}", name, v->location, offset);
        vertexLayout->setAttrib(name, v->location, type, offset, normalized);
        _vertexAttribsFlags |= flag;
    }
    else
    {
        // AXLOGD("warning: Attribute not found: {}", name);
    }
}

NS_AX_END
