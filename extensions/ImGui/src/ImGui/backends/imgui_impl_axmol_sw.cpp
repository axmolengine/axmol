#include "imgui_impl_axmol_sw.h"
#include "axmol/base/Director.h"
#include "axmol/base/EventListenerTouch.h"
#include "axmol/base/IMEDelegate.h"
#include "axmol/rhi/axmol-rhi.h"
#include <functional>
#include <imgui_internal.h>
#include "axmol/base/IMEDelegate.h"
#include "axmol/base/EventDispatcher.h"

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

// Text handling
class KeyboardInputDelegate : public IMEDelegate
{
protected:
    bool canAttachWithIME() override { return true; }

    bool canDetachWithIME() override { return true; }

    void controlKey(EventKeyboard::KeyCode keyCode) override
    {
        // Not handled at the moment
    }

    void insertText(const char* text, size_t len) override
    {
        ImGuiIO& io = ImGui::GetIO();
        for (int i = 0; i < len && text[i] != 0; ++i)
        {
            io.AddInputCharacter(text[i]);
        }
    }
};

// Axmol SingleWindow Platform data

struct ImGui_ImplAxmolSW_Data
{
    RenderView* Window{nullptr};
    double Time{0};
    bool InstalledCallbacks{false};

    // ImGui_ImplAxmolSW_Data()   { memset(this, 0, sizeof(*this)); }

    // axmol spec data
    Vec2 ViewResolution = Vec2(1920, 1080);
    ImVec2 LastValidMousePos;
    EventListener* TouchListener = nullptr;

    KeyboardInputDelegate KeyboardInputDelegate;
};

// Backend data stored in io.BackendPlatformUserData to allow support for multiple Dear ImGui contexts
static ImGui_ImplAxmolSW_Data* ImGui_ImplAxmolSW_GetBackendData()
{
    return ImGui::GetCurrentContext() ? (ImGui_ImplAxmolSW_Data*)ImGui::GetIO().BackendPlatformUserData : nullptr;
}

static ax::Vec2 convertToScreen(const Vec2& pos)
{
    auto* bd    = ImGui_ImplAxmolSW_GetBackendData();
    ImGuiIO& io = ImGui::GetIO();
    auto origin = bd->Window->getViewportRect().origin;
    auto uiX    = (pos.x * bd->Window->getScaleX() + origin.x) / io.DisplayFramebufferScale.x;
    auto uiY    = (pos.y * bd->Window->getScaleY() + origin.y) / io.DisplayFramebufferScale.y;

    return Vec2(uiX, uiY);
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

static int s_CapturedTouchId = -1;

// Functions
bool ImGui_ImplAxmolSW_Init(RenderView* window, bool install_callbacks)
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

    auto* touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->retain();
    bd->TouchListener = touchListener;

    touchListener->onTouchBegan = [](Touch* touch, Event* event) -> bool {
        ImGuiIO& io                = ImGui::GetIO();
        ImGui_ImplAxmolSW_Data* bd = ImGui_ImplAxmolSW_GetBackendData();
        auto location              = convertToScreen(touch->getLocationInView());
        auto touchPos              = ImVec2(location.x, location.y);

        // We perform our own hit test here because on Android there is no real mouse hover event.
        // ImGui's WantCaptureMouse is updated only after queued input events are processed
        // (i.e. in NewFrame), so in onTouchBegan it would still reflect the previous frame.
        // Without an immediate hit test, we can't know right now if this touch is on a UI element.
        // By checking the position against ImGui windows ourselves, we can decide instantly
        // whether to capture the touch and stop it from reaching the game scene.
        if (s_CapturedTouchId == -1 && ImGui_ImplAxmol_HitTest(touchPos))
        {
            s_CapturedTouchId = touch->getID();
            io.AddMousePosEvent(location.x, location.y);
            bd->LastValidMousePos = touchPos;
            io.AddMouseButtonEvent(0, true);
            event->stopPropagation();
            return true;
        }
        return false;
    };

    touchListener->onTouchMoved = [](Touch* touch, Event* /*event*/) {
        if (touch->getID() != s_CapturedTouchId)
            return;
        ImGuiIO& io                = ImGui::GetIO();
        ImGui_ImplAxmolSW_Data* bd = ImGui_ImplAxmolSW_GetBackendData();
        auto location              = convertToScreen(touch->getLocationInView());
        io.AddMousePosEvent(location.x, location.y);
        bd->LastValidMousePos = ImVec2(location.x, location.y);
    };

    touchListener->onTouchEnded = [](Touch* touch, Event* event) {
        if (touch->getID() != s_CapturedTouchId)
            return;
        ImGuiIO& io                = ImGui::GetIO();
        ImGui_ImplAxmolSW_Data* bd = ImGui_ImplAxmolSW_GetBackendData();
        auto location              = convertToScreen(touch->getLocationInView());
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

        s_CapturedTouchId = -1;
    };

    touchListener->onTouchCancelled = [](Touch* touch, Event* /*event*/) {
        if (touch->getID() != s_CapturedTouchId)
            return;

        ImGuiIO& io                = ImGui::GetIO();
        ImGui_ImplAxmolSW_Data* bd = ImGui_ImplAxmolSW_GetBackendData();
        auto location              = convertToScreen(touch->getLocationInView());
        io.AddMousePosEvent(location.x, location.y);
        bd->LastValidMousePos = ImVec2(location.x, location.y);
        io.AddMouseButtonEvent(0, false);

        s_CapturedTouchId = -1;
    };

    constexpr int highestPriority = (std::numeric_limits<int>::min)();
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(touchListener, highestPriority);

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

    Director::getInstance()->getEventDispatcher()->removeEventListener(bd->TouchListener);
    AX_SAFE_RELEASE_NULL(bd->TouchListener);

    IM_DELETE(bd);
}

void ImGui_ImplAxmolSW_NewFrame()
{
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplAxmolSW_Data* bd = ImGui_ImplAxmolSW_GetBackendData();
    IM_ASSERT(bd != nullptr && "Did you call ImGui_ImplAxmolSW_InitForXXX()?");

    // Setup display size (every frame to accommodate for window resizing)
    int32_t window_width  = bd->ViewResolution.width;
    int32_t window_height = bd->ViewResolution.height;
    int display_width     = bd->Window->getWindowSize().width;
    int display_height    = bd->Window->getWindowSize().height;

    io.DisplaySize = ImVec2((float)window_width, (float)window_height);
    if (window_width > 0 && window_height > 0)
        io.DisplayFramebufferScale = ImVec2((float)display_width / window_width, (float)display_height / window_height);

    // Setup time step
    auto now            = std::chrono::high_resolution_clock::now();
    auto duration       = now.time_since_epoch();
    double current_time = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() / 1e9;
    io.DeltaTime        = bd->Time > 0.0 ? (float)(current_time - bd->Time) : (float)(1.0f / 60.0f);
    bd->Time            = current_time;
}

// @imgui_impl_axmol.h
IMGUI_IMPL_API void ImGui_ImplAxmol_SetViewResolution(float width, float height)
{
    auto bd = ImGui_ImplAxmolSW_GetBackendData();
    bd->ViewResolution.set(width, height);
}

#if defined(__clang__)
#    pragma clang diagnostic pop
#endif
