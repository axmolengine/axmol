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

 ****************************************************************************/
#include "2d/CameraBackgroundBrush.h"
#include <stddef.h>  // offsetof
#include "base/Types.h"
#include "2d/Camera.h"
#include "base/Macros.h"
#include "base/Utils.h"
#include "base/Configuration.h"
#include "base/Director.h"
#include "renderer/Renderer.h"
#include "renderer/RenderState.h"
#include "renderer/TextureCube.h"
#include "renderer/Shaders.h"

#if AX_ENABLE_CACHE_TEXTURE_DATA
#    include "base/EventCustom.h"
#    include "base/EventListenerCustom.h"
#    include "base/EventType.h"
#    include "base/EventDispatcher.h"
#endif

namespace ax
{

CameraBackgroundBrush::CameraBackgroundBrush() {}

CameraBackgroundBrush::~CameraBackgroundBrush()
{
    AX_SAFE_RELEASE_NULL(_programState);
}

CameraBackgroundBrush* CameraBackgroundBrush::createNoneBrush()
{
    auto ret = new CameraBackgroundBrush();
    ret->init();

    ret->autorelease();
    return ret;
}

CameraBackgroundColorBrush* CameraBackgroundBrush::createColorBrush(const Color4F& color, float depth)
{
    return CameraBackgroundColorBrush::create(color, depth);
}

CameraBackgroundDepthBrush* CameraBackgroundBrush::createDepthBrush(float depth)
{
    return CameraBackgroundDepthBrush::create(depth);
}

CameraBackgroundSkyBoxBrush* CameraBackgroundBrush::createSkyboxBrush(std::string_view positive_x,
                                                                      std::string_view negative_x,
                                                                      std::string_view positive_y,
                                                                      std::string_view negative_y,
                                                                      std::string_view positive_z,
                                                                      std::string_view negative_z)
{
    return CameraBackgroundSkyBoxBrush::create(positive_x, negative_x, positive_y, negative_y, positive_z, negative_z);
}

//////////////////////////////////////////////////////////////////////////////////////////

CameraBackgroundDepthBrush::CameraBackgroundDepthBrush()
    : _depth(0.f)
    , _clearColor(false)
#if AX_ENABLE_CACHE_TEXTURE_DATA
    , _backToForegroundListener(nullptr)
#endif
{
#if AX_ENABLE_CACHE_TEXTURE_DATA
    _backToForegroundListener =
        EventListenerCustom::create(EVENT_RENDERER_RECREATED, [this](EventCustom*) { initBuffer(); });
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_backToForegroundListener, -1);
#endif
}
CameraBackgroundDepthBrush::~CameraBackgroundDepthBrush()
{
#if AX_ENABLE_CACHE_TEXTURE_DATA
    Director::getInstance()->getEventDispatcher()->removeEventListener(_backToForegroundListener);
#endif
}

CameraBackgroundDepthBrush* CameraBackgroundDepthBrush::create(float depth)
{
    auto ret = new CameraBackgroundDepthBrush();

    if (ret->init())
    {
        ret->_depth = depth;
        ret->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ret);
    }

    return ret;
}

bool CameraBackgroundDepthBrush::init()
{
    AX_SAFE_RELEASE_NULL(_programState);
    auto* program = backend::Program::getBuiltinProgram(backend::ProgramType::CAMERA_CLEAR);
    _programState = new backend::ProgramState(program);

    _locDepth = _programState->getUniformLocation("dpeth");

    auto& pipelineDescriptor        = _customCommand.getPipelineDescriptor();
    pipelineDescriptor.programState = _programState;

    _vertices.resize(4);
    _vertices[0].vertices = Vec3(-1, -1, 0);
    _vertices[1].vertices = Vec3(1, -1, 0);
    _vertices[2].vertices = Vec3(1, 1, 0);
    _vertices[3].vertices = Vec3(-1, 1, 0);

    _vertices[0].colors = _vertices[1].colors = _vertices[2].colors = _vertices[3].colors = Color4B(0, 0, 0, 1);

    _vertices[0].texCoords = Tex2F(0, 0);
    _vertices[1].texCoords = Tex2F(1, 0);
    _vertices[2].texCoords = Tex2F(1, 1);
    _vertices[3].texCoords = Tex2F(0, 1);

    _customCommand.setBeforeCallback(AX_CALLBACK_0(CameraBackgroundDepthBrush::onBeforeDraw, this));
    _customCommand.setAfterCallback(AX_CALLBACK_0(CameraBackgroundDepthBrush::onAfterDraw, this));

    initBuffer();
    return true;
}

