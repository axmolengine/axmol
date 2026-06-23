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

#include <optional>
#include <memory>
#include <vector>

#include "axmol/platform/PlatformMacros.h"
#include "axmol/base/Object.h"
#include "axmol/base/Types.h"
#include "axmol/math/Mat4.h"
#include "axmol/rhi/RHITypes.h"

namespace ax
{

class Renderer;
class Camera;
class RenderTexture;

/**
 * @brief Manages an offscreen render pass into a RenderTexture.
 *
 * RenderTexturePass is a lightweight helper that brackets a group of renderer
 * commands between begin() and end(). Commands submitted while the pass is active
 * are recorded into a dedicated renderer group. When that group is executed, the
 * pass saves the current render target and viewport, binds the RenderTexture's
 * render target, optionally applies a custom viewport, and restores the previous
 * state at the end of the group.
 *
 * This class is intentionally not a render target resource. It does not own the
 * RenderTexture, does not traverse a scene by itself, and does not flush the
 * renderer automatically. Callers are responsible for submitting draw commands
 * between begin() and end(), then flushing the renderer at an appropriate time.
 *
 * @section Command ordering
 *
 * Axmol's renderer does not execute commands strictly in submission order. Render
 * queues are sorted by type and global order; in particular, 3D commands may be
 * processed before regular GLOBALZ_ZERO commands. For this reason, the internal
 * begin callback must be scheduled with an early global order, and the internal
 * end callback must be scheduled with a late global order. This guarantees that
 * all commands submitted between begin() and end(), including 3D, 2D, debug draw,
 * and custom commands, are executed while the RenderTexture is bound.
 *
 * The clear() helpers are also scheduled after the render target has been bound
 * and before normal draw commands, so they clear the intended RenderTexture rather
 * than the previously active render target.
 *
 * @section Y-axis consistency across backends
 *
 * On OpenGL, offscreen FBO textures have a bottom-left origin, while D3D, Metal,
 * and Vulkan use a top-left origin. When automatic Y flipping is enabled, this
 * pass can flip the visiting camera's projection matrix on OpenGL so that the
 * rendered texture has a consistent orientation across backends.
 *
 * @note This guarantee applies only to rendering managed by RenderTexturePass.
 *       If you access RenderTexture::getRenderTarget() directly and manage the
 *       rendering pipeline yourself, you must handle backend-specific Y-axis
 *       orientation yourself.
 *
 * @section Usage
 *
 * @code
 * auto rt = RenderTexture::create(w, h);
 * auto pass = RefPtr<RenderTexturePass>(RenderTexturePass::obtain(rt), tlx::adopt_object);
 *
 * pass->begin(camera);
 * pass->clear(ClearFlag::COLOR | ClearFlag::DEPTH_AND_STENCIL,
 *             {.color = Color(0, 0, 0, 1)});
 * // Submit draw commands here.
 * pass->end();
 *
 * renderer->render();
 * @endcode
 *
 * Use a separate RenderTexturePass object for nested render target passes.
 */
class AX_DLL RenderTexturePass : public Object
{
public:
    /**
     * @brief Obtain a new render texture pass instance (ref count 1, does not enter autorelease pool).
     * @param rt Optional render texture to assign. If null, call setTarget() before begin().
     * @return A RenderTexturePass* with reference count 1. Call release() when done,
     *         or wrap in RefPtr<T>(ptr, tlx::adopt_object) for automatic management.
     */
    static RenderTexturePass* obtain(RenderTexture* rt = nullptr);

    /// @name Target
    /** @{ Set render target. Must be called before begin(), not while active. */
    void setTarget(RenderTexture* rt);
    /** @} */

#ifndef _AX_GEN_SCRIPT_BINDINGS
    /// @name Viewport
    /** @{ Set custom viewport. Must be called before begin(). */
    void setViewport(std::optional<Viewport> vp);
#endif

