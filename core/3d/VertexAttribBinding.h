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

#pragma once

#include <unordered_map>

#include "base/Object.h"
#include "renderer/backend/ProgramState.h"
#include "renderer/backend/Types.h"
#include "renderer/Pass.h"
#include "3d/3DProgramInfo.h"

NS_AX_BEGIN

class MeshIndexData;
class VertexAttribValue;

/**
 * Defines a binding between the vertex layout of a Mesh and the vertex
 * input attributes of a vertex shader (Effect).
 *
 * In a perfect world, this class would always be a binding directly between
 * a unique VertexFormat and an Effect, where the VertexFormat is simply the
 * definition of the layout of any anonymous vertex buffer. However, the OpenGL
 * mechanism for setting up these bindings is Vertex Array Objects (VAOs).
 * OpenGL requires a separate VAO per vertex buffer object (VBO), rather than per
 * vertex layout definition. Therefore, although we would like to define this
 * binding between a VertexFormat and Effect, we are specifying the binding
 * between a Mesh and Effect to satisfy the OpenGL requirement of one VAO per VBO.
 *
 * Note that this class still does provide a binding between a VertexFormat
 * and an Effect, however this binding is actually a client-side binding and
 * should only be used when writing custom code that use client-side vertex
 * arrays, since it is slower than the server-side VAOs used by OpenGL
 * (when creating a VertexAttribBinding between a Mesh and Effect).
 */
class AX_DLL VertexAttribBinding : public Object
{
public:
    /**
     * Creates a new VertexAttribBinding between the given MeshVertexData and GLProgramState.
     *
     * If a VertexAttribBinding matching the specified MeshVertexData and GLProgramState already
     * exists, it will be returned. Otherwise, a new VertexAttribBinding will
     * be returned. If OpenGL VAOs are enabled, the a new VAO will be created and
     * stored in the returned VertexAttribBinding, otherwise a client-side
     * array of vertex attribute bindings will be stored.
     *
     * @param mesh The mesh.
     * @param effect The effect.
     *
     * @return A VertexAttribBinding for the requested parameters.
     */
    static VertexAttribBinding* create(MeshIndexData* meshIndexData, Pass* pass, MeshCommand*);

    /**
     * Binds this vertex array object.
     */
    // void bind(backend::VertexLayout &layout);

    /**
     * Unbinds this vertex array object.
     */
    // void unbind();

    /**
     * Returns the vertex attrib flags
     */
    uint32_t getVertexAttribsFlags() const;

    bool hasAttribute(const shaderinfos::VertexKey& key) const;

private:
    /**
     * Constructor.
     */
    VertexAttribBinding();

    /**
     * Destructor.
     */
    ~VertexAttribBinding();

    /**
     * Hidden copy assignment operator.
     */
    VertexAttribBinding& operator=(const VertexAttribBinding&);

    bool init(MeshIndexData* meshIndexData, Pass* pass, MeshCommand*);
    void setVertexAttribPointer(VertexLayout* vertexLayout, std::string_view name,
                                backend::VertexFormat type,
                                bool normalized,
                                int offset,
                                int flag);
    const backend::AttributeBindInfo* getVertexAttribValue(std::string_view name);
    void parseAttributes();

    MeshIndexData* _meshIndexData;
    backend::ProgramState* _programState;
    uint32_t _vertexAttribsFlags;
};

NS_AX_END
