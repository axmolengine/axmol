/****************************************************************************
 Copyright (c) 2016 Chukong Technologies Inc.
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
 ****************************************************************************/

#include "axmol/platform/PlatformMacros.h"
#include "axmol/vr/VRGenericRenderer.h"
#include "axmol/vr/VRDistortionMesh.h"
#include "axmol/vr/VRDistortion.h"
#include "axmol/vr/VRGenericHeadTracker.h"
#include "axmol/renderer/Renderer.h"
#include "axmol/rhi/ProgramState.h"
#include "axmol/base/Director.h"
#include "axmol/2d/Scene.h"
#include "axmol/2d/Camera.h"
#include "axmol/2d/Sprite.h"
#include "axmol/platform/RenderView.h"

namespace ax::experimental
{

VRGenericRenderer::VRGenericRenderer()
    : _leftDistortionMesh(nullptr), _rightDistortionMesh(nullptr), _distortion(nullptr), _vignetteEnabled(true)
{
    _headTracker = new VRGenericHeadTracker;
    _director = Director::getInstance();
}

VRGenericRenderer::~VRGenericRenderer()
{
    AX_SAFE_RELEASE(_programState);
    AX_SAFE_RELEASE(_vertexLayout);
    AX_SAFE_DELETE(_headTracker);
    AX_SAFE_DELETE(_renderTexture);
    AX_SAFE_DELETE(_distortion);
    AX_SAFE_DELETE(_leftDistortionMesh);
    AX_SAFE_DELETE(_rightDistortionMesh);
}

void VRGenericRenderer::setup(RenderView* /*renderView*/)
{
    // set origin to 0,0 in case origin is not 0,0
    auto vp = Camera::getDefaultViewport();

    _leftEye.viewport.y      = vp.y / 2 + vp.height / 4;
    _leftEye.viewport.x      = vp.x / 4;
    _leftEye.viewport.width  = vp.width / 2;
    _leftEye.viewport.height = vp.height / 2;

    _rightEye.viewport.y      = vp.y / 2 + vp.height / 4;
    _rightEye.viewport.x      = _leftEye.viewport.width + vp.x / 2;
    _rightEye.viewport.width  = vp.width / 2;
    _rightEye.viewport.height = vp.height / 2;

    _screenSize = _director->getRenderView()->getFrameSize();

    _renderTexture = RenderTexture::create(_screenSize.width, _screenSize.height);
    _renderTexture->retain();
    _rtSize = _renderTexture->getContentSize();

    _distortion          = new Distortion();
    _leftDistortionMesh  = createDistortionMesh(VREye::EyeType::LEFT);
    _rightDistortionMesh = createDistortionMesh(VREye::EyeType::RIGHT);

    setupProgram();
}

void VRGenericRenderer::cleanup() {}

IVRHeadTracker* VRGenericRenderer::getHeadTracker()
{
    return _headTracker;
}

void VRGenericRenderer::render(Scene* scene, Renderer* renderer)
{
    const float eyeOffset = 0.5f;

    // Get head rotation and compute view transforms for both eyes
    auto headRotation = _headTracker->getLocalRotation();

    Mat4 leftTransform;
    Mat4::createTranslation(eyeOffset, 0, 0, &leftTransform);
    leftTransform *= headRotation;

    Mat4 rightTransform;
    Mat4::createTranslation(-eyeOffset, 0, 0, &rightTransform);
    rightTransform *= headRotation;

    auto texture = _renderTexture->getSprite()->getTexture();

    const Color clearColor = _director->getClearColor();

    // Push raster transform to scale scissor/viewport into RT space
    renderer->addCallbackCommand(AX_CALLBACK_0(VRGenericRenderer::pushRasterTransform, this, renderer));

    // Render left eye
    _renderTexture->beginWithClear(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    _renderTexture->setVirtualViewport(
        Vec2(0, 0),  // Start position on target texture
        Rect(_leftEye.viewport.x, _leftEye.viewport.y, _leftEye.viewport.w, _leftEye.viewport.h),
        Rect(_leftEye.viewport.x, _leftEye.viewport.y, _leftEye.viewport.w, _leftEye.viewport.h));

    scene->render(renderer, leftTransform);
    _renderTexture->end();

    // Render right eye
    _renderTexture->begin();
    _renderTexture->setVirtualViewport(
        Vec2(0, 0),  // Start position on target texture
        Rect(_rightEye.viewport.x, _rightEye.viewport.y, _rightEye.viewport.w, _rightEye.viewport.h),
        Rect(_rightEye.viewport.x, _rightEye.viewport.y, _rightEye.viewport.w, _rightEye.viewport.h));
    scene->render(renderer, rightTransform);
    _renderTexture->end();

    // Pop raster transform to restore renderer state
    renderer->addCallbackCommand(AX_CALLBACK_0(VRGenericRenderer::popRasterTransform, this, renderer));

    // Hacker: due to GroupCommand, scene->render internally calls renderer->render()
    // to finish drawing into offscreen RT. We need an empty begin/end to restore the default screen RT.
    _renderTexture->begin();
    _renderTexture->end();

    // Bind the rendered texture to the distortion shader
    _programState->setTexture(texture->getRHITexture());

    // Restore viewport for distortion rendering
    auto vpCmd     = renderer->nextCallbackCommand();
    auto defaultVP = Camera::getDefaultViewport();
    vpCmd->func    = [=]() { renderer->setViewport(defaultVP.x, defaultVP.y, defaultVP.width, defaultVP.height); };
    renderer->addCommand(vpCmd);

    // Submit distortion draw commands for both eyes
    renderer->addCommand(&_leftEyeCmd);
    renderer->addCommand(&_rightEyeCmd);
}

void VRGenericRenderer::pushRasterTransform(Renderer* renderer)
{
    RasterTransform xf;
    xf.sx = _rtSize.width / _screenSize.width;
    xf.sy = _rtSize.height / _screenSize.height;

    renderer->pushRasterTransform(xf);
}

void VRGenericRenderer::popRasterTransform(Renderer* renderer)
{
    renderer->popRasterTransform();
}

DistortionMesh* VRGenericRenderer::createDistortionMesh(VREye::EyeType eyeType)
{
    auto vp = Camera::getDefaultViewport();

    const float screenWidth  = _screenSize.width;
    const float screenHeight = _screenSize.height;
    const float xEyeOffsetScreen =
        (eyeType == VREye::EyeType::LEFT) ? screenWidth / 4 + vp.x : screenWidth * 3 / 4 + vp.x;
    const float yEyeOffsetScreen = screenHeight / 2 + vp.y;

    const float textureWidth      = _screenSize.width;
    const float textureHeight     = _screenSize.height;
    const float xEyeOffsetTexture = (eyeType == VREye::EyeType::LEFT) ? _screenSize.width / 4 : _screenSize.width * 3 / 4;
    const float yEyeOffsetTexture = _screenSize.height / 2;

    const float viewportX = (eyeType == VREye::EyeType::LEFT) ? 0 : textureWidth / 2;
    const float viewportY = 0;
    const float viewportW = textureWidth / 2;
    const float viewportH = textureHeight;

    return new DistortionMesh(_distortion, screenWidth, screenHeight, xEyeOffsetScreen, yEyeOffsetScreen, textureWidth,
                              textureHeight, xEyeOffsetTexture, yEyeOffsetTexture, viewportX, viewportY, viewportW,
                              viewportH, _vignetteEnabled);
}

void VRGenericRenderer::setupProgram()
{
    auto program = axpm->loadProgram("vr_vs", "vr_fs");

    _programState = new ProgramState(program);

    auto desc = axvlm->allocateVertexLayoutDesc();

    desc.startLayout(3);

    desc.addAttrib("a_position", program->getVertexInputDesc("a_position"), rhi::VertexFormat::FLOAT2, 0, false);
    desc.addAttrib("a_texCoord", program->getVertexInputDesc("a_texCoord"), rhi::VertexFormat::FLOAT2, sizeof(Vec2),
                   false);
    desc.addAttrib("a_vignette", program->getVertexInputDesc("a_vignette"), rhi::VertexFormat::FLOAT,
                   sizeof(Vec2) + sizeof(Vec2), false);

    desc.endLayout();

    _vertexLayout = axvlm->acquireVertexLayout(std::move(desc));

    _leftEyeCmd.setWeakPSVL(_programState, _vertexLayout);
    _rightEyeCmd.setWeakPSVL(_programState, _vertexLayout);

    _leftEyeCmd.setVertexBuffer(_leftDistortionMesh->_vbo);
    _leftEyeCmd.setIndexBuffer(_leftDistortionMesh->_ebo, CustomCommand::IndexFormat::U_SHORT);
    _leftEyeCmd.setDrawType(CustomCommand::DrawType::ELEMENT);
    _leftEyeCmd.setPrimitiveType(CustomCommand::PrimitiveType::TRIANGLE_STRIP);
    _leftEyeCmd.setIndexDrawInfo(0, _leftDistortionMesh->_indices);

    _rightEyeCmd.setVertexBuffer(_rightDistortionMesh->_vbo);
    _rightEyeCmd.setIndexBuffer(_rightDistortionMesh->_ebo, CustomCommand::IndexFormat::U_SHORT);
    _rightEyeCmd.setDrawType(CustomCommand::DrawType::ELEMENT);
    _rightEyeCmd.setPrimitiveType(CustomCommand::PrimitiveType::TRIANGLE_STRIP);
    _rightEyeCmd.setIndexDrawInfo(0, _rightDistortionMesh->_indices);
}

}  // namespace ax::experimental
