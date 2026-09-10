/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/scene/SceneCompositor.h"

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

SceneCompositor::SceneCompositor()
{
    _director = Director::getInstance();
}

void SceneCompositor::onRenderViewChanged(RenderViewCore* rv)
{
    _renderView = rv;
}

void SceneCompositor::pollEvents()
{
    AXASSERT(_renderView, "SceneCompositor is not bound to a RenderViewCore");
    _renderView->pollNativeEvents();
}

void SceneCompositor::renderScene(Renderer* renderer, Scene* scene)
{
    AXASSERT(renderer, "Invalid Renderer");
    AXASSERT(scene, "Invalid Scene");

    const auto& transform = scene->getNodeToParentTransform();

    auto& allCameras = scene->getCameras();
    for (const auto& camera : allCameras)
    {
        if (!camera->isVisible())
            continue;

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
            SceneRenderState renderState(renderer, camera);

            // Override viewport to render texture dimensions (camera->apply sets it to screen)
            // renderer->setViewport(vp.x, vp.y, vp.width, vp.height);

            camera->clearBackground(renderState);
            scene->visit(renderState, transform, 0);
#if defined(AX_ENABLE_NAVMESH)
            if (scene->_navMesh)
                scene->_navMesh->debugDraw(renderState);
#endif

            _renderTexturePass->end();

            renderer->render();
        }
        else
        {
            camera->apply();
            SceneRenderState renderState(renderer, camera);
            camera->clearBackground(renderState);
            scene->visit(renderState, transform, 0);
#if defined(AX_ENABLE_NAVMESH)
            if (scene->_navMesh)
                scene->_navMesh->debugDraw(renderState);
#endif

            renderer->render();
        }
    }

#if defined(AX_ENABLE_PHYSICS_3D) || defined(AX_ENABLE_NAVMESH)
    if (scene->_debugCamera) [[unlikely]]
    {
        scene->_debugCamera->setAdditionalTransform(Mat4::identity);
        SceneRenderState debugRenderState(renderer, scene->_debugCamera);

        scene->_debugCamera->apply();
        scene->_debugCamera->clearBackground(debugRenderState);

#    if defined(AX_ENABLE_NAVMESH)
        if (scene->_navMesh)
            scene->_navMesh->debugDraw(debugRenderState);
#    endif

#    if defined(AX_ENABLE_PHYSICS_3D)
        if (scene->_physicsWorld3D)
            scene->_physicsWorld3D->debugDraw(debugRenderState);
#    endif

        renderer->render();
    }
#endif
}

void SceneCompositor::setScissorRect(float x, float y, float w, float h)
{
    _director->getRenderer()->setScissorRect(x, y, w, h);
}

const ScissorRect& SceneCompositor::getScissorRect() const
{
    return _director->getRenderer()->getScissorRect();
}

}  // namespace ax
