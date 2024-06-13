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

#include "platform/GLViewImpl.h"

#include <cmath>
#include <unordered_map>

#include "platform/Application.h"
#include "base/Director.h"
#include "base/Touch.h"
#include "base/EventDispatcher.h"
#include "base/EventKeyboard.h"
#include "base/EventMouse.h"
#include "base/IMEDispatcher.h"
#include "base/Utils.h"
#include "base/UTF8.h"
#include "2d/Camera.h"
#if AX_ICON_SET_SUPPORT
#    include "platform/Image.h"
#endif /* AX_ICON_SET_SUPPORT */

#include "renderer/Renderer.h"

#if defined(AX_USE_METAL)
#    include <Metal/Metal.h>
#    include "renderer/backend/metal/DriverMTL.h"
#    include "renderer/backend/metal/UtilsMTL.h"
#else
#    include "renderer/backend/opengl/DriverGL.h"
#    include "renderer/backend/opengl/MacrosGL.h"
#    include "renderer/backend/opengl/OpenGLState.h"
#endif  // #if (AX_TARGET_PLATFORM == AX_PLATFORM_MAC)

/** glfw3native.h */
#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32)
#    ifndef GLFW_EXPOSE_NATIVE_WIN32
#        define GLFW_EXPOSE_NATIVE_WIN32
#    endif
#    ifndef GLFW_EXPOSE_NATIVE_WGL
#        define GLFW_EXPOSE_NATIVE_WGL
#    endif
#endif /* (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32) */

#if (AX_TARGET_PLATFORM == AX_PLATFORM_MAC)
#    ifndef GLFW_EXPOSE_NATIVE_NSGL
#        define GLFW_EXPOSE_NATIVE_NSGL
#    endif
#    ifndef GLFW_EXPOSE_NATIVE_COCOA
#        define GLFW_EXPOSE_NATIVE_COCOA
#    endif
#endif  // #if (AX_TARGET_PLATFORM == AX_PLATFORM_MAC)

#if (AX_TARGET_PLATFORM == AX_PLATFORM_LINUX)
#    ifndef GLFW_EXPOSE_NATIVE_X11
#        define GLFW_EXPOSE_NATIVE_X11
#    endif
#    ifndef GLFW_EXPOSE_NATIVE_WAYLAND
#        define GLFW_EXPOSE_NATIVE_WAYLAND
#    endif
#endif  // #if (AX_TARGET_PLATFORM == AX_PLATFORM_LINUX)

#if (AX_TARGET_PLATFORM != AX_PLATFORM_WASM)
#    include <GLFW/glfw3native.h>
#endif

#if defined(__EMSCRIPTEN__)
#    include <emscripten/html5.h>
#endif

NS_AX_BEGIN

class GLFWEventHandler
{
public:
    static void onGLFWError(int errorID, const char* errorDesc)
    {
        if (_view)
            _view->onGLFWError(errorID, errorDesc);
    }

    static void onGLFWMouseCallBack(GLFWwindow* window, int button, int action, int modify)
    {
        if (_view)
            _view->onGLFWMouseCallBack(window, button, action, modify);
    }

    static void onGLFWMouseMoveCallBack(GLFWwindow* window, double x, double y)
    {
        if (_view)
            _view->onGLFWMouseMoveCallBack(window, x, y);
    }
#if defined(__EMSCRIPTEN__)
    static int onWebTouchCallback(int eventType, const EmscriptenTouchEvent* touchEvent, void* /*userData*/)
    {
        if (_view)
            _view->onWebTouchCallback(eventType, touchEvent);
        return 0;
    }
#endif

    static void onGLFWMouseScrollCallback(GLFWwindow* window, double x, double y)
    {
        if (_view)
            _view->onGLFWMouseScrollCallback(window, x, y);
    }

    static void onGLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (_view)
            _view->onGLFWKeyCallback(window, key, scancode, action, mods);
    }

    static void onGLFWCharCallback(GLFWwindow* window, unsigned int character)
    {
        if (_view)
            _view->onGLFWCharCallback(window, character);
    }

    static void onGLFWWindowPosCallback(GLFWwindow* windows, int x, int y)
    {
        if (_view)
            _view->onGLFWWindowPosCallback(windows, x, y);
    }

    // Notes: Unused on windows or macos Metal renderer backend
    // static void onGLFWframebufferSize(GLFWwindow* window, int w, int h)
    // {
    //     if (_view)
    //         _view->onGLFWframebufferSize(window, w, h);
    // }

    static void onGLFWWindowSizeCallback(GLFWwindow* window, int width, int height)
    {
        if (_view)
            _view->onGLFWWindowSizeCallback(window, width, height);
    }

    static void setGLViewImpl(GLViewImpl* view) { _view = view; }

    static void onGLFWWindowIconifyCallback(GLFWwindow* window, int iconified)
    {
        if (_view)
        {
            _view->onGLFWWindowIconifyCallback(window, iconified);
        }
    }

    static void onGLFWWindowFocusCallback(GLFWwindow* window, int focused)
    {
        if (_view)
        {
            _view->onGLFWWindowFocusCallback(window, focused);
        }
    }

    static void onGLFWWindowCloseCallback(GLFWwindow* window)
    {
        if (_view)
        {
            _view->onGLFWWindowCloseCallback(window);
        }
    }

private:
    static GLViewImpl* _view;
};
GLViewImpl* GLFWEventHandler::_view = nullptr;

const std::string GLViewImpl::EVENT_WINDOW_POSITIONED = "glview_window_positioned";
const std::string GLViewImpl::EVENT_WINDOW_RESIZED    = "glview_window_resized";
const std::string GLViewImpl::EVENT_WINDOW_FOCUSED    = "glview_window_focused";
const std::string GLViewImpl::EVENT_WINDOW_UNFOCUSED  = "glview_window_unfocused";
const std::string GLViewImpl::EVENT_WINDOW_CLOSE      = "glview_window_close";

////////////////////////////////////////////////////

struct keyCodeItem
{
    int glfwKeyCode;
    EventKeyboard::KeyCode keyCode;
};

static std::unordered_map<int, EventKeyboard::KeyCode> g_keyCodeMap;

