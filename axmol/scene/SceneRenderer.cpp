/****************************************************************************
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

#include "axmol/scene/SceneRenderer.h"

#include "axmol/renderer/RenderTexture.h"
#include "axmol/renderer/Renderer.h"
#include "axmol/scene/Camera.h"
#include "axmol/scene/Scene.h"
#include "axmol/base/Director.h"

#if defined(AX_ENABLE_NAVMESH)
#    include "axmol/navmesh/NavMesh.h"
#endif
#if defined(AX_ENABLE_PHYSICS_3D)
#    include "axmol/physics/3d/PhysicsWorld3D.h"
#endif

namespace ax
{

SceneRenderer::SceneRenderer()
{
    _director = Director::getInstance();
}

void SceneRenderer::renderScene(Renderer* renderer, Scene* scene)
{
    AXASSERT(renderer, "Invalid Renderer");
    AXASSERT(scene, "Invalid Scene");

    const auto& transform = scene->getNodeToParentTransform();

    auto& allCameras = scene->getCameras();
    for (const auto& camera : allCameras)
    {
        if (!camera->isVisible())
            continue;

        Camera::setVisitingCamera(camera);

        camera->setAdditionalTransform(Mat4::identity);

        auto targetTexture = camera->getTargetTexture();
        if (targetTexture)
        {
            if (!_renderTexturePass)
                _renderTexturePass =
                    RefPtr<RenderTexturePass>(RenderTexturePass::obtain(targetTexture), tlx::adopt_object);
            _renderTexturePass->setTarget(targetTexture);
            _renderTexturePass->begin(camera);

            camera->apply();

            // Override viewport to render texture dimensions (camera->apply sets it to screen)
            // renderer->setViewport(vp.x, vp.y, vp.width, vp.height);

            camera->clearBackground();
            scene->visit(renderer, transform, 0);
#if defined(AX_ENABLE_NAVMESH)
            if (scene->_navMesh)
                scene->_navMesh->debugDraw(renderer);
#endif

            _renderTexturePass->end();

            renderer->render();
        }
        else
        {
            camera->apply();
            camera->clearBackground();
            scene->visit(renderer, transform, 0);
#if defined(AX_ENABLE_NAVMESH)
            if (scene->_navMesh)
                scene->_navMesh->debugDraw(renderer);
#endif

            renderer->render();
        }
    }

#if defined(AX_ENABLE_PHYSICS_3D) || defined(AX_ENABLE_NAVMESH)
    if (scene->_debugCamera) [[unlikely]]
    {
        Camera::setVisitingCamera(scene->_debugCamera);

        scene->_debugCamera->setAdditionalTransform(Mat4::identity);

        scene->_debugCamera->apply();
        scene->_debugCamera->clearBackground();

#    if defined(AX_ENABLE_NAVMESH)
        if (scene->_navMesh)
            scene->_navMesh->debugDraw(renderer);
#    endif

#    if defined(AX_ENABLE_PHYSICS_3D)
        if (scene->_physicsWorld3D)
            scene->_physicsWorld3D->debugDraw(renderer);
#    endif

        renderer->render();
    }
#endif

    Camera::setVisitingCamera(nullptr);
}

void SceneRenderer::setScissorRect(float x, float y, float w, float h)
{
    _director->getRenderer()->setScissorRect(x, y, w, h);
}

const ScissorRect& SceneRenderer::getScissorRect() const
{
    return _director->getRenderer()->getScissorRect();
}

}  // namespace ax
