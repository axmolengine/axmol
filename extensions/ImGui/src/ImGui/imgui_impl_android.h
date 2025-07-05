#pragma once
#include "imgui.h"
#include "platform/GLView.h"

struct ANativeWindow;
struct AInputEvent;

typedef void (*ImGuiImplCocos2dxLoadFontFun)(void* userdata);

/// ImGui glfw APIs
IMGUI_IMPL_API bool ImGui_ImplAndroid_InitForAxmol(ax::GLView* window, bool install_callbacks);
IMGUI_IMPL_API void ImGui_ImplAndroid_Shutdown();
IMGUI_IMPL_API void ImGui_ImplAndroid_NewFrame();

// Set the required view resolution for the UI
IMGUI_IMPL_API void ImGui_ImplAx_SetViewResolution(float width, float height);
