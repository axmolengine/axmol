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

#include "axmol/platform/RenderViewImpl.h"

#include <cmath>
#include <unordered_map>

#include "axmol/platform/Application.h"
#include "axmol/base/Director.h"
#include "axmol/base/Touch.h"
#include "axmol/base/EventDispatcher.h"
#include "axmol/base/EventKeyboard.h"
#include "axmol/base/EventMouse.h"
#include "axmol/base/IMEDispatcher.h"
#include "axmol/base/Utils.h"
#include "axmol/base/text_utils.h"
#include "axmol/2d/Camera.h"
#if AX_ICON_SET_SUPPORT
#    include "axmol/platform/Image.h"
#endif /* AX_ICON_SET_SUPPORT */

#include "axmol/renderer/Renderer.h"

#if AX_RENDER_API == AX_RENDER_API_MTL
#    include <Metal/Metal.h>
#    include "axmol/rhi/metal/DriverMTL.h"
#    include "axmol/rhi/metal/UtilsMTL.h"
#elif AX_RENDER_API == AX_RENDER_API_GL
#    include "axmol/rhi/opengl/DriverGL.h"
#    include "axmol/rhi/opengl/MacrosGL.h"
#    include "axmol/rhi/opengl/OpenGLState.h"
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

#ifndef NDEBUG
#    include "axmol/base/Scheduler.h"
#endif

namespace ax
{

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
    static EM_BOOL onWebTouchCallback(int eventType, const EmscriptenTouchEvent* touchEvent, void* /*userData*/)
    {
        if (_view)
            _view->onWebTouchCallback(eventType, touchEvent);
        return EM_FALSE;
    }

