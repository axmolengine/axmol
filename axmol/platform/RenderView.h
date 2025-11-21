/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
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

#pragma once

#include "axmol/base/Types.h"
#include "axmol/base/EventTouch.h"
#include "axmol/vr/VRBase.h"

#include <functional>
#include <vector>
#include <memory>

#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32)
#    include <windows.h>
#endif /* (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32) */

#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32) || (AX_TARGET_PLATFORM == AX_PLATFORM_LINUX)
#    define AX_ICON_SET_SUPPORT true
#endif /* (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32) || (AX_TARGET_PLATFORM == AX_PLATFORM_LINUX) */

namespace ax
{
class Scene;
class Renderer;
class Director;

/** There are some Resolution Policy for Adapt to the screen. */
enum class ResolutionPolicy
{
    /** The entire application is visible in the specified area without trying to preserve the original aspect ratio.
     * Distortion can occur, and the application may appear stretched or compressed.
     */
    EXACT_FIT,
    /** The entire application fills the specified area, without distortion but possibly with some cropping,
     * while maintaining the original aspect ratio of the application.
     */
    NO_BORDER,
    /** The entire application is visible in the specified area without distortion while maintaining the original
     * aspect ratio of the application. Borders can appear on two sides of the application.
     */
    SHOW_ALL,
    /** The application takes the height of the design resolution size and modifies the width of the internal
     * canvas so that it fits the aspect ratio of the device.
     * No distortion will occur however you must make sure your application works on different
     * aspect ratios.
     */
    FIXED_HEIGHT,
    /** The application takes the width of the design resolution size and modifies the height of the internal
     * canvas so that it fits the aspect ratio of the device.
     * No distortion will occur however you must make sure your application works on different
     * aspect ratios.
     */
    FIXED_WIDTH,

    UNKNOWN,
};

/**
 * @addtogroup platform
 * @{
 */

enum class WindowPlatform
{
    Unknown,     // Unknown or unsupported platform
    Win32,       // Windows desktop applications using HWND
    CoreWindow,  // UWP or Xbox applications using CoreWindow/AppWindow
    Cocoa,       // macOS applications using NSWindow
    X11,         // Linux applications using the X11 window system
    Wayland,     // Linux applications using the Wayland protocol
    UIKit,       // iOS/tvOS applications using UIView/UIWindow
    Android,     // Android applications using SurfaceView or native window
    Web          // WebAssembly applications using HTML canvas or DOM
};

/**
 * @brief By RenderView you can operate the frame information of EGL view through some function.
 */
class AX_DLL RenderView : public Object
{
    friend class Director;

public:
    enum SurfaceUpdateFlag : uint8_t
    {
        WindowSizeChanged = 1,       // Indicates that window size has changed
        RenderSizeChanged = 1 << 1,  // Indicates that render surface size has changed
        SilentUpdate      = 1 << 2,  // Temporary flag: suppresses event dispatch for this update only.
                                     // Should be stripped before accumulating into persistent state.
        AllUpdates         = WindowSizeChanged | RenderSizeChanged,
        AllUpdatesSilently = AllUpdates | SilentUpdate
    };

    /**
     */
    RenderView();
    /**
     * @lua NA
     */
    virtual ~RenderView();

    /** Force destroying EGL view, subclass must implement this method.
     *
     * @lua endToLua
     */
    virtual void end() = 0;

    /** Get whether graphics context is ready, subclass must implement this method. */
    virtual bool isGfxContextReady() = 0;

    /** Exchanges the front and back buffers, subclass must implement this method. */
    virtual void swapBuffers() = 0;

    /** Open or close IME keyboard , subclass must implement this method.
     *
     * @param open Open or close IME keyboard.
     */
    virtual void setIMEKeyboardState(bool open) = 0;

    /** When the window is closed, it will return false if the platforms is Ios or Android.
     * If the platforms is windows or Mac,it will return true.
     *
     * @return In ios and android it will return false,if in windows or Mac it will return true.
     */
    virtual bool windowShouldClose() { return false; };

    /** Polls the events. */
    virtual void pollEvents();

    virtual Vec2 getNativeWindowSize() const { return getWindowSize(); }