static keyCodeItem g_keyCodeStructArray[] = {
    /* The unknown key */
    {GLFW_KEY_UNKNOWN, EventKeyboard::KeyCode::KEY_NONE},

    /* Printable keys */
    {GLFW_KEY_SPACE, EventKeyboard::KeyCode::KEY_SPACE},
    {GLFW_KEY_APOSTROPHE, EventKeyboard::KeyCode::KEY_APOSTROPHE},
    {GLFW_KEY_COMMA, EventKeyboard::KeyCode::KEY_COMMA},
    {GLFW_KEY_MINUS, EventKeyboard::KeyCode::KEY_MINUS},
    {GLFW_KEY_PERIOD, EventKeyboard::KeyCode::KEY_PERIOD},
    {GLFW_KEY_SLASH, EventKeyboard::KeyCode::KEY_SLASH},
    {GLFW_KEY_0, EventKeyboard::KeyCode::KEY_0},
    {GLFW_KEY_1, EventKeyboard::KeyCode::KEY_1},
    {GLFW_KEY_2, EventKeyboard::KeyCode::KEY_2},
    {GLFW_KEY_3, EventKeyboard::KeyCode::KEY_3},
    {GLFW_KEY_4, EventKeyboard::KeyCode::KEY_4},
    {GLFW_KEY_5, EventKeyboard::KeyCode::KEY_5},
    {GLFW_KEY_6, EventKeyboard::KeyCode::KEY_6},
    {GLFW_KEY_7, EventKeyboard::KeyCode::KEY_7},
    {GLFW_KEY_8, EventKeyboard::KeyCode::KEY_8},
    {GLFW_KEY_9, EventKeyboard::KeyCode::KEY_9},
    {GLFW_KEY_SEMICOLON, EventKeyboard::KeyCode::KEY_SEMICOLON},
    {GLFW_KEY_EQUAL, EventKeyboard::KeyCode::KEY_EQUAL},
    {GLFW_KEY_A, EventKeyboard::KeyCode::KEY_A},
    {GLFW_KEY_B, EventKeyboard::KeyCode::KEY_B},
    {GLFW_KEY_C, EventKeyboard::KeyCode::KEY_C},
    {GLFW_KEY_D, EventKeyboard::KeyCode::KEY_D},
    {GLFW_KEY_E, EventKeyboard::KeyCode::KEY_E},
    {GLFW_KEY_F, EventKeyboard::KeyCode::KEY_F},
    {GLFW_KEY_G, EventKeyboard::KeyCode::KEY_G},
    {GLFW_KEY_H, EventKeyboard::KeyCode::KEY_H},
    {GLFW_KEY_I, EventKeyboard::KeyCode::KEY_I},
    {GLFW_KEY_J, EventKeyboard::KeyCode::KEY_J},
    {GLFW_KEY_K, EventKeyboard::KeyCode::KEY_K},
    {GLFW_KEY_L, EventKeyboard::KeyCode::KEY_L},
    {GLFW_KEY_M, EventKeyboard::KeyCode::KEY_M},
    {GLFW_KEY_N, EventKeyboard::KeyCode::KEY_N},
    {GLFW_KEY_O, EventKeyboard::KeyCode::KEY_O},
    {GLFW_KEY_P, EventKeyboard::KeyCode::KEY_P},
    {GLFW_KEY_Q, EventKeyboard::KeyCode::KEY_Q},
    {GLFW_KEY_R, EventKeyboard::KeyCode::KEY_R},
    {GLFW_KEY_S, EventKeyboard::KeyCode::KEY_S},
    {GLFW_KEY_T, EventKeyboard::KeyCode::KEY_T},
    {GLFW_KEY_U, EventKeyboard::KeyCode::KEY_U},
    {GLFW_KEY_V, EventKeyboard::KeyCode::KEY_V},
    {GLFW_KEY_W, EventKeyboard::KeyCode::KEY_W},
    {GLFW_KEY_X, EventKeyboard::KeyCode::KEY_X},
    {GLFW_KEY_Y, EventKeyboard::KeyCode::KEY_Y},
    {GLFW_KEY_Z, EventKeyboard::KeyCode::KEY_Z},
    {GLFW_KEY_LEFT_BRACKET, EventKeyboard::KeyCode::KEY_LEFT_BRACKET},
    {GLFW_KEY_BACKSLASH, EventKeyboard::KeyCode::KEY_BACK_SLASH},
    {GLFW_KEY_RIGHT_BRACKET, EventKeyboard::KeyCode::KEY_RIGHT_BRACKET},
    {GLFW_KEY_GRAVE_ACCENT, EventKeyboard::KeyCode::KEY_GRAVE},
    {GLFW_KEY_WORLD_1, EventKeyboard::KeyCode::KEY_GRAVE},
    {GLFW_KEY_WORLD_2, EventKeyboard::KeyCode::KEY_NONE},

    /* Function keys */
    {GLFW_KEY_ESCAPE, EventKeyboard::KeyCode::KEY_ESCAPE},
    {GLFW_KEY_ENTER, EventKeyboard::KeyCode::KEY_ENTER},
    {GLFW_KEY_TAB, EventKeyboard::KeyCode::KEY_TAB},
    {GLFW_KEY_BACKSPACE, EventKeyboard::KeyCode::KEY_BACKSPACE},
    {GLFW_KEY_INSERT, EventKeyboard::KeyCode::KEY_INSERT},
    {GLFW_KEY_DELETE, EventKeyboard::KeyCode::KEY_DELETE},
    {GLFW_KEY_RIGHT, EventKeyboard::KeyCode::KEY_RIGHT_ARROW},
    {GLFW_KEY_LEFT, EventKeyboard::KeyCode::KEY_LEFT_ARROW},
    {GLFW_KEY_DOWN, EventKeyboard::KeyCode::KEY_DOWN_ARROW},
    {GLFW_KEY_UP, EventKeyboard::KeyCode::KEY_UP_ARROW},
    {GLFW_KEY_PAGE_UP, EventKeyboard::KeyCode::KEY_PG_UP},
    {GLFW_KEY_PAGE_DOWN, EventKeyboard::KeyCode::KEY_PG_DOWN},
    {GLFW_KEY_HOME, EventKeyboard::KeyCode::KEY_HOME},
    {GLFW_KEY_END, EventKeyboard::KeyCode::KEY_END},
    {GLFW_KEY_CAPS_LOCK, EventKeyboard::KeyCode::KEY_CAPS_LOCK},
    {GLFW_KEY_SCROLL_LOCK, EventKeyboard::KeyCode::KEY_SCROLL_LOCK},
    {GLFW_KEY_NUM_LOCK, EventKeyboard::KeyCode::KEY_NUM_LOCK},
    {GLFW_KEY_PRINT_SCREEN, EventKeyboard::KeyCode::KEY_PRINT},
    {GLFW_KEY_PAUSE, EventKeyboard::KeyCode::KEY_PAUSE},
    {GLFW_KEY_F1, EventKeyboard::KeyCode::KEY_F1},
    {GLFW_KEY_F2, EventKeyboard::KeyCode::KEY_F2},
    {GLFW_KEY_F3, EventKeyboard::KeyCode::KEY_F3},
    {GLFW_KEY_F4, EventKeyboard::KeyCode::KEY_F4},
    {GLFW_KEY_F5, EventKeyboard::KeyCode::KEY_F5},
    {GLFW_KEY_F6, EventKeyboard::KeyCode::KEY_F6},
    {GLFW_KEY_F7, EventKeyboard::KeyCode::KEY_F7},
    {GLFW_KEY_F8, EventKeyboard::KeyCode::KEY_F8},
    {GLFW_KEY_F9, EventKeyboard::KeyCode::KEY_F9},
    {GLFW_KEY_F10, EventKeyboard::KeyCode::KEY_F10},
    {GLFW_KEY_F11, EventKeyboard::KeyCode::KEY_F11},
    {GLFW_KEY_F12, EventKeyboard::KeyCode::KEY_F12},
    {GLFW_KEY_F13, EventKeyboard::KeyCode::KEY_NONE},
    {GLFW_KEY_F14, EventKeyboard::KeyCode::KEY_NONE},
    {GLFW_KEY_F15, EventKeyboard::KeyCode::KEY_NONE},
    {GLFW_KEY_F16, EventKeyboard::KeyCode::KEY_NONE},
    {GLFW_KEY_F17, EventKeyboard::KeyCode::KEY_NONE},
    {GLFW_KEY_F18, EventKeyboard::KeyCode::KEY_NONE},
    {GLFW_KEY_F19, EventKeyboard::KeyCode::KEY_NONE},
    {GLFW_KEY_F20, EventKeyboard::KeyCode::KEY_NONE},
    {GLFW_KEY_F21, EventKeyboard::KeyCode::KEY_NONE},
    {GLFW_KEY_F22, EventKeyboard::KeyCode::KEY_NONE},
    {GLFW_KEY_F23, EventKeyboard::KeyCode::KEY_NONE},
    {GLFW_KEY_F24, EventKeyboard::KeyCode::KEY_NONE},
    {GLFW_KEY_F25, EventKeyboard::KeyCode::KEY_NONE},
    {GLFW_KEY_KP_0, EventKeyboard::KeyCode::KEY_0},
    {GLFW_KEY_KP_1, EventKeyboard::KeyCode::KEY_1},
    {GLFW_KEY_KP_2, EventKeyboard::KeyCode::KEY_2},
    {GLFW_KEY_KP_3, EventKeyboard::KeyCode::KEY_3},
    {GLFW_KEY_KP_4, EventKeyboard::KeyCode::KEY_4},
    {GLFW_KEY_KP_5, EventKeyboard::KeyCode::KEY_5},
    {GLFW_KEY_KP_6, EventKeyboard::KeyCode::KEY_6},
    {GLFW_KEY_KP_7, EventKeyboard::KeyCode::KEY_7},
    {GLFW_KEY_KP_8, EventKeyboard::KeyCode::KEY_8},
    {GLFW_KEY_KP_9, EventKeyboard::KeyCode::KEY_9},
    {GLFW_KEY_KP_DECIMAL, EventKeyboard::KeyCode::KEY_PERIOD},
    {GLFW_KEY_KP_DIVIDE, EventKeyboard::KeyCode::KEY_KP_DIVIDE},
    {GLFW_KEY_KP_MULTIPLY, EventKeyboard::KeyCode::KEY_KP_MULTIPLY},
    {GLFW_KEY_KP_SUBTRACT, EventKeyboard::KeyCode::KEY_KP_MINUS},
    {GLFW_KEY_KP_ADD, EventKeyboard::KeyCode::KEY_KP_PLUS},
    {GLFW_KEY_KP_ENTER, EventKeyboard::KeyCode::KEY_KP_ENTER},
    {GLFW_KEY_KP_EQUAL, EventKeyboard::KeyCode::KEY_EQUAL},
    {GLFW_KEY_LEFT_SHIFT, EventKeyboard::KeyCode::KEY_LEFT_SHIFT},
    {GLFW_KEY_LEFT_CONTROL, EventKeyboard::KeyCode::KEY_LEFT_CTRL},
    {GLFW_KEY_LEFT_ALT, EventKeyboard::KeyCode::KEY_LEFT_ALT},
    {GLFW_KEY_LEFT_SUPER, EventKeyboard::KeyCode::KEY_HYPER},
    {GLFW_KEY_RIGHT_SHIFT, EventKeyboard::KeyCode::KEY_RIGHT_SHIFT},
    {GLFW_KEY_RIGHT_CONTROL, EventKeyboard::KeyCode::KEY_RIGHT_CTRL},
    {GLFW_KEY_RIGHT_ALT, EventKeyboard::KeyCode::KEY_RIGHT_ALT},
    {GLFW_KEY_RIGHT_SUPER, EventKeyboard::KeyCode::KEY_HYPER},
    {GLFW_KEY_MENU, EventKeyboard::KeyCode::KEY_MENU},
    {GLFW_KEY_LAST, EventKeyboard::KeyCode::KEY_NONE}};

