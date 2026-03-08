#pragma once
#include "imgui_impl_axmol.h"
#include "axmol/platform/RenderView.h"

/// ImGui SingleWindow system APIs
IMGUI_IMPL_API bool ImGui_ImplAxmolSW_InitForAxmol(ax::RenderView* window, bool install_callbacks);
IMGUI_IMPL_API void ImGui_ImplAxmolSW_Shutdown();
IMGUI_IMPL_API void ImGui_ImplAxmolSW_NewFrame();