    /**
     * Get the zoomed window size
     * In general, it returns the screen size since the EGL view is a fullscreen view.
     *
     * @return The window size (aka logic size)
     */
    const Vec2& getWindowSize() const { return _windowSize; }

    /**
     * Set the zoomed window size
     *
     * @param width The width of the fram size.
     * @param height The height of the fram size.
     */
    virtual void setWindowSize(float, float) {}

    /** Set zoom factor for frame. This methods are for
     * debugging big resolution (e.g.new ipad) app on desktop.
     *
     * @param zoomFactor The zoom factor for frame.
     */
    virtual void setWindowZoomFactor(float /*zoomFactor*/) {}

    /** Get zoom factor for frame. This methods are for
     * debugging big resolution (e.g.new ipad) app on desktop.
     *
     * @return The zoom factor for frame.
     */
    virtual float getWindowZoomFactor() const { return 1.0; }

    const Vec2& getRenderSize() const { return _renderSize; }

#ifndef _AX_GEN_SCRIPT_BINDINGS
    /**
     * implicit deprecated APIs, use getWindowSize instead
     */
    AX_DEPRECATED(3.0) const Vec2& getFrameSize() const { return getWindowSize(); }
    AX_DEPRECATED(3.0) void setFrameSize(float width, float height) { setWindowSize(width, height); }
    AX_DEPRECATED(3.0) float getFrameZoomFactor() const { return getWindowZoomFactor(); }
    AX_DEPRECATED(3.0) void setFrameZoomFactor(float zoomFactor) { setWindowZoomFactor(zoomFactor); }
#endif

    /**
     * Hide or Show the mouse cursor if there is one.
     *
     * @param isVisible Hide or Show the mouse cursor if there is one.
     */
    virtual void setCursorVisible(bool /*isVisible*/) {}

    /** Get render scale.
     *
     * @return The render scale fbSize/windowSize aka backing scale factor
     */
    virtual float getRenderScale() const { return 1.0f; }

    /**
     * Get the visible area size of render viewport.
     *
     * @return The visible area size of render viewport.
     */
    virtual Vec2 getVisibleSize() const;

    /**
     * Get the visible origin point of render viewport.
     *
     * @return The visible origin point of render viewport.
     */
    virtual Vec2 getVisibleOrigin() const;

    /**
     * Get the visible rectangle of render viewport.
     *
     * @return The visible rectangle of render viewport.
     */
    virtual Rect getVisibleRect() const;

    /**
     * Gets safe area rectangle
     */
    virtual Rect getSafeAreaRect() const;

    /**
     * Set the design resolution size.
     * @param width Design resolution width.
     * @param height Design resolution height.
     * @param resolutionPolicy The resolution policy desired, you may choose:
     *                         [1] EXACT_FIT Fill screen by stretch-to-fit: if the design resolution ratio of width to
     * height is different from the screen resolution ratio, your game view will be stretched. [2] NO_BORDER Full screen
     * without black border: if the design resolution ratio of width to height is different from the screen resolution
     * ratio, two areas of your game view will be cut. [3] SHOW_ALL  Full screen with black border: if the design
     * resolution ratio of width to height is different from the screen resolution ratio, two black borders will be
     * shown.
     * @remark For applications with a static design resolution, this method should typically be called only once during
     * initialization.
     */
    virtual void setDesignResolutionSize(float width, float height, ResolutionPolicy resolutionPolicy);

    /** Get design resolution size.
     *  Default resolution size is the same as 'getWindowSize'.
     *
     * @return The design resolution size.
     */
    virtual const Vec2& getDesignResolutionSize() const;

    /**
     * Set render view port rectangle with points.
     *
     * @param x Set the points of x.
     * @param y Set the points of y.
     * @param w Set the width of  the view port
     * @param h Set the Height of the view port.
     */
    virtual void setViewportInPoints(float x, float y, float w, float h);

    /**
     * Set Scissor rectangle with points.
     *
     * @param x Set the points of x.
     * @param y Set the points of y.
     * @param w Set the width of  the view port
     * @param h Set the Height of the view port.
     */
    virtual void setScissorInPoints(float x, float y, float w, float h);

    /**
     * Get whether GL_SCISSOR_TEST is enable.
     *
     * @return Whether GL_SCISSOR_TEST is enable.
     */
    virtual bool isScissorEnabled();

