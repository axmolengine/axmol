/****************************************************************************
 Copyright (c) 2015-2016 Chukong Technologies Inc.
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

 Ideas taken from:
 - GamePlay3D: http://gameplay3d.org/
 - OGRE3D: http://www.ogre3d.org/
 - Qt3D: http://qt-project.org/
 ****************************************************************************/
#pragma once

#include <stdio.h>

#include "platform/PlatformMacros.h"
#include "renderer/RenderState.h"
#include "renderer/MeshCommand.h"
#include "renderer/GroupCommand.h"
#include "renderer/CallbackCommand.h"

namespace ax
{

class Technique;
class Node;
class VertexInputBinding;
class MeshIndexData;
class RenderState;

namespace rhi
{
class ProgramState;
class Buffer;
}  // namespace rhi

class AX_DLL Pass : public Object
{
    friend class Material;
    friend class Technique;
    friend class RenderState;
    friend class VertexInputBinding;

public:
    /** Creates a Pass with a GLProgramState.
     */
    static Pass* createWithProgramState(Technique* parent, rhi::ProgramState* programState);

    static Pass* create(Technique* parent);

    /** Returns the ProgramState */
    rhi::ProgramState* getProgramState() const;

    void draw(MeshCommand* meshCommand,
              float globalZOrder,
              rhi::Buffer* vertexBuffer,
              rhi::Buffer* indexBuffer,
              MeshCommand::PrimitiveType primitive,
              MeshCommand::IndexFormat indexFormat,
              unsigned int indexCount,
              const Mat4& modelView);

    /**
     * Sets a vertex attribute binding for this pass.
     *
     * When a mesh binding is set, the VertexInputBinding will be automatically
     * bound when the bind() method is called for the pass.
     *
     * @param binding The VertexInputBinding to set (or NULL to remove an existing binding).
     */
    void setVertexInputBinding(VertexInputBinding* binding);

    /**
     * Returns the vertex attribute binding for this pass.
     *
     * @return The vertex attribute binding for this pass.
     */
    VertexInputBinding* getVertexAttributeBinding() const;

    void setName(std::string_view name) { _name = name; }
    std::string_view getName() const { return _name; }

    inline RenderState::StateBlock& getStateBlock() { return _renderState._state; }

    /**
     * Returns a clone (deep-copy) of this instance */
    Pass* clone() const;

    void setTechnique(Technique* technique);

    void updateMVPUniform(const Mat4& modelView);

    void setUniformTexture(uint32_t slot, rhi::Texture*);      // u_tex0
    void setUniformNormTexture(uint32_t slot, rhi::Texture*);  // u_normalTex

    void setUniformColor(const void*, size_t);          // ucolor
    void setUniformMatrixPalette(const void*, size_t);  // u_matrixPalette

    void setUniformDirLightColor(const void*, size_t);
    void setUniformDirLightDir(const void*, size_t);

    void setUniformPointLightColor(const void*, size_t);
    void setUniformPointLightPosition(const void*, size_t);
    void setUniformPointLightRangeInverse(const void*, size_t);

    void setUniformSpotLightColor(const void*, size_t);
    void setUniformSpotLightPosition(const void*, size_t);
    void setUniformSpotLightDir(const void*, size_t);
    void setUniformSpotLightInnerAngleCos(const void*, size_t);
    void setUniformSpotLightOuterAngleCos(const void*, size_t);
    void setUniformSpotLightRangeInverse(const void*, size_t);

    void setUniformAmbientLigthColor(const void*, size_t);

protected:
    Pass();
    ~Pass();
    bool init(Technique* parent);
    bool initWithProgramState(Technique* parent, rhi::ProgramState* glProgramState);

    void setProgramState(rhi::ProgramState* programState);
    Node* getTarget() const;

    VertexInputBinding* _vertexInputBinding = nullptr;
    rhi::ProgramState* _programState      = nullptr;
    Technique* _technique                     = nullptr;
    bool _hashDirty                           = true;
    RenderState _renderState;
    std::string _name;

private:
    void initUniformLocations();
    void onBeforeVisitCmd(MeshCommand*);
    void onAfterVisitCmd(MeshCommand*);

    rhi::UniformLocation _locMVPMatrix;
    rhi::UniformLocation _locMVMatrix;
    rhi::UniformLocation _locPMatrix;
    rhi::UniformLocation _locNormalMatrix;

    rhi::UniformLocation _locTexture;        // u_tex0
    rhi::UniformLocation _locNormalTexture;  // u_normalTex

    rhi::UniformLocation _locColor;          // ucolor
    rhi::UniformLocation _locMatrixPalette;  // u_matrixPalette

    rhi::UniformLocation _locDirLightColor;
    rhi::UniformLocation _locDirLightDir;

    rhi::UniformLocation _locPointLightColor;
    rhi::UniformLocation _locPointLightPosition;
    rhi::UniformLocation _locPointLightRangeInverse;

    rhi::UniformLocation _locSpotLightColor;
    rhi::UniformLocation _locSpotLightPosition;
    rhi::UniformLocation _locSpotLightDir;
    rhi::UniformLocation _locSpotLightInnerAngleCos;
    rhi::UniformLocation _locSpotLightOuterAngleCos;
    rhi::UniformLocation _locSpotLightRangeInverse;

    rhi::UniformLocation _locAmbientLigthColor;

    // renderer state cache variables
    bool _rendererDepthTestEnabled                 = true;
    rhi::CompareFunction _rendererDepthCmpFunc = rhi::CompareFunction::LESS;
    rhi::CullMode _rendererCullMode            = rhi::CullMode::BACK;
    rhi::Winding _rendererWinding              = rhi::Winding::COUNTER_CLOCK_WISE;
    bool _rendererDepthWrite                       = false;
};

}
