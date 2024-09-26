/****************************************************************************
 Copyright (c) 2015-2016 Chukong Technologies Inc.
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

 Ideas taken from:
 - GamePlay3D: http://gameplay3d.org/
 - OGRE3D: http://www.ogre3d.org/
 - Qt3D: http://qt-project.org/
 ****************************************************************************/
#pragma once

#include <string>
#include <unordered_map>

#include "renderer/RenderState.h"
#include "renderer/Technique.h"
#include "renderer/CustomCommand.h"
#include "base/Object.h"
#include "base/Vector.h"
#include "math/Vec2.h"
#include "math/Vec3.h"
#include "math/Vec4.h"
#include "math/Mat4.h"
#include "platform/PlatformMacros.h"

namespace ax
{

class Technique;
class Pass;
class GLProgramState;
class Node;
class Properties;
class RenderState;

namespace backend
{
class ProgramState;
}

/// Material
class AX_DLL Material : public Object
{
    friend class Node;
    friend class Technique;
    friend class Pass;
    friend class MeshCommand;
    friend class Renderer;
    friend class Mesh;
    friend class RenderState;

public:
    /**
     * Creates a Material using the data from the Properties object defined at the specified URL,
     * where the URL is of the format "<file-path>.<extension>#<namespace-id>/<namespace-id>/.../<namespace-id>"
     * (and "#<namespace-id>/<namespace-id>/.../<namespace-id>" is optional).
     *
     * @param url The URL pointing to the Properties object defining the material.
     *
     * @return A new Material or NULL if there was an error.
     */
    static Material* createWithFilename(std::string_view path);

    /** Creates a Material with a GLProgramState.
     It will only contain one Technique and one Pass.
     Added in order to support legacy code.
     */
    static Material* createWithProgramState(backend::ProgramState* programState);

    /**
     * Creates a material from the specified properties object.
     *
     * @param materialProperties The properties object defining the
     *      material (must have namespace equal to 'material').
     * @return A new Material.
     */
    static Material* createWithProperties(Properties* materialProperties);

    void draw(MeshCommand* meshCommand,
              float globalZOrder,
              backend::Buffer* vertexBuffer,
              backend::Buffer* indexBuffer,
              CustomCommand::PrimitiveType primitive,
              CustomCommand::IndexFormat indexFormat,
              unsigned int indexCount,
              const Mat4& modelView);

    /// returns the material name
    std::string getName() const;
    /// sets the material name
    void setName(std::string_view name);

    /** Returns a Technique by its name.
     returns `nullptr` if the Technique can't be found.
     */
    Technique* getTechniqueByName(std::string_view name);

    /** Returns a Technique by index.
     returns `nullptr` if the index is invalid.
     */
    Technique* getTechniqueByIndex(ssize_t index);

    /** Returns the Technique used by the Material */
    Technique* getTechnique() const;

    /** Returns the list of Techniques */
    const Vector<Technique*>& getTechniques() const;

    /** Returns the number of Techniques in the Material. */
    ssize_t getTechniqueCount() const;

    /** Adds a Technique into the Material */
    void addTechnique(Technique* technique);

    /** Sets the current technique */
    void setTechnique(std::string_view techniqueName);

    /** returns a clone (deep-copy) of the material */
    virtual Material* clone() const;

    inline RenderState::StateBlock& getStateBlock() { return _renderState._state; }

    inline void setStateBlock(const RenderState::StateBlock& state) { _renderState._state = state; }

    RenderState* getRenderState() { return &_renderState; }

    /**
     * Set material primitive draw type
     * @param primitive Primitive draw type
     */
    void setPrimitiveType(ax::backend::PrimitiveType primitive) { _drawPrimitive = primitive; }

    /**
     * Get material primitive draw type
     * @return Primitive draw type
     */
    ax::backend::PrimitiveType getPrimitiveType() const { return _drawPrimitive; }

    /**
     * Enable material transparent rendering.
     * WARNING: depth testing will not work.
     */
    void setTransparent(bool value);

    /**
     * Is material transparent?
     */
    bool isTransparent() const { return _isTransparent; }

    /**
     * Enable material 2D queue rendering.
     */
    void setForce2DQueue(bool value);

    /**
     * Is material in 2D render queue?
     */
    bool isForce2DQueue() const { return _force2DQueue; }

protected:
    Material();
    ~Material();
    bool initWithProgramState(backend::ProgramState* state);
    bool initWithFile(std::string_view file);
    bool initWithProperties(Properties* materialProperties);

    void setTarget(Node* target);

    bool parseProperties(Properties* properties);
    bool parseTechnique(Properties* properties);
    bool parsePass(Technique* technique, Properties* properties);
    bool parseShader(Pass* pass, Properties* properties);
    bool parseSampler(backend::ProgramState* programState, Properties* properties);
    bool parseUniform(backend::ProgramState* programState, Properties* properties, const char* uniformName);
    bool parseRenderState(RenderState::StateBlock* state, Properties* properties);

    // material name
    std::string _name;

    RenderState _renderState;

    // array of techniques
    Vector<Technique*> _techniques;

    // weak pointer since it is being help by _techniques
    Technique* _currentTechnique = nullptr;

    // weak reference
    Node* _target = nullptr;

    hlookup::string_map<int> _textureSlots;
    int _textureSlotIndex = 0;

    bool _isTransparent = false;  // is this mesh transparent.
    bool _force2DQueue = false;   // render meshes using this material in 2D render queue.

    ax::backend::PrimitiveType _drawPrimitive =
        ax::backend::PrimitiveType::TRIANGLE;  // primitive draw type for meshes
};

}