//////////////////////////////////////////////////////////////////////////
// implement GLViewImpl
//////////////////////////////////////////////////////////////////////////

GLViewImpl::GLViewImpl(bool initglfw)
    : _captured(false)
    , _isInRetinaMonitor(false)
    , _isRetinaEnabled(false)
    , _retinaFactor(1)
    , _frameZoomFactor(1.0f)
    , _mainWindow(nullptr)
    , _monitor(nullptr)
    , _mouseX(0.0f)
    , _mouseY(0.0f)
{
    _viewName = "axmol2";
    g_keyCodeMap.clear();
    for (auto&& item : g_keyCodeStructArray)
    {
        g_keyCodeMap[item.glfwKeyCode] = item.keyCode;
    }

    GLFWEventHandler::setGLViewImpl(this);
    if (initglfw)
    {
        glfwSetErrorCallback(GLFWEventHandler::onGLFWError);
        glfwInit();
    }
}

GLViewImpl::~GLViewImpl()
{
    AXLOGI("deallocing GLViewImpl: {}", fmt::ptr(this));
    GLFWEventHandler::setGLViewImpl(nullptr);
    glfwTerminate();
}

#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32)
HWND GLViewImpl::getWin32Window()
{
    return glfwGetWin32Window(_mainWindow);
}
#endif /* (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32) */

#if (AX_TARGET_PLATFORM == AX_PLATFORM_MAC)
void* GLViewImpl::getCocoaWindow()
{
    return (void*)glfwGetCocoaWindow(_mainWindow);
}
void* GLViewImpl::getNSGLContext()
{
    return (void*)glfwGetNSGLContext(_mainWindow);
}  // stevetranby: added
#endif  // #if (AX_TARGET_PLATFORM == AX_PLATFORM_MAC)

#if (AX_TARGET_PLATFORM == AX_PLATFORM_LINUX)
void* GLViewImpl::getX11Window()
{
    return (void*)glfwGetX11Window(_mainWindow);
}
void* GLViewImpl::getX11Display()
{
    return (void*)glfwGetX11Display();
}
/* TODO: Implement AX_PLATFORM_LINUX_WAYLAND
void* GLViewImpl::getWaylandWindow()
{
    return (void*)glfwGetWaylandWindow(_mainWindow);
}
void* GLViewImpl::getWaylandDisplay()
{
    return (void*)glfwGetWaylandDisplay();
}
*/
#endif  // #if (AX_TARGET_PLATFORM == AX_PLATFORM_LINUX)

GLViewImpl* GLViewImpl::create(std::string_view viewName)
{
    return GLViewImpl::create(viewName, false);
}

