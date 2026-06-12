#include "imgui_impl_axmol_sw.h"
#include "axmol/base/Director.h"
#include "axmol/base/PointerEventListener.h"
#include "axmol/base/KeyboardEventListener.h"
#include "axmol/rhi/axmol-rhi.h"
#include "axmol/base/InputDelegate.h"
#include "axmol/base/EventDispatcher.h"
#include "axmol/platform/RenderViewCore.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <imgui_internal.h>

using namespace ax;
using namespace ax::rhi;

// Clang warnings with -Weverything
#if defined(__clang__)
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wold-style-cast"   // warning: use of old-style cast
#    pragma clang diagnostic ignored "-Wsign-conversion"  // warning: implicit conversion changes signedness
#    if __has_warning("-Wzero-as-null-pointer-constant")
#        pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#    endif
#endif

struct ImGui_ImplAxmolSW_KeyCodeHash
{
    size_t operator()(KeyboardEvent::KeyCode keyCode) const
    {
        return static_cast<size_t>(keyCode);
    }
};

static ImGuiKey ImGui_ImplAxmolSW_KeyToImGuiKey(KeyboardEvent::KeyCode keyCode)
{
    static const std::unordered_map<KeyboardEvent::KeyCode, ImGuiKey, ImGui_ImplAxmolSW_KeyCodeHash> keyMap = {
        {KeyboardEvent::KeyCode::KEY_TAB, ImGuiKey_Tab},
        {KeyboardEvent::KeyCode::KEY_LEFT_ARROW, ImGuiKey_LeftArrow},
        {KeyboardEvent::KeyCode::KEY_KP_LEFT, ImGuiKey_LeftArrow},
        {KeyboardEvent::KeyCode::KEY_RIGHT_ARROW, ImGuiKey_RightArrow},
        {KeyboardEvent::KeyCode::KEY_KP_RIGHT, ImGuiKey_RightArrow},
        {KeyboardEvent::KeyCode::KEY_UP_ARROW, ImGuiKey_UpArrow},
        {KeyboardEvent::KeyCode::KEY_KP_UP, ImGuiKey_UpArrow},
        {KeyboardEvent::KeyCode::KEY_DOWN_ARROW, ImGuiKey_DownArrow},
        {KeyboardEvent::KeyCode::KEY_KP_DOWN, ImGuiKey_DownArrow},
        {KeyboardEvent::KeyCode::KEY_PG_UP, ImGuiKey_PageUp},
        {KeyboardEvent::KeyCode::KEY_KP_PG_UP, ImGuiKey_PageUp},
        {KeyboardEvent::KeyCode::KEY_PG_DOWN, ImGuiKey_PageDown},
        {KeyboardEvent::KeyCode::KEY_KP_PG_DOWN, ImGuiKey_PageDown},
        {KeyboardEvent::KeyCode::KEY_HOME, ImGuiKey_Home},
        {KeyboardEvent::KeyCode::KEY_KP_HOME, ImGuiKey_Home},
        {KeyboardEvent::KeyCode::KEY_END, ImGuiKey_End},
        {KeyboardEvent::KeyCode::KEY_KP_END, ImGuiKey_End},
        {KeyboardEvent::KeyCode::KEY_INSERT, ImGuiKey_Insert},
        {KeyboardEvent::KeyCode::KEY_KP_INSERT, ImGuiKey_Insert},
        {KeyboardEvent::KeyCode::KEY_DELETE, ImGuiKey_Delete},
        {KeyboardEvent::KeyCode::KEY_KP_DELETE, ImGuiKey_Delete},
        {KeyboardEvent::KeyCode::KEY_BACKSPACE, ImGuiKey_Backspace},
        {KeyboardEvent::KeyCode::KEY_SPACE, ImGuiKey_Space},
        {KeyboardEvent::KeyCode::KEY_ENTER, ImGuiKey_Enter},
        {KeyboardEvent::KeyCode::KEY_RETURN, ImGuiKey_Enter},
        {KeyboardEvent::KeyCode::KEY_KP_ENTER, ImGuiKey_KeypadEnter},
        {KeyboardEvent::KeyCode::KEY_ESCAPE, ImGuiKey_Escape},
        {KeyboardEvent::KeyCode::KEY_APOSTROPHE, ImGuiKey_Apostrophe},
        {KeyboardEvent::KeyCode::KEY_COMMA, ImGuiKey_Comma},
        {KeyboardEvent::KeyCode::KEY_MINUS, ImGuiKey_Minus},
        {KeyboardEvent::KeyCode::KEY_PERIOD, ImGuiKey_Period},
        {KeyboardEvent::KeyCode::KEY_SLASH, ImGuiKey_Slash},
        {KeyboardEvent::KeyCode::KEY_SEMICOLON, ImGuiKey_Semicolon},
        {KeyboardEvent::KeyCode::KEY_EQUAL, ImGuiKey_Equal},
        {KeyboardEvent::KeyCode::KEY_LEFT_BRACKET, ImGuiKey_LeftBracket},
        {KeyboardEvent::KeyCode::KEY_BACK_SLASH, ImGuiKey_Backslash},
        {KeyboardEvent::KeyCode::KEY_RIGHT_BRACKET, ImGuiKey_RightBracket},
        {KeyboardEvent::KeyCode::KEY_GRAVE, ImGuiKey_GraveAccent},
        {KeyboardEvent::KeyCode::KEY_TILDE, ImGuiKey_GraveAccent},
        {KeyboardEvent::KeyCode::KEY_CAPS_LOCK, ImGuiKey_CapsLock},
        {KeyboardEvent::KeyCode::KEY_SCROLL_LOCK, ImGuiKey_ScrollLock},
        {KeyboardEvent::KeyCode::KEY_NUM_LOCK, ImGuiKey_NumLock},
        {KeyboardEvent::KeyCode::KEY_PRINT, ImGuiKey_PrintScreen},
        {KeyboardEvent::KeyCode::KEY_PAUSE, ImGuiKey_Pause},
        {KeyboardEvent::KeyCode::KEY_0, ImGuiKey_0},
        {KeyboardEvent::KeyCode::KEY_1, ImGuiKey_1},
        {KeyboardEvent::KeyCode::KEY_2, ImGuiKey_2},
        {KeyboardEvent::KeyCode::KEY_3, ImGuiKey_3},
        {KeyboardEvent::KeyCode::KEY_4, ImGuiKey_4},
        {KeyboardEvent::KeyCode::KEY_5, ImGuiKey_5},
        {KeyboardEvent::KeyCode::KEY_6, ImGuiKey_6},
        {KeyboardEvent::KeyCode::KEY_7, ImGuiKey_7},
        {KeyboardEvent::KeyCode::KEY_8, ImGuiKey_8},
        {KeyboardEvent::KeyCode::KEY_9, ImGuiKey_9},
        {KeyboardEvent::KeyCode::KEY_A, ImGuiKey_A},
        {KeyboardEvent::KeyCode::KEY_CAPITAL_A, ImGuiKey_A},
        {KeyboardEvent::KeyCode::KEY_B, ImGuiKey_B},
        {KeyboardEvent::KeyCode::KEY_CAPITAL_B, ImGuiKey_B},
        {KeyboardEvent::KeyCode::KEY_C, ImGuiKey_C},
        {KeyboardEvent::KeyCode::KEY_CAPITAL_C, ImGuiKey_C},
        {KeyboardEvent::KeyCode::KEY_D, ImGuiKey_D},
        {KeyboardEvent::KeyCode::KEY_CAPITAL_D, ImGuiKey_D},
        {KeyboardEvent::KeyCode::KEY_E, ImGuiKey_E},
        {KeyboardEvent::KeyCode::KEY_CAPITAL_E, ImGuiKey_E},
        {KeyboardEvent::KeyCode::KEY_F, ImGuiKey_F},
        {KeyboardEvent::KeyCode::KEY_CAPITAL_F, ImGuiKey_F},
        {KeyboardEvent::KeyCode::KEY_G, ImGuiKey_G},
        {KeyboardEvent::KeyCode::KEY_CAPITAL_G, ImGuiKey_G},
        {KeyboardEvent::KeyCode::KEY_H, ImGuiKey_H},
        {KeyboardEvent::KeyCode::KEY_CAPITAL_H, ImGuiKey_H},
        {KeyboardEvent::KeyCode::KEY_I, ImGuiKey_I},
        {KeyboardEvent::KeyCode::KEY_CAPITAL_I, ImGuiKey_I},
        {KeyboardEvent::KeyCode::KEY_J, ImGuiKey_J},
        {KeyboardEvent::KeyCode::KEY_CAPITAL_J, ImGuiKey_J},
        {KeyboardEvent::KeyCode::KEY_K, ImGuiKey_K},
        {KeyboardEvent::KeyCode::KEY_CAPITAL_K, ImGuiKey_K},
        {KeyboardEvent::KeyCode::KEY_L, ImGuiKey_L},
        {KeyboardEvent::KeyCode::KEY_CAPITAL_L, ImGuiKey_L},
        {KeyboardEvent::KeyCode::KEY_M, ImGuiKey_M},
        {KeyboardEvent::KeyCode::KEY_CAPITAL_M, ImGuiKey_M},
        {KeyboardEvent::KeyCode::KEY_N, ImGuiKey_N},
        {KeyboardEvent::KeyCode::KEY_CAPITAL_N, ImGuiKey_N},
        {KeyboardEvent::KeyCode::KEY_O, ImGuiKey_O},
        {KeyboardEvent::KeyCode::KEY_CAPITAL_O, ImGuiKey_O},
        {KeyboardEvent::KeyCode::KEY_P, ImGuiKey_P},
        {KeyboardEvent::KeyCode::KEY_CAPITAL_P, ImGuiKey_P},
        {KeyboardEvent::KeyCode::KEY_Q, ImGuiKey_Q},
        {KeyboardEvent::KeyCode::KEY_CAPITAL_Q, ImGuiKey_Q},
        {KeyboardEvent::KeyCode::KEY_R, ImGuiKey_R},
        {KeyboardEvent::KeyCode::KEY_CAPITAL_R, ImGuiKey_R},
        {KeyboardEvent::KeyCode::KEY_S, ImGuiKey_S},
        {KeyboardEvent::KeyCode::KEY_CAPITAL_S, ImGuiKey_S},
        {KeyboardEvent::KeyCode::KEY_T, ImGuiKey_T},
        {KeyboardEvent::KeyCode::KEY_CAPITAL_T, ImGuiKey_T},
        {KeyboardEvent::KeyCode::KEY_U, ImGuiKey_U},
        {KeyboardEvent::KeyCode::KEY_CAPITAL_U, ImGuiKey_U},
        {KeyboardEvent::KeyCode::KEY_V, ImGuiKey_V},
        {KeyboardEvent::KeyCode::KEY_CAPITAL_V, ImGuiKey_V},
        {KeyboardEvent::KeyCode::KEY_W, ImGuiKey_W},
        {KeyboardEvent::KeyCode::KEY_CAPITAL_W, ImGuiKey_W},
        {KeyboardEvent::KeyCode::KEY_X, ImGuiKey_X},
        {KeyboardEvent::KeyCode::KEY_CAPITAL_X, ImGuiKey_X},
        {KeyboardEvent::KeyCode::KEY_Y, ImGuiKey_Y},
        {KeyboardEvent::KeyCode::KEY_CAPITAL_Y, ImGuiKey_Y},
        {KeyboardEvent::KeyCode::KEY_Z, ImGuiKey_Z},
        {KeyboardEvent::KeyCode::KEY_CAPITAL_Z, ImGuiKey_Z},
        {KeyboardEvent::KeyCode::KEY_KP_PLUS, ImGuiKey_KeypadAdd},
        {KeyboardEvent::KeyCode::KEY_KP_MINUS, ImGuiKey_KeypadSubtract},
        {KeyboardEvent::KeyCode::KEY_KP_MULTIPLY, ImGuiKey_KeypadMultiply},
        {KeyboardEvent::KeyCode::KEY_KP_DIVIDE, ImGuiKey_KeypadDivide},
        {KeyboardEvent::KeyCode::KEY_LEFT_SHIFT, ImGuiKey_LeftShift},
        {KeyboardEvent::KeyCode::KEY_RIGHT_SHIFT, ImGuiKey_RightShift},
        {KeyboardEvent::KeyCode::KEY_LEFT_CTRL, ImGuiKey_LeftCtrl},
        {KeyboardEvent::KeyCode::KEY_RIGHT_CTRL, ImGuiKey_RightCtrl},
        {KeyboardEvent::KeyCode::KEY_LEFT_ALT, ImGuiKey_LeftAlt},
        {KeyboardEvent::KeyCode::KEY_RIGHT_ALT, ImGuiKey_RightAlt},
        {KeyboardEvent::KeyCode::KEY_HYPER, ImGuiKey_LeftSuper},
        {KeyboardEvent::KeyCode::KEY_MENU, ImGuiKey_Menu},
        {KeyboardEvent::KeyCode::KEY_F1, ImGuiKey_F1},
        {KeyboardEvent::KeyCode::KEY_F2, ImGuiKey_F2},
        {KeyboardEvent::KeyCode::KEY_F3, ImGuiKey_F3},
        {KeyboardEvent::KeyCode::KEY_F4, ImGuiKey_F4},
        {KeyboardEvent::KeyCode::KEY_F5, ImGuiKey_F5},
        {KeyboardEvent::KeyCode::KEY_F6, ImGuiKey_F6},
        {KeyboardEvent::KeyCode::KEY_F7, ImGuiKey_F7},
        {KeyboardEvent::KeyCode::KEY_F8, ImGuiKey_F8},
        {KeyboardEvent::KeyCode::KEY_F9, ImGuiKey_F9},
        {KeyboardEvent::KeyCode::KEY_F10, ImGuiKey_F10},
        {KeyboardEvent::KeyCode::KEY_F11, ImGuiKey_F11},
        {KeyboardEvent::KeyCode::KEY_F12, ImGuiKey_F12},
    };

    auto it = keyMap.find(keyCode);
    return it != keyMap.end() ? it->second : ImGuiKey_None;
}

