#pragma once
#include "imgui.h"
#include "platform/CCGLView.h"

struct ANativeWindow;
struct AInputEvent;

typedef void (*ImGuiImplCocos2dxLoadFontFun)(void* userdata);

/// ImGui glfw APIs
IMGUI_IMPL_API bool ImGui_ImplAndroid_InitForAx(ax::GLView* window, bool install_callbacks);
IMGUI_IMPL_API void ImGui_ImplAndroid_Shutdown();
IMGUI_IMPL_API void ImGui_ImplAndroid_NewFrame();

/// ImGui axmol render APIs
IMGUI_IMPL_API void ImGui_ImplAx_Init();
IMGUI_IMPL_API void ImGui_ImplAx_Shutdown();
IMGUI_IMPL_API void ImGui_ImplAx_NewFrame();
IMGUI_IMPL_API void ImGui_ImplAx_RenderDrawData(ImDrawData* draw_data);
IMGUI_IMPL_API void ImGui_ImplAx_RenderPlatform();

// Get FontTexture object ax::Texture2D*
IMGUI_IMPL_API void ImGui_ImplAx_SetCustomFontLoader(ImGuiImplCocos2dxLoadFontFun fun, void* userdata);
IMGUI_IMPL_API void* ImGui_ImplAx_GetFontsTexture();

// Sets Device objects dirty
IMGUI_IMPL_API void ImGui_ImplAx_SetDeviceObjectsDirty();

// Set the required view resolution for the UI
IMGUI_IMPL_API void ImGui_ImplAx_SetViewResolution(float width, float height);
