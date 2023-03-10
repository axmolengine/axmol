set(ax_root ${_AX_ROOT_PATH})

set(lua_bindings_manual_headers
    manual/CCComponentLua.h
    manual/3d/axlua_3d_manual.h
    manual/CCLuaStack.h
    manual/CCLuaEngine.h
    manual/lua_module_register.h
    manual/CCLuaBridge.h
    manual/extension/axlua_extension_manual.h
    manual/LuaBasicConversions.h
    manual/spine/LuaSkeletonAnimation.h
    manual/platform/ios/CCLuaObjcBridge.h
    manual/network/axlua_network_manual.h
    manual/network/lua_xml_http_request.h
    manual/network/lua_downloader.h
    manual/audioengine/axlua_audioengine_manual.h
    manual/Lua-BindingsExport.h
    manual/tolua_fix.h
    manual/navmesh/axlua_navmesh_manual.h
    manual/navmesh/axlua_navmesh_conversions.h
    manual/base/LuaScriptHandlerMgr.h
    manual/base/axlua_base_manual.hpp
    manual/AxluaLoader.h
    manual/CCLuaValue.h
    manual/physics3d/axlua_physics3d_manual.h
    manual/cocostudio/lua-cocostudio-conversions.h
    manual/cocostudio/CustomGUIReader.h
    manual/ui/axlua_ui_manual.hpp
    manual/controller/axlua_controller_manual.hpp
    )

set(lua_bindings_manual_files
    manual/CCLuaBridge.cpp
    manual/CCLuaEngine.cpp
    manual/CCLuaStack.cpp
    manual/CCLuaValue.cpp
    manual/AxluaLoader.cpp
    manual/LuaBasicConversions.cpp
    manual/lua_module_register.cpp
    manual/tolua_fix.cpp
    manual/CCComponentLua.cpp
    manual/base/LuaScriptHandlerMgr.cpp
    manual/base/axlua_base_manual.cpp
    manual/physics/axlua_physics_manual.cpp
    manual/3d/axlua_3d_manual.cpp
    manual/cocostudio/CustomGUIReader.cpp
    manual/cocostudio/axlua_cocostudio_manual.cpp
    manual/cocostudio/lua-cocostudio-conversions.cpp
    manual/cocostudio/axlua_csloader_manual.cpp
    manual/controller/axlua_controller_manual.cpp
    manual/extension/axlua_extension_manual.cpp
    manual/network/axlua_network_manual.cpp
    manual/network/lua_xml_http_request.cpp
    manual/network/lua_downloader.cpp
    manual/spine/axlua_spine_manual.cpp
    manual/spine/LuaSkeletonAnimation.cpp
    manual/ui/axlua_ui_manual.cpp
    manual/audioengine/axlua_audioengine_manual.cpp
    manual/physics3d/axlua_physics3d_manual.cpp
    manual/navmesh/axlua_navmesh_conversions.cpp
    manual/navmesh/axlua_navmesh_manual.cpp
    )

set(lua_bindings_auto_headers
    auto/axlua_audioengine_auto.hpp
    auto/axlua_3d_auto.hpp
    auto/axlua_controller_auto.hpp
    auto/axlua_spine_auto.hpp
    auto/axlua_studio_auto.hpp
    auto/axlua_base_auto.hpp
    auto/axlua_navmesh_auto.hpp
    auto/axlua_csloader_auto.hpp
    auto/axlua_physics3d_auto.hpp
    auto/axlua_ui_auto.hpp
    auto/axlua_physics_auto.hpp
    auto/axlua_extension_auto.hpp
    auto/axlua_backend_auto.hpp
    )

set(lua_bindings_auto_files
    auto/axlua_3d_auto.cpp
    auto/axlua_base_auto.cpp
    auto/axlua_controller_auto.cpp
    auto/axlua_extension_auto.cpp
    auto/axlua_physics_auto.cpp
    auto/axlua_spine_auto.cpp
    auto/axlua_studio_auto.cpp
    auto/axlua_csloader_auto.cpp
    auto/axlua_ui_auto.cpp
    auto/axlua_audioengine_auto.cpp
    auto/axlua_physics3d_auto.cpp
    auto/axlua_navmesh_auto.cpp
    auto/axlua_backend_auto.cpp
    )

