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
 * @brief Engine-side scene composition policy.
 *
 * SceneCompositor is the Director-owned strategy for frame-bound scene work.
 * The base implementation performs frame preparation through the active RenderView,
 * then iterates all cameras in the scene, applies their transforms, and issues
 * draw commands to the Renderer.
 *
 * Subclassing:
 * - Override `renderScene` to inject custom composition logic.
 *   For example, VR compositors can render each eye into a dedicated render
 *   target while still relying on the base implementation for per-camera
 *   traversal.
 * - Override `onRenderViewChanged` to recreate framebuffer-sized resources
 *   when the render view is created or resized.
 * - Override `setScissorRect` / `getScissorRect` to apply custom
 *   viewport-to-raster transforms (e.g., per-eye scissor in VR).
 *
 * @note The base `setScissorRect` / `getScissorRect` delegate to
 *       `Renderer::setScissorRect` / `getScissorRect` directly.
 *
 * @since v3.0
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
     * Called before scheduler update.
     * Base implementation forwards to the active RenderView::pollEvents() if available,
     * otherwise performs no operation.
     *
     * Custom compositors can override this to poll runtime events, wait for frame timing,
     * or dispatch input before game logic runs.
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
    virtual void onRenderViewChanged(RenderViewCore* rv) {}

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
    RefPtr<RenderTexturePass> _renderTexturePass;
};

}  // namespace ax
