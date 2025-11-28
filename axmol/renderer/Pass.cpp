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
#include "axmol/renderer/Pass.h"
#include <xxhash.h>
#include "axmol/renderer/Texture2D.h"
#include "axmol/renderer/Technique.h"
#include "axmol/renderer/Material.h"
#include "axmol/rhi/ProgramState.h"
#include "axmol/3d/MeshVertexIndexData.h"
#include "axmol/3d/VertexInputBinding.h"
#include "axmol/base/Director.h"
#include "axmol/renderer/Renderer.h"

#include "axmol/base/Types.h"
#include "axmol/base/Utils.h"
#include "axmol/2d/Node.h"

namespace ax
{

// uniform names

static const char* s_dirLightUniformColorName = "u_DirLightSourceColor";
static const char* s_dirLightUniformDirName   = "u_DirLightSourceDirection";

static const char* s_pointLightUniformColorName        = "u_PointLightSourceColor";
static const char* s_pointLightUniformPositionName     = "u_PointLightSourcePosition";
static const char* s_pointLightUniformRangeInverseName = "u_PointLightSourceRangeInverse";

static const char* s_spotLightUniformColorName         = "u_SpotLightSourceColor";
static const char* s_spotLightUniformPositionName      = "u_SpotLightSourcePosition";
static const char* s_spotLightUniformDirName           = "u_SpotLightSourceDirection";
static const char* s_spotLightUniformInnerAngleCosName = "u_SpotLightSourceInnerAngleCos";
static const char* s_spotLightUniformOuterAngleCosName = "u_SpotLightSourceOuterAngleCos";
static const char* s_spotLightUniformRangeInverseName  = "u_SpotLightSourceRangeInverse";

static const char* s_ambientLightUniformColorName = "u_AmbientLightSourceColor";

Pass* Pass::create(Technique* technique)
{
    auto pass = new Pass();
    if (pass->init(technique))
    {
        pass->autorelease();
        return pass;
    }
    AX_SAFE_DELETE(pass);
    return nullptr;
}

Pass* Pass::createWithProgramState(Technique* technique, rhi::ProgramState* programState)
{
    auto pass = new Pass();
    if (pass->initWithProgramState(technique, programState))
    {
        pass->autorelease();
        return pass;
    }
    AX_SAFE_DELETE(pass);
    return nullptr;
}

bool Pass::init(Technique* technique)
{
    _technique = technique;
    return true;
}

bool Pass::initWithProgramState(Technique* technique, rhi::ProgramState* programState)
{
    _technique = technique;
    setProgramState(programState);
    return true;
}

Pass::Pass() {}

Pass::~Pass()
{
    AX_SAFE_RELEASE(_vertexInputBinding);
    AX_SAFE_RELEASE(_programState);
    AX_SAFE_RELEASE(_vertexLayout);
}

Pass* Pass::clone() const
{
    auto pass          = new Pass();
    pass->_renderState = _renderState;

    auto clonedPS = _programState->clone();
    pass->setProgramState(clonedPS);
    clonedPS->release();

    pass->_vertexInputBinding = _vertexInputBinding;
    AX_SAFE_RETAIN(pass->_vertexInputBinding);

    pass->setTechnique(_technique);

    pass->autorelease();

    return pass;
}

rhi::ProgramState* Pass::getProgramState() const
{
    return _programState;
}

void Pass::setProgramState(rhi::ProgramState* programState)
{
    if (Object::assign(_programState, programState))
    {
        initUniformLocations();
        _hashDirty = true;
    }
}

void Pass::setVertexLayout(rhi::VertexLayout* vertexLayout)
{
    Object::assign(_vertexLayout, vertexLayout);
}

void Pass::initUniformLocations()
{
    auto ps = _programState;

    _locMVPMatrix    = ps->getUniformLocation("u_MVPMatrix");
    _locMVMatrix     = ps->getUniformLocation("u_MVMatrix");
    _locPMatrix      = ps->getUniformLocation("u_PMatrix");
    _locNormalMatrix = ps->getUniformLocation("u_NormalMatrix");

    _locTexture       = ps->getUniformLocation("u_tex0");
    _locNormalTexture = ps->getUniformLocation("u_normalTex");

    _locColor         = ps->getUniformLocation("u_color");
    _locMatrixPalette = ps->getUniformLocation("u_matrixPalette");

    _locDirLightColor = ps->getUniformLocation(s_dirLightUniformColorName);
    _locDirLightDir   = ps->getUniformLocation(s_dirLightUniformDirName);

    _locPointLightColor        = ps->getUniformLocation(s_pointLightUniformColorName);
    _locPointLightPosition     = ps->getUniformLocation(s_pointLightUniformPositionName);
    _locPointLightRangeInverse = ps->getUniformLocation(s_pointLightUniformRangeInverseName);

    _locSpotLightColor         = ps->getUniformLocation(s_spotLightUniformColorName);
    _locSpotLightPosition      = ps->getUniformLocation(s_spotLightUniformPositionName);
    _locSpotLightDir           = ps->getUniformLocation(s_spotLightUniformDirName);
    _locSpotLightInnerAngleCos = ps->getUniformLocation(s_spotLightUniformInnerAngleCosName);
    _locSpotLightOuterAngleCos = ps->getUniformLocation(s_spotLightUniformOuterAngleCosName);
    _locSpotLightRangeInverse  = ps->getUniformLocation(s_spotLightUniformRangeInverseName);

    _locAmbientLigthColor = ps->getUniformLocation(s_ambientLightUniformColorName);
}

void Pass::draw(MeshCommand* meshCommand,
                float globalZOrder,
                rhi::Buffer* vertexBuffer,
                rhi::Buffer* indexBuffer,
                MeshCommand::PrimitiveType primitive,
                MeshCommand::IndexFormat indexFormat,
                unsigned int indexCount,
                const Mat4& modelView)
{

    meshCommand->setBeforeCallback(AX_CALLBACK_0(Pass::onBeforeVisitCmd, this, meshCommand));
    meshCommand->setAfterCallback(AX_CALLBACK_0(Pass::onAfterVisitCmd, this, meshCommand));
    meshCommand->init(globalZOrder, modelView);
    meshCommand->setPrimitiveType(primitive);
    meshCommand->setIndexBuffer(indexBuffer, indexFormat);
    meshCommand->setVertexBuffer(vertexBuffer);
    meshCommand->setIndexDrawInfo(0, indexCount);
    meshCommand->setWeakPSVL(_programState, _vertexLayout);

    auto* renderer = Director::getInstance()->getRenderer();

    renderer->addCommand(meshCommand);
}

void Pass::updateMVPUniform(const Mat4& modelView)
{
    auto& matrixP = Director::getInstance()->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
    auto mvp      = matrixP * modelView;
    _programState->setUniform(_locMVPMatrix, mvp.m, sizeof(mvp.m));
    if (_locMVMatrix)
    {
        _programState->setUniform(_locMVMatrix, modelView.m, sizeof(modelView.m));
    }

    if (_locPMatrix)
    {
        _programState->setUniform(_locPMatrix, matrixP.m, sizeof(matrixP.m));
    }

    if (_locNormalMatrix)
    {
        auto normalMatrix = utils::getNormalMat3OfMat4(modelView);
        _programState->setUniform(_locNormalMatrix, &normalMatrix, sizeof(normalMatrix));
    }
}

void Pass::onBeforeVisitCmd(MeshCommand* command)
{
    auto* renderer = Director::getInstance()->getRenderer();

    // save renderer states
    _rendererDepthTestEnabled = renderer->getDepthTest();
    _rendererDepthCmpFunc     = renderer->getDepthCompareFunc();
    _rendererCullMode         = renderer->getCullMode();
    _rendererDepthWrite       = renderer->getDepthWrite();
    _rendererWinding          = renderer->getWinding();
    // apply state blocks
    _renderState.bindPass(this, command);

    updateMVPUniform(command->getMV());
}

void Pass::onAfterVisitCmd(MeshCommand* command)
{
    auto* renderer = Director::getInstance()->getRenderer();
    // restore renderer states
    renderer->setDepthTest(_rendererDepthTestEnabled);
    renderer->setDepthCompareFunc(_rendererDepthCmpFunc);
    renderer->setCullMode(_rendererCullMode);
    renderer->setDepthWrite(_rendererDepthWrite);
    renderer->setWinding(_rendererWinding);
}

Node* Pass::getTarget() const
{
    AXASSERT(_technique && _technique->_material, "Pass must have a Technique and Material");

    Material* material = _technique->_material;
    return material->_target;
}

void Pass::setTechnique(Technique* technique)
{
    _technique = technique;  // weak reference
}

void Pass::setVertexInputBinding(VertexInputBinding* binding)
{
    Object::assign(_vertexInputBinding, binding);
}

VertexInputBinding* Pass::getVertexAttributeBinding() const
{
    return _vertexInputBinding;
}

void Pass::setUniformTexture(uint32_t slot, rhi::Texture* tex)
{
    _programState->setTexture(_locTexture, slot, tex);
}

void Pass::setUniformNormTexture(uint32_t slot, rhi::Texture* tex)
{
    _programState->setTexture(_locNormalTexture, slot, tex);
}

#define TRY_SET_UNIFORM(loc)                                         \
    do                                                               \
    {                                                                \
        if (loc)                                                     \
        {                                                            \
            _programState->setUniform(loc, data, (uint32_t)dataLen); \
        }                                                            \
    } while (false)

void Pass::setUniformColor(const void* data, size_t dataLen)
{
    TRY_SET_UNIFORM(_locColor);
}

void Pass::setUniformMatrixPalette(const void* data, size_t dataLen)
{
    TRY_SET_UNIFORM(_locMatrixPalette);
}

void Pass::setUniformDirLightColor(const void* data, size_t dataLen)
{
    TRY_SET_UNIFORM(_locDirLightColor);
}

void Pass::setUniformDirLightDir(const void* data, size_t dataLen)
{
    TRY_SET_UNIFORM(_locDirLightDir);
}

void Pass::setUniformPointLightColor(const void* data, size_t dataLen)
{
    TRY_SET_UNIFORM(_locPointLightColor);
}

void Pass::setUniformPointLightPosition(const void* data, size_t dataLen)
{
    TRY_SET_UNIFORM(_locPointLightPosition);
}

void Pass::setUniformPointLightRangeInverse(const void* data, size_t dataLen)
{
    TRY_SET_UNIFORM(_locPointLightRangeInverse);
}

void Pass::setUniformSpotLightColor(const void* data, size_t dataLen)
{
    TRY_SET_UNIFORM(_locSpotLightColor);
}

void Pass::setUniformSpotLightPosition(const void* data, size_t dataLen)
{
    TRY_SET_UNIFORM(_locSpotLightPosition);
}

void Pass::setUniformSpotLightDir(const void* data, size_t dataLen)
{
    TRY_SET_UNIFORM(_locSpotLightDir);
}

void Pass::setUniformSpotLightInnerAngleCos(const void* data, size_t dataLen)
{
    TRY_SET_UNIFORM(_locSpotLightInnerAngleCos);
}

void Pass::setUniformSpotLightOuterAngleCos(const void* data, size_t dataLen)
{
    TRY_SET_UNIFORM(_locSpotLightOuterAngleCos);
}

void Pass::setUniformSpotLightRangeInverse(const void* data, size_t dataLen)
{
    TRY_SET_UNIFORM(_locSpotLightRangeInverse);
}

void Pass::setUniformAmbientLigthColor(const void* data, size_t dataLen)
{
    TRY_SET_UNIFORM(_locAmbientLigthColor);
}

}  // namespace ax
