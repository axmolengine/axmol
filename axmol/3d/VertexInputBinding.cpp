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
#include "xxhash/xxhash.h"

namespace ax
{

/**
* axmol-3.0: we use meshIndexData+program+instancing as key to ensure cache hit
* older version: cache miss always due to programState always changes when switch
* render objects
*/
static tsl::robin_map<uint32_t, VertexInputBinding*>& _bindingCache()
{
    static tsl::robin_map<uint32_t, VertexInputBinding*> __vertexInputBindingCache;
    return __vertexInputBindingCache;
}

void VertexInputBinding::clearCache() {
    auto& cache = _bindingCache();
    for (auto& [_, vib] : cache)
        AX_SAFE_RELEASE(vib);
    cache.clear();
}

VertexInputBinding::~VertexInputBinding()
{
    AX_SAFE_RELEASE(_programState);
    AX_SAFE_RELEASE(_vertexLayout);
}

VertexInputBinding* VertexInputBinding::spawn(MeshIndexData* meshIndexData,
                                              Pass* pass,
                                              MeshCommand* command,
                                              bool instancing)
{
    AXASSERT(meshIndexData && pass && pass->getProgramState(), "Invalid MeshIndexData and/or programState");

    // Search for an existing vertex attribute binding that can be used.
    struct HashMe
    {
        MeshIndexData* meshData;
        Program* shaderProg;
        bool instancing;
    };

    HashMe hashMe;
    memset(&hashMe, 0, sizeof(hashMe));
    hashMe.meshData   = meshIndexData;
    hashMe.shaderProg = pass->getProgramState()->getProgram();
    hashMe.instancing = instancing;

    auto hash   = XXH32(&hashMe, sizeof(hashMe), 0);
    auto& cache = _bindingCache();
    auto it     = cache.find(hash);
    if (it != cache.end())
    {
        auto b = it->second;
        pass->setVertexLayout(b->_vertexLayout);
        return b;
    }

    auto b = new VertexInputBinding();
    b->init(meshIndexData, pass, command, instancing);
    b->_hash = hash;
    cache.emplace(hash, b);

    return b;
}

bool VertexInputBinding::init(MeshIndexData* meshIndexData, Pass* pass, MeshCommand* command, bool instancing)
{
    AXASSERT(meshIndexData && pass && pass->getProgramState(), "Invalid arguments");

    _programState = pass->getProgramState();
    _programState->retain();

    auto meshVertexData = meshIndexData->getMeshVertexData();
    auto attributeCount = meshVertexData->getMeshVertexAttribCount();

    // Set attributes

    int offset = 0;
    auto desc  = axvlm->allocateVertexLayoutDesc();
    desc.startLayout(attributeCount);
    for (auto k = 0; k < attributeCount; k++)
    {
        auto meshattribute = meshVertexData->getMeshVertexAttrib(k);
        setVertexInputPointer(desc, shaderinfos::getAttributeName(meshattribute.vertexAttrib), meshattribute.type,
                              false, offset, 1 << k);
        offset += meshattribute.getAttribSizeBytes();
    }

    if (instancing)
    {
        auto program = _programState->getProgram();
        desc.addAttrib(rhi::VERTEX_INPUT_NAME_INSTANCE, program->getVertexInputDesc(rhi::VertexInputKind::INSTANCE),
                       rhi::VertexFormat::MAT4, 0, false, 1);
    }

   /*
     * If certain vertex inputs are missing in the shader (e.g., 'a_normal'),
     * set the stride manually to match the vertex data in MeshIndexData
     * instead of relying on the automatically computed stride.
     */
    desc.endLayout(offset);

    _vertexLayout = axvlm->acquireVertexLayout(std::forward<VertexLayoutDesc>(desc));
    pass->setVertexLayout(_vertexLayout);

    AXASSERT(offset == meshVertexData->getSizePerVertex(), "vertex layout mismatch!");

    return true;
}

uint32_t VertexInputBinding::getVertexAttribsFlags() const
{
    return _vertexAttribsFlags;
}

bool VertexInputBinding::hasAttribute(const shaderinfos::VertexKey& key) const
{
    auto& name         = shaderinfos::getAttributeName(key);
    auto& vertexInputs = _programState->getActiveVertexInputs();
    return vertexInputs.find(name) != vertexInputs.end();
}

void VertexInputBinding::setVertexInputPointer(VertexLayoutDesc& desc,
                                               std::string_view name,
                                               rhi::VertexFormat type,
                                               bool normalized,
                                               int offset,
                                               int flag)
{
    auto v = _programState->getVertexInputDesc(name);
    if (v)
    {
        // AXLOGD("VertexInputBinding: set attribute '{}' location: {}, offset: {}", name, v->location, offset);
        desc.addAttrib(name, v, type, offset, normalized);
        _vertexAttribsFlags |= flag;
    }
    else
    {
        AXLOGI("VertexInputBinding: attribute: '{}' not present in shader", name);
    }
}

}  // namespace ax
