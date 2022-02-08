#pragma once
#include "imgui.h"

struct GLFWwindow;

typedef void (*ImGuiImplCocos2dxLoadFontFun)(void* userdata);

/// ImGui glfw APIs
IMGUI_IMPL_API bool ImGui_ImplGlfw_InitForAdxe(GLFWwindow* window, bool install_callbacks);
IMGUI_IMPL_API void ImGui_ImplGlfw_Shutdown();
IMGUI_IMPL_API void ImGui_ImplGlfw_NewFrame();

/// ImGui adxe render APIs
IMGUI_IMPL_API void ImGui_ImplAdxe_Init();
IMGUI_IMPL_API void ImGui_ImplAdxe_Shutdown();
IMGUI_IMPL_API void ImGui_ImplAdxe_NewFrame();
IMGUI_IMPL_API void ImGui_ImplAdxe_RenderDrawData(ImDrawData* draw_data);
IMGUI_IMPL_API void ImGui_ImplAdxe_RenderPlatform();

// Get FontTexture object cocos2d::Texture2D*
IMGUI_IMPL_API void ImGui_ImplAdxe_SetCustomFontLoader(ImGuiImplCocos2dxLoadFontFun fun, void* userdata);
IMGUI_IMPL_API void* ImGui_ImplAdxe_GetFontsTexture();

// Sets Device objects dirty
IMGUI_IMPL_API void ImGui_ImplAdxe_SetDeviceObjectsDirty();