static void ImGui_ImplAxmolSW_AddKeyEvent(KeyboardEvent::KeyCode keyCode, bool down)
{
    ImGuiIO& io = ImGui::GetIO();
    switch (keyCode)
    {
    case KeyboardEvent::KeyCode::KEY_LEFT_CTRL:
    case KeyboardEvent::KeyCode::KEY_RIGHT_CTRL:
        io.AddKeyEvent(ImGuiMod_Ctrl, down);
        break;
    case KeyboardEvent::KeyCode::KEY_LEFT_SHIFT:
    case KeyboardEvent::KeyCode::KEY_RIGHT_SHIFT:
        io.AddKeyEvent(ImGuiMod_Shift, down);
        break;
    case KeyboardEvent::KeyCode::KEY_LEFT_ALT:
    case KeyboardEvent::KeyCode::KEY_RIGHT_ALT:
        io.AddKeyEvent(ImGuiMod_Alt, down);
        break;
    case KeyboardEvent::KeyCode::KEY_HYPER:
        io.AddKeyEvent(ImGuiMod_Super, down);
        break;
    default:
        break;
    }

    auto imguiKey = ImGui_ImplAxmolSW_KeyToImGuiKey(keyCode);
    if (imguiKey != ImGuiKey_None)
        io.AddKeyEvent(imguiKey, down);
}

