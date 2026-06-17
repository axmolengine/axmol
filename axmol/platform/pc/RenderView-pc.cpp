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

The RenderView for win32,linux,macos,wasm

****************************************************************************/

#include "axmol/platform/pc/RenderView-pc.h"

#include <cmath>
#include <unordered_map>

#include "axmol/platform/Application.h"
#include "axmol/base/Director.h"
#include "axmol/base/PointerEvent.h"
#include "axmol/base/EventDispatcher.h"
#include "axmol/base/KeyboardEvent.h"
#include "axmol/base/InputSystem.h"
#include "axmol/base/Utils.h"
#include "axmol/base/text_utils.h"
#include "axmol/scene/Camera.h"
#if AX_ICON_SET_SUPPORT
#    include "axmol/platform/Image.h"
#endif /* AX_ICON_SET_SUPPORT */

#include "axmol/renderer/Renderer.h"

#if AX_ENABLE_MTL
#    include <Metal/Metal.h>
#    include "axmol/rhi/metal/DriverMTL.h"
#    include "axmol/rhi/metal/UtilsMTL.h"
#endif
#if AX_ENABLE_GL
#    include "axmol/rhi/opengl/DriverGL.h"
#    include "axmol/rhi/opengl/MacrosGL.h"
#    include "axmol/rhi/opengl/OpenGLState.h"
#endif
#if AX_ENABLE_VK
#    include "axmol/rhi/vulkan/DriverVK.h"
#endif  // #if (AX_TARGET_PLATFORM == AX_PLATFORM_MAC)

#include "axmol/rhi/DriverContext.h"

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

#if defined(_WIN32)
#    pragma comment(lib, "imm32.lib")
#endif

#if defined(__linux__)
#    pragma push_macro("None")
#    undef None
#endif

namespace ax
{

using namespace rhi;

#if defined(__EMSCRIPTEN__)
struct IVec2
{
    int x{0};
    int y{0};
};
struct WebFullscreenState
{
    WebFullscreenState()
    {
        EmscriptenFullscreenChangeEvent fs;
        if (emscripten_get_fullscreen_status(&fs) == EMSCRIPTEN_RESULT_SUCCESS)
        {
            isFullscreen = fs.isFullscreen;
        }
    }

    IVec2 windowedSize;
    bool isFullscreen{false};
};
static std::unique_ptr<WebFullscreenState> s_fullscreenState;
#endif

class GLFWEventHandler
{
public:
    static void onGLFWError(int errorID, const char* errorDesc)
    {
        if (_view)
            _view->onGLFWError(errorID, errorDesc);
    }

    // WebAssembly: use w3c pointerevent and scroll event for mouse/touch/pen events, not use GLFW
#if defined(__EMSCRIPTEN__)
    static EM_BOOL onWebOrientationChangeCallback(int eventType,
                                                  const EmscriptenOrientationChangeEvent* e,
                                                  void* /*userData*/)
    {
        if (_view)
            _view->onWebOrientationChangeCallback(eventType, e);
        return EM_TRUE;
    }

    static EM_BOOL onWebFullscreenCallback(int eventType, const EmscriptenFullscreenChangeEvent* e, void* /*userData*/)
    {
        if (_view)
            _view->onWebFullscreenCallback(eventType, e);
        return EM_TRUE;
    }
#else
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
    static void onGLFWMouseScrollCallback(GLFWwindow* window, double x, double y)
    {
        if (_view)
            _view->onGLFWMouseScrollCallback(window, x, y);
    }
#endif

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

    static void onGLFWFramebufferSizeCallback(GLFWwindow* window, int width, int height)
    {
        if (_view)
            _view->onGLFWFramebufferSizeCallback(window, width, height);
    }

    static void onGLFWWindowSizeCallback(GLFWwindow* window, int width, int height)
    {
        if (_view)
            _view->onGLFWWindowSizeCallback(window, width, height);
    }

    static void setRenderView(RenderView* view) { _view = view; }

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