void CameraBackgroundDepthBrush::initBuffer()
{
    uint16_t indices[6] = {0, 1, 2, 2, 3, 0};
    _customCommand.createVertexBuffer(sizeof(_vertices[0]), _vertices.size(), CustomCommand::BufferUsage::STATIC);
    _customCommand.createIndexBuffer(CustomCommand::IndexFormat::U_SHORT, sizeof(indices) / sizeof(indices[0]),
                                     CustomCommand::BufferUsage::STATIC);

    _customCommand.updateVertexBuffer(_vertices.data(), sizeof(_vertices[0]) * _vertices.size());
    _customCommand.updateIndexBuffer(indices, sizeof(indices));
}

void CameraBackgroundDepthBrush::drawBackground(Camera* /*camera*/)
{
    auto* renderer = Director::getInstance()->getRenderer();
    // `clear screen` should be executed before other commands
    auto* groupCommand = renderer->getNextGroupCommand();
    groupCommand->init(-1.0f);
    _customCommand.init(0.0f);

    renderer->addCommand(groupCommand);
    renderer->pushGroup(groupCommand->getRenderQueueID());

    auto& pipelineDescriptor = _customCommand.getPipelineDescriptor();
    auto& blend              = pipelineDescriptor.blendDescriptor;
    blend.writeMask          = _clearColor ? backend::ColorWriteMask::ALL : backend::ColorWriteMask::NONE;

    // draw
    _programState->setUniform(_locDepth, &_depth, sizeof(_depth));

    renderer->addCommand(&_customCommand);

    renderer->popGroup();
}

void CameraBackgroundDepthBrush::onBeforeDraw()
{
    auto* renderer               = Director::getInstance()->getRenderer();
    _stateBlock.stencilWriteMask = renderer->getStencilWriteMask();
    _stateBlock.depthTest        = renderer->getDepthTest();
    _stateBlock.compareFunc      = renderer->getDepthCompareFunction();

    renderer->setStencilWriteMask(0);
    renderer->setDepthTest(true);
    renderer->setDepthCompareFunction(backend::CompareFunction::ALWAYS);
}

void CameraBackgroundDepthBrush::onAfterDraw()
{
    auto* renderer = Director::getInstance()->getRenderer();
    renderer->setStencilWriteMask(_stateBlock.stencilWriteMask);
    renderer->setDepthTest(_stateBlock.depthTest);
    renderer->setDepthCompareFunction(_stateBlock.compareFunc);
}

//////////////////////////////////////////////////////////////////////////////////////////

CameraBackgroundColorBrush::CameraBackgroundColorBrush() : _color(0.f, 0.f, 0.f, 0.f) {}

CameraBackgroundColorBrush::~CameraBackgroundColorBrush() {}

bool CameraBackgroundColorBrush::init()
{
    CameraBackgroundDepthBrush::init();
    this->_clearColor = true;
    return true;
}

void CameraBackgroundColorBrush::drawBackground(Camera* camera)
{
    BlendFunc op = {BlendFunc::ALPHA_NON_PREMULTIPLIED.src, BlendFunc::ALPHA_NON_PREMULTIPLIED.dst};

    auto& blend                = _customCommand.getPipelineDescriptor().blendDescriptor;
    blend.sourceRGBBlendFactor = blend.sourceAlphaBlendFactor = op.src;
    blend.destinationRGBBlendFactor = blend.destinationAlphaBlendFactor = op.dst;
    blend.blendEnabled                                                  = true;

    CameraBackgroundDepthBrush::drawBackground(camera);
}