static void ImGui_ImplAxmolSW_AddPressedKeyEvent(KeyboardEvent::KeyCode keyCode)
{
    ImGui_ImplAxmolSW_AddKeyEvent(keyCode, true);
    ImGui_ImplAxmolSW_AddKeyEvent(keyCode, false);
}

// Text handling
class KeyboardInputDelegate : public InputDelegate
{
protected:
    bool canAttachWithIME() const override { return true; }

    bool canDetachWithIME() const override { return true; }

    void controlKey(KeyboardEvent::KeyCode keyCode) override
    {
        ImGui_ImplAxmolSW_AddPressedKeyEvent(keyCode);
    }

    void deleteBackward(unsigned int numChars) override
    {
        for (unsigned int i = 0; i < numChars; ++i)
            ImGui_ImplAxmolSW_AddPressedKeyEvent(KeyboardEvent::KeyCode::KEY_BACKSPACE);
    }

    void insertText(std::string_view text) override
    {
        ImGuiIO& io = ImGui::GetIO();
        std::string input{text};
        io.AddInputCharactersUTF8(input.c_str());
    }
};

// Axmol SingleWindow Platform data

struct ImGui_ImplAxmolSW_Data
{
    RenderViewCore* Window{nullptr};
    double Time{0};
    bool InstalledCallbacks{false};