if(APPLE)
    # CCLuaObjcBridge.mm needed by ios and mac
    set(lua_bindings_manual_headers
        ${lua_bindings_manual_headers}
        manual/platform/ios/CCLuaObjcBridge.h
        )
    set(lua_bindings_manual_files
        ${lua_bindings_manual_files}
        manual/platform/ios/CCLuaObjcBridge.mm
        )
    if(IOS)
        set(lua_bindings_manual_headers
            ${lua_bindings_manual_headers}
            manual/ui/axlua_webview_manual.hpp
            auto/axlua_webview_auto.hpp
            auto/axlua_video_auto.hpp
            manual/ui/axlua_video_manual.hpp
            )
        set(lua_bindings_manual_files
            ${lua_bindings_manual_files}
            manual/ui/axlua_webview_manual.cpp
            auto/axlua_webview_auto.cpp
            manual/ui/axlua_video_manual.cpp
            auto/axlua_video_auto.cpp
            )
    endif()
elseif(ANDROID)
    set(lua_bindings_manual_headers
        ${lua_bindings_manual_headers}
        manual/ui/axlua_webview_manual.hpp
        auto/axlua_webview_auto.hpp
        auto/axlua_video_auto.hpp
        manual/ui/axlua_video_manual.hpp
        )
    set(lua_bindings_manual_files
        ${lua_bindings_manual_files}
        manual/ui/axlua_webview_manual.cpp
        auto/axlua_webview_auto.cpp
        manual/ui/axlua_video_manual.cpp
        auto/axlua_video_auto.cpp
        )
elseif(WINDOWS)
    if(AX_ENABLE_MFMEDIA)
        set(lua_bindings_manual_headers
            ${lua_bindings_manual_headers}
            auto/axlua_video_auto.hpp
            manual/ui/axlua_video_manual.hpp
            )
        set(lua_bindings_manual_files
            ${lua_bindings_manual_files}
            manual/ui/axlua_video_manual.cpp
            auto/axlua_video_auto.cpp
            )
    endif()
    if(AX_ENABLE_MSEDGE_WEBVIEW2)
        set(lua_bindings_manual_headers
            ${lua_bindings_manual_headers}
            manual/ui/axlua_webview_manual.hpp
            auto/axlua_webview_auto.hpp
            )
        set(lua_bindings_manual_files
            ${lua_bindings_manual_files}
            manual/ui/axlua_webview_manual.cpp
            auto/axlua_webview_auto.cpp
            )
    endif()
endif()

set(lua_bindings_files
    ${lua_bindings_manual_files}
    ${lua_bindings_auto_files}
    ${_AX_DEPRECATED_SRC}
    )
set(lua_bindings_headers
    ${lua_bindings_manual_headers}
    ${lua_bindings_auto_headers}
    ${_AX_DEPRECATED_HEADER}
    )

set(lua_bindings_files
    ${lua_bindings_files}
    "${ax_root}/thirdparty/yasio/bindings/lyasio.cpp"
    "${ax_root}/thirdparty/yasio/bindings/yasio_cclua.cpp")

list(APPEND lua_bindings_files ${lua_bindings_headers})

set(_AX_LUA_LIB axlua CACHE INTERNAL "The axmol lua bindings lib name" )

add_library(${_AX_LUA_LIB} STATIC ${lua_bindings_files})
target_link_libraries(${_AX_LUA_LIB} ${_AX_CORE_LIB})
add_dependencies(${_AX_LUA_LIB} ${_AX_CORE_LIB})

if(MSVC)
    target_compile_options(${_AX_LUA_LIB} PRIVATE /bigobj)
endif()

get_target_property(lua_ext_depend thirdparty AX_LUA_DEPEND)
target_link_libraries(${_AX_LUA_LIB} ${lua_ext_depend})

target_include_directories(${_AX_LUA_LIB}
    PUBLIC ${ax_root}/thirdparty/lua
    PRIVATE manual/platform/android
    PRIVATE manual/platform/android/jni
    PRIVATE ${ax_root}/extensions/spine/runtime/include
    PRIVATE ${ax_root}/extensions/cocostudio
    INTERFACE ${ax_root}/extensions/scripting/lua-bindings/manual
    PRIVATE ${ax_root}/extensions/spine/runtime/include
)

if(WINDOWS)
    target_compile_definitions(${_AX_LUA_LIB} PUBLIC _USRLUASTATIC)
    if (WINRT)
        target_compile_options(${_AX_LUA_LIB} PUBLIC /wd4700)
    endif()
endif()

set_target_properties(${_AX_LUA_LIB}
                      PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                      FOLDER "Extensions"
                      )

if(XCODE OR VS)
    ax_mark_code_files("${_AX_LUA_LIB}")
endif()