    static void onWebClickCallback()
    {
        if (_view)
            _view->onWebClickCallback();
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

    static void onGLFWWindowSizeCallback(GLFWwindow* window, int width, int height)
    {
        if (_view)
            _view->onGLFWWindowSizeCallback(window, width, height);
    }

    static void setRenderViewImpl(RenderViewImpl* view) { _view = view; }

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
    static RenderViewImpl* _view;
};
RenderViewImpl* GLFWEventHandler::_view = nullptr;

const std::string RenderViewImpl::EVENT_WINDOW_POSITIONED = "glview_window_positioned";
const std::string RenderViewImpl::EVENT_WINDOW_RESIZED    = "glview_window_resized";
const std::string RenderViewImpl::EVENT_WINDOW_FOCUSED    = "glview_window_focused";
const std::string RenderViewImpl::EVENT_WINDOW_UNFOCUSED  = "glview_window_unfocused";
const std::string RenderViewImpl::EVENT_WINDOW_CLOSE      = "glview_window_close";

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
// implement RenderViewImpl
//////////////////////////////////////////////////////////////////////////

static EventMouse::MouseButton checkMouseButton(GLFWwindow* window)
{
    EventMouse::MouseButton mouseButton{EventMouse::MouseButton::BUTTON_UNSET};
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        mouseButton = static_cast<ax::EventMouse::MouseButton>(GLFW_MOUSE_BUTTON_LEFT);
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        mouseButton = static_cast<ax::EventMouse::MouseButton>(GLFW_MOUSE_BUTTON_RIGHT);
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
    {
        mouseButton = static_cast<ax::EventMouse::MouseButton>(GLFW_MOUSE_BUTTON_MIDDLE);
    }
    return mouseButton;
}

RenderViewImpl::RenderViewImpl(bool initglfw)
    : _captured(false)
    , _isHighDPI(false)
    , _renderScale(1.0f)
    , _windowZoomFactor(1.0f)
    , _mainWindow(nullptr)
    , _monitor(nullptr)
    , _mouseX(0.0f)
    , _mouseY(0.0f)
{
    _viewName = "axmol3";
    g_keyCodeMap.clear();
    for (auto&& item : g_keyCodeStructArray)
    {
        g_keyCodeMap[item.glfwKeyCode] = item.keyCode;
    }

    GLFWEventHandler::setRenderViewImpl(this);
    if (initglfw)
    {
        glfwSetErrorCallback(GLFWEventHandler::onGLFWError);
        glfwInit();
    }
}

RenderViewImpl::~RenderViewImpl()
{
    AXLOGD("deallocing RenderViewImpl: {}", fmt::ptr(this));
    GLFWEventHandler::setRenderViewImpl(nullptr);
    glfwTerminate();
}

void* RenderViewImpl::getNativeWindow() const
{
#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
    return glfwGetWin32Window(_mainWindow);
#elif AX_TARGET_PLATFORM == AX_PLATFORM_MAC
    return (void*)glfwGetCocoaWindow(_mainWindow);
#elif AX_TARGET_PLATFORM == AX_PLATFORM_LINUX
    int platform = glfwGetPlatform();
    return platform == GLFW_PLATFORM_WAYLAND ? (void*)glfwGetWaylandWindow(_mainWindow)
                                             : (void*)glfwGetX11Window(_mainWindow);
#else
    return nullptr;
#endif
}

void* RenderViewImpl::getNativeDisplay() const
{
#if AX_TARGET_PLATFORM == AX_PLATFORM_MAC
    return (void*)glfwGetNSGLContext(_mainWindow);
#elif AX_TARGET_PLATFORM == AX_PLATFORM_LINUX
    int platform = glfwGetPlatform();
    return platform == GLFW_PLATFORM_WAYLAND ? (void*)glfwGetWaylandDisplay() : (void*)glfwGetX11Display();
#else
    return nullptr;
#endif
}

WindowPlatform RenderViewImpl::getWindowPlatform() const
{
#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
    return WindowPlatform::Win32;
#elif AX_TARGET_PLATFORM == AX_PLATFORM_MAC
    return WindowPlatform::Cocoa;
#elif AX_TARGET_PLATFORM == AX_PLATFORM_LINUX
    int platform = glfwGetPlatform();
    return platform == GLFW_PLATFORM_WAYLAND ? WindowPlatform::Wayland : WindowPlatform::X11;
#elif AX_TARGET_PLATFORM == AX_PLATFORM_WASM
    return WindowPlatform::Web;
#else
    return WindowPlatform::Unknown;
#endif
}

RenderViewImpl* RenderViewImpl::create(std::string_view viewName)
{
    return RenderViewImpl::create(viewName, false);
}

RenderViewImpl* RenderViewImpl::create(std::string_view viewName, bool resizable)
{
    auto ret = new RenderViewImpl;
    if (ret->initWithRect(viewName, ax::Rect(0, 0, 960, 640), 1.0f, resizable))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

RenderViewImpl* RenderViewImpl::createWithRect(std::string_view viewName,
                                               const ax::Rect& rect,
                                               float windowZoomFactor,
                                               bool resizable)
{
    auto ret = new RenderViewImpl;
    if (ret->initWithRect(viewName, rect, windowZoomFactor, resizable))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

RenderViewImpl* RenderViewImpl::createWithFullscreen(std::string_view viewName)
{
    auto ret = new RenderViewImpl();
    if (ret->initWithFullScreen(viewName))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

RenderViewImpl* RenderViewImpl::createWithFullscreen(std::string_view viewName,
                                                     const GLFWvidmode& videoMode,
                                                     GLFWmonitor* monitor)
{
    auto ret = new RenderViewImpl();
    if (ret->initWithFullscreen(viewName, videoMode, monitor))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

bool RenderViewImpl::initWithRect(std::string_view viewName,
                                  const ax::Rect& rect,
                                  float windowZoomFactor,
                                  bool resizable)
{
    setViewName(viewName);

    _windowZoomFactor = windowZoomFactor;

    Vec2 requestWinSize = rect.size * windowZoomFactor;

#if AX_RENDER_API == AX_RENDER_API_GL
#    if AX_GLES_PROFILE
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, AX_GLES_PROFILE / AX_GLES_PROFILE_DEN);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#    else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // We don't want the old OpenGL
#    endif
#else  // Other Graphics driver, don't create gl context.
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif

    glfwWindowHint(GLFW_RESIZABLE, resizable ? GL_TRUE : GL_FALSE);
    glfwWindowHint(GLFW_RED_BITS, _gfxContextAttrs.redBits);
    glfwWindowHint(GLFW_GREEN_BITS, _gfxContextAttrs.greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, _gfxContextAttrs.blueBits);
    glfwWindowHint(GLFW_ALPHA_BITS, _gfxContextAttrs.alphaBits);
    glfwWindowHint(GLFW_DEPTH_BITS, _gfxContextAttrs.depthBits);
    glfwWindowHint(GLFW_STENCIL_BITS, _gfxContextAttrs.stencilBits);

    glfwWindowHint(GLFW_SAMPLES, _gfxContextAttrs.multisamplingCount);

    glfwWindowHint(GLFW_VISIBLE, _gfxContextAttrs.visible);
    glfwWindowHint(GLFW_DECORATED, _gfxContextAttrs.decorated);

#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32)
    glfwWindowHintPointer(GLFW_WIN32_HWND_PARENT, _gfxContextAttrs.viewParent);
#endif

#if AX_RENDER_API == AX_RENDER_API_D3D
    // Initialize the D3D driver before creating the window to avoid a brief white flash
    // caused by driver initialization stutter (hundreds of milliseconds) after the window appears.
    axdrv;
#endif

    _mainWindow = glfwCreateWindow(static_cast<int>(requestWinSize.width), static_cast<int>(requestWinSize.height),
                                   _viewName.c_str(), _monitor, nullptr);

    if (_mainWindow == nullptr)
    {
        std::string message = "Can't create window";
        if (!_glfwError.empty())
        {
            message.append("\nMore info: \n");
            message.append(_glfwError);
        }

        showAlert(message, "Error launch application");
        utils::killCurrentProcess();  // kill current process, don't cause crash when driver issue.
        return false;
    }

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
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(_mainWindow, &fbWidth, &fbHeight);

    int w, h;
    glfwGetWindowSize(_mainWindow, &w, &h);

    handleWindowResized(w, h, fbWidth, fbHeight);

#if AX_RENDER_API == AX_RENDER_API_MTL
    CGSize size;
    size.width  = static_cast<CGFloat>(fbWidth);
    size.height = static_cast<CGFloat>(fbHeight);
    // Initialize device.
    id<MTLDevice> device = MTLCreateSystemDefaultDevice();
    if (!device)
    {
        AXLOGD("Doesn't support metal.");
        return false;
    }

    NSView* contentView = [(id)getNativeWindow() contentView];
    [contentView setWantsLayer:YES];
    CAMetalLayer* layer = [CAMetalLayer layer];
    [layer setDevice:device];
    [layer setPixelFormat:MTLPixelFormatBGRA8Unorm];
    [layer setFramebufferOnly:YES];
    [layer setDrawableSize:size];
    layer.displaySyncEnabled = _gfxContextAttrs.vsync;
    [contentView setLayer:layer];
    rhi::mtl::DriverImpl::setCAMetalLayer(layer);
#elif AX_RENDER_API == AX_RENDER_API_GL
    glfwMakeContextCurrent(_mainWindow);
    glfwSetWindowUserPointer(_mainWindow, rhi::gl::__state);
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
    else
    {
        EM_ASM({
            document.addEventListener('click', function(event){
                Module.ccall("axmol_onwebclickcallback");
            });
        });
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
#    if AX_RENDER_API == AX_RENDER_API_GL
    loadGL();
    // Init driver after load GL
    axdrv;
#    endif
#endif

#if AX_RENDER_API == AX_RENDER_API_GL
#    if !defined(__EMSCRIPTEN__)
    glfwSwapInterval(_gfxContextAttrs.vsync ? 1 : 0);
#    endif
    // Will cause OpenGL error 0x0500 when use ANGLE-GLES on desktop
#    if !AX_GLES_PROFILE
    // Enable point size by default.
#        if defined(GL_VERSION_2_0)
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
#        else
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);
#        endif
    if (_gfxContextAttrs.multisamplingCount > 0)
        glEnable(GL_MULTISAMPLE);
#    endif
    CHECK_GL_ERROR_DEBUG();
#endif
    return true;
}

bool RenderViewImpl::initWithFullScreen(std::string_view viewName)
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

bool RenderViewImpl::initWithFullscreen(std::string_view viewname, const GLFWvidmode& videoMode, GLFWmonitor* monitor)
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

bool RenderViewImpl::isGfxContextReady()
{
    return nullptr != _mainWindow;
}

void RenderViewImpl::end()
{
    if (_mainWindow)
    {
        glfwSetWindowShouldClose(_mainWindow, 1);
        _mainWindow = nullptr;
    }
    // Release self. Otherwise, RenderViewImpl could not be freed.
    release();
}

void RenderViewImpl::swapBuffers()
{
#if AX_RENDER_API == AX_RENDER_API_GL
    if (_mainWindow)
        glfwSwapBuffers(_mainWindow);
#endif
}

bool RenderViewImpl::windowShouldClose()
{
    if (_mainWindow)
        return glfwWindowShouldClose(_mainWindow) ? true : false;
    else
        return true;
}

void RenderViewImpl::pollEvents()
{
    glfwPollEvents();
}

void RenderViewImpl::setIMEKeyboardState(bool /*bOpen*/) {}

#if AX_ICON_SET_SUPPORT
void RenderViewImpl::setIcon(std::string_view filename) const
{
    this->setIcon(std::vector<std::string_view>{filename});
}

void RenderViewImpl::setIcon(const std::vector<std::string_view>& filelist) const
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

void RenderViewImpl::setDefaultIcon() const
{
    GLFWwindow* window = this->getWindow();
    glfwSetWindowIcon(window, 0, nullptr);
}
#endif /* AX_ICON_SET_SUPPORT */

void RenderViewImpl::setCursorVisible(bool isVisible)
{
    if (_mainWindow == NULL)
        return;

    if (isVisible)
        glfwSetInputMode(_mainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    else
        glfwSetInputMode(_mainWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void RenderViewImpl::setWindowZoomFactor(float zoomFactor)
{
    AXASSERT(zoomFactor > 0.0f, "zoomFactor must be larger than 0");

    if (std::abs(_windowZoomFactor - zoomFactor) < FLT_EPSILON)
        return;

    _windowZoomFactor = zoomFactor;
    applyWindowSize();
}

float RenderViewImpl::getWindowZoomFactor() const
{
    return _windowZoomFactor;
}

bool RenderViewImpl::isFullscreen() const
{
    return (_monitor != nullptr);
}

void RenderViewImpl::setFullscreen()
{
    setFullscreen(-1, -1, -1);
}

void RenderViewImpl::setFullscreen(int w, int h, int refreshRate)
{
    auto monitor = glfwGetPrimaryMonitor();
    if (nullptr == monitor || monitor == _monitor)
    {
        return;
    }
    this->setFullscreen(monitor, w, h, refreshRate);
}

void RenderViewImpl::setFullscreen(int monitorIndex)
{
    setFullscreen(monitorIndex, -1, -1, -1);
}

void RenderViewImpl::setFullscreen(int monitorIndex, int w, int h, int refreshRate)
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

void RenderViewImpl::setFullscreen(GLFWmonitor* monitor, int w, int h, int refreshRate)
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

void RenderViewImpl::setWindowed(int width, int height, bool borderless)
{
    if (!this->isFullscreen())
    {
        glfwSetWindowAttrib(_mainWindow, GLFW_DECORATED, borderless ? GLFW_FALSE : GLFW_TRUE);

        if (glfwGetWindowAttrib(_mainWindow, GLFW_MAXIMIZED))
            glfwRestoreWindow(_mainWindow);
        this->setWindowSize((float)width, (float)height);
    }
    else
    {
        width *= _windowZoomFactor;
        height *= _windowZoomFactor;
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

Vec2 RenderViewImpl::getNativeWindowSize() const
{
    if (_mainWindow != nullptr)
    {
        int w = 0, h = 0;
        glfwGetWindowSize(_mainWindow, &w, &h);
        return Vec2(w, h);
    }
    return Vec2{};
}

void RenderViewImpl::getWindowPosition(int* xpos, int* ypos)
{
    if (_mainWindow != nullptr)
        glfwGetWindowPos(_mainWindow, xpos, ypos);
}

int RenderViewImpl::getMonitorCount() const
{
    int count = 0;
    glfwGetMonitors(&count);
    return count;
}

Vec2 RenderViewImpl::getMonitorSize() const
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

void RenderViewImpl::setWindowSizeLimits(int minwidth, int minheight, int maxwidth, int maxheight)
{
    if (_mainWindow == NULL)
        return;

    glfwSetWindowSizeLimits(_mainWindow, minwidth, minheight, maxwidth, maxheight);
}

void RenderViewImpl::updateRenderScale(int windowWidth, int framebufferWidth)
{
    _isHighDPI   = framebufferWidth > windowWidth;
    _renderScale = static_cast<float>(framebufferWidth) / windowWidth;
}

/*
 * Handle window resize events (including fullscreen toggle, manual resize, HiDPI scaling).
 * Updates HiDPI flag, render scale, logical window size, and design resolution (viewport)
 * based on logical window size (w,h) and framebuffer size (fbWidth, fbHeight).
 *
 * Difference from cocos2d-x:
 * - cocos2d-x calls setWindowSize() directly on window size change.
 * - axmol uses this method instead to apply screen size and update viewport.
 *
 * The cocos2d-x original behavior (incorrect):
 *   1. First time entering fullscreen: w,h = 1920,1080
 *   2. Second or later entering fullscreen: triggers WindowSizeCallback twice:
 *        1) w,h = 976,679
 *        2) w,h = 1024,768
 *
 * Platform note (Windows + GLFW):
 * - glfwSetWindowMonitor may trigger intermediate window size events when toggling fullscreen.
 * - This happens because GLFW temporarily removes WS_OVERLAPPEDWINDOW style and calls SetWindowLong,
 *   causing a windowed-size update before ChangeDisplaySettingsExW applies the final fullscreen resolution.
 *
 * Current GLFW fire event order:
 *  -> framebufferSize
 *  -> windowSize
 */
void RenderViewImpl::handleWindowResized(int w, int h, int fbWidth, int fbHeight)
{
    updateRenderScale(w, fbWidth);

    RenderView::setWindowSize(w / _windowZoomFactor, h / _windowZoomFactor);
    updateDesignResolutionSize();
}

void RenderViewImpl::setWindowSize(float width, float height)
{
    RenderView::setWindowSize(width, height);
    applyWindowSize();
}

void RenderViewImpl::applyWindowSize()
{
    if (_windowSize.width > 0 && _windowSize.height > 0)
    {
        glfwSetWindowSize(_mainWindow, (int)(_windowSize.width * _windowZoomFactor),
                          (int)(_windowSize.height * _windowZoomFactor));
    }
}

void RenderViewImpl::setViewportInPoints(float x, float y, float w, float h)
{
    Viewport vp;
    vp.x = (int)(x * _viewScale.x * _renderScale * _windowZoomFactor +
                 _viewportRect.origin.x * _renderScale * _windowZoomFactor);
    vp.y = (int)(y * _viewScale.y * _renderScale * _windowZoomFactor +
                 _viewportRect.origin.y * _renderScale * _windowZoomFactor);
    vp.w = (unsigned int)(w * _viewScale.x * _renderScale * _windowZoomFactor);
    vp.h = (unsigned int)(h * _viewScale.y * _renderScale * _windowZoomFactor);
    Camera::setDefaultViewport(vp);
}

void RenderViewImpl::setScissorInPoints(float x, float y, float w, float h)
{
    auto x1      = (int)(x * _viewScale.x * _renderScale * _windowZoomFactor +
                    _viewportRect.origin.x * _renderScale * _windowZoomFactor);
    auto y1      = (int)(y * _viewScale.y * _renderScale * _windowZoomFactor +
                    _viewportRect.origin.y * _renderScale * _windowZoomFactor);
    auto width1  = (unsigned int)(w * _viewScale.x * _renderScale * _windowZoomFactor);
    auto height1 = (unsigned int)(h * _viewScale.y * _renderScale * _windowZoomFactor);

    setScissorRect(x1, y1, width1, height1);
}

ax::Rect RenderViewImpl::getScissorInPoints() const
{
    auto& rect = getScissorRect();

    float x = (rect.x - _viewportRect.origin.x * _renderScale * _windowZoomFactor) /
              (_viewScale.x * _renderScale * _windowZoomFactor);
    float y = (rect.y - _viewportRect.origin.y * _renderScale * _windowZoomFactor) /
              (_viewScale.y * _renderScale * _windowZoomFactor);
    float w = rect.width / (_viewScale.x * _renderScale * _windowZoomFactor);
    float h = rect.height / (_viewScale.y * _renderScale * _windowZoomFactor);
    return ax::Rect(x, y, w, h);
}

void RenderViewImpl::onGLFWError(int errorID, const char* errorDesc)
{
    if (_mainWindow)
    {
        _glfwError = fmt::format("GLFWError #{} Happen, {}", errorID, errorDesc);
    }
    else
    {
        _glfwError.append(fmt::format("GLFWError #{} Happen, {}\n", errorID, errorDesc));
    }
    AXLOGE("{}", _glfwError);
}

void RenderViewImpl::onGLFWMouseCallBack(GLFWwindow* /*window*/, int button, int action, int /*modify*/)
{
    if (!_isTouchDevice)
    {
        if (GLFW_MOUSE_BUTTON_LEFT == button)
        {
            if (GLFW_PRESS == action)
            {
                _captured = true;
                if (this->getViewportRect().equals(ax::Rect::ZERO) ||
                    this->getViewportRect().containsPoint(Vec2(_mouseX, _mouseY)))
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

    float cursorX = transformInputX(_mouseX);
    float cursorY = transformInputY(_mouseY);

    if (GLFW_PRESS == action)
    {
        EventMouse event(EventMouse::MouseEventType::MOUSE_DOWN);
        event.setMouseInfo(cursorX, cursorY, static_cast<ax::EventMouse::MouseButton>(button));
        Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
    }
    else if (GLFW_RELEASE == action)
    {
        EventMouse event(EventMouse::MouseEventType::MOUSE_UP);
        event.setMouseInfo(cursorX, cursorY, static_cast<ax::EventMouse::MouseButton>(button));
        Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
    }
}

void RenderViewImpl::onGLFWMouseMoveCallBack(GLFWwindow* window, double x, double y)
{
    _mouseX = (float)x;
    _mouseY = (float)y;

    _mouseX /= _windowZoomFactor;
    _mouseY /= _windowZoomFactor;

    if (!_isTouchDevice)
    {
        if (_captured)
        {
            intptr_t id = 0;
            this->handleTouchesMove(1, &id, &_mouseX, &_mouseY);
        }
    }

    float cursorX = transformInputX(_mouseX);
    float cursorY = transformInputY(_mouseY);

    EventMouse event(EventMouse::MouseEventType::MOUSE_MOVE);
    // Set current button
    EventMouse::MouseButton mouseButton{EventMouse::MouseButton::BUTTON_UNSET};
    event.setMouseInfo(cursorX, cursorY, checkMouseButton(window));
    Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
}

#if defined(__EMSCRIPTEN__)
void RenderViewImpl::onWebTouchCallback(int eventType, const EmscriptenTouchEvent* touchEvent)
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

void RenderViewImpl::onWebClickCallback()
{
    if (!_isTouchDevice)
    {
        if (_captured)
        {
            _captured   = false;
            intptr_t id = 0;
            this->handleTouchesCancel(1, &id, &_mouseX, &_mouseY);
        }
    }
}
#endif

void RenderViewImpl::onGLFWMouseScrollCallback(GLFWwindow* window, double x, double y)
{
    EventMouse event(EventMouse::MouseEventType::MOUSE_SCROLL);
    float cursorX = transformInputX(_mouseX);
    float cursorY = transformInputY(_mouseY);
    event.setScrollData((float)x, -(float)y);
    event.setMouseInfo(cursorX, cursorY, checkMouseButton(window));
    Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
}

void RenderViewImpl::onGLFWKeyCallback(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int /*mods*/)
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
            IMEDispatcher::sharedDispatcher()->dispatchDeleteBackward(1);
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
#if defined(AX_PLATFORM_PC) && !defined(NDEBUG)
    else
    {
        auto director = Director::getInstance();
        switch (g_keyCodeMap[key])
        {
        case EventKeyboard::KeyCode::KEY_SPACE:
            if (director->isPaused())
                director->resume();
            else
                director->pause();
            break;
        case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
            director->resume();
            director->getScheduler()->schedule([director](float) { director->pause(); }, director, 0, 0, 0, false,
                                               "step");
            break;
        }
    }
#endif
}

void RenderViewImpl::onGLFWCharCallback(GLFWwindow* /*window*/, unsigned int charCode)
{
    std::string utf8String;
    text_utils::UTF32ToUTF8(std::u32string_view{(char32_t*)&charCode, (size_t)1}, utf8String);
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

void RenderViewImpl::onGLFWWindowPosCallback(GLFWwindow* /*window*/, int x, int y)
{
    auto director = Director::getInstance();
    director->setViewport();

    Vec2 pos(x, y);
    director->getEventDispatcher()->dispatchCustomEvent(RenderViewImpl::EVENT_WINDOW_POSITIONED, &pos);
}

void RenderViewImpl::onGLFWWindowSizeCallback(GLFWwindow* /*window*/, int w, int h)
{
    if (w && h && _resolutionPolicy != ResolutionPolicy::UNKNOWN)
    {
        // Query the actual framebuffer size here to handle HiDPI displays.
        // On macOS/Windows/Linux with Retina/HiDPI, framebuffer size may differ from window size.
        int fbWidth = 0, fbHeight = 0;
        glfwGetFramebufferSize(_mainWindow, &fbWidth, &fbHeight);

        handleWindowResized(w, h, fbWidth, fbHeight);
        onFramebufferResized(fbWidth, fbHeight);
        Size size(w, h);
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(RenderViewImpl::EVENT_WINDOW_RESIZED, &size);
    }
}

void RenderViewImpl::onGLFWWindowIconifyCallback(GLFWwindow* /*window*/, int iconified)
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

void RenderViewImpl::onGLFWWindowFocusCallback(GLFWwindow* /*window*/, int focused)
{
    if (focused == GL_TRUE)
    {
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(RenderViewImpl::EVENT_WINDOW_FOCUSED,
                                                                           nullptr);
    }
    else
    {
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(RenderViewImpl::EVENT_WINDOW_UNFOCUSED,
                                                                           nullptr);
    }
}

void RenderViewImpl::onGLFWWindowCloseCallback(GLFWwindow* window)
{
    bool isClose = true;
    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(RenderViewImpl::EVENT_WINDOW_CLOSE, &isClose);
    if (isClose == false)
    {
        glfwSetWindowShouldClose(window, 0);
    }
}

#if AX_TARGET_PLATFORM != AX_PLATFORM_MAC && AX_RENDER_API == AX_RENDER_API_GL
static bool loadFboExtensions()
{
    // If the current opengl driver doesn't have framebuffers methods, check if an extension exists
    if (glGenFramebuffers == nullptr)
    {
        auto driver = axdrv;
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
bool RenderViewImpl::loadGL()
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

}  // namespace ax

#if defined(__EMSCRIPTEN__)
extern "C" {
void axmol_onwebclickcallback()
{
    ax::GLFWEventHandler::onWebClickCallback();
}
}
#endif
