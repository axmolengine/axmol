set(cocos_root ${COCOS2DX_ROOT_PATH})

set(lua_bindings_manual_headers
    manual/CCComponentLua.h
    manual/3d/lua_cocos2dx_3d_manual.h
    manual/CCLuaStack.h
    manual/CCLuaEngine.h
    manual/lua_module_register.h
    manual/CCLuaBridge.h
    manual/extension/lua_cocos2dx_extension_manual.h
    manual/LuaBasicConversions.h
    manual/spine/LuaSkeletonAnimation.h
    manual/platform/ios/CCLuaObjcBridge.h
    manual/network/lua_cocos2dx_network_manual.h
    manual/network/lua_xml_http_request.h
    manual/network/lua_downloader.h
    manual/audioengine/lua_cocos2dx_audioengine_manual.h
    manual/Lua-BindingsExport.h
    manual/tolua_fix.h
    manual/navmesh/lua_cocos2dx_navmesh_manual.h
    manual/navmesh/lua_cocos2dx_navmesh_conversions.h
    manual/cocos2d/lua_cocos2dx_deprecated.h
    manual/cocos2d/LuaScriptHandlerMgr.h
    manual/cocos2d/lua_cocos2dx_manual.hpp
    manual/Cocos2dxLuaLoader.h
    manual/CCLuaValue.h
    manual/physics3d/lua_cocos2dx_physics3d_manual.h
    manual/cocostudio/lua-cocos-studio-conversions.h
    manual/cocostudio/CustomGUIReader.h
    manual/ui/lua_cocos2dx_ui_manual.hpp
    manual/controller/lua_cocos2dx_controller_manual.hpp
    )

set(lua_bindings_manual_files
    manual/CCLuaBridge.cpp
    manual/CCLuaEngine.cpp
    manual/CCLuaStack.cpp
    manual/CCLuaValue.cpp
    manual/Cocos2dxLuaLoader.cpp
    manual/LuaBasicConversions.cpp
    manual/lua_module_register.cpp
    manual/tolua_fix.cpp
    manual/CCComponentLua.cpp
    manual/cocos2d/LuaScriptHandlerMgr.cpp
    manual/cocos2d/lua_cocos2dx_deprecated.cpp
    manual/cocos2d/lua_cocos2dx_manual.cpp
    manual/cocos2d/lua_cocos2dx_physics_manual.cpp
    manual/3d/lua_cocos2dx_3d_manual.cpp
    manual/cocostudio/CustomGUIReader.cpp
    manual/cocostudio/lua_cocos2dx_coco_studio_manual.cpp
    manual/cocostudio/lua-cocos-studio-conversions.cpp
    manual/cocostudio/lua_cocos2dx_csloader_manual.cpp
    manual/controller/lua_cocos2dx_controller_manual.cpp
    manual/extension/lua_cocos2dx_extension_manual.cpp
    manual/network/lua_cocos2dx_network_manual.cpp
    manual/network/lua_xml_http_request.cpp
    manual/network/lua_downloader.cpp
    manual/spine/lua_cocos2dx_spine_manual.cpp
    manual/spine/LuaSkeletonAnimation.cpp
    manual/ui/lua_cocos2dx_ui_manual.cpp
    manual/audioengine/lua_cocos2dx_audioengine_manual.cpp
    manual/physics3d/lua_cocos2dx_physics3d_manual.cpp
    manual/navmesh/lua_cocos2dx_navmesh_conversions.cpp
    manual/navmesh/lua_cocos2dx_navmesh_manual.cpp
    )

set(lua_bindings_auto_headers
    auto/lua_cocos2dx_audioengine_auto.hpp
    auto/lua_cocos2dx_3d_auto.hpp
    auto/lua_cocos2dx_controller_auto.hpp
    auto/lua_cocos2dx_spine_auto.hpp
    auto/lua_cocos2dx_studio_auto.hpp
    auto/lua_cocos2dx_auto.hpp
    auto/lua_cocos2dx_navmesh_auto.hpp
    auto/lua_cocos2dx_csloader_auto.hpp
    auto/lua_cocos2dx_physics3d_auto.hpp
    auto/lua_cocos2dx_ui_auto.hpp
    auto/lua_cocos2dx_physics_auto.hpp
    auto/lua_cocos2dx_extension_auto.hpp
    auto/lua_cocos2dx_backend_auto.hpp
    )

