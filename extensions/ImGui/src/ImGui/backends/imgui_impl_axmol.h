/*
 * ImGui Axmol renderer backend
 */
#pragma once
#include "imgui.h"
#include <functional>
#include "axmol/platform/PlatformConfig.h"

#if AX_RENDER_API == AX_RENDER_API_GL
#    include "axmol/rhi/opengl/OpenGLState.h"
#endif

extern "C" {
struct GLFWwindow;
}

typedef void (*ImGuiImplAxmolUpdateFontsFn)(void* userdata);

IMGUI_IMPL_API void ImGui_ImplAxmol_Init();
IMGUI_IMPL_API void ImGui_ImplAxmol_Shutdown();
IMGUI_IMPL_API void ImGui_ImplAxmol_NewFrame();
IMGUI_IMPL_API void ImGui_ImplAxmol_RenderDrawData(ImDrawData* draw_data);
IMGUI_IMPL_API void ImGui_ImplAxmol_RenderPlatform();

IMGUI_IMPL_API void ImGui_ImplAxmol_MakeCurrent(GLFWwindow* window);

IMGUI_IMPL_API void ImGui_ImplAxmol_PostCommand(std::function<void()>&& func);

// (Optional) Called by Init/NewFrame/Shutdown
IMGUI_IMPL_API bool ImGui_ImplAxmol_CreateDeviceObjects();
IMGUI_IMPL_API void ImGui_ImplAxmol_DestroyDeviceObjects();

// Sets custom font atlas builder function
IMGUI_IMPL_API void ImGui_ImplAxmol_SetUpdateFontsFunc(ImGuiImplAxmolUpdateFontsFn func, void* userdata);

// Sets Device objects dirty
IMGUI_IMPL_API void ImGui_ImplAxmol_MarkFontsDirty();

// Sets the required view resolution for the UI
IMGUI_IMPL_API void ImGui_ImplAxmol_SetViewResolution(float width, float height);
