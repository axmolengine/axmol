#pragma once

#if defined(AX_PLATFORM_PC) || (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID) || defined(__EMSCRIPTEN__)
#    if __has_include("ImGui/ImGuiPresenter.h")
#        define AX_ENABLE_EXT_IMGUI 1
#    endif
#endif

#ifndef AX_ENABLE_EXT_IMGUI
#    define AX_ENABLE_EXT_IMGUI 0
#endif