    /**
     * @brief Computes a virtual viewport rectangle for rendering into a RenderTexture.
     *
     * This function maps a sub-rectangle of the full render target (`fullRect`) into
     * the coordinate space of the specified full viewport (`fullViewport`), relative
     * to the RenderTexture begin offset (`rtBegin`). It produces a `Viewport` object
     * that can be used for scissor or rendering operations inside the RenderTexture.
     *
     * @param rtBegin       The origin offset (x, y) of the RenderTexture region
     *                      where rendering begins.
     * @param fullRect      The full rectangle of the RenderTexture in local coordinates.
     * @param fullViewport  The full viewport rectangle in local coordinates.
     *
     * @return A `Viewport` structure containing the computed x/y position and width/height
     *         of the virtual viewport. If any input rectangle has non-positive dimensions,
     *         an empty `Viewport` is returned.
     *
     * @note The calculation scales the rectangle coordinates by the ratio of
     *       `fullViewport.size` to `fullRect.size`, ensuring that the resulting
     *       viewport aligns correctly with the RenderTexture’s coordinate system.
     */
    Viewport makeVirtualViewport(const Vec2& rtBegin, const Rect& fullRect, const Rect& fullViewport) const;
    /** @} */

    /// @name Lifecycle
    /** @{ */
    /**
     * @brief Begin offscreen rendering.
     * @param camera Optional visiting camera override.
     * Saves current render target/viewport, pushes render group.
     */
    void begin(const Camera* camera = nullptr);

    /**
     * @brief End offscreen rendering.
     * Restores previous render target/viewport and visiting camera.
     */
    void end();

    bool isActive() const { return _active; }
    /** @} */

    /// @name Clear (must be called between begin() / end())
    /** @{ */
    /**
     * @brief Clear the active render target.
     * @param flags  Bitmask of ClearFlag (COLOR|DEPTH|STENCIL).
     * @param value  Clear parameters (color, depth, stencil).
     * @pre Must be called between begin() and end().
     */
    void clear(ClearFlag flags, const ClearValue& value = ClearValue{});

    /** @brief Clear color buffer only. @pre begin() .. end() */
    void clearColor(const Color& color) { clear(ClearFlag::COLOR, {.color = color}); }

    /** @brief Clear depth buffer only. @pre begin() .. end() */
    void clearDepth(float depth) { clear(ClearFlag::DEPTH, {.depth = depth}); }

    /** @brief Clear stencil buffer only. @pre begin() .. end() */
    void clearStencil(unsigned int stencil) { clear(ClearFlag::STENCIL, {.stencil = stencil}); }

    /** @brief Clear all buffers (color|depth|stencil). @pre begin() .. end() */
    void clearAll(const ClearValue& value = ClearValue{})
    {
        clear(ClearFlag::COLOR | ClearFlag::DEPTH | ClearFlag::STENCIL, value);
    }

    /** @} */
    /**
     * @brief Enables or disables automatic visiting camera override for this pass.
     *
     * When enabled, begin() temporarily sets the selected camera as the visiting
     * camera and applies backend-specific projection adjustments when needed.
     * end() restores the previous visiting camera and projection state.
     *
     * Advanced renderers that manage their own camera loop, such as VR or custom
     * multi-view renderers, can disable this behavior and handle camera state
     * explicitly.
     *
     * @param enabled True to let RenderTexturePass override the visiting camera;
     *                false to leave camera state untouched.
     */
    void setCameraOverrideEnabled(bool enabled) { _cameraOverrideEnabled = enabled; }

    /**
     * @brief Returns whether this pass automatically overrides the visiting camera.
     */
    bool isCameraOverrideEnabled() const { return _cameraOverrideEnabled; }

private:
    RenderTexturePass();
    RenderTexturePass(RenderTexture* renderTexture);
    virtual ~RenderTexturePass() override;

    struct SavedState
    {
        rhi::RenderTarget* renderTarget = nullptr;
        std::optional<Viewport> viewport;
    };

    void updateViewport();

    Renderer* _renderer{nullptr};
    RenderTexture* _renderTexture{nullptr};  // weak ref
    bool _active{false};
    bool _autoFlipY{false};

    // When enabled, begin() temporarily overrides the visiting camera and applies
    // backend-specific projection adjustments when needed. end() restores the
    // previous camera state. Advanced renderers can disable this and manage camera
    // state explicitly.
    bool _cameraOverrideEnabled{true};

    std::optional<Viewport> _viewport;

    const Camera* _savedCamera{nullptr};
    std::optional<Mat4> _savedProjection;
    std::vector<SavedState> _savedStates;

    AX_DISALLOW_COPY_AND_ASSIGN(RenderTexturePass);
};

}  // namespace ax
