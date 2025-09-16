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
// Implement RenderView based on GLFW for targets: win32,osx,web(wasm)
#pragma once
#include "axmol/platform/GL.h"
#include "axmol/base/Object.h"
#include "axmol/platform/Common.h"
#include "axmol/platform/RenderView.h"
#include "GLFW/glfw3.h"
#if defined(__EMSCRIPTEN__)
#    include "axmol/tlx/pod_vector.hpp"
struct EmscriptenMouseEvent;
struct EmscriptenTouchEvent;
struct EmscriptenFullscreenChangeEvent;
#endif

namespace ax
{

class GLFWEventHandler;
class AX_DLL RenderViewImpl : public RenderView
{
    friend class GLFWEventHandler;

public:
    static RenderViewImpl* create(std::string_view viewName);
    static RenderViewImpl* create(std::string_view viewName, bool resizable);
    static RenderViewImpl* createWithRect(std::string_view viewName,
                                          const Rect& rect,
                                          float zoomFactor = 1.0f,
                                          bool resizable   = false);
    static RenderViewImpl* createWithFullscreen(std::string_view viewName);
    static RenderViewImpl* createWithFullscreen(std::string_view viewName,
                                                const GLFWvidmode& videoMode,
                                                GLFWmonitor* monitor);

    float getWindowZoomFactor() const override;
    // void centerWindow();

    bool windowShouldClose() override;
    void pollEvents() override;
    GLFWwindow* getWindow() const { return _mainWindow; }

    bool isFullscreen() const;

    /* Sets primary monitor full screen with default w*h(refresh rate) */
    void setFullscreen();
    /* Sets primary monitor full screen with w*h(refresh rate) */
    void setFullscreen(int w, int h, int refreshRate);

    /* Sets monitor full screen with default w*h(refresh rate) */
    void setFullscreen(int monitorIndex);
    /// <summary>
    /// Sets monitor full screen with w*h(refresh rate)
    /// </summary>
    /// <param name="monitorIndex">the 0 based index of monitor</param>
    /// <param name="w">the width of hardware resolution in full screen, -1 use default value</param>
    /// <param name="h">the height of hardware resolution in full screen, -1 use default value</param>
    /// <param name="refreshRate">the display refresh rate, usually 60, -1 use default value</param>
    void setFullscreen(int monitorIndex, int w, int h, int refreshRate);

    /* for internal use */
    void setFullscreen(GLFWmonitor* monitor, int w, int h, int refreshRate);
    void setWindowed(int width, int height, bool borderless = false);

    Vec2 getNativeWindowSize() const override;

    void getWindowPosition(int* xpos, int* ypos);

    void setWindowSizeLimits(int minwidth,
                             int minheight,
                             int maxwidth  = GLFW_DONT_CARE,
                             int maxheight = GLFW_DONT_CARE);

    int getMonitorCount() const;
    Vec2 getMonitorSize() const;

    /* override functions */
    bool isGfxContextReady() override;
    void end() override;
    void swapBuffers() override;
    void setWindowSize(float width, float height) override;
    void setIMEKeyboardState(bool bOpen) override;

#if AX_ICON_SET_SUPPORT
    void setIcon(std::string_view filename) const override;
    void setIcon(const std::vector<std::string_view>& filelist) const override;
    void setDefaultIcon() const override;
#endif /* AX_ICON_SET_SUPPORT */

    /*
     * Set zoom factor for window. This method is for debugging big resolution (e.g.new ipad) app on desktop.
     */
    void setWindowZoomFactor(float zoomFactor) override;
    /**
     * Hide or Show the mouse cursor if there is one.
     */
    void setCursorVisible(bool isVisible) override;

    /** Get render scale */
    float getRenderScale() const override { return _renderScale; }

    void* getNativeWindow() const override;
    void* getNativeDisplay() const override;
    WindowPlatform getWindowPlatform() const override;

protected:
    RenderViewImpl(bool initglfw = true);
    ~RenderViewImpl() override;