GLViewImpl* GLViewImpl::create(std::string_view viewName, bool resizable)
{
    auto ret = new GLViewImpl;
    if (ret->initWithRect(viewName, ax::Rect(0, 0, 960, 640), 1.0f, resizable))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

GLViewImpl* GLViewImpl::createWithRect(std::string_view viewName,
                                       const ax::Rect& rect,
                                       float frameZoomFactor,
                                       bool resizable)
{
    auto ret = new GLViewImpl;
    if (ret->initWithRect(viewName, rect, frameZoomFactor, resizable))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

GLViewImpl* GLViewImpl::createWithFullScreen(std::string_view viewName)
{
    auto ret = new GLViewImpl();
    if (ret->initWithFullScreen(viewName))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

GLViewImpl* GLViewImpl::createWithFullScreen(std::string_view viewName,
                                             const GLFWvidmode& videoMode,
                                             GLFWmonitor* monitor)
{
    auto ret = new GLViewImpl();
    if (ret->initWithFullscreen(viewName, videoMode, monitor))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

bool GLViewImpl::initWithRect(std::string_view viewName, const ax::Rect& rect, float frameZoomFactor, bool resizable)
{
    setViewName(viewName);

    _frameZoomFactor = frameZoomFactor;

    Vec2 windowSize = rect.size * frameZoomFactor;

#if AX_GLES_PROFILE
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, AX_GLES_PROFILE / AX_GLES_PROFILE_DEN);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#elif defined(AX_USE_GL)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // We don't want the old OpenGL
#endif

    glfwWindowHint(GLFW_RESIZABLE, resizable ? GL_TRUE : GL_FALSE);
    glfwWindowHint(GLFW_RED_BITS, _glContextAttrs.redBits);
    glfwWindowHint(GLFW_GREEN_BITS, _glContextAttrs.greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, _glContextAttrs.blueBits);
    glfwWindowHint(GLFW_ALPHA_BITS, _glContextAttrs.alphaBits);
    glfwWindowHint(GLFW_DEPTH_BITS, _glContextAttrs.depthBits);
    glfwWindowHint(GLFW_STENCIL_BITS, _glContextAttrs.stencilBits);

    glfwWindowHint(GLFW_SAMPLES, _glContextAttrs.multisamplingCount);

    glfwWindowHint(GLFW_VISIBLE, _glContextAttrs.visible);
    glfwWindowHint(GLFW_DECORATED, _glContextAttrs.decorated);

#if defined(AX_USE_METAL)
    // Don't create gl context.
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif

#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32)
    glfwWindowHintPointer(GLFW_WIN32_HWND_PARENT, _glContextAttrs.viewParent);
#endif

    _mainWindow = glfwCreateWindow(static_cast<int>(windowSize.width), static_cast<int>(windowSize.height),
                                   _viewName.c_str(), _monitor, nullptr);

    if (_mainWindow == nullptr)
    {
        std::string message = "Can't create window";
        if (!_glfwError.empty())
        {
            message.append("\nMore info: \n");
            message.append(_glfwError);
        }

        messageBox(message.c_str(), "Error launch application");
        utils::killCurrentProcess();  // kill current process, don't cause crash when driver issue.
        return false;
    }

    int actualWidth, actualHeight;
    glfwGetWindowSize(_mainWindow, &actualWidth, &actualHeight);
    if (static_cast<int>(windowSize.width) != actualWidth || static_cast<int>(windowSize.height) != actualHeight)
    {
        windowSize.set(static_cast<float>(actualWidth), static_cast<float>(actualHeight));
    }

#if defined(AX_USE_METAL)
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(_mainWindow, &fbWidth, &fbHeight);

    CGSize size;
    size.width  = static_cast<CGFloat>(fbWidth);
    size.height = static_cast<CGFloat>(fbHeight);
    // Initialize device.
    id<MTLDevice> device = MTLCreateSystemDefaultDevice();
    if (!device)
    {
        AXLOG("Doesn't support metal.");
        return false;
    }

    NSView* contentView = [(id)getCocoaWindow() contentView];
    [contentView setWantsLayer:YES];
    CAMetalLayer* layer = [CAMetalLayer layer];
    [layer setDevice:device];
    [layer setPixelFormat:MTLPixelFormatBGRA8Unorm];
    [layer setFramebufferOnly:YES];
    [layer setDrawableSize:size];
    layer.displaySyncEnabled = _glContextAttrs.vsync;
    [contentView setLayer:layer];
    backend::DriverMTL::setCAMetalLayer(layer);
#endif

#if defined(AX_USE_GL)
    glfwMakeContextCurrent(_mainWindow);
    glfwSetWindowUserPointer(_mainWindow, backend::__gl);
#endif
    /*
     *  Note that the created window and context may differ from what you requested,
     *  as not all parameters and hints are
     *  [hard constraints](@ref window_hints_hard).  This includes the size of the
     *  window, especially for full screen windows.  To retrieve the actual
     *  attributes of the created window and context, use queries like @ref
     *  glfwGetWindowAttrib and @ref glfwGetWindowSize.
     *
     *  see declaration glfwCreateWindow
     */
#if !defined(__APPLE__)
    handleWindowSize(static_cast<int>(windowSize.width), static_cast<int>(windowSize.height));
#else
    // sense retina
    setFrameSize(rect.size.width, rect.size.height);
#endif

    glfwSetMouseButtonCallback(_mainWindow, GLFWEventHandler::onGLFWMouseCallBack);
    glfwSetCursorPosCallback(_mainWindow, GLFWEventHandler::onGLFWMouseMoveCallBack);
#if defined(__EMSCRIPTEN__)
    // clang-format off
    _isTouchDevice = !!EM_ASM_INT(return (('ontouchstart' in window) ||
        (navigator.maxTouchPoints > 0) ||
        (navigator.msMaxTouchPoints > 0)) ? 1 : 0;
    );
    if (_isTouchDevice)
    {
        emscripten_set_touchstart_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, GLFWEventHandler::onWebTouchCallback);
        emscripten_set_touchend_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, GLFWEventHandler::onWebTouchCallback);
        emscripten_set_touchmove_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, GLFWEventHandler::onWebTouchCallback);
        emscripten_set_touchcancel_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 1, GLFWEventHandler::onWebTouchCallback);
    }
    // clang-format on
#endif

    glfwSetScrollCallback(_mainWindow, GLFWEventHandler::onGLFWMouseScrollCallback);
    glfwSetCharCallback(_mainWindow, GLFWEventHandler::onGLFWCharCallback);
    glfwSetKeyCallback(_mainWindow, GLFWEventHandler::onGLFWKeyCallback);
    glfwSetWindowPosCallback(_mainWindow, GLFWEventHandler::onGLFWWindowPosCallback);
    glfwSetWindowSizeCallback(_mainWindow, GLFWEventHandler::onGLFWWindowSizeCallback);
    glfwSetWindowIconifyCallback(_mainWindow, GLFWEventHandler::onGLFWWindowIconifyCallback);
    glfwSetWindowFocusCallback(_mainWindow, GLFWEventHandler::onGLFWWindowFocusCallback);
    glfwSetWindowCloseCallback(_mainWindow, GLFWEventHandler::onGLFWWindowCloseCallback);

#if (AX_TARGET_PLATFORM != AX_PLATFORM_MAC)
    loadGL();

    // Init driver after load GL
    backend::DriverBase::getInstance();
#endif

#if defined(AX_USE_GL)
#    if !defined(__EMSCRIPTEN__)
    glfwSwapInterval(_glContextAttrs.vsync ? 1 : 0);
#    endif
    // Will cause OpenGL error 0x0500 when use ANGLE-GLES on desktop
#    if !AX_GLES_PROFILE
    // Enable point size by default.
#        if defined(GL_VERSION_2_0)
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
#        else
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);
#        endif
    if (_glContextAttrs.multisamplingCount > 0)
        glEnable(GL_MULTISAMPLE);
#    endif
    CHECK_GL_ERROR_DEBUG();
#endif
    //    // GLFW v3.2 no longer emits "onGLFWWindowSizeFunCallback" at creation time. Force default viewport:
    //    setViewPortInPoints(0, 0, neededWidth, neededHeight);
    //
    return true;
}

bool GLViewImpl::initWithFullScreen(std::string_view viewName)
{
    // Create fullscreen window on primary monitor at its current video mode.
    _monitor = glfwGetPrimaryMonitor();
    if (nullptr == _monitor)
        return false;

    const GLFWvidmode* videoMode = glfwGetVideoMode(_monitor);

    // These are soft constraints. If the video mode is retrieved at runtime, the resulting window and context should
    // match these exactly. If invalid attribs are passed (eg. from an outdated cache), window creation will NOT fail
    // but the actual window/context may differ.
    glfwWindowHint(GLFW_REFRESH_RATE, videoMode->refreshRate);
    glfwWindowHint(GLFW_RED_BITS, videoMode->redBits);
    glfwWindowHint(GLFW_BLUE_BITS, videoMode->blueBits);
    glfwWindowHint(GLFW_GREEN_BITS, videoMode->greenBits);

    return initWithRect(viewName, ax::Rect(0, 0, (float)videoMode->width, (float)videoMode->height), 1.0f, false);
}

bool GLViewImpl::initWithFullscreen(std::string_view viewname, const GLFWvidmode& videoMode, GLFWmonitor* monitor)
{
    // Create fullscreen on specified monitor at the specified video mode.
    _monitor = monitor;
    if (nullptr == _monitor)
        return false;

    // These are soft constraints. If the video mode is retrieved at runtime, the resulting window and context should
    // match these exactly. If invalid attribs are passed (eg. from an outdated cache), window creation will NOT fail
    // but the actual window/context may differ.
    glfwWindowHint(GLFW_REFRESH_RATE, videoMode.refreshRate);
    glfwWindowHint(GLFW_RED_BITS, videoMode.redBits);
    glfwWindowHint(GLFW_BLUE_BITS, videoMode.blueBits);
    glfwWindowHint(GLFW_GREEN_BITS, videoMode.greenBits);

    return initWithRect(viewname, ax::Rect(0, 0, (float)videoMode.width, (float)videoMode.height), 1.0f, false);
}

