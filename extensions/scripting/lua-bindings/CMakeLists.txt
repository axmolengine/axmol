set(cocos_root ${AXYS_ROOT_PATH})

set(lua_bindings_manual_headers
    manual/CCComponentLua.h
    manual/3d/lua_axis_3d_manual.h
    manual/CCLuaStack.h
    manual/CCLuaEngine.h
    manual/lua_module_register.h
    manual/CCLuaBridge.h
    manual/extension/lua_axis_extension_manual.h
    manual/LuaBasicConversions.h
    manual/spine/LuaSkeletonAnimation.h
    manual/platform/ios/CCLuaObjcBridge.h
    manual/network/lua_axis_network_manual.h
    manual/network/lua_xml_http_request.h
    manual/network/lua_downloader.h
    manual/audioengine/lua_axis_audioengine_manual.h
    manual/Lua-BindingsExport.h
    manual/tolua_fix.h
    manual/navmesh/lua_axis_navmesh_manual.h
    manual/navmesh/lua_axis_navmesh_conversions.h
    manual/base/LuaScriptHandlerMgr.h
    manual/base/lua_axis_base_manual.hpp
    manual/AxisLuaLoader.h
    manual/CCLuaValue.h
    manual/physics3d/lua_axis_physics3d_manual.h
    manual/cocostudio/lua-cocostudio-conversions.h
    manual/cocostudio/CustomGUIReader.h
    manual/ui/lua_axis_ui_manual.hpp
    manual/controller/lua_axis_controller_manual.hpp
    )

set(lua_bindings_manual_files
    manual/CCLuaBridge.cpp
    manual/CCLuaEngine.cpp
    manual/CCLuaStack.cpp
    manual/CCLuaValue.cpp
    manual/AxisLuaLoader.cpp
    manual/LuaBasicConversions.cpp
    manual/lua_module_register.cpp
    manual/tolua_fix.cpp
    manual/CCComponentLua.cpp
    manual/base/LuaScriptHandlerMgr.cpp
    manual/base/lua_axis_base_manual.cpp
    manual/physics/lua_axis_physics_manual.cpp
    manual/3d/lua_axis_3d_manual.cpp
    manual/cocostudio/CustomGUIReader.cpp
    manual/cocostudio/lua_axis_cocostudio_manual.cpp
    manual/cocostudio/lua-cocostudio-conversions.cpp
    manual/cocostudio/lua_axis_csloader_manual.cpp
    manual/controller/lua_axis_controller_manual.cpp
    manual/extension/lua_axis_extension_manual.cpp
    manual/network/lua_axis_network_manual.cpp
    manual/network/lua_xml_http_request.cpp
    manual/network/lua_downloader.cpp
    manual/spine/lua_axis_spine_manual.cpp
    manual/spine/LuaSkeletonAnimation.cpp
    manual/ui/lua_axis_ui_manual.cpp
    manual/audioengine/lua_axis_audioengine_manual.cpp
    manual/physics3d/lua_axis_physics3d_manual.cpp
    manual/navmesh/lua_axis_navmesh_conversions.cpp
    manual/navmesh/lua_axis_navmesh_manual.cpp
    )

set(lua_bindings_auto_headers
    auto/lua_axis_audioengine_auto.hpp
    auto/lua_axis_3d_auto.hpp
    auto/lua_axis_controller_auto.hpp
    auto/lua_axis_spine_auto.hpp
    auto/lua_axis_studio_auto.hpp
    auto/lua_axis_base_auto.hpp
    auto/lua_axis_navmesh_auto.hpp
    auto/lua_axis_csloader_auto.hpp
    auto/lua_axis_physics3d_auto.hpp
    auto/lua_axis_ui_auto.hpp
    auto/lua_axis_physics_auto.hpp
    auto/lua_axis_extension_auto.hpp
    auto/lua_axis_backend_auto.hpp
    )

set(lua_bindings_auto_files
    auto/lua_axis_3d_auto.cpp
    auto/lua_axis_base_auto.cpp
    auto/lua_axis_controller_auto.cpp
    auto/lua_axis_extension_auto.cpp
    auto/lua_axis_physics_auto.cpp
    auto/lua_axis_spine_auto.cpp
    auto/lua_axis_studio_auto.cpp
    auto/lua_axis_csloader_auto.cpp
    auto/lua_axis_ui_auto.cpp
    auto/lua_axis_audioengine_auto.cpp
    auto/lua_axis_physics3d_auto.cpp
    auto/lua_axis_navmesh_auto.cpp
    auto/lua_axis_backend_auto.cpp
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
            manual/ui/lua_axis_webview_manual.hpp
            auto/lua_axis_webview_auto.hpp
            auto/lua_axis_video_auto.hpp
            manual/ui/lua_axis_video_manual.hpp
            )
        set(lua_bindings_manual_files
            ${lua_bindings_manual_files}
            manual/ui/lua_axis_webview_manual.cpp
            auto/lua_axis_webview_auto.cpp
            manual/ui/lua_axis_video_manual.cpp
            auto/lua_axis_video_auto.cpp
            )
    endif()
