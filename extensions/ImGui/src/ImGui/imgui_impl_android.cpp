#include "imgui_impl_android.h"
#include "imgui_impl_axmol.h"
#include "base/Director.h"
#include "base/EventListenerTouch.h"
#include "base/IMEDelegate.h"
#include "renderer/backend/Backend.h"
#include <functional>
#include <android/native_window.h>
#include <android/input.h>
#include <android/keycodes.h>
#include <android/log.h>
#include <imgui_internal.h>
#include "base/IMEDelegate.h"
#include "base/EventDispatcher.h"

using namespace ax;
using namespace backend;

// Clang warnings with -Weverything
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"     // warning: use of old-style cast
#pragma clang diagnostic ignored "-Wsign-conversion"    // warning: implicit conversion changes signedness
#if __has_warning("-Wzero-as-null-pointer-constant")
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
#endif

// Text handling
class KeyboardInputDelegate : public IMEDelegate
{
protected:
    bool canAttachWithIME() override
    {
        return true;
    }

    bool canDetachWithIME() override
    {
        return true;
    }

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


// Android data

struct ImGui_ImplAndroid_Data
{
    GLView*          Window{nullptr};
    double           Time{0};
    bool             InstalledCallbacks{false};

    // ImGui_ImplAndroid_Data()   { memset(this, 0, sizeof(*this)); }

    // axmol spec data
    Vec2 ViewResolution = Vec2(1920, 1080);
    ImVec2         LastValidMousePos;
    EventListener* TouchListener = nullptr;

    KeyboardInputDelegate KeyboardInputDelegate;
};

// Backend data stored in io.BackendPlatformUserData to allow support for multiple Dear ImGui contexts
static ImGui_ImplAndroid_Data* ImGui_ImplAndroid_GetBackendData()
{
    return ImGui::GetCurrentContext() ? (ImGui_ImplAndroid_Data*)ImGui::GetIO().BackendPlatformUserData : nullptr;
}

static ax::Vec2 convertToUICoordinates(const Vec2& pos)
{
    auto* bd = ImGui_ImplAndroid_GetBackendData();
    ImGuiIO& io = ImGui::GetIO();
    auto origin = bd->Window->getViewPortRect().origin;
    auto uiX = (pos.x * bd->Window->getScaleX() + origin.x) / io.DisplayFramebufferScale.x;
    auto uiY = (pos.y * bd->Window->getScaleY() + origin.y) / io.DisplayFramebufferScale.y;

    return Vec2(uiX, uiY);
}

// Functions
bool ImGui_ImplAndroid_InitForAxmol(GLView* window, bool install_callbacks)
{
    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.BackendPlatformUserData == nullptr && "Already initialized a platform backend!");

    // Setup backend capabilities flags
    ImGui_ImplAndroid_Data* bd = IM_NEW(ImGui_ImplAndroid_Data)();
    io.BackendPlatformUserData = (void*)bd;
    io.BackendPlatformName = "imgui_impl_android";
    //io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
    //io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;    // We can create multi-viewports on the Platform side (optional)

    bd->Window = window;
    bd->Time = 0.0;

    // = bd->Window;

    // Our mouse update function expect PlatformHandle to be filled for the main viewport
    ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    main_viewport->PlatformHandle = (void*)bd->Window;

    io.AddFocusEvent(true);

    auto* touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->retain();
    bd->TouchListener = touchListener;

    touchListener->onTouchBegan = [](Touch* touch, Event* event) -> bool {
        ImGuiIO& io = ImGui::GetIO();
        ImGui_ImplAndroid_Data* bd = ImGui_ImplAndroid_GetBackendData();
        auto location = convertToUICoordinates(touch->getLocationInView());
        io.AddMousePosEvent(location.x, location.y);
        bd->LastValidMousePos = ImVec2(location.x, location.y);
        io.AddMouseButtonEvent(0, true);

        ImGui::UpdateHoveredWindowAndCaptureFlags(bd->LastValidMousePos);
        if (ImGui::GetIO().WantCaptureMouse)
        {
            event->stopPropagation();
        }

        // We can't check if we're actually hovering over a ImGui element, since the
        // AddMousePosEvent is not instant, it's queued. So, just return true here
        // to indicate that we're handling this event.
        return true;
    };

    touchListener->onTouchMoved = [](Touch* touch, Event* /*event*/) {
        ImGuiIO& io = ImGui::GetIO();
        ImGui_ImplAndroid_Data* bd = ImGui_ImplAndroid_GetBackendData();
        auto location = convertToUICoordinates(touch->getLocationInView());
        io.AddMousePosEvent(location.x, location.y);
        bd->LastValidMousePos = ImVec2(location.x, location.y);
    };