    // ImGui_ImplAxmolSW_Data()   { memset(this, 0, sizeof(*this)); }

    // axmol spec data
    ImVec2 LastValidMousePos;
    EventListener* PointerListener = nullptr;
    EventListener* KeyboardListener = nullptr;

    intptr_t CapturedPointerId = -1;

    KeyboardInputDelegate KeyboardInputDelegate;
};

// Backend data stored in io.BackendPlatformUserData to allow support for multiple Dear ImGui contexts
static ImGui_ImplAxmolSW_Data* ImGui_ImplAxmolSW_GetBackendData()
{
    return ImGui::GetCurrentContext() ? (ImGui_ImplAxmolSW_Data*)ImGui::GetIO().BackendPlatformUserData : nullptr;
}

static bool ImGui_ImplAxmol_HitTest(const ImVec2& p)
{
    ImGuiContext* ctx = ImGui::GetCurrentContext();
    if (!ctx)
        return false;

    for (ImGuiWindow* win : ctx->Windows)
    {
        if (!win)
            continue;
        if (!win->Active)
            continue;
        if (win->Hidden)
            continue;
        if (win->Flags & ImGuiWindowFlags_NoInputs)
            continue;
        ImRect r(win->Pos, ImVec2(win->Pos.x + win->Size.x, win->Pos.y + win->Size.y));
        if (r.Contains(p))
            return true;
    }
    return false;
}