elseif(ANDROID)
    set(lua_bindings_manual_headers
        ${lua_bindings_manual_headers}
        manual/ui/lua_axis_webview_manual.hpp
        auto/lua_axis_webview_auto.hpp
        auto/lua_axis_video_auto.hpp
        manual/ui/lua_axis_video_manual.hpp
        )
    set(lua_bindings_manual_files
        ${lua_bindings_manual_files}
        manual/ui/lua_axis_webview_manual.cpp
        auto/lua_axis_webview_auto.cpp
        manual/ui/lua_axis_video_manual.cpp
        auto/lua_axis_video_auto.cpp
        )
elseif(WINDOWS)
    if(AX_ENABLE_MFMEDIA)
        set(lua_bindings_manual_headers
            ${lua_bindings_manual_headers}
            auto/lua_axis_video_auto.hpp
            manual/ui/lua_axis_video_manual.hpp
            )
        set(lua_bindings_manual_files
            ${lua_bindings_manual_files}
            manual/ui/lua_axis_video_manual.cpp
            auto/lua_axis_video_auto.cpp
            )
    endif()
    if(AX_ENABLE_MSEDGE_WEBVIEW2)
        set(lua_bindings_manual_headers
            ${lua_bindings_manual_headers}
            manual/ui/lua_axis_webview_manual.hpp
            auto/lua_axis_webview_auto.hpp
            )
        set(lua_bindings_manual_files
            ${lua_bindings_manual_files}
            manual/ui/lua_axis_webview_manual.cpp
            auto/lua_axis_webview_auto.cpp
            )
    endif()
endif()

set(lua_bindings_files
    ${lua_bindings_manual_files}
    ${lua_bindings_auto_files}
    ${COCOS_DEPRECATED_SRC}
    )
set(lua_bindings_headers
    ${lua_bindings_manual_headers}
    ${lua_bindings_auto_headers}
    ${COCOS_DEPRECATED_HEADER}
    )

set(lua_bindings_files
    ${lua_bindings_files}
    "${cocos_root}/thirdparty/yasio/bindings/lyasio.cpp"
    "${cocos_root}/thirdparty/yasio/bindings/yasio_cclua.cpp")

list(APPEND lua_bindings_files ${lua_bindings_headers})

set(AXIS_LUA_LIB axislua CACHE INTERNAL "The AXIS lua bindings lib name" )

add_library(${AXIS_LUA_LIB} STATIC ${lua_bindings_files})
target_link_libraries(${AXIS_LUA_LIB} ${AXIS_CORE_LIB})
add_dependencies(${AXIS_LUA_LIB} ${AXIS_CORE_LIB})

if(MSVC)
    target_compile_options(${AXIS_LUA_LIB} PRIVATE /bigobj)
endif()

get_target_property(lua_ext_depend thirdparty AX_LUA_DEPEND)
target_link_libraries(${AXIS_LUA_LIB} ${lua_ext_depend})

target_include_directories(${AXIS_LUA_LIB}
    PUBLIC ${cocos_root}/thirdparty/lua
    PRIVATE manual/platform/android
    PRIVATE manual/platform/android/jni
    PRIVATE ${cocos_root}/extensions/spine/runtime/include
    PRIVATE ${cocos_root}/extensions/cocostudio
    INTERFACE ${cocos_root}/extensions/scripting/lua-bindings/manual
    PRIVATE ${cocos_root}/extensions/spine/runtime/include
)

if(WINDOWS)
    target_compile_definitions(${AXIS_LUA_LIB} PUBLIC _USRLUASTATIC)
endif()

if(AX_ENABLE_MSEDGE_WEBVIEW2)
endif()

axis_config_pred(${AXIS_LUA_LIB} AX_ENABLE_MFMEDIA)
axis_config_pred(${AXIS_LUA_LIB} AX_ENABLE_MSEDGE_WEBVIEW2)

set_target_properties(${AXIS_LUA_LIB}
                      PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                      FOLDER "Extensions"
                      )

if(XCODE OR VS)
    cocos_mark_code_files("${AXIS_LUA_LIB}")
endif()

