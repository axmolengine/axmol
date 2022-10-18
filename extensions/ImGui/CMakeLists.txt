set(target_name ImGui)

include_directories(imgui)

set(HEADER
    ImGuiPresenter.h
    # CCImGuiColorTextEdit.h
    imgui/imconfig.h
    imgui/imgui.h
    imgui/imgui_internal.h
    imgui/imstb_rectpack.h
    imgui/imstb_textedit.h
    imgui/imstb_truetype.h
    imgui/misc/cpp/imgui_stdlib.h
    #~ imgui_markdown/imgui_markdown.h
    #~ ImGuiColorTextEdit/TextEditor.h
    #~ implot/implot.h
)

set(SOURCE
    ImGuiPresenter.cpp
    # CCImGuiColorTextEdit.cpp
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_tables.cpp
    imgui/misc/cpp/imgui_stdlib.cpp
    #~ ImGuiColorTextEdit/TextEditor.cpp
    #~ implot/implot.cpp
    #~ implot/implot_demo.cpp
)

if(ANDROID)
    list(APPEND HEADER imgui_impl_ax_android.cpp)
    list(APPEND SOURCE imgui_impl_ax_android.h)
else()
    list(APPEND HEADER imgui_impl_ax.cpp)
    list(APPEND SOURCE imgui_impl_ax.h)
endif()

#~ if(AX_ENABLE_EXT_LUA)
#~     include_directories(
#~         lua-bindings
#~         ${_AX_ROOT_PATH}/thirdparty/lua/luajit/include
#~         ${_AX_ROOT_PATH}/thirdparty/lua/tolua
#~     )
#~     list(APPEND HEADER
#~         lua-bindings/imgui_lua.hpp
#~         lua-bindings/lua_conversion.hpp
#~         lua-bindings/lua_imgui_auto.hpp
#~         lua-bindings/lua_imguiDrawList_auto.hpp
#~         lua-bindings/lua_imguiFont_auto.hpp
#~         lua-bindings/lua_imguiIO_auto.hpp
#~         lua-bindings/lua_imguiStyle_auto.hpp
#~         lua-bindings/lua_imguiViewport_auto.hpp
#~         lua-bindings/lua_ImGuiColorTextEdit_auto.hpp
#~         lua-bindings/lua_implot_auto.hpp
#~     )
#~     list(APPEND SOURCE
#~         lua-bindings/imgui_lua.cpp
#~         lua-bindings/lua_imguiDrawList_auto.cpp
#~         lua-bindings/lua_imguiFont_auto.cpp
#~         lua-bindings/lua_imguiIO_auto.cpp
#~         lua-bindings/lua_imguiStyle_auto.cpp
#~         lua-bindings/lua_imguiViewport_auto.cpp
#~         lua-bindings/lua_imgui_auto.cpp
#~         lua-bindings/lua_ImGuiColorTextEdit_auto.cpp
#~         lua-bindings/lua_implot_auto.cpp
#~     )
#~ endif()

add_library(${target_name} STATIC
        ${HEADER}
        ${SOURCE})

setup_ax_extension_config(${target_name})