bool GLViewImpl::isOpenGLReady()
{
    return nullptr != _mainWindow;
}

void GLViewImpl::end()
{
    if (_mainWindow)
    {
        glfwSetWindowShouldClose(_mainWindow, 1);
        _mainWindow = nullptr;
    }
    // Release self. Otherwise, GLViewImpl could not be freed.
    release();
}

void GLViewImpl::swapBuffers()
{
#if defined(AX_USE_GL)
    if (_mainWindow)
        glfwSwapBuffers(_mainWindow);
#endif
}

bool GLViewImpl::windowShouldClose()
{
    if (_mainWindow)
        return glfwWindowShouldClose(_mainWindow) ? true : false;
    else
        return true;
}

void GLViewImpl::pollEvents()
{
    glfwPollEvents();
}

void GLViewImpl::enableRetina(bool enabled)
{
#if (AX_TARGET_PLATFORM == AX_PLATFORM_MAC)
    _isRetinaEnabled = enabled;
    updateFrameSize();
#endif
}

void GLViewImpl::setIMEKeyboardState(bool /*bOpen*/) {}

#if AX_ICON_SET_SUPPORT
void GLViewImpl::setIcon(std::string_view filename) const
{
    this->setIcon(std::vector<std::string_view>{filename});
}

void GLViewImpl::setIcon(const std::vector<std::string_view>& filelist) const
{
    if (filelist.empty())
        return;
    std::vector<Image*> icons;
    for (auto const& filename : filelist)
    {
        Image* icon = new Image();
        if (icon->initWithImageFile(filename))
        {
            icons.emplace_back(icon);
        }
        else
        {
            AX_SAFE_DELETE(icon);
        }
    }

    if (icons.empty())
        return;  // No valid images
    size_t iconsCount = icons.size();
    auto images       = new GLFWimage[iconsCount];
    for (size_t i = 0; i < iconsCount; i++)
    {
        auto& image  = images[i];
        auto& icon   = icons[i];
        image.width  = icon->getWidth();
        image.height = icon->getHeight();
        image.pixels = icon->getData();
    };

    GLFWwindow* window = this->getWindow();
    glfwSetWindowIcon(window, iconsCount, images);

    AX_SAFE_DELETE_ARRAY(images);
    for (auto&& icon : icons)
    {
        AX_SAFE_DELETE(icon);
    }
}

void GLViewImpl::setDefaultIcon() const
{
    GLFWwindow* window = this->getWindow();
    glfwSetWindowIcon(window, 0, nullptr);
}
#endif /* AX_ICON_SET_SUPPORT */

