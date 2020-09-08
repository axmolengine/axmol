#pragma once
#include "imgui/imgui.h"

typedef void (*ImGuiImplCocos2dxLoadFontFun)(void* userdata);

IMGUI_IMPL_API bool     ImGui_ImplCocos2dx_Init(bool install_callbacks);
IMGUI_IMPL_API void     ImGui_ImplCocos2dx_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplCocos2dx_NewFrame();
IMGUI_IMPL_API void     ImGui_ImplCocos2dx_RenderDrawData(ImDrawData* draw_data);
IMGUI_IMPL_API void     ImGui_ImplCocos2dx_RenderPlatform();

// Called by Init/NewFrame/Shutdown
// TODO: maybe hide for internal use only
IMGUI_IMPL_API bool     ImGui_ImplCocos2dx_CreateDeviceObjects();
IMGUI_IMPL_API void     ImGui_ImplCocos2dx_DestroyDeviceObjects();

// TODO: check whether needs public this API
IMGUI_IMPL_API bool     ImGui_ImplCocos2dx_CreateFontsTexture();
IMGUI_IMPL_API void     ImGui_ImplCocos2dx_DestroyFontsTexture();


// Get FontTexture object cocos2d::Texture2D*
IMGUI_IMPL_API void     ImGui_ImplCocos2dx_SetCustomFontLoader(ImGuiImplCocos2dxLoadFontFun fun, void* userdata);
IMGUI_IMPL_API void*    ImGui_ImplCocos2dx_GetFontsTexture();

// Sets Device objects dirty
IMGUI_IMPL_API void     ImGui_ImplCocos2dx_SetDeviceObjectsDirty();
