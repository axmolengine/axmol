#pragma once

#include "imgui.h"

namespace ax
{
class RenderViewCore;
}

/// ImGui Axmol SingleWindow platform spec APIs
IMGUI_IMPL_API bool ImGui_ImplAxmolSW_Init(ax::RenderViewCore* window, bool install_callbacks);
IMGUI_IMPL_API void ImGui_ImplAxmolSW_Shutdown();
IMGUI_IMPL_API void ImGui_ImplAxmolSW_NewFrame();
