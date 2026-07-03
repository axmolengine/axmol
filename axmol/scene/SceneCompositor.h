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
     * Called after scheduler update, prior to prepareFrame().
     * Responsible for polling platform/window events and runtime events
     * (e.g. glfwPollEvents, OpenXR xrPollEvent + xrWaitFrame).
     *
     * This is separated from prepareFrame() so that event polling always occurs
     * at a stable point in the frame loop, regardless of which compositor is active,
     * allowing safe compositor switches without mid-frame teardown hazards.
     */
    virtual void pollEvents();

    /**
     * Called after pollFrameEvents(), before renderScene().
     * Responsible for compositor-specific per-frame state setup
     * (e.g. syncing scale factors, pointer ray cameras).
     *
     * Base implementation is a no-op.
     */
    virtual void prepareFrame();

    /** Composes the scene for the current frame.
     *  @param renderer The Renderer to submit draw commands into.
     *  @param scene    The scene graph to traverse and compose.
     */
    virtual void renderScene(Renderer* renderer, Scene* scene);

    /** Called when the render view size changes (or is first assigned).
     *  Override to recreate framebuffer-sized resources (e.g., VR render texture).
     *  Default is a no-op.
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
    Director* _director{nullptr};  // weak-ref
    RenderViewCore* _renderView{nullptr};  // weak-ref
    RefPtr<RenderTexturePass> _renderTexturePass;
};

}  // namespace ax