    /**
     * Get the current scissor rectangle.
     *
     * @return The current scissor rectangle.
     */
    virtual Rect getScissorInPoints() const;

    /*
     * Sets the view name. will change the window title on desktop platform
     * @param viewname A string will be set to the view as name.
     */
    virtual void setViewName(std::string_view viewname);

    /** Get the view name.
     *
     * @return The view name.
     */
    std::string_view getViewName() const;

    /** Touch events are handled by default; if you want to customize your handlers, please override this function.
     *
     * @param num The number of touch.
     * @param ids The identity of the touch.
     * @param xs The points of x.
     * @param ys The points of y.
     */
    virtual void handleTouchesBegin(int num, intptr_t ids[], float xs[], float ys[]);

    /** Touch events are handled by default; if you want to customize your handlers, please override this function.
     *
     * @param num The number of touch.
     * @param ids The identity of the touch.
     * @param xs The points of x.
     * @param ys The points of y.
     */
    virtual void handleTouchesMove(int num, intptr_t ids[], float xs[], float ys[]);

    /** Touch events are handled by default; if you want to customize your handlers, please override this function.
     *
     * @param num The number of touch.
     * @param ids The identity of the touch.
     * @param xs The points of x.
     * @param ys The points of y.
     * @param fs The force of 3d touches.
     # @param ms The maximum force of 3d touches
     */
    virtual void handleTouchesMove(int num, intptr_t ids[], float xs[], float ys[], float fs[], float ms[]);

    /** Touch events are handled by default; if you want to customize your handlers, please override this function.
     *
     * @param num The number of touch.
     * @param ids The identity of the touch.
     * @param xs The points of x.
     * @param ys The points of y.
     */
    virtual void handleTouchesEnd(int num, intptr_t ids[], float xs[], float ys[]);

    /** Touch events are handled by default; if you want to customize your handlers, please override this function.
     *
     * @param num The number of touch.
     * @param ids The identity of the touch.
     * @param xs The points of x.
     * @param ys The points of y.
     */
    virtual void handleTouchesCancel(int num, intptr_t ids[], float xs[], float ys[]);

    /** Set window icon (implemented for windows and linux).
     *
     * @param filename A path to image file, e.g., "icons/cusom.png".
     */
    virtual void setIcon(std::string_view filename) const {};

    /** Set window icon (implemented for windows and linux).
     * Best icon (based on size) will be auto selected.
     *
     * @param filelist The array contains icons.
     */
    virtual void setIcon(const std::vector<std::string_view>& filelist) const {};

    /** Set default window icon (implemented for windows and linux).
     * On windows it will use icon from .exe file (if included).
     * On linux it will use default window icon.
     */
    virtual void setDefaultIcon() const {};

    /**
     * Get the render view port rectangle.
     *
     * @return Return the render view port rectangle.
     */
    const Rect& getViewportRect() const;

    /**
     * Get list of all active touches.
     *
     * @return A list of all active touches.
     */
    std::vector<Touch*> getAllTouches() const;

    /**
     * Get scale factor of the horizontal direction.
     *
     * @return Scale factor of the horizontal direction.
     */
    float getScaleX() const;

    /**
     * Get scale factor of the vertical direction.
     *
     * @return Scale factor of the vertical direction.
     */
    float getScaleY() const;

    /** Returns the current Resolution policy.
     *
     * @return The current Resolution policy.
     */
    ResolutionPolicy getResolutionPolicy() const { return _resolutionPolicy; }

    /**
     * @brief Get the Native Window object
     *
     * @return void*
     *    win32: HWND
     *    winrt: PresentTarget*
     *    linux: x11 or wayland window
     *    macOS: NSWindow*
     *    iOS: UIWindow*
     */
    virtual void* getNativeWindow() const { return nullptr; }

    /**
     * @brief Get the Native Display object
     *
     * @return void*
     *   linux: x11 or wayland display
     *   macOS: NSGLContext*
     *   iOS/tvOS: EARenderView*
     */
    virtual void* getNativeDisplay() const { return nullptr; }

    /**
     * @brief Get the Window Platform object
     *
     * @return WindowPlatform
     */
    virtual WindowPlatform getWindowPlatform() const { return WindowPlatform::Unknown; };