    touchListener->onTouchEnded = [](Touch* touch, Event* event) {
        ImGuiIO& io = ImGui::GetIO();
        ImGui_ImplAndroid_Data* bd = ImGui_ImplAndroid_GetBackendData();
        auto location = convertToUICoordinates(touch->getLocationInView());
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
    };

    touchListener->onTouchCancelled = [](Touch* touch, Event* /*event*/) {
        ImGuiIO& io = ImGui::GetIO();
        ImGui_ImplAndroid_Data* bd = ImGui_ImplAndroid_GetBackendData();
        auto location = convertToUICoordinates(touch->getLocationInView());
        io.AddMousePosEvent(location.x, location.y);
        bd->LastValidMousePos = ImVec2(location.x, location.y);
        io.AddMouseButtonEvent(0, false);
    };

    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(touchListener, 1);

    return true;
}

void ImGui_ImplAndroid_Shutdown()
{
    ImGui_ImplAndroid_Data* bd = ImGui_ImplAndroid_GetBackendData();
    IM_ASSERT(bd != nullptr && "No platform backend to shutdown, or already shutdown?");
    ImGuiIO& io = ImGui::GetIO();

    io.BackendPlatformName = nullptr;
    io.BackendPlatformUserData = nullptr;
    io.BackendRendererUserData = nullptr;

#if defined(__ANDROID__)
    Director::getInstance()->getEventDispatcher()->removeEventListener(bd->TouchListener);
    AX_SAFE_RELEASE_NULL(bd->TouchListener);
#endif

    IM_DELETE(bd);
}

void ImGui_ImplAndroid_NewFrame()
{
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplAndroid_Data* bd = ImGui_ImplAndroid_GetBackendData();
    IM_ASSERT(bd != nullptr && "Did you call ImGui_ImplAndroid_InitForXXX()?");

    // Setup display size (every frame to accommodate for window resizing)
    int32_t window_width = bd->ViewResolution.width;
    int32_t window_height = bd->ViewResolution.height;
    int display_width = bd->Window->getFrameSize().width;
    int display_height = bd->Window->getFrameSize().height;

    io.DisplaySize = ImVec2((float)window_width, (float)window_height);
    if (window_width > 0 && window_height > 0)
        io.DisplayFramebufferScale = ImVec2((float)display_width / window_width, (float)display_height / window_height);

    // Setup time step
    struct timespec current_timespec;
    clock_gettime(CLOCK_MONOTONIC, &current_timespec);
    double current_time = (double)(current_timespec.tv_sec) + (current_timespec.tv_nsec / 1000000000.0);
    io.DeltaTime = bd->Time > 0.0 ? (float)(current_time - bd->Time) : (float)(1.0f / 60.0f);
    bd->Time = current_time;
}

//--------------------------------------------------------------------------------------------------------
// MULTI-VIEWPORT / PLATFORM INTERFACE SUPPORT
// This is an _advanced_ and _optional_ feature, allowing the backend to create and handle multiple viewports simultaneously.
// If you are new to dear imgui or creating a new binding for dear imgui, it is recommended that you completely ignore this section first..
//--------------------------------------------------------------------------------------------------------

// Helper structure we store in the void* RenderUserData field of each ImGuiViewport to easily retrieve our backend data.
struct ImGui_ImplAndroid_ViewportData
{
    GLView* Window;
    bool        WindowOwned;
    int         IgnoreWindowPosEventFrame;
    int         IgnoreWindowSizeEventFrame;

    ImGui_ImplAndroid_ViewportData()  { Window = nullptr; WindowOwned = false; IgnoreWindowSizeEventFrame = IgnoreWindowPosEventFrame = -1; }
    ~ImGui_ImplAndroid_ViewportData() { IM_ASSERT(Window == nullptr); }
};

static void ImGui_ImplAndroid_WindowCloseCallback(GLView* window)
{
    if (ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(window))
        viewport->PlatformRequestClose = true;
}

// @imgui_impl_axmol.h
IMGUI_IMPL_API void ImGui_ImplAxmol_SetViewResolution(float width, float height)
{
    auto bd = ImGui_ImplAndroid_GetBackendData();
    bd->ViewResolution.set(width, height);
}

#if defined(__clang__)
#pragma clang diagnostic pop
#endif
