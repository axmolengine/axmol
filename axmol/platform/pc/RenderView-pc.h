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
#include "axmol/platform/RenderViewCore.h"
#if AX_ENABLE_VK
#    include "glad/vulkan.h"
#endif
#include "GLFW/glfw3.h"
#if defined(__EMSCRIPTEN__)
#    include "axmol/tlx/vector.hpp"
struct EmscriptenTouchEvent;
struct EmscriptenFullscreenChangeEvent;
struct EmscriptenOrientationChangeEvent;
#endif

namespace ax
{

class GLFWEventHandler;
class AX_DLL RenderView : public RenderViewCore
{
    friend class GLFWEventHandler;

public:
    static RenderView* create(std::string_view viewName);
    static RenderView* create(std::string_view viewName, bool resizable);
    static RenderView* createWithRect(std::string_view viewName,
                                      const Rect& rect,
                                      float zoomFactor = 1.0f,
                                      bool resizable   = false);
    static RenderView* createWithFullscreen(std::string_view viewName);
    static RenderView* createWithFullscreen(std::string_view viewName,
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
    void setIcon(std::span<const std::string_view> filelist) const override;
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

    void* getNativeWindow() const override;
    SurfaceHandle getNativeDisplay() const override;
    WindowPlatform getWindowPlatform() const override;

    void setViewName(std::string_view viewName) override;

    /**
     * @brief Check if a specific key is currently pressed.
     *
     * This function queries the current state of a keyboard key using GLFW.
     * It is typically used for continuous input checks or detecting key
     * combinations (e.g., Ctrl+S, Alt+F4).
     *
     * @param key Keyboard key code, must be one of the GLFW_KEY_XXX constants.
     *            Examples include:
     *            - GLFW_KEY_LEFT_ALT
     *            - GLFW_KEY_RIGHT_ALT
     *            - GLFW_KEY_A, GLFW_KEY_B, ...
     *
     * @return true  if the specified key is currently pressed (GLFW_PRESS).
     * @return false if the specified key is currently released (GLFW_RELEASE).
     *
     * @note The state is associated with the given GLFW window context and
     *       does not represent global system key state.
     *
     * @see glfwGetKey
     */
    bool isKeyPressed(int key) const;

protected:
    RenderView(bool initglfw = true);
    ~RenderView() override;

    bool initWithRect(std::string_view viewName, const Rect& rect, float zoomFactor, bool resizable);
    bool initWithFullScreen(std::string_view viewName);
    bool initWithFullscreen(std::string_view viewname, const GLFWvidmode& videoMode, GLFWmonitor* monitor);

    // GLFW callbacks
    void onGLFWError(int errorID, const char* errorDesc);

#if defined(__EMSCRIPTEN__)
    void onWebOrientationChangeCallback(int eventType, const EmscriptenOrientationChangeEvent* e);
    void onWebFullscreenCallback(int eventType, const EmscriptenFullscreenChangeEvent* e);
#else
    void onGLFWMouseCallBack(GLFWwindow* window, int button, int action, int modify);
    void onGLFWMouseMoveCallBack(GLFWwindow* window, double x, double y);
    void onGLFWMouseScrollCallback(GLFWwindow* window, double x, double y);
#endif
    void onGLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void onGLFWCharCallback(GLFWwindow* window, unsigned int character);
    void onGLFWWindowPosCallback(GLFWwindow* windows, int x, int y);
    void onGLFWFramebufferSizeCallback(GLFWwindow* window, int fbWidth, int fbHeight);
    void onGLFWWindowSizeCallback(GLFWwindow* window, int w, int h);
    void onGLFWWindowIconifyCallback(GLFWwindow* window, int iconified);
    void onGLFWWindowFocusCallback(GLFWwindow* window, int focused);
    void onGLFWWindowCloseCallback(GLFWwindow* window);
    void onGLFWPreeditCallback(GLFWwindow* window,
                               int preedit_count,
                               unsigned int* preedit_string,
                               int block_count,
                               int* block_sizes,
                               int focused_block,
                               int caret);

protected:
    void updateScaledWindowSize(int w, int h, uint8_t updaetFlag);

    /* resize platform window when user set zoomFactor, windowSize */
    void applyWindowSize();

    RenderScaleMode _renderScaleMode{};

    float _windowZoomFactor;

    bool _isTouchDevice{false};  // Whether the current platform supports touch input.

#if !defined(__EMSCRIPTEN__)
    Vec2 _mousePosition;  // Current mouse position, used for synthesizing mouse events when touch events are cancelled.
    uint32_t _pressedButtons{0};
#endif

    std::unordered_map<int, KeyboardEvent::KeyCode> _keyCodeMap;

    GLFWwindow* _mainWindow;
    GLFWmonitor* _monitor;

    void* _vkSurface{nullptr};

    std::string _glfwError;

public:
    // View will trigger an event when window is resized, gains or loses focus
    static const std::string_view EVENT_WINDOW_POSITIONED;
    static const std::string_view EVENT_WINDOW_RESIZED;
    static const std::string_view EVENT_WINDOW_FOCUSED;
    static const std::string_view EVENT_WINDOW_UNFOCUSED;
    static const std::string_view EVENT_WINDOW_CLOSE;
    static const std::string_view EVENT_WINDOW_CURSOR_ENTER;

private:
    void updateRenderScale();
    AX_DISALLOW_COPY_AND_ASSIGN(RenderView);
};

}  // end of namespace ax