    /**
     * Renders a Scene with a Renderer
     * This method is called directly by the Director
     */
    void renderScene(Scene* scene, Renderer* renderer);

    /**
     * Enable or disable interactions.
     * When disabled, it prevents touches to be dispatched and will cancel current touches
     */
    void setInteractive(bool interactive);
#ifdef AX_ENABLE_VR
    void setVR(std::unique_ptr<IVRRenderer>&& impl);
    const std::unique_ptr<IVRRenderer>& getVR() const { return _vrRenderer; }
#endif

    /**
     * @brief Updates the render surface size (framebuffer/render target) and synchronizes related view parameters.
     *
     * This method performs the following actions:
     * - Sets `_renderSize` to the specified dimensions;
     * - On mobile platforms (Android/iOS), `_windowSize` is synchronized to match `_renderSize`;
     * - On desktop platforms, `_windowSize` is only initialized to `_renderSize` if it hasn't been set yet;
     * - If `_designResolutionSize` is unset (`Vec2::ZERO`), it is initialized to `_renderSize`;
     * - Calls `updateDesignResolution()` to recalculate `_viewScale` and `_viewportRect` based on the current
     *   `ResolutionPolicy`, and updates the Director's canvas size and projection matrix accordingly.
     *
     * @param width      The target width of the render surface. Its meaning depends on `updateFlag`:
     *                   it may represent the framebuffer width, logical window width, or design resolution width.
     * @param height     The target height of the render surface. Its meaning depends on `updateFlag`:
     *                   it may represent the framebuffer height, logical window height, or design resolution height.
     * @param updateFlag Optional flags to control which parts of the view should be updated.
     *                   Defaults to `SurfaceUpdateFlag::AllUpdates`.
     *
     * @warning This method may initialize `_windowSize` and `_designResolutionSize` on first invocation.
     * @note No update will occur if the given size is (0, 0).
     *
     * @internal This method is intended for internal use by platform-specific window or surface managers.
     *           It should be called when the native surface size changes (e.g., orientation change, resize event).
     *
     * @see updateDesignResolution(), setDesignResolutionSize()
     */
    [[internal]] void updateRenderSurface(float width, float height, uint8_t updateFlag);

protected:
    float transformInputX(float x) { return (x - _viewportRect.origin.x) / _viewScale.x; }
    float transformInputY(float y) { return (y - _viewportRect.origin.y) / _viewScale.y; }

    void maybeDispatchResizeEvent(uint8_t updateFlag);

    /**
     * @brief Callback invoked after the RenderView size has changed and all related updates are complete.
     *
     * This method is called once the RenderView's dimensions have been updated and
     * all dependent states (such as viewport, scaling, and layout) are ready.
     * It serves as a notification hook for any components that need to respond
     * to the final, settled render size.
     */
    void onSurfaceResized();

    void setScissorRect(float x, float y, float w, float h);
    const ScissorRect& getScissorRect() const;

    /**
     * queue a priority operation in render thread for non-PC platforms, even through app in background
     * invoked by Director
     */
    virtual void queueOperation(AsyncOperation op, void* param = nullptr);

    void updateDesignResolution();

    void handleTouchesOfEndOrCancel(EventTouch::EventCode eventCode, int num, intptr_t ids[], float xs[], float ys[]);

    Vec2 _renderSize;
    // The window size aka logic size, may scaled by windowZoomFactor in high DPI display
    Vec2 _windowSize;
    // resolution size, it is the size appropriate for the app resources.
    Vec2 _designResolutionSize;
    // the view port size
    Rect _viewportRect;
    // the view name
    std::string _viewName;

    Vec2 _viewScale;
    ResolutionPolicy _resolutionPolicy;

    // Flags indicating whether the window or framebuffer size was updated.
    // On desktop platforms, callback order is: framebufferSize => windowSize.
    // On WebAssembly, the order is reversed: windowSize => framebufferSize.
    uint8_t _surfaceUpdateFlags{0};

    bool _isResolutionUpdateLocked{false};

#ifdef AX_ENABLE_VR
    std::unique_ptr<IVRRenderer> _vrRenderer{nullptr};
#endif

private:
    void cancelAllTouches();

    bool _interactive;
};

// end of platform group
/// @}

}  // namespace ax