void CameraBackgroundColorBrush::setColor(const Color4F& color)
{
    for (auto&& vert : _vertices)
    {
        vert.colors = Color4B(color);
    }
    _customCommand.updateVertexBuffer(_vertices.data(), sizeof(_vertices[0]) * _vertices.size());
}

CameraBackgroundColorBrush* CameraBackgroundColorBrush::create(const Color4F& color, float depth)
{
    auto ret = new CameraBackgroundColorBrush();

    if (ret->init())
    {
        ret->setColor(color);
        ret->setDepth(depth);
        ret->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ret);
    }

    return ret;
}

/////////////////////////////////////////////////////////////////////////////////////////////
CameraBackgroundSkyBoxBrush::CameraBackgroundSkyBoxBrush()
    : _texture(nullptr)
    , _actived(true)
    , _textureValid(true)
#if AX_ENABLE_CACHE_TEXTURE_DATA
    , _backToForegroundListener(nullptr)
#endif
{
#if AX_ENABLE_CACHE_TEXTURE_DATA
    _backToForegroundListener =
        EventListenerCustom::create(EVENT_RENDERER_RECREATED, [this](EventCustom*) { initBuffer(); });
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_backToForegroundListener, -1);
#endif
}

CameraBackgroundSkyBoxBrush::~CameraBackgroundSkyBoxBrush()
{
    AX_SAFE_RELEASE(_texture);
#if AX_ENABLE_CACHE_TEXTURE_DATA
    Director::getInstance()->getEventDispatcher()->removeEventListener(_backToForegroundListener);
#endif
}

CameraBackgroundSkyBoxBrush* CameraBackgroundSkyBoxBrush::create(std::string_view positive_x,
                                                                 std::string_view negative_x,
                                                                 std::string_view positive_y,
                                                                 std::string_view negative_y,
                                                                 std::string_view positive_z,
                                                                 std::string_view negative_z)
{
    CameraBackgroundSkyBoxBrush* ret = nullptr;

    auto texture = TextureCube::create(positive_x, negative_x, positive_y, negative_y, positive_z, negative_z);

    if (texture != nullptr)
    {

        Texture2D::TexParams tRepeatParams;
        tRepeatParams.magFilter    = backend::SamplerFilter::LINEAR;
        tRepeatParams.minFilter    = backend::SamplerFilter::LINEAR;
        tRepeatParams.sAddressMode = backend::SamplerAddressMode::CLAMP_TO_EDGE;
        tRepeatParams.tAddressMode = backend::SamplerAddressMode::CLAMP_TO_EDGE;
        texture->setTexParameters(tRepeatParams);

        ret = new CameraBackgroundSkyBoxBrush;

        if (ret->init())
        {
            ret->setTexture(texture);
            ret->autorelease();
        }
        else
        {
            AX_SAFE_DELETE(texture);
            AX_SAFE_DELETE(ret);
        }
    }

    return ret;
}

CameraBackgroundSkyBoxBrush* CameraBackgroundSkyBoxBrush::create()
{
    auto ret = new CameraBackgroundSkyBoxBrush();

    if (ret->init())
    {
        ret->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ret);
    }

    return ret;
}

void CameraBackgroundSkyBoxBrush::drawBackground(Camera* camera)
{
    auto* renderer = Director::getInstance()->getRenderer();
    // `clear screen` should be executed before other commands
    auto* groupCommand = renderer->getNextGroupCommand();
    groupCommand->init(-1.0f);
    _customCommand.init(0.0f);

    if (!_actived)
        return;

    Mat4 cameraModelMat = camera->getNodeToWorldTransform();

    auto& pipelineDescriptor                        = _customCommand.getPipelineDescriptor();
    pipelineDescriptor.blendDescriptor.blendEnabled = false;

    Vec4 color(1.f, 1.f, 1.f, 1.f);
    cameraModelMat.m[12] = cameraModelMat.m[13] = cameraModelMat.m[14] = 0;

    _programState->setUniform(_uniformColorLoc, &color, sizeof(color));
    _programState->setUniform(_uniformCameraRotLoc, cameraModelMat.m, sizeof(cameraModelMat.m));


    renderer->addCommand(groupCommand);
    renderer->pushGroup(groupCommand->getRenderQueueID());

    renderer->addCommand(&_customCommand);

    renderer->popGroup();

    AX_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1, 8);
}

