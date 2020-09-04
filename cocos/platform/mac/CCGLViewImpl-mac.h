/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

http://www.cocos2d-x.org

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

#include "base/CCRef.h"
#include "platform/CCCommon.h"
#include "platform/CCGLView.h"
#include "glfw3.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#ifndef GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#ifndef GLFW_EXPOSE_NATIVE_WGL
#define GLFW_EXPOSE_NATIVE_WGL
#endif
#include "glfw3native.h"
#endif /* (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) */

#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
#ifndef GLFW_EXPOSE_NATIVE_NSGL
#define GLFW_EXPOSE_NATIVE_NSGL
#endif
#ifndef GLFW_EXPOSE_NATIVE_COCOA
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include "glfw3native.h"
#endif // #if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)

NS_CC_BEGIN

class GLFWEventHandler;
class CC_DLL GLViewImpl : public GLView
{
    friend class GLFWEventHandler;
public:
    static GLViewImpl* create(const std::string& viewName);
    static GLViewImpl* create(const std::string& viewName, bool resizable);
    static GLViewImpl* createWithRect(const std::string& viewName, Rect size, float frameZoomFactor = 1.0f, bool resizable = false);
    static GLViewImpl* createWithFullScreen(const std::string& viewName);
    static GLViewImpl* createWithFullScreen(const std::string& viewName, const GLFWvidmode &videoMode, GLFWmonitor *monitor);

    /*
     *frameZoomFactor for frame. This method is for debugging big resolution (e.g.new ipad) app on desktop.
     */

    //void resize(int width, int height);

    float getFrameZoomFactor() const override;
    //void centerWindow();

    virtual void setViewPortInPoints(float x , float y , float w , float h) override;
    virtual void setScissorInPoints(float x , float y , float w , float h) override;
    virtual Rect getScissorRect() const override;

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
    void setFullscreen(GLFWmonitor *monitor, int w, int h, int refreshRate);
    void setWindowed(int width, int height);

    int getMonitorCount() const;
    Size getMonitorSize() const;

    /* override functions */
    virtual bool isOpenGLReady() override;
    virtual void end() override;
    virtual void swapBuffers() override;
    virtual void setFrameSize(float width, float height) override;
    virtual void setIMEKeyboardState(bool bOpen) override;

#if CC_ICON_SET_SUPPORT
    virtual void setIcon(const std::string& filename) const override;
    virtual void setIcon(const std::vector<std::string>& filelist) const override;
    virtual void setDefaultIcon() const override;
#endif /* CC_ICON_SET_SUPPORT */

    /*
     * Set zoom factor for frame. This method is for debugging big resolution (e.g.new ipad) app on desktop.
     */
    void setFrameZoomFactor(float zoomFactor) override;
    /**
     * Hide or Show the mouse cursor if there is one.
     */
    virtual void setCursorVisible(bool isVisible) override;
    /** Retina support is disabled by default
     *  @note This method is only available on Mac.
     */
    void enableRetina(bool enabled);
    /** Check whether retina display is enabled. */
    bool isRetinaEnabled() const { return _isRetinaEnabled; };
    
    /** Get retina factor */
    int getRetinaFactor() const override { return _retinaFactor; }
    
    id getCocoaWindow() override { return glfwGetCocoaWindow(_mainWindow); }
    id getNSGLContext() override { return glfwGetNSGLContext(_mainWindow); } // stevetranby: added

protected:
    GLViewImpl(bool initglfw = true);
    virtual ~GLViewImpl();

    bool initWithRect(const std::string& viewName, Rect rect, float frameZoomFactor, bool resizable);
    bool initWithFullScreen(const std::string& viewName);
    bool initWithFullscreen(const std::string& viewname, const GLFWvidmode &videoMode, GLFWmonitor *monitor);
    /* update frame layout when enter/exit full screen mode */
    void updateWindowSize();

    void updateFrameSize();

    // GLFW callbacks
    void onGLFWError(int errorID, const char* errorDesc);
    void onGLFWMouseCallBack(GLFWwindow* window, int button, int action, int modify);
    void onGLFWMouseMoveCallBack(GLFWwindow* window, double x, double y);
    void onGLFWMouseScrollCallback(GLFWwindow* window, double x, double y);
    void onGLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void onGLFWCharCallback(GLFWwindow* window, unsigned int character);
    void onGLFWWindowPosCallback(GLFWwindow* windows, int x, int y);
    void onGLFWWindowSizeCallback(GLFWwindow *window, int width, int height);
    void onGLFWWindowIconifyCallback(GLFWwindow* window, int iconified);
    void onGLFWWindowFocusCallback(GLFWwindow* window, int focused);

    bool _captured;
    bool _supportTouch;
    bool _isInRetinaMonitor;
    bool _isRetinaEnabled;
    int  _retinaFactor;  // Should be 1 or 2

    float _frameZoomFactor;

    GLFWwindow* _mainWindow;
    GLFWmonitor* _monitor;

    std::string _glfwError;

    float _mouseX;
    float _mouseY;

public:
    // View will trigger an event when window is resized, gains or loses focus
    static const std::string EVENT_WINDOW_RESIZED;
    static const std::string EVENT_WINDOW_FOCUSED;
    static const std::string EVENT_WINDOW_UNFOCUSED;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(GLViewImpl);
};

NS_CC_END   // end of namespace   cocos2d
