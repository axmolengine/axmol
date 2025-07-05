set(target_name ImGui)

include_directories(src/ImGui/imgui)

set(HEADER
    src/ImGui/ImGuiPresenter.h
    src/ImGui/imgui/imconfig.h
    src/ImGui/imgui/imgui.h
    src/ImGui/imgui/imgui_internal.h
    src/ImGui/imgui/imstb_rectpack.h
    src/ImGui/imgui/imstb_textedit.h
    src/ImGui/imgui/imstb_truetype.h
    src/ImGui/imgui/misc/cpp/imgui_stdlib.h
    src/ImGui/imgui/misc/freetype/imgui_freetype.h
)

set(SOURCE
    src/ImGui/ImGuiPresenter.cpp
    src/ImGui/imgui/imgui.cpp
    src/ImGui/imgui/imgui_demo.cpp
    src/ImGui/imgui/imgui_draw.cpp
    src/ImGui/imgui/imgui_widgets.cpp
    src/ImGui/imgui/imgui_tables.cpp
    src/ImGui/imgui/misc/cpp/imgui_stdlib.cpp
    src/ImGui/imgui/misc/freetype/imgui_freetype.cpp
)

list(APPEND SOURCE src/ImGui/imgui_impl_axmol.cpp)
list(APPEND HEADER src/ImGui/imgui_impl_axmol.h)

if(ANDROID)
    list(APPEND SOURCE src/ImGui/imgui_impl_android.cpp)
    list(APPEND HEADER src/ImGui/imgui_impl_android.h)
else()
    list(APPEND SOURCE src/ImGui/imgui_impl_glfw.cpp)
    list(APPEND HEADER src/ImGui/imgui_impl_glfw.h)
endif()

add_library(${target_name} STATIC
        ${HEADER}
        ${SOURCE})

if(WASM)
    target_compile_definitions(${target_name} PRIVATE EMSCRIPTEN_USE_EMBEDDED_GLFW3=1)
endif()

target_include_directories(${target_name} 
    PUBLIC src
    PUBLIC src/ImGui/imgui
    
    INTERFACE src
    INTERFACE src/ImGui/imgui
    )

ax_find_shaders(${CMAKE_CURRENT_LIST_DIR}/shaders IMGUI_SHADER_SOURCES)
ax_target_compile_shaders(${target_name} FILES ${IMGUI_SHADER_SOURCES} CUSTOM)

setup_ax_extension_config(${target_name})