bool CameraBackgroundSkyBoxBrush::init()
{

    _customCommand.setBeforeCallback(AX_CALLBACK_0(CameraBackgroundSkyBoxBrush::onBeforeDraw, this));
    _customCommand.setAfterCallback(AX_CALLBACK_0(CameraBackgroundSkyBoxBrush::onAfterDraw, this));

    AX_SAFE_RELEASE_NULL(_programState);
    auto* program        = backend::Program::getBuiltinProgram(backend::ProgramType::SKYBOX_3D);
    _programState        = new backend::ProgramState(program);
    _uniformColorLoc     = _programState->getUniformLocation("u_color");
    _uniformCameraRotLoc = _programState->getUniformLocation("u_cameraRot");
    _uniformEnvLoc       = _programState->getUniformLocation("u_Env");

    auto& pipelineDescriptor        = _customCommand.getPipelineDescriptor();
    pipelineDescriptor.programState = _programState;
    // disable blend
    pipelineDescriptor.blendDescriptor.blendEnabled = false;

    initBuffer();

    return true;
}

void CameraBackgroundSkyBoxBrush::initBuffer()
{
    // init vertex buffer object
    Vec3 vexBuf[] = {Vec3(1, -1, 1),  Vec3(1, 1, 1),  Vec3(-1, 1, 1),  Vec3(-1, -1, 1),
                     Vec3(1, -1, -1), Vec3(1, 1, -1), Vec3(-1, 1, -1), Vec3(-1, -1, -1)};

    _customCommand.createVertexBuffer(sizeof(vexBuf[0]), sizeof(vexBuf) / sizeof(vexBuf[0]),
                                      CustomCommand::BufferUsage::STATIC);
    _customCommand.updateVertexBuffer(vexBuf, sizeof(vexBuf));

    // init index buffer object
    uint16_t idxBuf[] = {
        2, 1, 0, 3, 2, 0,  // font
        1, 5, 4, 1, 4, 0,  // right
        4, 5, 6, 4, 6, 7,  // back
        7, 6, 2, 7, 2, 3,  // left
        2, 6, 5, 2, 5, 1,  // up
        3, 0, 4, 3, 4, 7   // down
    };
    _customCommand.createIndexBuffer(CustomCommand::IndexFormat::U_SHORT, sizeof(idxBuf) / sizeof(idxBuf[0]),
                                     CustomCommand::BufferUsage::STATIC);
    _customCommand.updateIndexBuffer(idxBuf, sizeof(idxBuf));
}

void CameraBackgroundSkyBoxBrush::setTexture(TextureCube* texture)
{
    AX_SAFE_RETAIN(texture);
    AX_SAFE_RELEASE(_texture);
    _texture = texture;
    _programState->setTexture(_uniformEnvLoc, 0, _texture->getBackendTexture());
}

bool CameraBackgroundSkyBoxBrush::isActived() const
{
    return _actived;
}
void CameraBackgroundSkyBoxBrush::setActived(bool actived)
{
    _actived = actived;
}

void CameraBackgroundSkyBoxBrush::setTextureValid(bool valid)
{
    _textureValid = valid;
}

bool CameraBackgroundSkyBoxBrush::isValid()
{
    return _actived;
}

void CameraBackgroundSkyBoxBrush::onBeforeDraw()
{
    auto* renderer         = Director::getInstance()->getRenderer();
    _stateBlock.depthTest  = renderer->getDepthTest();
    _stateBlock.depthWrite = renderer->getDepthWrite();
    _stateBlock.depthFunc  = renderer->getDepthCompareFunction();
    _stateBlock.cullMode   = renderer->getCullMode();
}

void CameraBackgroundSkyBoxBrush::onAfterDraw()
{
    auto* renderer = Director::getInstance()->getRenderer();
    renderer->setDepthTest(_stateBlock.depthTest);
    renderer->setDepthWrite(_stateBlock.depthWrite);
    renderer->setDepthCompareFunction(_stateBlock.depthFunc);
    renderer->setCullMode(_stateBlock.cullMode);
}

}