// Functions
bool ImGui_ImplAxmolSW_Init(RenderViewCore* window, bool install_callbacks)
{
    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.BackendPlatformUserData == nullptr && "Already initialized a platform backend!");

    // Setup backend capabilities flags
    ImGui_ImplAxmolSW_Data* bd = IM_NEW(ImGui_ImplAxmolSW_Data)();
    io.BackendPlatformUserData = (void*)bd;
    io.BackendPlatformName     = "imgui_impl_axmol_sw";
    // We can honor io.WantSetMousePos requests (optional, rarely used)
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    bd->Window = window;
    bd->Time   = 0.0;

    // = bd->Window;

    // Our mouse update function expect PlatformHandle to be filled for the main viewport
    ImGuiViewport* main_viewport  = ImGui::GetMainViewport();
    main_viewport->PlatformHandle = (void*)bd->Window;

    io.AddFocusEvent(true);

    auto listener = PointerEventListener::create();
    listener->retain();
    bd->PointerListener = listener;

    listener->onPointerDown = [bd](PointerEvent* event) -> bool {
        ImGuiIO& io                = ImGui::GetIO();
        ImGui_ImplAxmolSW_Data* bd = ImGui_ImplAxmolSW_GetBackendData();
        auto location              = event->getScreenLocation();
        auto pointerPos              = ImVec2(location.x, location.y);

        // We perform our own hit test here because on Android there is no real mouse hover event.
        // ImGui's WantCaptureMouse is updated only after queued input events are processed
        // (i.e. in NewFrame), so in onPointerDown it would still reflect the previous frame.
        // Without an immediate hit test, we can't know right now if this touch is on a UI element.
        // By checking the position against ImGui windows ourselves, we can decide instantly
        // whether to capture the touch and stop it from reaching the game scene.
        if (bd->CapturedPointerId == -1 && ImGui_ImplAxmol_HitTest(pointerPos))
        {
            bd->CapturedPointerId = event->getPointerId();
            io.AddMousePosEvent(location.x, location.y);
            bd->LastValidMousePos = pointerPos;
            io.AddMouseButtonEvent(0, true);
            event->stopPropagation();
            return true;
        }
        return false;
    };

    auto updateMousePosition = [](PointerEvent* event) {
        ImGuiIO& io                = ImGui::GetIO();
        ImGui_ImplAxmolSW_Data* bd = ImGui_ImplAxmolSW_GetBackendData();
        auto location              = event->getScreenLocation();
        io.AddMousePosEvent(location.x, location.y);
        bd->LastValidMousePos = ImVec2(location.x, location.y);
    };

    listener->onPointerMove = [bd, updateMousePosition](PointerEvent* event) {
        if (bd->CapturedPointerId != -1 && event->getPointerId() != bd->CapturedPointerId)
            return;
        updateMousePosition(event);
    };

    listener->onPointerUp = [bd](PointerEvent* event) {
        if (event->getPointerId() != bd->CapturedPointerId)
            return;
        ImGuiIO& io                = ImGui::GetIO();
        ImGui_ImplAxmolSW_Data* bd = ImGui_ImplAxmolSW_GetBackendData();
        auto location              = event->getScreenLocation();
        io.AddMousePosEvent(location.x, location.y);
        bd->LastValidMousePos = ImVec2(location.x, location.y);
        io.AddMouseButtonEvent(0, false);

        if (ImGui::GetIO().WantTextInput)
        {
            bd->KeyboardInputDelegate.attachWithIME();
            bd->Window->setIMEKeyboardState(true);
        }
        else
        {
            bd->KeyboardInputDelegate.detachWithIME();
            bd->Window->setIMEKeyboardState(false);
        }

        bd->CapturedPointerId = -1;
    };

    listener->onPointerCancel = [bd](PointerEvent* event) {
        if (event->getPointerId() != bd->CapturedPointerId)
            return;

        ImGuiIO& io                = ImGui::GetIO();
        ImGui_ImplAxmolSW_Data* bd = ImGui_ImplAxmolSW_GetBackendData();
        auto location              = event->getScreenLocation();
        io.AddMousePosEvent(location.x, location.y);
        bd->LastValidMousePos = ImVec2(location.x, location.y);
        io.AddMouseButtonEvent(0, false);

        bd->CapturedPointerId = -1;
    };

    listener->onPointerScroll = [bd](PointerEvent* event) -> bool {
        ImGuiIO& io                = ImGui::GetIO();
        ImGui_ImplAxmolSW_Data* bd = ImGui_ImplAxmolSW_GetBackendData();
        
        auto location              = event->getScreenLocation();
        auto pointerPos            = ImVec2(location.x, location.y);

        io.AddMousePosEvent(location.x, location.y);
        bd->LastValidMousePos = pointerPos;

        auto scrollDelta = event->getScrollDelta();
        
#if defined(__EMSCRIPTEN__)
        scrollDelta *= 0.1f;
#endif
        io.AddMouseWheelEvent(scrollDelta.x, -scrollDelta.y);

        if (ImGui_ImplAxmol_HitTest(pointerPos))
            return true;
        
        return false;
    };

    constexpr int highestPriority = (std::numeric_limits<int>::min)();
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listener, highestPriority);

    auto keyboardListener = KeyboardEventListener::create();
    keyboardListener->retain();
    bd->KeyboardListener = keyboardListener;

    auto handleKeyEvent = [](KeyboardEvent* event, bool down) {
        ImGui_ImplAxmolSW_AddKeyEvent(event->getKeyCode(), down);

        auto& io = ImGui::GetIO();
        if (event && (io.WantCaptureKeyboard || io.WantTextInput))
            event->stopPropagation();
    };
    keyboardListener->onKeyPressed = [handleKeyEvent](KeyboardEvent* event) {
        handleKeyEvent(event, true);
    };
    keyboardListener->onKeyReleased = [handleKeyEvent](KeyboardEvent* event) {
        handleKeyEvent(event, false);
    };
    keyboardListener->onKeyRepeat = [handleKeyEvent](KeyboardEvent* event) {
        handleKeyEvent(event, true);
    };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(keyboardListener, highestPriority);

    return true;
}