void GLViewImpl::setCursorVisible(bool isVisible)
{
    if (_mainWindow == NULL)
        return;

    if (isVisible)
        glfwSetInputMode(_mainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    else
        glfwSetInputMode(_mainWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void GLViewImpl::setFrameZoomFactor(float zoomFactor)
{
    AXASSERT(zoomFactor > 0.0f, "zoomFactor must be larger than 0");

    if (std::abs(_frameZoomFactor - zoomFactor) < FLT_EPSILON)
    {
        return;
    }

    _frameZoomFactor = zoomFactor;
    updateFrameSize();
}

float GLViewImpl::getFrameZoomFactor() const
{
    return _frameZoomFactor;
}

bool GLViewImpl::isFullscreen() const
{
    return (_monitor != nullptr);
}

void GLViewImpl::setFullscreen()
{
    setFullscreen(-1, -1, -1);
}

void GLViewImpl::setFullscreen(int w, int h, int refreshRate)
{
    auto monitor = glfwGetPrimaryMonitor();
    if (nullptr == monitor || monitor == _monitor)
    {
        return;
    }
    this->setFullscreen(monitor, w, h, refreshRate);
}

void GLViewImpl::setFullscreen(int monitorIndex)
{
    setFullscreen(monitorIndex, -1, -1, -1);
}

void GLViewImpl::setFullscreen(int monitorIndex, int w, int h, int refreshRate)
{
    int count              = 0;
    GLFWmonitor** monitors = glfwGetMonitors(&count);
    if (monitorIndex < 0 || monitorIndex >= count)
    {
        return;
    }
    GLFWmonitor* monitor = monitors[monitorIndex];
    if (nullptr == monitor || _monitor == monitor)
    {
        return;
    }
    this->setFullscreen(monitor, w, h, refreshRate);
}

void GLViewImpl::setFullscreen(GLFWmonitor* monitor, int w, int h, int refreshRate)
{
    _monitor = monitor;

    const GLFWvidmode* videoMode = glfwGetVideoMode(_monitor);
    if (w == -1)
        w = videoMode->width;
    if (h == -1)
        h = videoMode->height;
    if (refreshRate == -1)
        refreshRate = videoMode->refreshRate;

    glfwSetWindowMonitor(_mainWindow, _monitor, 0, 0, w, h, refreshRate);
}

void GLViewImpl::setWindowed(int width, int height, bool borderless)
{
    if (!this->isFullscreen())
    {
        glfwSetWindowAttrib(_mainWindow, GLFW_DECORATED, borderless ? GLFW_FALSE : GLFW_TRUE);

        if (glfwGetWindowAttrib(_mainWindow, GLFW_MAXIMIZED))
            glfwRestoreWindow(_mainWindow);
        this->setFrameSize((float)width, (float)height);
    }
    else
    {
        width *= _frameZoomFactor;
        height *= _frameZoomFactor;
        const GLFWvidmode* videoMode = glfwGetVideoMode(_monitor);
        int xpos = 0, ypos = 0;
        glfwGetMonitorPos(_monitor, &xpos, &ypos);
        xpos += (int)((videoMode->width - width) * 0.5f);
        ypos += (int)((videoMode->height - height) * 0.5f);
        _monitor = nullptr;
        glfwSetWindowAttrib(_mainWindow, GLFW_DECORATED, borderless ? GLFW_FALSE : GLFW_TRUE);
        glfwSetWindowMonitor(_mainWindow, nullptr, xpos, ypos, width, height, GLFW_DONT_CARE);
#if (AX_TARGET_PLATFORM == AX_PLATFORM_MAC)
        // on mac window will sometimes lose title when windowed
        glfwSetWindowTitle(_mainWindow, _viewName.c_str());
#endif
    }
}

void GLViewImpl::getWindowPosition(int* xpos, int* ypos)
{
    if (_mainWindow != nullptr)
    {
        glfwGetWindowPos(_mainWindow, xpos, ypos);
    }
}

void GLViewImpl::getWindowSize(int* width, int* height)
{
    if (_mainWindow != nullptr)
    {
        glfwGetWindowSize(_mainWindow, width, height);
    }
}

int GLViewImpl::getMonitorCount() const
{
    int count = 0;
    glfwGetMonitors(&count);
    return count;
}

Vec2 GLViewImpl::getMonitorSize() const
{
    GLFWmonitor* monitor = _monitor;
    if (nullptr == monitor)
    {
        GLFWwindow* window = this->getWindow();
        monitor            = glfwGetWindowMonitor(window);
    }
    if (nullptr == monitor)
    {
        monitor = glfwGetPrimaryMonitor();
    }
    if (nullptr != monitor)
    {
        const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
        Vec2 size                    = Vec2((float)videoMode->width, (float)videoMode->height);
        return size;
    }
    return Vec2::ZERO;
}

void GLViewImpl::handleWindowSize(int w, int h)
{
    /*
    * x-studio spec, fix view size incorrect when window size changed
    * diff with cocos2d-x, axmol don't call this->setFrameSize when window size changed,
    * instead, invoke this function to apply screenSize, update design size(update viewport)

      The cocos2d-x original behavior(incorrect):
      1. first time enter full screen: w,h=1920,1080
      2. second or later enter full screen: will trigger 2 times WindowSizeCallback
        1). w,h=976,679
        2). w,h=1024,768

      @remark:
      1. glfwSetWindowMonitor will fire window size change event in full screen mode
    */
    GLView::setFrameSize(w / _frameZoomFactor, h / _frameZoomFactor);
#if (AX_TARGET_PLATFORM == AX_PLATFORM_MAC)
    // Fix #1787, update retina state when switch between fullscreen and windowed mode
    int fbWidth = 0, fbHeight = 0;
    glfwGetFramebufferSize(_mainWindow, &fbWidth, &fbHeight);
    _isInRetinaMonitor = fbWidth == 2 * w && fbHeight == 2 * h;
    if (_isInRetinaMonitor)
        _retinaFactor = _isRetinaEnabled ? 1 : 2;
    else
        _retinaFactor = 1;
#endif
    updateDesignResolutionSize();
}

void GLViewImpl::updateFrameSize()
{
    if (_screenSize.width > 0 && _screenSize.height > 0)
    {
        int w = 0, h = 0;
        glfwGetWindowSize(_mainWindow, &w, &h);

        int fbWidth = 0, fbHeight = 0;
        glfwGetFramebufferSize(_mainWindow, &fbWidth, &fbHeight);

        _isInRetinaMonitor = fbWidth == 2 * w && fbHeight == 2 * h;
        if (_isInRetinaMonitor)
        {
            _retinaFactor = _isRetinaEnabled ? 1 : 2;
            glfwSetWindowSize(_mainWindow, _screenSize.width / 2 * _retinaFactor * _frameZoomFactor,
                              _screenSize.height / 2 * _retinaFactor * _frameZoomFactor);
        }
        else
        {
            _retinaFactor = 1;
            glfwSetWindowSize(_mainWindow, (int)(_screenSize.width * _retinaFactor * _frameZoomFactor),
                              (int)(_screenSize.height * _retinaFactor * _frameZoomFactor));
        }
    }
}

void GLViewImpl::setFrameSize(float width, float height)
{
    GLView::setFrameSize(width, height);
    updateFrameSize();
}

void GLViewImpl::setViewPortInPoints(float x, float y, float w, float h)
{
    Viewport vp;
    vp.x = (int)(x * _scaleX * _retinaFactor * _frameZoomFactor +
                 _viewPortRect.origin.x * _retinaFactor * _frameZoomFactor);
    vp.y = (int)(y * _scaleY * _retinaFactor * _frameZoomFactor +
                 _viewPortRect.origin.y * _retinaFactor * _frameZoomFactor);
    vp.w = (unsigned int)(w * _scaleX * _retinaFactor * _frameZoomFactor);
    vp.h = (unsigned int)(h * _scaleY * _retinaFactor * _frameZoomFactor);
    Camera::setDefaultViewport(vp);
}

void GLViewImpl::setScissorInPoints(float x, float y, float w, float h)
{
    auto x1       = (int)(x * _scaleX * _retinaFactor * _frameZoomFactor +
                    _viewPortRect.origin.x * _retinaFactor * _frameZoomFactor);
    auto y1       = (int)(y * _scaleY * _retinaFactor * _frameZoomFactor +
                    _viewPortRect.origin.y * _retinaFactor * _frameZoomFactor);
    auto width1   = (unsigned int)(w * _scaleX * _retinaFactor * _frameZoomFactor);
    auto height1  = (unsigned int)(h * _scaleY * _retinaFactor * _frameZoomFactor);
    auto renderer = Director::getInstance()->getRenderer();
    renderer->setScissorRect(x1, y1, width1, height1);
}

ax::Rect GLViewImpl::getScissorRect() const
{
    auto renderer = Director::getInstance()->getRenderer();
    auto& rect    = renderer->getScissorRect();

    float x = (rect.x - _viewPortRect.origin.x * _retinaFactor * _frameZoomFactor) /
              (_scaleX * _retinaFactor * _frameZoomFactor);
    float y = (rect.y - _viewPortRect.origin.y * _retinaFactor * _frameZoomFactor) /
              (_scaleY * _retinaFactor * _frameZoomFactor);
    float w = rect.width / (_scaleX * _retinaFactor * _frameZoomFactor);
    float h = rect.height / (_scaleY * _retinaFactor * _frameZoomFactor);
    return ax::Rect(x, y, w, h);
}

void GLViewImpl::onGLFWError(int errorID, const char* errorDesc)
{
    if (_mainWindow)
    {
        _glfwError = StringUtils::format("GLFWError #%d Happen, %s", errorID, errorDesc);
    }
    else
    {
        _glfwError.append(StringUtils::format("GLFWError #%d Happen, %s\n", errorID, errorDesc));
    }
    AXLOGE("{}", _glfwError);
}

void GLViewImpl::onGLFWMouseCallBack(GLFWwindow* /*window*/, int button, int action, int /*modify*/)
{
    if (!_isTouchDevice)
    {
        if (GLFW_MOUSE_BUTTON_LEFT == button)
        {
            if (GLFW_PRESS == action)
            {
                _captured = true;
                if (this->getViewPortRect().equals(ax::Rect::ZERO) ||
                    this->getViewPortRect().containsPoint(Vec2(_mouseX, _mouseY)))
                {
                    intptr_t id = 0;
                    this->handleTouchesBegin(1, &id, &_mouseX, &_mouseY);
                }
            }
            else if (GLFW_RELEASE == action)
            {
                if (_captured)
                {
                    _captured   = false;
                    intptr_t id = 0;
                    this->handleTouchesEnd(1, &id, &_mouseX, &_mouseY);
                }
            }
        }
    }

    // Because OpenGL and axmol uses different Y axis, we need to convert the coordinate here
    float cursorX = (_mouseX - _viewPortRect.origin.x) / _scaleX;
    float cursorY = (_viewPortRect.origin.y + _viewPortRect.size.height - _mouseY) / _scaleY;

    if (GLFW_PRESS == action)
    {
        EventMouse event(EventMouse::MouseEventType::MOUSE_DOWN);
        event.setCursorPosition(cursorX, cursorY);
        event.setMouseButton(static_cast<ax::EventMouse::MouseButton>(button));
        Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
    }
    else if (GLFW_RELEASE == action)
    {
        EventMouse event(EventMouse::MouseEventType::MOUSE_UP);
        event.setCursorPosition(cursorX, cursorY);
        event.setMouseButton(static_cast<ax::EventMouse::MouseButton>(button));
        Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
    }
}

void GLViewImpl::onGLFWMouseMoveCallBack(GLFWwindow* window, double x, double y)
{
    _mouseX = (float)x;
    _mouseY = (float)y;

    _mouseX /= this->getFrameZoomFactor();
    _mouseY /= this->getFrameZoomFactor();

    if (_isInRetinaMonitor)
    {
        if (_retinaFactor == 1)
        {
            _mouseX *= 2;
            _mouseY *= 2;
        }
    }
    if (!_isTouchDevice)
    {
        if (_captured)
        {
            intptr_t id = 0;
            this->handleTouchesMove(1, &id, &_mouseX, &_mouseY);
        }
    }

    // Because OpenGL and axmol uses different Y axis, we need to convert the coordinate here
    float cursorX = (_mouseX - _viewPortRect.origin.x) / _scaleX;
    float cursorY = (_viewPortRect.origin.y + _viewPortRect.size.height - _mouseY) / _scaleY;

    EventMouse event(EventMouse::MouseEventType::MOUSE_MOVE);
    // Set current button
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        event.setMouseButton(static_cast<ax::EventMouse::MouseButton>(GLFW_MOUSE_BUTTON_LEFT));
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        event.setMouseButton(static_cast<ax::EventMouse::MouseButton>(GLFW_MOUSE_BUTTON_RIGHT));
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
    {
        event.setMouseButton(static_cast<ax::EventMouse::MouseButton>(GLFW_MOUSE_BUTTON_MIDDLE));
    }
    event.setCursorPosition(cursorX, cursorY);
    Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
}

#if defined(__EMSCRIPTEN__)
void GLViewImpl::onWebTouchCallback(int eventType, const EmscriptenTouchEvent* touchEvent)
{
    float boundingX = EM_ASM_INT(return canvas.getBoundingClientRect().left);
    float boundingY = EM_ASM_INT(return canvas.getBoundingClientRect().top);
    int canvasWidth, canvasHeight;
    emscripten_get_canvas_element_size("#canvas", &canvasWidth, &canvasHeight);
    double cssWidth, cssHeight;
    emscripten_get_element_css_size("#canvas", &cssWidth, &cssHeight);
    const auto zoomX = canvasWidth / cssWidth;
    const auto zommY = canvasHeight / cssHeight;

    int numTouches = touchEvent->numTouches;
    _touchesId.resize(numTouches);
    _touchesX.resize(numTouches);
    _touchesY.resize(numTouches);
    for (int i = 0; i < numTouches; i++)
    {
        _touchesId[i] = (touchEvent->touches[i].identifier);
        // convert coords screen(origin:left-top) to canvas
        _touchesX[i] = ((touchEvent->touches[i].targetX - boundingX) * zoomX);
        _touchesY[i] = ((touchEvent->touches[i].targetY - boundingY) * zommY);
    }
    if (numTouches)
    {
        switch (eventType)
        {
        case EMSCRIPTEN_EVENT_TOUCHSTART:
            _captured = true;
            handleTouchesBegin(numTouches, _touchesId.data(), _touchesX.data(), _touchesY.data());
            break;
        case EMSCRIPTEN_EVENT_TOUCHEND:
            handleTouchesEnd(numTouches, _touchesId.data(), _touchesX.data(), _touchesY.data());
            _captured = false;
            break;
        case EMSCRIPTEN_EVENT_TOUCHMOVE:
            if (_captured)
                handleTouchesMove(numTouches, _touchesId.data(), _touchesX.data(), _touchesY.data());
            break;
        case EMSCRIPTEN_EVENT_TOUCHCANCEL:
            handleTouchesCancel(numTouches, _touchesId.data(), _touchesX.data(), _touchesY.data());
            _captured = false;
            break;
        }
    }
}
#endif

void GLViewImpl::onGLFWMouseScrollCallback(GLFWwindow* /*window*/, double x, double y)
{
    EventMouse event(EventMouse::MouseEventType::MOUSE_SCROLL);
    // Because OpenGL and axmol uses different Y axis, we need to convert the coordinate here
    float cursorX = (_mouseX - _viewPortRect.origin.x) / _scaleX;
    float cursorY = (_viewPortRect.origin.y + _viewPortRect.size.height - _mouseY) / _scaleY;
    event.setScrollData((float)x, -(float)y);
    event.setCursorPosition(cursorX, cursorY);
    Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
}

void GLViewImpl::onGLFWKeyCallback(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int /*mods*/)
{
    // x-studio spec, for repeat press key support.
    EventKeyboard event(g_keyCodeMap[key], action);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->dispatchEvent(&event);

    if (GLFW_RELEASE != action)
    {
        switch (g_keyCodeMap[key])
        {
        case EventKeyboard::KeyCode::KEY_BACKSPACE:
            IMEDispatcher::sharedDispatcher()->dispatchDeleteBackward();
            break;
        case EventKeyboard::KeyCode::KEY_HOME:
        case EventKeyboard::KeyCode::KEY_KP_HOME:
        case EventKeyboard::KeyCode::KEY_DELETE:
        case EventKeyboard::KeyCode::KEY_KP_DELETE:
        case EventKeyboard::KeyCode::KEY_END:
        case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
        case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
        case EventKeyboard::KeyCode::KEY_ESCAPE:
            IMEDispatcher::sharedDispatcher()->dispatchControlKey(g_keyCodeMap[key]);
            break;
        default:
            break;
        }
    }
}

void GLViewImpl::onGLFWCharCallback(GLFWwindow* /*window*/, unsigned int charCode)
{
    std::string utf8String;
    StringUtils::UTF32ToUTF8(std::u32string_view{(char32_t*)&charCode, (size_t)1}, utf8String);
    static std::unordered_set<std::string_view> controlUnicode = {
        "\xEF\x9C\x80",  // up
        "\xEF\x9C\x81",  // down
        "\xEF\x9C\x82",  // left
        "\xEF\x9C\x83",  // right
        "\xEF\x9C\xA8",  // delete
        "\xEF\x9C\xA9",  // home
        "\xEF\x9C\xAB",  // end
        "\xEF\x9C\xAC",  // pageup
        "\xEF\x9C\xAD",  // pagedown
        "\xEF\x9C\xB9"   // clear
    };
    // Check for send control key
    if (controlUnicode.find(utf8String) == controlUnicode.end())
    {
        IMEDispatcher::sharedDispatcher()->dispatchInsertText(utf8String.c_str(), utf8String.size());
    }
}

void GLViewImpl::onGLFWWindowPosCallback(GLFWwindow* /*window*/, int x, int y)
{
    Director::getInstance()->setViewport();

    Vec2 pos(x, y);
    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(GLViewImpl::EVENT_WINDOW_POSITIONED, &pos);
}

void GLViewImpl::onGLFWWindowSizeCallback(GLFWwindow* /*window*/, int w, int h)
{
    if (w && h && _resolutionPolicy != ResolutionPolicy::UNKNOWN)
    {
        handleWindowSize(w, h);

#if defined(AX_USE_METAL)
        // update metal attachment texture size.
        int fbWidth, fbHeight;
        glfwGetFramebufferSize(_mainWindow, &fbWidth, &fbHeight);
        backend::UtilsMTL::resizeDefaultAttachmentTexture(fbWidth, fbHeight);
#endif

        Size size(w, h);
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(GLViewImpl::EVENT_WINDOW_RESIZED, &size);
    }
}

void GLViewImpl::onGLFWWindowIconifyCallback(GLFWwindow* /*window*/, int iconified)
{
    if (iconified == GL_TRUE)
    {
        Application::getInstance()->applicationDidEnterBackground();
    }
    else
    {
        Application::getInstance()->applicationWillEnterForeground();
    }
}

void GLViewImpl::onGLFWWindowFocusCallback(GLFWwindow* /*window*/, int focused)
{
    if (focused == GL_TRUE)
    {
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(GLViewImpl::EVENT_WINDOW_FOCUSED, nullptr);
    }
    else
    {
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(GLViewImpl::EVENT_WINDOW_UNFOCUSED, nullptr);
    }
}

void GLViewImpl::onGLFWWindowCloseCallback(GLFWwindow* window)
{
    bool isClose = true;
    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(GLViewImpl::EVENT_WINDOW_CLOSE, &isClose);
    if (isClose == false)
    {
        glfwSetWindowShouldClose(window, 0);
    }
}

#if (AX_TARGET_PLATFORM != AX_PLATFORM_MAC)
static bool loadFboExtensions()
{
    // If the current opengl driver doesn't have framebuffers methods, check if an extension exists
    if (glGenFramebuffers == nullptr)
    {
        auto driver = backend::DriverGL::getInstance();
        AXLOGW("OpenGL: glGenFramebuffers is nullptr, try to detect an extension");
        if (driver->hasExtension("ARB_framebuffer_object"sv))
        {
            AXLOGI("OpenGL: ARB_framebuffer_object is supported");

            glIsRenderbuffer      = (PFNGLISRENDERBUFFERPROC)glfwGetProcAddress("glIsRenderbuffer");
            glBindRenderbuffer    = (PFNGLBINDRENDERBUFFERPROC)glfwGetProcAddress("glBindRenderbuffer");
            glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)glfwGetProcAddress("glDeleteRenderbuffers");
            glGenRenderbuffers    = (PFNGLGENRENDERBUFFERSPROC)glfwGetProcAddress("glGenRenderbuffers");
            glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)glfwGetProcAddress("glRenderbufferStorage");
            glGetRenderbufferParameteriv =
                (PFNGLGETRENDERBUFFERPARAMETERIVPROC)glfwGetProcAddress("glGetRenderbufferParameteriv");
            glIsFramebuffer          = (PFNGLISFRAMEBUFFERPROC)glfwGetProcAddress("glIsFramebuffer");
            glBindFramebuffer        = (PFNGLBINDFRAMEBUFFERPROC)glfwGetProcAddress("glBindFramebuffer");
            glDeleteFramebuffers     = (PFNGLDELETEFRAMEBUFFERSPROC)glfwGetProcAddress("glDeleteFramebuffers");
            glGenFramebuffers        = (PFNGLGENFRAMEBUFFERSPROC)glfwGetProcAddress("glGenFramebuffers");
            glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)glfwGetProcAddress("glCheckFramebufferStatus");
            glFramebufferTexture1D   = (PFNGLFRAMEBUFFERTEXTURE1DPROC)glfwGetProcAddress("glFramebufferTexture1D");
            glFramebufferTexture2D   = (PFNGLFRAMEBUFFERTEXTURE2DPROC)glfwGetProcAddress("glFramebufferTexture2D");
            glFramebufferTexture3D   = (PFNGLFRAMEBUFFERTEXTURE3DPROC)glfwGetProcAddress("glFramebufferTexture3D");
            glFramebufferRenderbuffer =
                (PFNGLFRAMEBUFFERRENDERBUFFERPROC)glfwGetProcAddress("glFramebufferRenderbuffer");
            glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)glfwGetProcAddress(
                "glGetFramebufferAttachmentParameteriv");
            glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)glfwGetProcAddress("glGenerateMipmap");
        }
        else if (driver->hasExtension("EXT_framebuffer_object"sv))
        {
            AXLOGI("OpenGL: EXT_framebuffer_object is supported");
            glIsRenderbuffer      = (PFNGLISRENDERBUFFERPROC)glfwGetProcAddress("glIsRenderbufferEXT");
            glBindRenderbuffer    = (PFNGLBINDRENDERBUFFERPROC)glfwGetProcAddress("glBindRenderbufferEXT");
            glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)glfwGetProcAddress("glDeleteRenderbuffersEXT");
            glGenRenderbuffers    = (PFNGLGENRENDERBUFFERSPROC)glfwGetProcAddress("glGenRenderbuffersEXT");
            glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)glfwGetProcAddress("glRenderbufferStorageEXT");
            glGetRenderbufferParameteriv =
                (PFNGLGETRENDERBUFFERPARAMETERIVPROC)glfwGetProcAddress("glGetRenderbufferParameterivEXT");
            glIsFramebuffer      = (PFNGLISFRAMEBUFFERPROC)glfwGetProcAddress("glIsFramebufferEXT");
            glBindFramebuffer    = (PFNGLBINDFRAMEBUFFERPROC)glfwGetProcAddress("glBindFramebufferEXT");
            glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)glfwGetProcAddress("glDeleteFramebuffersEXT");
            glGenFramebuffers    = (PFNGLGENFRAMEBUFFERSPROC)glfwGetProcAddress("glGenFramebuffersEXT");
            glCheckFramebufferStatus =
                (PFNGLCHECKFRAMEBUFFERSTATUSPROC)glfwGetProcAddress("glCheckFramebufferStatusEXT");
            glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC)glfwGetProcAddress("glFramebufferTexture1DEXT");
            glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)glfwGetProcAddress("glFramebufferTexture2DEXT");
            glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC)glfwGetProcAddress("glFramebufferTexture3DEXT");
            glFramebufferRenderbuffer =
                (PFNGLFRAMEBUFFERRENDERBUFFERPROC)glfwGetProcAddress("glFramebufferRenderbufferEXT");
            glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)glfwGetProcAddress(
                "glGetFramebufferAttachmentParameterivEXT");
            glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)glfwGetProcAddress("glGenerateMipmapEXT");
        }
        else if (driver->hasExtension("GL_ANGLE_framebuffer_blit"sv))
        {
            AXLOGI("OpenGL: GL_ANGLE_framebuffer_object is supported");

            glIsRenderbuffer      = (PFNGLISRENDERBUFFERPROC)glfwGetProcAddress("glIsRenderbufferOES");
            glBindRenderbuffer    = (PFNGLBINDRENDERBUFFERPROC)glfwGetProcAddress("glBindRenderbufferOES");
            glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)glfwGetProcAddress("glDeleteRenderbuffersOES");
            glGenRenderbuffers    = (PFNGLGENRENDERBUFFERSPROC)glfwGetProcAddress("glGenRenderbuffersOES");
            glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)glfwGetProcAddress("glRenderbufferStorageOES");
            // glGetRenderbufferParameteriv =
            // (PFNGLGETRENDERBUFFERPARAMETERIVPROC)glfwGetProcAddress("glGetRenderbufferParameterivOES");
            glIsFramebuffer      = (PFNGLISFRAMEBUFFERPROC)glfwGetProcAddress("glIsFramebufferOES");
            glBindFramebuffer    = (PFNGLBINDFRAMEBUFFERPROC)glfwGetProcAddress("glBindFramebufferOES");
            glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)glfwGetProcAddress("glDeleteFramebuffersOES");
            glGenFramebuffers    = (PFNGLGENFRAMEBUFFERSPROC)glfwGetProcAddress("glGenFramebuffersOES");
            glCheckFramebufferStatus =
                (PFNGLCHECKFRAMEBUFFERSTATUSPROC)glfwGetProcAddress("glCheckFramebufferStatusOES");
            glFramebufferRenderbuffer =
                (PFNGLFRAMEBUFFERRENDERBUFFERPROC)glfwGetProcAddress("glFramebufferRenderbufferOES");
            glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)glfwGetProcAddress("glFramebufferTexture2DOES");
            glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)glfwGetProcAddress(
                "glGetFramebufferAttachmentParameterivOES");
            glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)glfwGetProcAddress("glGenerateMipmapOES");
        }
        else
        {
            AXLOGE("OpenGL: No framebuffers extension is supported");
            AXLOGE("OpenGL: Any call to Fbo will crash!");
            return false;
        }
    }
    return true;
}

// helper
bool GLViewImpl::loadGL()
{
#    if (AX_TARGET_PLATFORM != AX_PLATFORM_MAC)

    // glad: load all OpenGL function pointers
    // ---------------------------------------
#        if !AX_GLES_PROFILE
    if (!gladLoadGL(glfwGetProcAddress))
    {
        AXLOGE("glad: Failed to Load GL");
        return false;
    }
#        else
    if (!gladLoadGLES2(glfwGetProcAddress))
    {
        AXLOGE("glad: Failed to Load GLES2");
        return false;
    }
#        endif

    loadFboExtensions();
#    endif  // (AX_TARGET_PLATFORM != AX_PLATFORM_MAC)

    return true;
}

#endif

NS_AX_END  // end of namespace ax;
