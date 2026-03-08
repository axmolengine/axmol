#pragma once

#include "imgui.h"
#include "axmol/platform/RenderView.h"

/// ImGui Axmol SingleWindow platform spec APIs
IMGUI_IMPL_API bool ImGui_ImplAxmolSW_Init(ax::RenderView* window, bool install_callbacks);
IMGUI_IMPL_API void ImGui_ImplAxmolSW_Shutdown();
IMGUI_IMPL_API void ImGui_ImplAxmolSW_NewFrame();
