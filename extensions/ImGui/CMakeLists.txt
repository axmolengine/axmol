set(target_name ImGui)

set(HEADER
  src/ImGui/ImGuiPresenter.h
  src/ImGui/imconfig.h
  src/ImGui/imgui.h
  src/ImGui/imgui_internal.h
  src/ImGui/imstb_rectpack.h
  src/ImGui/imstb_textedit.h
  src/ImGui/misc/cpp/imgui_stdlib.h
  src/ImGui/misc/freetype/imgui_freetype.h
)

set(SOURCE
  src/ImGui/ImGuiPresenter.cpp
  src/ImGui/imgui.cpp
  src/ImGui/imgui_demo.cpp
  src/ImGui/imgui_draw.cpp
  src/ImGui/imgui_widgets.cpp
  src/ImGui/imgui_tables.cpp
  src/ImGui/misc/cpp/imgui_stdlib.cpp
  src/ImGui/misc/freetype/imgui_freetype.cpp
)

# implot
list(APPEND HEADER
  src/ImGui/implot/implot.h
  src/ImGui/implot/implot_internal.h
)

list(APPEND SOURCE
  src/ImGui/implot/implot.cpp
  src/ImGui/implot/implot_items.cpp
  src/ImGui/implot/implot_demo.cpp
)

list(APPEND SOURCE src/ImGui/backends/imgui_impl_axmol.cpp)
list(APPEND HEADER src/ImGui/backends/imgui_impl_axmol.h)

if(WINRT OR ANDROID OR IOS)
  list(APPEND SOURCE src/ImGui/backends/imgui_impl_axmol_sw.cpp)
  list(APPEND HEADER src/ImGui/backends/imgui_impl_axmol_sw.h)
else()
  list(APPEND SOURCE src/ImGui/backends/imgui_impl_glfw.cpp)
  list(APPEND HEADER src/ImGui/backends/imgui_impl_glfw.h)
endif()

add_library(${target_name} STATIC
  ${HEADER}
  ${SOURCE})

target_include_directories(${target_name}
  PRIVATE src/ImGui
  INTERFACE src
  INTERFACE src/ImGui/implot
)

ax_add_shader_target_for(${target_name} PATH "${CMAKE_CURRENT_LIST_DIR}/shaders")

setup_ax_extension_config(${target_name})