    static void onGLFWPreeditCallback(GLFWwindow* window,
                                      int preedit_count,
                                      unsigned int* preedit_string,
                                      int block_count,
                                      int* block_sizes,
                                      int focused_block,
                                      int caret)
    {
        if (_view)
        {
            _view->onGLFWPreeditCallback(window, preedit_count, preedit_string, block_count, block_sizes, focused_block,
                                         caret);
        }
    }

private:
    static RenderView* _view;
};
RenderView* GLFWEventHandler::_view = nullptr;

static constexpr intptr_t MOUSE_POINTER_ID = 0;

////////////////////////////////////////////////////

struct KeyCodeItem
{
    int glfwKeyCode;
    KeyboardEvent::KeyCode keyCode;
};

static constexpr KeyCodeItem s_keyCodeItems[] = {
    /* The unknown key */
    {GLFW_KEY_UNKNOWN, KeyboardEvent::KeyCode::KEY_NONE},

    /* Printable keys */
    {GLFW_KEY_SPACE, KeyboardEvent::KeyCode::KEY_SPACE},
    {GLFW_KEY_APOSTROPHE, KeyboardEvent::KeyCode::KEY_APOSTROPHE},
    {GLFW_KEY_COMMA, KeyboardEvent::KeyCode::KEY_COMMA},
    {GLFW_KEY_MINUS, KeyboardEvent::KeyCode::KEY_MINUS},
    {GLFW_KEY_PERIOD, KeyboardEvent::KeyCode::KEY_PERIOD},
    {GLFW_KEY_SLASH, KeyboardEvent::KeyCode::KEY_SLASH},
    {GLFW_KEY_0, KeyboardEvent::KeyCode::KEY_0},
    {GLFW_KEY_1, KeyboardEvent::KeyCode::KEY_1},
    {GLFW_KEY_2, KeyboardEvent::KeyCode::KEY_2},
    {GLFW_KEY_3, KeyboardEvent::KeyCode::KEY_3},
    {GLFW_KEY_4, KeyboardEvent::KeyCode::KEY_4},
    {GLFW_KEY_5, KeyboardEvent::KeyCode::KEY_5},
    {GLFW_KEY_6, KeyboardEvent::KeyCode::KEY_6},
    {GLFW_KEY_7, KeyboardEvent::KeyCode::KEY_7},
    {GLFW_KEY_8, KeyboardEvent::KeyCode::KEY_8},
    {GLFW_KEY_9, KeyboardEvent::KeyCode::KEY_9},
    {GLFW_KEY_SEMICOLON, KeyboardEvent::KeyCode::KEY_SEMICOLON},
    {GLFW_KEY_EQUAL, KeyboardEvent::KeyCode::KEY_EQUAL},
    {GLFW_KEY_A, KeyboardEvent::KeyCode::KEY_A},
    {GLFW_KEY_B, KeyboardEvent::KeyCode::KEY_B},
    {GLFW_KEY_C, KeyboardEvent::KeyCode::KEY_C},
    {GLFW_KEY_D, KeyboardEvent::KeyCode::KEY_D},
    {GLFW_KEY_E, KeyboardEvent::KeyCode::KEY_E},
    {GLFW_KEY_F, KeyboardEvent::KeyCode::KEY_F},
    {GLFW_KEY_G, KeyboardEvent::KeyCode::KEY_G},
    {GLFW_KEY_H, KeyboardEvent::KeyCode::KEY_H},
    {GLFW_KEY_I, KeyboardEvent::KeyCode::KEY_I},
    {GLFW_KEY_J, KeyboardEvent::KeyCode::KEY_J},
    {GLFW_KEY_K, KeyboardEvent::KeyCode::KEY_K},
    {GLFW_KEY_L, KeyboardEvent::KeyCode::KEY_L},
    {GLFW_KEY_M, KeyboardEvent::KeyCode::KEY_M},
    {GLFW_KEY_N, KeyboardEvent::KeyCode::KEY_N},
    {GLFW_KEY_O, KeyboardEvent::KeyCode::KEY_O},
    {GLFW_KEY_P, KeyboardEvent::KeyCode::KEY_P},
    {GLFW_KEY_Q, KeyboardEvent::KeyCode::KEY_Q},
    {GLFW_KEY_R, KeyboardEvent::KeyCode::KEY_R},
    {GLFW_KEY_S, KeyboardEvent::KeyCode::KEY_S},
    {GLFW_KEY_T, KeyboardEvent::KeyCode::KEY_T},
    {GLFW_KEY_U, KeyboardEvent::KeyCode::KEY_U},
    {GLFW_KEY_V, KeyboardEvent::KeyCode::KEY_V},
    {GLFW_KEY_W, KeyboardEvent::KeyCode::KEY_W},
    {GLFW_KEY_X, KeyboardEvent::KeyCode::KEY_X},
    {GLFW_KEY_Y, KeyboardEvent::KeyCode::KEY_Y},
    {GLFW_KEY_Z, KeyboardEvent::KeyCode::KEY_Z},
    {GLFW_KEY_LEFT_BRACKET, KeyboardEvent::KeyCode::KEY_LEFT_BRACKET},
    {GLFW_KEY_BACKSLASH, KeyboardEvent::KeyCode::KEY_BACK_SLASH},
    {GLFW_KEY_RIGHT_BRACKET, KeyboardEvent::KeyCode::KEY_RIGHT_BRACKET},
    {GLFW_KEY_GRAVE_ACCENT, KeyboardEvent::KeyCode::KEY_GRAVE},
    {GLFW_KEY_WORLD_1, KeyboardEvent::KeyCode::KEY_GRAVE},
    {GLFW_KEY_WORLD_2, KeyboardEvent::KeyCode::KEY_NONE},

    /* Function keys */
    {GLFW_KEY_ESCAPE, KeyboardEvent::KeyCode::KEY_ESCAPE},
    {GLFW_KEY_ENTER, KeyboardEvent::KeyCode::KEY_ENTER},
    {GLFW_KEY_TAB, KeyboardEvent::KeyCode::KEY_TAB},
    {GLFW_KEY_BACKSPACE, KeyboardEvent::KeyCode::KEY_BACKSPACE},
    {GLFW_KEY_INSERT, KeyboardEvent::KeyCode::KEY_INSERT},
    {GLFW_KEY_DELETE, KeyboardEvent::KeyCode::KEY_DELETE},
    {GLFW_KEY_RIGHT, KeyboardEvent::KeyCode::KEY_RIGHT_ARROW},
    {GLFW_KEY_LEFT, KeyboardEvent::KeyCode::KEY_LEFT_ARROW},
    {GLFW_KEY_DOWN, KeyboardEvent::KeyCode::KEY_DOWN_ARROW},
    {GLFW_KEY_UP, KeyboardEvent::KeyCode::KEY_UP_ARROW},
    {GLFW_KEY_PAGE_UP, KeyboardEvent::KeyCode::KEY_PG_UP},
    {GLFW_KEY_PAGE_DOWN, KeyboardEvent::KeyCode::KEY_PG_DOWN},
    {GLFW_KEY_HOME, KeyboardEvent::KeyCode::KEY_HOME},
    {GLFW_KEY_END, KeyboardEvent::KeyCode::KEY_END},
    {GLFW_KEY_CAPS_LOCK, KeyboardEvent::KeyCode::KEY_CAPS_LOCK},
    {GLFW_KEY_SCROLL_LOCK, KeyboardEvent::KeyCode::KEY_SCROLL_LOCK},
    {GLFW_KEY_NUM_LOCK, KeyboardEvent::KeyCode::KEY_NUM_LOCK},
    {GLFW_KEY_PRINT_SCREEN, KeyboardEvent::KeyCode::KEY_PRINT},
    {GLFW_KEY_PAUSE, KeyboardEvent::KeyCode::KEY_PAUSE},
    {GLFW_KEY_F1, KeyboardEvent::KeyCode::KEY_F1},
    {GLFW_KEY_F2, KeyboardEvent::KeyCode::KEY_F2},
    {GLFW_KEY_F3, KeyboardEvent::KeyCode::KEY_F3},
    {GLFW_KEY_F4, KeyboardEvent::KeyCode::KEY_F4},
    {GLFW_KEY_F5, KeyboardEvent::KeyCode::KEY_F5},
    {GLFW_KEY_F6, KeyboardEvent::KeyCode::KEY_F6},
    {GLFW_KEY_F7, KeyboardEvent::KeyCode::KEY_F7},
    {GLFW_KEY_F8, KeyboardEvent::KeyCode::KEY_F8},
    {GLFW_KEY_F9, KeyboardEvent::KeyCode::KEY_F9},
    {GLFW_KEY_F10, KeyboardEvent::KeyCode::KEY_F10},
    {GLFW_KEY_F11, KeyboardEvent::KeyCode::KEY_F11},
    {GLFW_KEY_F12, KeyboardEvent::KeyCode::KEY_F12},
    {GLFW_KEY_F13, KeyboardEvent::KeyCode::KEY_NONE},
    {GLFW_KEY_F14, KeyboardEvent::KeyCode::KEY_NONE},
    {GLFW_KEY_F15, KeyboardEvent::KeyCode::KEY_NONE},
    {GLFW_KEY_F16, KeyboardEvent::KeyCode::KEY_NONE},
    {GLFW_KEY_F17, KeyboardEvent::KeyCode::KEY_NONE},
    {GLFW_KEY_F18, KeyboardEvent::KeyCode::KEY_NONE},
    {GLFW_KEY_F19, KeyboardEvent::KeyCode::KEY_NONE},
    {GLFW_KEY_F20, KeyboardEvent::KeyCode::KEY_NONE},
    {GLFW_KEY_F21, KeyboardEvent::KeyCode::KEY_NONE},
    {GLFW_KEY_F22, KeyboardEvent::KeyCode::KEY_NONE},
    {GLFW_KEY_F23, KeyboardEvent::KeyCode::KEY_NONE},
    {GLFW_KEY_F24, KeyboardEvent::KeyCode::KEY_NONE},
    {GLFW_KEY_F25, KeyboardEvent::KeyCode::KEY_NONE},
    {GLFW_KEY_KP_0, KeyboardEvent::KeyCode::KEY_0},
    {GLFW_KEY_KP_1, KeyboardEvent::KeyCode::KEY_1},
    {GLFW_KEY_KP_2, KeyboardEvent::KeyCode::KEY_2},
    {GLFW_KEY_KP_3, KeyboardEvent::KeyCode::KEY_3},
    {GLFW_KEY_KP_4, KeyboardEvent::KeyCode::KEY_4},
    {GLFW_KEY_KP_5, KeyboardEvent::KeyCode::KEY_5},
    {GLFW_KEY_KP_6, KeyboardEvent::KeyCode::KEY_6},
    {GLFW_KEY_KP_7, KeyboardEvent::KeyCode::KEY_7},
    {GLFW_KEY_KP_8, KeyboardEvent::KeyCode::KEY_8},
    {GLFW_KEY_KP_9, KeyboardEvent::KeyCode::KEY_9},
    {GLFW_KEY_KP_DECIMAL, KeyboardEvent::KeyCode::KEY_PERIOD},
    {GLFW_KEY_KP_DIVIDE, KeyboardEvent::KeyCode::KEY_KP_DIVIDE},
    {GLFW_KEY_KP_MULTIPLY, KeyboardEvent::KeyCode::KEY_KP_MULTIPLY},
    {GLFW_KEY_KP_SUBTRACT, KeyboardEvent::KeyCode::KEY_KP_MINUS},
    {GLFW_KEY_KP_ADD, KeyboardEvent::KeyCode::KEY_KP_PLUS},
    {GLFW_KEY_KP_ENTER, KeyboardEvent::KeyCode::KEY_KP_ENTER},
    {GLFW_KEY_KP_EQUAL, KeyboardEvent::KeyCode::KEY_EQUAL},
    {GLFW_KEY_LEFT_SHIFT, KeyboardEvent::KeyCode::KEY_LEFT_SHIFT},
    {GLFW_KEY_LEFT_CONTROL, KeyboardEvent::KeyCode::KEY_LEFT_CTRL},
    {GLFW_KEY_LEFT_ALT, KeyboardEvent::KeyCode::KEY_LEFT_ALT},
    {GLFW_KEY_LEFT_SUPER, KeyboardEvent::KeyCode::KEY_HYPER},
    {GLFW_KEY_RIGHT_SHIFT, KeyboardEvent::KeyCode::KEY_RIGHT_SHIFT},
    {GLFW_KEY_RIGHT_CONTROL, KeyboardEvent::KeyCode::KEY_RIGHT_CTRL},
    {GLFW_KEY_RIGHT_ALT, KeyboardEvent::KeyCode::KEY_RIGHT_ALT},
    {GLFW_KEY_RIGHT_SUPER, KeyboardEvent::KeyCode::KEY_HYPER},
    {GLFW_KEY_MENU, KeyboardEvent::KeyCode::KEY_MENU},
    {GLFW_KEY_LAST, KeyboardEvent::KeyCode::KEY_NONE}};

// wasm input bridge
#if defined(__EMSCRIPTEN__)
extern "C" {

/**
 * Invoked by JavaScript to feed the finalized UTF-8 text into the engine's InputSystem.
 * This allows any active text input control (like InputField) to automatically receive characters.
 */
EMSCRIPTEN_KEEPALIVE void axmol_onwebinserttext(const char* utf8Text, int length)
{
    if (utf8Text && length > 0)
    {
        // Forward the string to the global dispatcher, which handles the active focused widget
        ax::InputSystem::getInstance()->dispatchInsertText(std::string_view(utf8Text, length));
    }
}

EMSCRIPTEN_KEEPALIVE void axmol_onwebdeletebackward(int count)
{
    if (count > 0)
        ax::InputSystem::getInstance()->dispatchDeleteBackward(static_cast<unsigned int>(count));
}

EMSCRIPTEN_KEEPALIVE void
axmol_onwebpointerevent(int type, int id, float x, float y, float pressure, int pointerType, int button, int buttons)
{
    auto inputSystem = ax::InputSystem::getInstance();

    // Map Web pointer types to the strictly defined Engine PointerType enum
    ax::PointerType mappedType = ax::PointerType::Mouse;
    if (pointerType == 1)
        mappedType = ax::PointerType::Touch;
    else if (pointerType == 2)
        mappedType = ax::PointerType::Pen;

    // itmask Validation (Beautiful Coincidence)
    // The W3C 'buttons' bitmask (1=Left, 2=Right, 4=Middle) perfectly aligns
    // with the engine's '(1 << NativeButtonIndex)' logic:
    // Left(0): 1<<0 = 1 | Right(1): 1<<1 = 2 | Middle(2): 1<<2 = 4
    // Therefore, we can safely cast and pass the raw JS bitmask directly.
    uint32_t activeButtonsMask = static_cast<uint32_t>(buttons);

    // Map button, buttons bitmask identical to axmol, but button not
    if (button == 1)
        button = 2;  // Axmol Middle
    else if (button == 2)
        button = 1;  // Axmol Right

    // Assemble the modernized cohesive PointerInputState
    // W3C Pointer Events button values are aligned with both GLFW mouse button indices and Axmol’s InputButton
    // enumeration.
    ax::PointerInputState pointerState{.id             = static_cast<intptr_t>(id),
                                       .pressure       = pressure,
                                       .button         = button,
                                       .pressedButtons = activeButtonsMask,
                                       .type           = mappedType};

    // Bundle coordinates into a single Vector payload
    ax::Vec2 pos(x, y);

    // 5. Dispatch seamlessly into the unified core engine pipeline
    switch (type)
    {
    case 0:
        inputSystem->handlePointerDown(pos, pointerState);
        break;
    case 1:
        // Contract enforcement from Event.h:
        // "For PointerMove events the button value MUST be -1."
        // While W3C usually sends -1 for moves, we force it here to guarantee architecture safety.
        pointerState.button = ax::InputButton::None;
        inputSystem->handlePointerMove(pos, pointerState);
        break;
    case 2:
        inputSystem->handlePointerUp(pos, pointerState);
        break;
    case 3:
        inputSystem->handlePointerCancel(pos, pointerState);
        break;
    }
}

// Expose a scroll entry point for the Web bridge.
// Parameters:
//   id        - pointer id (if available from the browser), otherwise 0 for mouse
//   x, y      - canvas-local coordinates (pixels)
//   deltaX/Y  - normalized scroll delta in pixels (bridge should normalize deltaMode)
//   pointerType - 0=mouse,1=touch,2=pen
//   buttons   - current buttons bitmask (W3C 'buttons' bitmask)
EMSCRIPTEN_KEEPALIVE void
axmol_onwebpointerscroll(int id, float x, float y, float deltaX, float deltaY, int pointerType, int buttons)
{
    auto inputSystem = ax::InputSystem::getInstance();

    // Map pointerType to engine enum
    ax::PointerType mappedType = ax::PointerType::Mouse;
    if (pointerType == 1)
        mappedType = ax::PointerType::Touch;
    else if (pointerType == 2)
        mappedType = ax::PointerType::Pen;

    // Build a minimal PointerInputState for scroll events.
    // button is None for pure scroll; pressedButtons carries the active buttons mask.
    ax::PointerInputState pointerState{.id             = static_cast<intptr_t>(id),
                                       .pressure       = 0.0f,
                                       .button         = ax::InputButton::None,
                                       .pressedButtons = static_cast<uint32_t>(buttons),
                                       .type           = mappedType};

    // Convert coordinates to Vec2 and forward to InputSystem
    ax::Vec2 pos(x, y);
    ax::Vec2 scrollDelta(deltaX, deltaY);

    // Forward to existing scroll handler
    inputSystem->handlePointerScroll(pos, scrollDelta, pointerState);
}
}

static bool isWebInputFieldProxyFocused()
{
    // clang-format off
    return MAIN_THREAD_EM_ASM_INT({
        var proxy = Module['axmol_ime_proxy'];
        return !!(proxy && document.activeElement === proxy &&
                proxy.getAttribute('data-mode') === 'inputfield');
    }) != 0;
    // clang-format on
}

static void initWebInputBridge()
{
    // clang-format off

    // Execute inline JavaScript on the main thread to dynamically bind standard
    // W3C PointerEvents directly to the engine's active HTML5 Canvas element.
    MAIN_THREAD_EM_ASM(({
        var canvas = Module['canvas'];
        if (!canvas) {
            console.error("[Axmol] Target Canvas not found. PointerEvents initialization aborted.");
            return;
        }

        // Enable programmatic focus (Required to steal focus from input box)
        if (!canvas.hasAttribute("tabindex")) {
            canvas.setAttribute("tabindex", "0");
        }

        // Obliterate the ugly browser focus outline completely
        //    This kills the blue/black glowing borders when canvas.focus() is called!
        canvas.style.outline = "none";
        canvas.style.boxShadow = "none"; // Guards against some specific WebKit/Safari shadows
        canvas.style.webkitTapHighlightColor = "rgba(0,0,0,0)"; // Disables flash on mobile touch

        // Centralized event translator and forwarder
        // Centralized event translator and forwarder
        var mousePointerActive = false;

        function dispatchToPointerSystem(e, eventType, forcedPointerType) {
            var editboxInput = Module.axmol_editbox_input;
            if (editboxInput && e.target === editboxInput) {
                return;
            }

            if (eventType === 0 && editboxInput && document.activeElement === editboxInput) {
                if (canvas) {
                    canvas.focus();
                }
            }

            // Keep preventDefault() active for general canvas clicks to guard the game
            // against webpage zooming, scrolling, and blue highlight selections.
            if (e.cancelable) {
                // Suppress browser default behaviors (such as pinch-to-zoom, rubber-banding, or scrolling)
                e.preventDefault();
            }

            // Transform viewport coordinates (clientX/Y) into precise, local Canvas pixel coordinates.
            // This safely normalizes any CSS resizing, high-DPI Retina scaling, or full-screen layout shifts.
            var rect = canvas.getBoundingClientRect();
            var canvasX = (e.clientX - rect.left);
            var canvasY = (e.clientY - rect.top);

            // Handle pressure telemetry and deploy the architecture safety firewall:
            // Non-pressure devices or standard mice return 0.0f by default during execution.
            // If the pointer is actively pressed down and reports 0, forcefully elevate it to 1.0f.
            var rawPressure = typeof e.pressure === 'number' ? e.pressure : 0.0;
            if (e.buttons > 0 && rawPressure === 0) {
                rawPressure = 1.0;
            }

            var pointerType = forcedPointerType || e.pointerType || 'mouse';
            var pointerId = pointerType === 'mouse' ? 0 : e.pointerId;

            // Identify device type: 0 = mouse, 1 = touch, 2 = pen
            var ptrType = 1;
            if (pointerType === 'mouse') ptrType = 0;
            else if (pointerType === 'pen') ptrType = 2;

            // Cross the WebAssembly boundary to shoot the telemetry directly into C++ core
            // Appended parameters: ptrType (device type), e.button (triggering button), e.buttons (active button bitmask)
            Module._axmol_onwebpointerevent(eventType, pointerId, canvasX, canvasY, rawPressure, ptrType, e.button, e.buttons || 0);
        }

        // Bind Down Event: Triggers pointer locking/capturing for boundary-proof dragging
        canvas.addEventListener('pointerdown', function(e) {
            // Lock the pointer context to the canvas. Subsequent 'move' and 'up' events
            // will continue tracking safely even if the user drags completely outside the browser window.
            try {
                canvas.setPointerCapture(e.pointerId);
            } catch (err) {
                // Fail-safe catch block for older or restrictive browser sandboxes
            }
            if (e.pointerType === 'mouse') {
                return;
            }
            dispatchToPointerSystem(e, 0);
        }, {passive: false});

        // Bind Move Event: Streams high-frequency telemetry including structural pressure fluctuations
        canvas.addEventListener('pointermove', function(e) {
            dispatchToPointerSystem(e, 1);
        }, {passive: false});

        // Bind Up Event: Automatically triggers pointer release via browser specifications
        canvas.addEventListener('pointerup', function(e) {
            // Note: browser internally executes releasePointerCapture(e.pointerId) upon pointerup
            if (e.pointerType === 'mouse') {
                return;
            }
            dispatchToPointerSystem(e, 2);
        }, {passive: false});

        // Bind Cancel Event: Intercepts OS-level interruptions (e.g., system popups or notifications)
        canvas.addEventListener('pointercancel', function(e) {
            if (e.pointerType === 'mouse') {
                return;
            }
            dispatchToPointerSystem(e, 3);
        }, {passive: false});

        // PointerEvents intentionally do not emit pointerdown/pointerup for every
        // chorded mouse button transition. Use mouse down/up for per-button
        // lifecycle parity with GLFW/native desktop, while pointermove remains the
        // unified movement stream.
        canvas.addEventListener('mousedown', function(e) {
            mousePointerActive = true;
            dispatchToPointerSystem(e, 0, 'mouse');
        }, {passive: false});

        window.addEventListener('mouseup', function(e) {
            if (!mousePointerActive) {
                return;
            }
            dispatchToPointerSystem(e, 2, 'mouse');
            if ((e.buttons || 0) === 0) {
                mousePointerActive = false;
            }
        }, {passive: false});

        // Bind Wheel Event: normalize deltaMode and forward as PointerScroll to the engine.
        // Note: wheel events may not always include pointerId; fall back to 0 (mouse).
        canvas.addEventListener('wheel', function(e) {
            // Prevent page scrolling / pinch-zoom when interacting with the canvas.
            if (e.cancelable) e.preventDefault();

            // Compute canvas-local coordinates
            var rect = canvas.getBoundingClientRect();
            var canvasX = (e.clientX - rect.left);
            var canvasY = (e.clientY - rect.top);

            // Normalize deltaMode to pixels:
            // 0 = DOM_DELTA_PIXEL, 1 = DOM_DELTA_LINE, 2 = DOM_DELTA_PAGE
            // Use reasonable fallbacks: line -> 16px, page -> viewport height.
            var deltaX = e.deltaX;
            var deltaY = e.deltaY;
            if (e.deltaMode === 1) { // lines
                var LINE_HEIGHT = 16; // conservative default line height in pixels
                deltaX *= LINE_HEIGHT;
                deltaY *= LINE_HEIGHT;
            } else if (e.deltaMode === 2) { // pages
                deltaX *= window.innerHeight;
                deltaY *= window.innerHeight;
            }

            // Determine pointerId if available (some browsers include pointerId on wheel events)
            var pid = (typeof e.pointerId !== 'undefined') ? e.pointerId : 0;

            // Determine device type mapping (0=mouse,1=touch,2=pen)
            var ptrType = 0;
            if (e.pointerType === 'touch') ptrType = 1;
            else if (e.pointerType === 'pen') ptrType = 2;
            // If pointerType is not present on wheel events, assume mouse (0).

            // Forward normalized scroll to native layer
            // Module._axmol_onwebpointerscroll(id, x, y, deltaX, deltaY, ptrType, buttons)
            Module._axmol_onwebpointerscroll(pid, canvasX, canvasY, deltaX, deltaY, ptrType, e.buttons || 0);
        }, { passive: false });

        console.log("[Axmol] Unified PointerEvents successfully attached to Canvas. Legacy GLFW mouse/touch bypassed.");
    }));

    // Inject the shadow HTML input bridge to intercept native browser IME events
    MAIN_THREAD_EM_ASM(({
        // 1. Rename to standardized proxy name
        if (!Module['axmol_ime_proxy']) {

            var stringToUTF8WithLen = function(str) {
                var byteLen = lengthBytesUTF8(str);
                var ptr = _malloc(byteLen + 1);
                stringToUTF8(str, ptr, byteLen + 1);
                return { ptr: ptr, length: byteLen };
            };
            Module.stringToUTF8WithLen = stringToUTF8WithLen;

            // Initialize as standard text input
            var proxy = document.createElement('input');
            proxy.type = 'text';
            proxy.id = 'axmol_ime_proxy';

            // Hide globally but keep it in DOM
            proxy.style.position = 'absolute';
            proxy.style.opacity = '0';
            proxy.style.left = '-9999px';
            proxy.style.top = '-9999px';
            proxy.style.zIndex = '-1';
            proxy.style.pointerEvents = 'none'; // Ensure clicks pass through when in ghost mode

            document.body.appendChild(proxy);
            Module['axmol_ime_proxy'] = proxy;

            var isComposing = false;
            var suppressNextDeleteInput = false;

            proxy.addEventListener('compositionstart', function() {
                isComposing = true;
            });

            proxy.addEventListener('compositionend', function(e) {
                isComposing = false;
                // Only dispatch IME if not hijacked by EditBox UI
                if (e.data && proxy.getAttribute('data-mode') !== 'editbox') {
                    var result = stringToUTF8WithLen(e.data);
                    Module._axmol_onwebinserttext(result.ptr, result.length);
                    _free(result.ptr);
                    proxy.value = "";
                }
            });

            proxy.addEventListener('keydown', function(e) {
                if (proxy.getAttribute('data-mode') !== 'inputfield') return;
                if (isComposing) return;

                if (e.key === 'Backspace') {
                    if (e.cancelable) {
                        e.preventDefault();
                    }
                    Module._axmol_onwebdeletebackward(1);
                    proxy.value = "";
                    suppressNextDeleteInput = true;
                    setTimeout(function() {
                        suppressNextDeleteInput = false;
                    }, 0);
                }
            }, { passive: false });

            proxy.addEventListener('beforeinput', function(e) {
                if (proxy.getAttribute('data-mode') !== 'inputfield') return;
                if (isComposing) return;

                if (e.inputType === 'deleteContentBackward') {
                    if (e.cancelable) {
                        e.preventDefault();
                    }
                    if (suppressNextDeleteInput) {
                        suppressNextDeleteInput = false;
                        proxy.value = "";
                        return;
                    }
                    Module._axmol_onwebdeletebackward(1);
                    proxy.value = "";
                }
            }, { passive: false });

            proxy.addEventListener('input', function(e) {
                // Only dispatch regular input if not hijacked by EditBox UI
                if (proxy.getAttribute('data-mode') === 'editbox') return;

                if (!isComposing && e.inputType !== 'deleteContentBackward') {
                    if (proxy.value) {
                        var result = stringToUTF8WithLen(proxy.value);
                        Module._axmol_onwebinserttext(result.ptr, result.length);
                        _free(result.ptr);
                    }
                    proxy.value = "";
                } else if (!isComposing && e.inputType === 'deleteContentBackward') {
                    if (suppressNextDeleteInput) {
                        suppressNextDeleteInput = false;
                        proxy.value = "";
                        return;
                    }
                    Module._axmol_onwebdeletebackward(1);
                    proxy.value = "";
                }
            });
        }
    }));
    // clang-format on
}

#endif

//////////////////////////////////////////////////////////////////////////
// implement RenderView
//////////////////////////////////////////////////////////////////////////

RenderView::RenderView(bool initglfw) : _windowZoomFactor(1.0f), _mainWindow(nullptr), _monitor(nullptr)
{
    _viewName = "axmol3";
    for (auto&& item : s_keyCodeItems)
    {
        _keyCodeMap[item.glfwKeyCode] = item.keyCode;
    }

    GLFWEventHandler::setRenderView(this);
    if (initglfw)
    {
        glfwSetErrorCallback(GLFWEventHandler::onGLFWError);
        glfwInit();
    }
}

RenderView::~RenderView()
{
    AXLOGD("deallocing RenderView: {}", fmt::ptr(this));
    GLFWEventHandler::setRenderView(nullptr);
    glfwTerminate();
}

void* RenderView::getNativeWindow() const
{
#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
    return glfwGetWin32Window(_mainWindow);
#elif AX_TARGET_PLATFORM == AX_PLATFORM_MAC
    return (void*)glfwGetCocoaWindow(_mainWindow);
#elif AX_TARGET_PLATFORM == AX_PLATFORM_LINUX
#    if defined(AX_ENABLE_WAYLAND)
    int platform = glfwGetPlatform();
    return platform == GLFW_PLATFORM_WAYLAND ? (void*)glfwGetWaylandWindow(_mainWindow)
                                             : (void*)glfwGetX11Window(_mainWindow);
#    else
    return (void*)glfwGetX11Window(_mainWindow);
#    endif
#else
    return nullptr;
#endif
}

SurfaceHandle RenderView::getNativeDisplay() const
{
    auto driverType = DriverContext::currentDriverType();
    if (driverType == DriverType::Vulkan)
        return _vkSurface;

#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
    return glfwGetWin32Window(_mainWindow);
#elif AX_TARGET_PLATFORM == AX_PLATFORM_MAC
    return driverType == DriverType::Metal ? (void*)glfwGetCocoaView(_mainWindow)
                                           : (void*)glfwGetNSGLContext(_mainWindow);
    return (void*)glfwGetNSGLContext(_mainWindow);
#elif AX_TARGET_PLATFORM == AX_PLATFORM_LINUX
#    if defined(AX_ENABLE_WAYLAND)
    int platform = glfwGetPlatform();
    return platform == GLFW_PLATFORM_WAYLAND ? (void*)glfwGetWaylandDisplay() : (void*)glfwGetX11Display();
#    else
    return (void*)glfwGetX11Display();
#    endif
#else
    return nullptr;
#endif
}

WindowPlatform RenderView::getWindowPlatform() const
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

RenderView* RenderView::create(std::string_view viewName)
{
    return RenderView::create(viewName, false);
}

RenderView* RenderView::create(std::string_view viewName, bool resizable)
{
    auto ret = new RenderView;
    if (ret->initWithRect(viewName, ax::Rect(0, 0, 960, 640), 1.0f, resizable))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

RenderView* RenderView::createWithRect(std::string_view viewName,
                                       const ax::Rect& rect,
                                       float windowZoomFactor,
                                       bool resizable)
{
    auto ret = new RenderView;
    if (ret->initWithRect(viewName, rect, windowZoomFactor, resizable))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

RenderView* RenderView::createWithFullscreen(std::string_view viewName)
{
    auto ret = new RenderView();
    if (ret->initWithFullScreen(viewName))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

RenderView* RenderView::createWithFullscreen(std::string_view viewName,
                                             const GLFWvidmode& videoMode,
                                             GLFWmonitor* monitor)
{
    auto ret = new RenderView();
    if (ret->initWithFullscreen(viewName, videoMode, monitor))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

bool RenderView::initWithRect(std::string_view viewName, const ax::Rect& rect, float windowZoomFactor, bool resizable)
{
    _viewName         = viewName;
    _windowZoomFactor = windowZoomFactor;

    Vec2 requestWinSize = rect.size * windowZoomFactor;

    // Try to initialize a high-performance graphics driver first.
    // If any of the high-performance APIs (D3D11/D3D12/Vulkan/Metal) are enabled,
    // the runtime will attempt initialization in the default priority order.
    // If all attempts fail, OpenGL will then be explicitly selected as the fallback.
    DriverContext::makeCurrentDriver();
    const auto fallbackGL = DriverContext::isOpenGL();
    if (fallbackGL)
    {
#if AX_GLES_PROFILE
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
        glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, AX_GLES_PROFILE / AX_GLES_PROFILE_DEN);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#else
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  // We want OpenGL 3.3
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // We don't want the old OpenGL
#endif
    }
    else  // Other Graphics driver, don't create gl context.
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    auto& contextAttrs = Application::getContextAttrs();

    glfwWindowHint(GLFW_RESIZABLE, resizable ? GL_TRUE : GL_FALSE);
    glfwWindowHint(GLFW_RED_BITS, contextAttrs.redBits);
    glfwWindowHint(GLFW_GREEN_BITS, contextAttrs.greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, contextAttrs.blueBits);
    glfwWindowHint(GLFW_ALPHA_BITS, contextAttrs.alphaBits);
    glfwWindowHint(GLFW_DEPTH_BITS, contextAttrs.depthBits);
    glfwWindowHint(GLFW_STENCIL_BITS, contextAttrs.stencilBits);

    glfwWindowHint(GLFW_SAMPLES, contextAttrs.multisamplingCount);

    const auto requireShowByUser = contextAttrs.visible;
    glfwWindowHint(GLFW_VISIBLE, false);
    glfwWindowHint(GLFW_DECORATED, contextAttrs.decorated);

#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32)
    glfwWindowHintPointer(GLFW_WIN32_HWND_PARENT, contextAttrs.windowParent);
#endif

    _renderScaleMode = contextAttrs.renderScaleMode;
#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32 || AX_TARGET_PLATFORM == AX_PLATFORM_LINUX || \
    AX_TARGET_PLATFORM == AX_PLATFORM_WASM
    // On Linux X11 platforms, GLFW does not support fractional DPI scaling (e.g., 1.5x).
    // To ensure consistent rendering across high-DPI displays, we disable GLFW_SCALE_TO_MONITOR
    // and apply custom scaling logic based on platform-specific DPI detection.
    // GLFW_SCALE_TO_MONITOR support Win32, X11, Wasm
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, _renderScaleMode == RenderScaleMode::Physical ? GLFW_TRUE : GLFW_FALSE);
#endif

    _mainWindow =
        glfwCreateWindow(static_cast<int>(std::lround(requestWinSize.width)),
                         static_cast<int>(std::lround(requestWinSize.height)), _viewName.c_str(), _monitor, nullptr);
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

    glfwSetWindowSizeLimits(_mainWindow, 1, 1, GLFW_DONT_CARE, GLFW_DONT_CARE);

#if AX_ENABLE_GL
    if (fallbackGL)
    {
        glfwMakeContextCurrent(_mainWindow);
        DriverContext::activateCurrentDriver();

        glfwSetWindowUserPointer(_mainWindow, gl::__state);
    }
#endif

    if (requireShowByUser)
        glfwShowWindow(_mainWindow);

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
    updateRenderSurface(fbWidth, fbHeight, SurfaceUpdateFlag::RenderSizeChanged | SurfaceUpdateFlag::SilentUpdate);

#if AX_ENABLE_VK
    if (DriverContext::isVulkan())
    {
        auto _createSurface = [](VkInstance inst, void* window, VkSurfaceKHR* surface) {
            return glfwCreateWindowSurface(inst, static_cast<GLFWwindow*>(window), nullptr, surface);
        };
        auto driver = static_cast<vk::DriverImpl*>(axdrv);
        const vk::SurfaceCreateInfo createInfo{
            .window = _mainWindow, .width = fbWidth, .height = fbHeight, .createFunc = _createSurface};
        bool ok = driver->recreateSurface(createInfo);
        if (!ok)
        {
            AXLOGE("Failed to create Vulkan window surface.");
            return false;
        }
        _vkSurface = driver->getSurface();
    }
#endif

    int w, h;
    glfwGetWindowSize(_mainWindow, &w, &h);
    updateScaledWindowSize(w, h, SurfaceUpdateFlag::WindowSizeChanged | SurfaceUpdateFlag::SilentUpdate);

#if defined(__EMSCRIPTEN__)
    s_fullscreenState = std::make_unique<WebFullscreenState>();
    // clang-format off
    emscripten_set_orientationchange_callback(this, EM_TRUE, GLFWEventHandler::onWebOrientationChangeCallback);
    emscripten_set_fullscreenchange_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, this, EM_TRUE, GLFWEventHandler::onWebFullscreenCallback);

    _isTouchDevice = !!EM_ASM_INT(
        return window.matchMedia('(pointer: coarse)').matches && !window.matchMedia('(any-hover: hover)').matches;
    );
    const auto maxTouchPoints = EM_ASM_INT(
        return navigator.maxTouchPoints;
    );
    AXLOGI("RenderView::initWithRect: isTouchDevice: {}, maxTouchPoints: {}", _isTouchDevice, maxTouchPoints);

    initWebInputBridge();
    // clang-format on
#else
    glfwSetMouseButtonCallback(_mainWindow, GLFWEventHandler::onGLFWMouseCallBack);
    glfwSetCursorPosCallback(_mainWindow, GLFWEventHandler::onGLFWMouseMoveCallBack);
    glfwSetScrollCallback(_mainWindow, GLFWEventHandler::onGLFWMouseScrollCallback);
    glfwSetPreeditCallback(_mainWindow, GLFWEventHandler::onGLFWPreeditCallback);
#endif

    glfwSetCharCallback(_mainWindow, GLFWEventHandler::onGLFWCharCallback);
    glfwSetKeyCallback(_mainWindow, GLFWEventHandler::onGLFWKeyCallback);
    glfwSetWindowPosCallback(_mainWindow, GLFWEventHandler::onGLFWWindowPosCallback);
    glfwSetFramebufferSizeCallback(_mainWindow, GLFWEventHandler::onGLFWFramebufferSizeCallback);
    glfwSetWindowSizeCallback(_mainWindow, GLFWEventHandler::onGLFWWindowSizeCallback);
    glfwSetWindowIconifyCallback(_mainWindow, GLFWEventHandler::onGLFWWindowIconifyCallback);
    glfwSetWindowFocusCallback(_mainWindow, GLFWEventHandler::onGLFWWindowFocusCallback);
    glfwSetWindowCloseCallback(_mainWindow, GLFWEventHandler::onGLFWWindowCloseCallback);

    glfwSetCursorEnterCallback(_mainWindow, [](GLFWwindow* window, int entered) {
        bool flag = !!entered;
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(EVENT_WINDOW_CURSOR_ENTER, &flag);
    });

#if AX_ENABLE_GL
    if (fallbackGL)
    {
#    if !defined(__EMSCRIPTEN__)
        glfwSwapInterval(contextAttrs.vsync ? 1 : 0);
#    endif
        // Will cause OpenGL error 0x0500 when use ANGLE-GLES on desktop
#    if !AX_GLES_PROFILE
        // Enable point size by default.
#        if defined(GL_VERSION_2_0)
        glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
#        else
        glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);
#        endif
        if (contextAttrs.multisamplingCount > 0)
            glEnable(GL_MULTISAMPLE);
#    endif
        CHECK_GL_ERROR_DEBUG();
    }
#endif

    setIMEKeyboardState(false);

    return true;
}

bool RenderView::initWithFullScreen(std::string_view viewName)
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

bool RenderView::initWithFullscreen(std::string_view viewname, const GLFWvidmode& videoMode, GLFWmonitor* monitor)
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

void RenderView::setViewName(std::string_view viewName)
{
    RenderViewCore::setViewName(viewName);
    if (_mainWindow)
        glfwSetWindowTitle(_mainWindow, _viewName.c_str());
}

bool RenderView::isKeyPressed(int key) const
{
    return _mainWindow && glfwGetKey(_mainWindow, key) == GLFW_PRESS;
}

bool RenderView::isGfxContextReady()
{
    return nullptr != _mainWindow;
}

void RenderView::end()
{
    _vkSurface = nullptr;

    if (_mainWindow)
    {
        glfwSetWindowShouldClose(_mainWindow, 1);
        _mainWindow = nullptr;
    }
    // Release self. Otherwise, RenderView could not be freed.
    release();
}

void RenderView::swapBuffers()
{
#if AX_ENABLE_GL
    if (_mainWindow && DriverContext::isOpenGL())
        glfwSwapBuffers(_mainWindow);
#endif
}

bool RenderView::windowShouldClose()
{
    if (_mainWindow)
        return glfwWindowShouldClose(_mainWindow) ? true : false;
    else
        return true;
}

void RenderView::pollEvents()
{
    glfwPollEvents();
}

void RenderView::setIMEKeyboardState(bool bOpen)
{
    if (!_mainWindow)
        return;

    auto lastPointerPosition = InputSystem::getInstance()->getLastPointerPosition();

#if !defined(__EMSCRIPTEN__)
#    ifdef _WIN32
    ::glfwFocusWindow(_mainWindow);
#    endif
    glfwSetInputMode(_mainWindow, GLFW_IME, bOpen ? 1 : 0);

    if (bOpen)
        glfwSetPreeditCursorRectangle(_mainWindow, static_cast<int>(lastPointerPosition.x),
                                      static_cast<int>(lastPointerPosition.y), 1, 20);
#else
    // clang-format off
    // Synchronize browser DOM focus state with the engine's internal IME state
    // We position the invisible proxy element at the specific mouse coordinates
    EM_ASM({
        var proxy = Module['axmol_ime_proxy'];
        var canvas = Module['canvas'];

        if (proxy && canvas)
        {
            if ($0) // bOpen == true
            {
                // 1. Get the bounding box of the canvas in the viewport to get absolute offset
                var rect = canvas.getBoundingClientRect();

                // 2. Calculate the position relative to the viewport:
                // Canvas top-left + Mouse offset (passed as $1, $2)
                var styleX = (rect.left + $1) + 'px';
                var styleY = (rect.top + $2) + 'px';

                // 3. Set the proxy to fixed position at the exact mouse cursor location
                proxy.style.position = 'fixed';
                proxy.style.left = styleX;
                proxy.style.top = styleY;
                proxy.style.zIndex = '9999';

                // 4. Configure proxy for IME input (invisible, non-blocking)
                proxy.setAttribute('data-mode', 'inputfield');
                proxy.style.opacity = "0";
                proxy.style.pointerEvents = "none";
                proxy.value = "";
                proxy.focus();
            }
            else
            {
                // Only blur if InputField is currently holding the focus
                if (proxy.getAttribute('data-mode') === 'inputfield')
                {
                    proxy.blur();
                }
            }
        }
    },
    bOpen, (int)lastPointerPosition.x, (int)lastPointerPosition.y);
    // clang-format on
#endif
}

#if AX_ICON_SET_SUPPORT
void RenderView::setIcon(std::string_view filename) const
{
    this->setIcon({filename});
}

void RenderView::setIcon(std::span<const std::string_view> filelist) const
{
    if (filelist.empty())
        return;
    std::vector<Image*> icons;
    for (auto& filename : filelist)
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

void RenderView::setDefaultIcon() const
{
    GLFWwindow* window = this->getWindow();
    glfwSetWindowIcon(window, 0, nullptr);
}
#endif /* AX_ICON_SET_SUPPORT */

void RenderView::setCursorVisible(bool isVisible)
{
    if (_mainWindow == NULL)
        return;

    if (isVisible)
        glfwSetInputMode(_mainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    else
        glfwSetInputMode(_mainWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

float RenderView::getWindowZoomFactor() const
{
    return _windowZoomFactor;
}

bool RenderView::isFullscreen() const
{
    return (_monitor != nullptr);
}

void RenderView::setFullscreen()
{
    setFullscreen(-1, -1, -1);
}

void RenderView::setFullscreen(int w, int h, int refreshRate)
{
    auto monitor = glfwGetPrimaryMonitor();
    if (nullptr == monitor || monitor == _monitor)
    {
        return;
    }
    this->setFullscreen(monitor, w, h, refreshRate);
}

void RenderView::setFullscreen(int monitorIndex)
{
    setFullscreen(monitorIndex, -1, -1, -1);
}

void RenderView::setFullscreen(int monitorIndex, int w, int h, int refreshRate)
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

void RenderView::setFullscreen(GLFWmonitor* monitor, int w, int h, int refreshRate)
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

void RenderView::setWindowed(int width, int height, bool borderless)
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

Vec2 RenderView::getNativeWindowSize() const
{
    if (_mainWindow != nullptr)
    {
        int w = 0, h = 0;
        glfwGetWindowSize(_mainWindow, &w, &h);
        return Vec2(w, h);
    }
    return Vec2{};
}

void RenderView::getWindowPosition(int* xpos, int* ypos)
{
    if (_mainWindow != nullptr && getWindowPlatform() != WindowPlatform::Wayland)
        glfwGetWindowPos(_mainWindow, xpos, ypos);
}

int RenderView::getMonitorCount() const
{
    int count = 0;
    glfwGetMonitors(&count);
    return count;
}

Vec2 RenderView::getMonitorSize() const
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
    return Vec2::zero;
}

void RenderView::setWindowSizeLimits(int minwidth, int minheight, int maxwidth, int maxheight)
{
    if (_mainWindow == NULL)
        return;

    glfwSetWindowSizeLimits(_mainWindow, minwidth, minheight, maxwidth, maxheight);
}

void RenderView::onGLFWFramebufferSizeCallback(GLFWwindow* window, int fbWidth, int fbHeight)
{
    AXLOGD("RenderView::onGLFWFramebufferSizeCallback: ({}, {})", fbWidth, fbHeight);

    updateRenderSurface(fbWidth, fbHeight, SurfaceUpdateFlag::RenderSizeChanged);
}

void RenderView::onGLFWWindowSizeCallback(GLFWwindow* /*window*/, int w, int h)
{
    AXLOGD("RenderView::onGLFWWindowSizeCallback: ({}, {})", w, h);

    updateScaledWindowSize(w, h, SurfaceUpdateFlag::WindowSizeChanged);

    Size size(w, h);

    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(RenderView::EVENT_WINDOW_RESIZED, &size);
}

void RenderView::setWindowZoomFactor(float zoomFactor)
{
    AXASSERT(zoomFactor > 0.0f, "zoomFactor must be larger than 0");

    if (std::abs(_windowZoomFactor - zoomFactor) < FLT_EPSILON)
        return;

    _windowZoomFactor = zoomFactor;
    applyWindowSize();
}

void RenderView::setWindowSize(float width, float height)
{
    if (width == 0 || height == 0)
        return;
    Vec2 requestSize{width, height};
    if (requestSize.equals(_windowSize))
        return;

    _windowSize.set(width, height);
    applyWindowSize();
}

void RenderView::updateScaledWindowSize(int w, int h, uint8_t updateFlag)
{
    updateRenderScale();

    double scaledWidth  = w / (double)_windowZoomFactor;
    double scaledHeight = h / (double)_windowZoomFactor;

    // Translate to logical size on platforms where pixels and screen coordinates always map 1:1 (Win32, X11)
    // Note: wasm coordinates not map 1:1 when _renderScaleMode is RenderScaleMode::Physical
    if (_renderScaleMode == RenderScaleMode::Physical)
    {
        auto windowPlatform = getWindowPlatform();
        if (windowPlatform == WindowPlatform::Win32 || windowPlatform == WindowPlatform::X11)
        {
            const auto factor = (1 / (double)_renderScale);
            scaledWidth *= factor;
            scaledHeight *= factor;
        }
    }

    Vec2 scaledSize{static_cast<float>(std::round(scaledWidth)), static_cast<float>(std::round(scaledHeight))};
    if (!scaledSize.equals(_windowSize))
        updateRenderSurface(scaledSize.width, scaledSize.height, updateFlag);
}

void RenderView::applyWindowSize()
{
    double unscaledWidth  = _windowSize.width * _windowZoomFactor,
           unscaledHeight = _windowSize.height * _windowZoomFactor;
    // Translate to physical size on platforms where pixels and screen coordinates always map 1:1
    if (_renderScaleMode == RenderScaleMode::Physical)
    {
        auto windowPlatform = getWindowPlatform();
        if (windowPlatform == WindowPlatform::Win32 || windowPlatform == WindowPlatform::X11)
        {
            unscaledWidth *= _renderScale;
            unscaledHeight *= _renderScale;
        }
    }
    glfwSetWindowSize(_mainWindow, static_cast<int>(std::lround(unscaledWidth)),
                      static_cast<int>(std::lround(unscaledHeight)));

    // process platform that window size callback not trigger(wayland)
    maybeDispatchResizeEvent(SurfaceUpdateFlag::WindowSizeChanged);
}

/**
 * Updates the render scale and input scale factors based on the current platform
 * and render scale mode.
 *
 * - On platforms where screen coordinates map 1:1 to physical pixels (Win32, X11),
 *   high-DPI scaling is only applied when in Physical mode. In this case, _inputScale
 *   always 1.0
 *   .
 * - On other platforms (e.g., macOS, Wayland), _renderScale is still queried to adjust r
 *   endering for high-DPI displays (e.g., viewport size). and _inputScale shoud same with
 *   render scale to converts from screen coordinates to the render view's logical coordinate space
 *
 * This function uses glfwGetWindowContentScale() to retrieve the current content scale
 * factor, which may change when moving the window between monitors with different DPI
 * settings.
 *
 * renderScale: for computing logical window size
 * inputScale: for transform input axis
 */
void RenderView::updateRenderScale()
{
    float inputScale{1.0f};

    auto windowPlatform = getWindowPlatform();
    if (windowPlatform == WindowPlatform::Win32 || windowPlatform == WindowPlatform::X11 ||
        windowPlatform == WindowPlatform::Web)
    {
        if (_renderScaleMode == RenderScaleMode::Physical)
        {
            float ignoreVal;
            glfwGetWindowContentScale(_mainWindow, &_renderScale, &ignoreVal);
            inputScale = windowPlatform != WindowPlatform::Web ? 1.0f : _renderScale;
        }
        else
        {
            inputScale = _renderScale = 1.0f;
        }
    }
    else
    {
        float ignoreVal;
        glfwGetWindowContentScale(_mainWindow, &_renderScale, &ignoreVal);
        inputScale = _renderScale;
    }

    // Update InputSystem with the computed input scale so it can apply the
    // appropriate scaling when dispatching input events.
    InputSystem::getInstance()->setInputScale(inputScale);
}

void RenderView::onGLFWError(int errorID, const char* errorDesc)
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

#if !defined(__EMSCRIPTEN__)
void RenderView::onGLFWMouseCallBack(GLFWwindow* /*window*/, int button, int action, int /*mods*/)
{
    // TODO mods support
    auto inputSystem = InputSystem::getInstance();

    if (action == GLFW_PRESS)
    {
        _pressedButtons |= (1u << button);

        PointerInputState pointerData{
            .id = MOUSE_POINTER_ID, .button = button, .pressedButtons = _pressedButtons, .type = PointerType::Mouse};
        inputSystem->handlePointerDown(_mousePosition, pointerData);
    }
    else if (action == GLFW_RELEASE)
    {
        _pressedButtons &= ~(1u << button);

        PointerInputState pointerData{
            .id = MOUSE_POINTER_ID, .button = button, .pressedButtons = _pressedButtons, .type = PointerType::Mouse};
        inputSystem->handlePointerUp(_mousePosition, pointerData);
    }
}

void RenderView::onGLFWMouseMoveCallBack(GLFWwindow* window, double x, double y)
{
    _mousePosition.x = static_cast<float>(x);
    _mousePosition.y = static_cast<float>(y);

    PointerInputState pointerState{.id             = MOUSE_POINTER_ID,
                                   .button         = static_cast<int>(InputButton::None),
                                   .pressedButtons = _pressedButtons,
                                   .type           = PointerType::Mouse};
    InputSystem::getInstance()->handlePointerMove(_mousePosition, pointerState);
}

void RenderView::onGLFWMouseScrollCallback(GLFWwindow* window, double x, double y)
{
    PointerInputState pointerState{.id             = MOUSE_POINTER_ID,
                                   .button         = InputButton::None,
                                   .pressedButtons = _pressedButtons,
                                   .type           = PointerType::Mouse};

    InputSystem::getInstance()->handlePointerScroll(_mousePosition, Vec2{static_cast<float>(x), -static_cast<float>(y)},
                                                    pointerState);
}
#endif

void RenderView::onGLFWKeyCallback(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int /*mods*/)
{
    auto keyCode = _keyCodeMap[key];
#if defined(__EMSCRIPTEN__)
    if (isWebInputFieldProxyFocused() && keyCode == KeyboardEvent::KeyCode::KEY_BACKSPACE)
        return;
#endif

    InputPhase phase{};
    switch (action)
    {
    case GLFW_PRESS:
        phase = InputPhase::KeyDown;
        break;
    case GLFW_RELEASE:
        phase = InputPhase::KeyUp;
        break;
    case GLFW_REPEAT:
        phase = InputPhase::KeyRepeat;
        break;
    }

    InputSystem::getInstance()->handleKeyEvent(keyCode, phase);
}

void RenderView::onGLFWCharCallback(GLFWwindow* /*window*/, unsigned int charCode)
{
#if defined(__EMSCRIPTEN__)
    if (isWebInputFieldProxyFocused())
        return;
#endif

    // static std::unordered_set<std::string_view> controlUnicode = {
    //     "\xEF\x9C\x80",  // up
    //     "\xEF\x9C\x81",  // down
    //     "\xEF\x9C\x82",  // left
    //     "\xEF\x9C\x83",  // right
    //     "\xEF\x9C\xA8",  // delete
    //     "\xEF\x9C\xA9",  // home
    //     "\xEF\x9C\xAB",  // end
    //     "\xEF\x9C\xAC",  // pageup
    //     "\xEF\x9C\xAD",  // pagedown
    //     "\xEF\x9C\xB9"   // clear
    // };

    static const std::unordered_set<char32_t> controlUnicode = {
        U'\uF700',  // up
        U'\uF701',  // down
        U'\uF702',  // left
        U'\uF703',  // right
        U'\uF728',  // delete
        U'\uF729',  // home
        U'\uF72B',  // end
        U'\uF72C',  // pageup
        U'\uF72D',  // pagedown
        U'\uF739'   // clear
    };

    // Check for send control key

    if (!controlUnicode.contains(static_cast<char32_t>(charCode)))
    {
        std::string utf8String;
        char32_t codepoint = static_cast<char32_t>(charCode);
        text_utils::UTF32ToUTF8(std::u32string_view{&codepoint, 1zu}, utf8String);
        InputSystem::getInstance()->dispatchInsertText(utf8String);
    }
}

void RenderView::onGLFWPreeditCallback(GLFWwindow* window,
                                       int preedit_count,
                                       unsigned int* preedit_string,  // UTF-32
                                       int /*block_count*/,
                                       int* /*block_sizes*/,
                                       int /*focused_block*/,
                                       int caret)
{
    std::string utf8String;

    if (preedit_count > 0)
    {
        text_utils::UTF32ToUTF8(
            std::u32string_view{std::bit_cast<char32_t*>(preedit_string), static_cast<size_t>(preedit_count)},
            utf8String);
    }

    InputSystem::getInstance()->dispatchUpdatePreedit(utf8String, caret);
}

void RenderView::onGLFWWindowPosCallback(GLFWwindow* /*window*/, int x, int y)
{
    auto director = Director::getInstance();
    director->setViewport();

    Vec2 pos(x, y);
    director->getEventDispatcher()->dispatchCustomEvent(RenderView::EVENT_WINDOW_POSITIONED, &pos);
}

void RenderView::onGLFWWindowIconifyCallback(GLFWwindow* /*window*/, int iconified)
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

void RenderView::onGLFWWindowFocusCallback(GLFWwindow* /*window*/, int focused)
{
    if (focused == GL_TRUE)
    {
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(RenderView::EVENT_WINDOW_FOCUSED, nullptr);
    }
    else
    {
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(RenderView::EVENT_WINDOW_UNFOCUSED, nullptr);
    }
}

void RenderView::onGLFWWindowCloseCallback(GLFWwindow* window)
{
    bool isClose = true;
    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(RenderView::EVENT_WINDOW_CLOSE, &isClose);
    if (isClose == false)
    {
        glfwSetWindowShouldClose(window, 0);
    }
}

#if defined(__EMSCRIPTEN__)
void RenderView::onWebOrientationChangeCallback(int /*eventType*/, const EmscriptenOrientationChangeEvent* e)
{
    AXLOGD("onWebOrientationChangeCallback: orientationIndex:{}, orientationAngle:{}", e->orientationIndex,
           e->orientationAngle);

    if (s_fullscreenState->isFullscreen)
    {
        int screenWidth = 0, screenHeight = 0;
        emscripten_get_screen_size(&screenWidth, &screenHeight);
        AXLOGD("Screen size after orientation change: ({}, {})", screenWidth, screenHeight);
        glfwSetWindowSize(_mainWindow, screenWidth, screenHeight);
    }
    // else: browser handling canvas size
}

void RenderView::onWebFullscreenCallback(int /*eventType*/, const EmscriptenFullscreenChangeEvent* e)
{
    if (e->isFullscreen == s_fullscreenState->isFullscreen)
        return;

    auto& windowedSize              = s_fullscreenState->windowedSize;
    s_fullscreenState->isFullscreen = e->isFullscreen;
    if (e->isFullscreen)
    {
        glfwGetWindowSize(_mainWindow, &windowedSize.x, &windowedSize.y);

        AXLOGD("onWebFullscreenCallback: enter full screen: ({},{}) => ({},{})", windowedSize.x, windowedSize.y,
               e->screenWidth, e->screenHeight);
        glfwSetWindowSize(_mainWindow, e->screenWidth, e->screenHeight);
    }
    else
    {
        AXLOGD("onWebFullscreenCallback: exit full screen => ({},{}) => ({},{})", e->screenWidth, e->screenHeight,
               windowedSize.x, windowedSize.y);
        glfwSetWindowSize(_mainWindow, windowedSize.x, windowedSize.y);
    }
}

#endif

}  // namespace ax

#if defined(__linux__)
#    pragma pop_macro("None")
#endif