    bool initWithRect(std::string_view viewName, const Rect& rect, float zoomFactor, bool resizable);
    bool initWithFullScreen(std::string_view viewName);
    bool initWithFullscreen(std::string_view viewname, const GLFWvidmode& videoMode, GLFWmonitor* monitor);
#if (AX_TARGET_PLATFORM != AX_PLATFORM_MAC)  // Windows, Linux: use glad to loadGL
    bool loadGL();
#endif

    // GLFW callbacks
    void onGLFWError(int errorID, const char* errorDesc);
    void onGLFWMouseCallBack(GLFWwindow* window, int button, int action, int modify);
    void onGLFWMouseMoveCallBack(GLFWwindow* window, double x, double y);
#if defined(__EMSCRIPTEN__)
    void onWebFullscreenCallback(int eventType, const EmscriptenFullscreenChangeEvent* e);
    void onWebTouchCallback(int eventType, const EmscriptenTouchEvent* touchEvent);
    void onWebClickCallback();
#endif
    void onGLFWMouseScrollCallback(GLFWwindow* window, double x, double y);
    void onGLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void onGLFWCharCallback(GLFWwindow* window, unsigned int character);
    void onGLFWWindowPosCallback(GLFWwindow* windows, int x, int y);
    void onGLFWFramebufferSizeCallback(GLFWwindow* window, int fbWidth, int fbHeight);
    void onGLFWWindowSizeCallback(GLFWwindow* window, int w, int h);
    void onGLFWWindowIconifyCallback(GLFWwindow* window, int iconified);
    void onGLFWWindowFocusCallback(GLFWwindow* window, int focused);
    void onGLFWWindowCloseCallback(GLFWwindow* window);

protected:
    void updateScaledWindowSize(int w, int h);

    /* resize platform window when user set zoomFactor, windowSize */
    void applyWindowSize();

    void maybeDispatchResizeEvent(uint8_t updateFlag);

    bool _isTouchDevice = false;
    bool _captured;

    enum WindowUpdateFlag : uint8_t
    {
        WindowSizeChanged      = 1 << 0,
        FramebufferSizeChanged = 1 << 1,
        AllUpdates             = WindowSizeChanged | FramebufferSizeChanged
    };

    // Flags indicating whether the window or framebuffer size was updated.
    // On desktop platforms, callback order is: framebufferSize => windowSize.
    // On WebAssembly, the order is reversed: windowSize => framebufferSize.
    uint8_t _windowUpdateFlags{0};

    RenderScaleMode _renderScaleMode{};

    // Render scale factor:
    // - Used to convert physical window size to logical size.
    // - Also applied as the input scaling factor on platforms where
    //   screen coordinates do not map 1:1 to physical pixels.
    float _renderScale{1.0f};

    // Input scale factor:
    // - Always 1.0 on platforms with a 1:1 mapping between screen coordinates
    //   and physical pixels.
    // - On other platforms, matches _renderScale to account for DPI scaling.
    float _inputScale{1.0f};

    float _windowZoomFactor;

    GLFWwindow* _mainWindow;
    GLFWmonitor* _monitor;

    std::string _glfwError;

#if defined(__EMSCRIPTEN__)
    axstd::pod_vector<intptr_t> _touchesId;
    axstd::pod_vector<float> _touchesX;
    axstd::pod_vector<float> _touchesY;
#endif

    float _mouseX;
    float _mouseY;

public:
    // View will trigger an event when window is resized, gains or loses focus
    static const std::string EVENT_WINDOW_POSITIONED;
    static const std::string EVENT_WINDOW_RESIZED;
    static const std::string EVENT_WINDOW_FOCUSED;
    static const std::string EVENT_WINDOW_UNFOCUSED;
    static const std::string EVENT_WINDOW_CLOSE;

private:
    void updateRenderScale();
    AX_DISALLOW_COPY_AND_ASSIGN(RenderViewImpl);
};

}  // end of namespace ax
