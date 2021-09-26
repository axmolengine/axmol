#pragma once
#include "imgui/imgui.h"

typedef void (*ImGuiImplCocos2dxLoadFontFun)(void* userdata);

IMGUI_IMPL_API bool     ImGui_ImplAdxe_Init(bool install_callbacks);
IMGUI_IMPL_API void     ImGui_ImplAdxe_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplAdxe_NewFrame();
IMGUI_IMPL_API void     ImGui_ImplAdxe_RenderDrawData(ImDrawData* draw_data);
IMGUI_IMPL_API void     ImGui_ImplAdxe_RenderPlatform();

// Called by Init/NewFrame/Shutdown
// TODO: maybe hide for internal use only
IMGUI_IMPL_API bool     ImGui_ImplAdxe_CreateDeviceObjects();
IMGUI_IMPL_API void     ImGui_ImplAdxe_DestroyDeviceObjects();

// TODO: check whether needs public this API
IMGUI_IMPL_API bool     ImGui_ImplAdxe_CreateFontsTexture();
IMGUI_IMPL_API void     ImGui_ImplAdxe_DestroyFontsTexture();


// Get FontTexture object cocos2d::Texture2D*
IMGUI_IMPL_API void     ImGui_ImplAdxe_SetCustomFontLoader(ImGuiImplCocos2dxLoadFontFun fun, void* userdata);
IMGUI_IMPL_API void*    ImGui_ImplAdxe_GetFontsTexture();

// Sets Device objects dirty
IMGUI_IMPL_API void     ImGui_ImplAdxe_SetDeviceObjectsDirty();
