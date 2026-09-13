/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/math/Mat4.h"
#include "axmol/platform/PlatformMacros.h"
#include "axmol/base/RefPtr.h"
#include "axmol/base/Types.h"
#include "axmol/renderer/RenderTexturePass.h"

namespace ax
{

class Renderer;
class Scene;
class RenderViewCore;
class Director;

/**
 * @brief RenderView-owned scene composition policy.
 *
 * SceneCompositor is the RenderViewCore-owned strategy that defines how
 * the current scene is composed into the active render surface.
 *
 * The base implementation iterates all visible cameras, applies their
 * transforms, and submits draw commands to the Renderer. Specialized
 * compositors, such as VRSceneCompositor, may render the scene into
 * custom per-view or per-eye render targets before presenting it.
 */
class AX_DLL SceneCompositor
{
public:
    SceneCompositor();
    virtual ~SceneCompositor() = default;

    /** Returns whether this compositor is currently presenting a VR scene.
     *  Base returns false
     */
    virtual bool isVRActive() const { return false; }

    /**
     * Called each frame before scheduler update to poll compositor-owned events.
     *
     * The base implementation polls native window/platform events by forwarding
     * to RenderViewCore::pollNativeEvents(). Specialized compositors may extend
     * this to poll runtime-specific events, such as OpenXR frame events.
     */
    virtual void pollEvents();

    /** Composes the scene for the current frame.
     *  @param renderer The Renderer to submit draw commands into.
     *  @param scene    The scene graph to traverse and compose.
     */
    virtual void renderScene(Renderer* renderer, Scene* scene);

    /** Called when this compositor is bound to a RenderViewCore or when the
     *  render surface changes.
     *
     *  The base implementation stores the RenderViewCore weak reference.
     *  Subclasses may override to recreate framebuffer-sized or runtime resources.
     */
    virtual void onRenderViewChanged(RenderViewCore* rv);

    /** Applies a scissor rect, optionally transformed by the active raster transform.
     *  Base implementation delegates to Renderer::setScissorRect.
     */
    virtual void setScissorRect(float x, float y, float w, float h);
    /** Returns the last scissor rect set through this compositor.
     *  Base implementation delegates to Renderer::getScissorRect.
     */
    virtual const ScissorRect& getScissorRect() const;

protected:
    Director* _director{nullptr};          // weak-ref
    RenderViewCore* _renderView{nullptr};  // weak-ref
    RefPtr<RenderTexturePass> _renderTexturePass;
};

}  // namespace ax
