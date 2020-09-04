set(target_name ImGui)

#~ if(WINDOWS)
#~     include_directories(${COCOS2DX_ROOT_PATH}/external/win32-specific/gles/include/OGLES)
#~ endif()
include_directories(imgui)

set(HEADER
    CCImGuiEXT.h
    CCImGuiLayer.h
    # CCImGuiColorTextEdit.h
    imgui_impl_cocos2dx.h
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
    CCImGuiEXT.cpp
    CCImGuiLayer.cpp
    # CCImGuiColorTextEdit.cpp
    imgui_impl_cocos2dx.cpp
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
    imgui/misc/cpp/imgui_stdlib.cpp
    #~ ImGuiColorTextEdit/TextEditor.cpp
    #~ implot/implot.cpp
    #~ implot/implot_demo.cpp
)

#~ if(BUILD_LUA_LIBS)
#~     include_directories(
#~         lua-bindings
#~         ${COCOS2DX_ROOT_PATH}/external/lua/luajit/include
#~         ${COCOS2DX_ROOT_PATH}/external/lua/tolua
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

setup_cocos_extension_config(${target_name})