void ImGui_ImplAxmolSW_Shutdown()
{
    ImGui_ImplAxmolSW_Data* bd = ImGui_ImplAxmolSW_GetBackendData();
    IM_ASSERT(bd != nullptr && "No platform backend to shutdown, or already shutdown?");
    ImGuiIO& io = ImGui::GetIO();

    io.BackendPlatformName     = nullptr;
    io.BackendPlatformUserData = nullptr;
    io.BackendRendererUserData = nullptr;

    Director::getInstance()->getEventDispatcher()->removeEventListener(bd->PointerListener);
    AX_SAFE_RELEASE_NULL(bd->PointerListener);

    Director::getInstance()->getEventDispatcher()->removeEventListener(bd->KeyboardListener);
    AX_SAFE_RELEASE_NULL(bd->KeyboardListener);

    IM_DELETE(bd);
}

void ImGui_ImplAxmolSW_NewFrame()
{
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplAxmolSW_Data* bd = ImGui_ImplAxmolSW_GetBackendData();
    IM_ASSERT(bd != nullptr && "Did you call ImGui_ImplAxmolSW_InitForXXX()?");

    // Setup display size (every frame to accommodate for window resizing)
    auto renderView  = bd->Window;
    auto winSize     = renderView->getWindowSize();
    auto renderScale = renderView->getRenderScale();

    // 1. Convert to absolute physical frame buffer pixels
    float physicalWidth  = winSize.width * renderScale;
    float physicalHeight = winSize.height * renderScale;

    // 2. Map ImGui strictly to the Physical Pixel Space to restore "Desktop-like" crisp size
    // and perfectly align with the scaled physical touch coordinates from InputSystem.
    io.DisplaySize = ImVec2(physicalWidth, physicalHeight);
    
    if (physicalWidth > 0 && physicalHeight > 0)
    {
        // 3. Since DisplaySize is already physical, the scale relative to itself is strictly 1.0!
        io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    }

    // Setup time step
    auto now            = std::chrono::high_resolution_clock::now();
    auto duration       = now.time_since_epoch();
    double current_time = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() / 1e9;
    io.DeltaTime        = bd->Time > 0.0 ? (float)(current_time - bd->Time) : (float)(1.0f / 60.0f);
    bd->Time            = current_time;
}

#if defined(__clang__)
#    pragma clang diagnostic pop
#endif