set(lua_bindings_auto_files
    auto/lua_cocos2dx_3d_auto.cpp
    auto/lua_cocos2dx_auto.cpp
    auto/lua_cocos2dx_controller_auto.cpp
    auto/lua_cocos2dx_extension_auto.cpp
    auto/lua_cocos2dx_physics_auto.cpp
    auto/lua_cocos2dx_spine_auto.cpp
    auto/lua_cocos2dx_studio_auto.cpp
    auto/lua_cocos2dx_csloader_auto.cpp
    auto/lua_cocos2dx_ui_auto.cpp
    auto/lua_cocos2dx_audioengine_auto.cpp
    auto/lua_cocos2dx_physics3d_auto.cpp
    auto/lua_cocos2dx_navmesh_auto.cpp
    auto/lua_cocos2dx_backend_auto.cpp
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
            manual/ui/lua_cocos2dx_webview_manual.hpp
            auto/lua_cocos2dx_webview_auto.hpp
            auto/lua_cocos2dx_video_auto.hpp
            manual/ui/lua_cocos2dx_video_manual.hpp
            )
        set(lua_bindings_manual_files
            ${lua_bindings_manual_files}
            manual/ui/lua_cocos2dx_webview_manual.cpp
            auto/lua_cocos2dx_webview_auto.cpp
            manual/ui/lua_cocos2dx_video_manual.cpp
            auto/lua_cocos2dx_video_auto.cpp
            )
    endif()
elseif(ANDROID)
    set(lua_bindings_manual_headers
        ${lua_bindings_manual_headers}
        manual/ui/lua_cocos2dx_webview_manual.hpp
        auto/lua_cocos2dx_webview_auto.hpp
        auto/lua_cocos2dx_video_auto.hpp
        manual/ui/lua_cocos2dx_video_manual.hpp
        )
    set(lua_bindings_manual_files
        ${lua_bindings_manual_files}
        manual/ui/lua_cocos2dx_webview_manual.cpp
        auto/lua_cocos2dx_webview_auto.cpp
        manual/ui/lua_cocos2dx_video_manual.cpp
        auto/lua_cocos2dx_video_auto.cpp
        )
endif()

option(BUILD_YASIO_LUA "Build yasio with cclua support" ON)

include(../deprecated/CMakeLists.txt)

if(BUILD_YASIO_LUA)
    set(lua_bindings_files
        ${lua_bindings_files}
        ${cocos_root}/external/yasio/bindings/lyasio.cpp
        ${cocos_root}/external/yasio/bindings/yasio_cclua.cpp)

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

list(APPEND lua_bindings_files ${lua_bindings_headers})

set(EGNX_LUA_LIB libcclua CACHE INTERNAL "The EGNX lua bindings lib name" )

add_library(${EGNX_LUA_LIB} STATIC ${lua_bindings_files})
target_link_libraries(${EGNX_LUA_LIB} ${EGNX_CORE_LIB})
add_dependencies(${EGNX_LUA_LIB} ${EGNX_CORE_LIB})

if(BUILD_YASIO_LUA)
    target_compile_definitions(${EGNX_LUA_LIB} PRIVATE CC_ENABLE_YASIO_LUA=1)
endif()

get_target_property(lua_ext_depend external CC_LUA_DEPEND)
target_link_libraries(${EGNX_LUA_LIB} ${lua_ext_depend})

target_include_directories(${EGNX_LUA_LIB}
    PRIVATE manual/platform/android
    PRIVATE manual/platform/android/jni
    PRIVATE ${cocos_root}/extensions/spine/runtime/include
    INTERFACE ${cocos_root}/extensions/scripting/lua-bindings/auto
    INTERFACE ${cocos_root}/extensions/scripting/lua-bindings/manual
)

if(WINDOWS)
    target_compile_definitions(${EGNX_LUA_LIB} PUBLIC _USRLUASTATIC)
endif()

target_compile_definitions(${EGNX_LUA_LIB} PUBLIC LUA_COMPAT_MODULE)

set_target_properties(${EGNX_LUA_LIB}
                      PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                      FOLDER "Extensions"
                      )

if(XCODE OR VS)
    cocos_mark_code_files("${EGNX_LUA_LIB}")
endif()

