cmake_minimum_required(VERSION 3.10)

set(APP_NAME lua_tests)

project(${APP_NAME})

set (_AX_TOP_LEVEL_APP FALSE)
if(NOT DEFINED BUILD_ENGINE_DONE)
    set(_AX_TOP_LEVEL_APP TRUE)
    if(XCODE)
        set(CMAKE_XCODE_GENERATE_TOP_LEVEL_PROJECT_ONLY TRUE)
    endif()

    set(_AX_ROOT "$ENV{AX_ROOT}")
    if(NOT (_AX_ROOT STREQUAL ""))
        set(_AX_ROOT_PATH "${_AX_ROOT}")
        file(TO_CMAKE_PATH ${_AX_ROOT_PATH} _AX_ROOT_PATH) # string(REPLACE "\\" "/" _AX_ROOT_PATH ${_AX_ROOT_PATH})
        message(STATUS "Using system env var _AX_ROOT=${_AX_ROOT}")
    else()
        message(FATAL_ERROR "Please run setup.py add system env var 'AX_ROOT' to specific the engine root")
    endif()
    set(CMAKE_MODULE_PATH ${_AX_ROOT_PATH}/cmake/Modules/)

    include(AXBuildSet)
    set(AX_ENABLE_EXT_LUA ON)

    set(_AX_USE_PREBUILT FALSE)
    if (WIN32 AND DEFINED AX_PREBUILT_DIR AND IS_DIRECTORY ${_AX_ROOT_PATH}/${AX_PREBUILT_DIR})
        set(_AX_USE_PREBUILT TRUE)
    endif()

    if (NOT _AX_USE_PREBUILT)
        add_subdirectory(${_AX_ROOT_PATH}/core ${ENGINE_BINARY_PATH}/axmol/core)
    endif()
endif()

# The common cross-platforms source files and header files
file(GLOB_RECURSE GAME_HEADER
    Source/*.h
    )
file(GLOB_RECURSE GAME_SOURCE
    Source/*.cpp
    )
    
set(GAME_INC_DIRS
  "${CMAKE_CURRENT_SOURCE_DIR}/Source"
)

set(content_folder "${CMAKE_CURRENT_SOURCE_DIR}/Content")
set(engine_script_folder "${_AX_ROOT_PATH}/extensions/scripting/lua-bindings/script")

if(APPLE)
    ax_mark_multi_resources(content_files RES_TO "Resources" FOLDERS ${content_folder})
    set(common_content_files ${content_files})
    if (NOT _AX_TOP_LEVEL_APP)
        ax_mark_multi_resources(engine_script_files RES_TO "Resources/src/axmol" FOLDERS ${engine_script_folder})
        set(common_content_files ${common_content_files} ${engine_script_files})
    endif()
elseif(WINDOWS)
    ax_mark_multi_resources(content_files RES_TO "Content" FOLDERS ${content_folder})
    set(common_content_files ${content_files})
    if (NOT _AX_TOP_LEVEL_APP)
        ax_mark_multi_resources(engine_script_files RES_TO "Content/src/axmol" FOLDERS ${engine_script_folder})
        set(common_content_files ${common_content_files} ${engine_script_files})
    endif()
endif()

if(ANDROID)
    # the APP_NAME should match on AndroidManifest.xml
    list(APPEND GAME_SOURCE
         proj.android/app/jni/main.cpp
         )
elseif(LINUX)
    list(APPEND GAME_SOURCE
         proj.linux/main.cpp
         )
    list(APPEND GAME_SOURCE ${common_content_files})
elseif(WINDOWS)
    if(NOT WINRT)
        list(APPEND GAME_HEADER
             proj.win32/main.h
             )
        list(APPEND GAME_SOURCE
             proj.win32/main.cpp
             )
    else()
        ax_setup_winrt_sources()
        ax_mark_multi_resources(res_files RES_TO "Content/res" FOLDERS "${_AX_ROOT_PATH}/tests/cpp-tests/Content")
        list(APPEND common_content_files ${res_files})
    endif()
    list(APPEND GAME_SOURCE ${common_content_files})
elseif(APPLE)
    if(IOS)
        list(APPEND GAME_HEADER
             proj.ios_mac/ios/AppController.h
             proj.ios_mac/ios/LuaObjectCBridgeTest.h
             proj.ios_mac/ios/RootViewController.h
             )

        if (TVOS)
            set(APP_UI_RES
                proj.ios_mac/ios/LaunchScreenBackground.png
                proj.ios_mac/ios/targets/tvos/LaunchScreen.storyboard
                proj.ios_mac/ios/targets/tvos/Images.xcassets
                )
        else()
            set(APP_UI_RES
                proj.ios_mac/ios/LaunchScreenBackground.png
                proj.ios_mac/ios/targets/ios/LaunchScreen.storyboard
                proj.ios_mac/ios/targets/ios/Images.xcassets
                )
        endif()

        list(APPEND GAME_SOURCE
             proj.ios_mac/ios/main.m
             proj.ios_mac/ios/LuaObjectCBridgeTest.mm
             proj.ios_mac/ios/AppController.mm
             proj.ios_mac/ios/RootViewController.mm
             ${APP_UI_RES}
             )
    elseif(MACOSX)
        list(APPEND GAME_HEADER
             proj.ios_mac/mac/LuaObjectCBridgeTest.h
             )
        set(APP_UI_RES
            proj.ios_mac/mac/Icon.icns
            proj.ios_mac/mac/Info.plist
            )
        list(APPEND GAME_SOURCE
             proj.ios_mac/mac/main.cpp
             proj.ios_mac/mac/LuaObjectCBridgeTest.mm
             ${APP_UI_RES}
             )
    endif()
    list(APPEND GAME_SOURCE ${common_content_files})
endif()

# mark app complie info and libs info
set(APP_SOURCES
    ${GAME_HEADER}
    ${GAME_SOURCE}
    )
if(NOT ANDROID)
    add_executable(${APP_NAME} ${APP_SOURCES})
else()
    add_library(${APP_NAME} SHARED ${APP_SOURCES})
    
    # whole archive for jni
    target_link_libraries(${APP_NAME} -Wl,--whole-archive cpp_android_spec -Wl,--no-whole-archive)

    add_subdirectory(${_AX_ROOT_PATH}/extensions/scripting/lua-bindings/proj.android ${ENGINE_BINARY_PATH}/extensions/lua-android)
    if(${CMAKE_ANDROID_ARCH_ABI} STREQUAL "x86")
        #add `-Wl,--no-warn-shared-textrel` to fix warning "shared library text segment is not shareable clang"
        target_link_libraries(${APP_NAME} -Wl,--whole-archive lua_android_spec -Wl,--no-whole-archive -Wl,--no-warn-shared-textrel)
    else()
        target_link_libraries(${APP_NAME} -Wl,--whole-archive lua_android_spec -Wl,--no-whole-archive)
    endif()

    config_android_shared_libs("org.axmol.lib" "${CMAKE_CURRENT_SOURCE_DIR}/proj.android/app/src")
endif()

if (NOT _AX_USE_PREBUILT)
    target_link_libraries(${APP_NAME} ${_AX_LUA_LIB})
endif()

# The optional thirdparties(not dependent by engine)
if (AX_WITH_YAML_CPP)
    list(APPEND GAME_INC_DIRS "${_AX_ROOT_PATH}/thirdparty/yaml-cpp/include")
endif()

target_include_directories(${APP_NAME} PRIVATE ${GAME_INC_DIRS})


# mark app resources, resource will be copy auto after mark
ax_setup_app_config(${APP_NAME})
if(APPLE)
    set_target_properties(${APP_NAME} PROPERTIES RESOURCE "${APP_UI_RES}")
    set_xcode_property(${APP_NAME} INSTALL_PATH "\$(LOCAL_APPS_DIR)")
    set_xcode_property(${APP_NAME} PRODUCT_BUNDLE_IDENTIFIER "org.axmol.lua-tests")

    if(MACOSX)
        set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios_mac/mac/Info.plist")
        set_target_properties(${APP_NAME} PROPERTIES
            LINK_FLAGS "-pagezero_size 10000 -image_base 100000000"
        )
    elseif(TVOS)
        set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}//proj.ios_mac/ios/targets/tvos/Info.plist")
        set_xcode_property(${APP_NAME} ASSETCATALOG_COMPILER_APPICON_NAME "Brand Assets")
        set_xcode_property(${APP_NAME} DEVELOPMENT_TEAM "")
        set_xcode_property(${APP_NAME} CODE_SIGN_IDENTIFY "iPhone Developer")
    elseif(IOS)
        set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}//proj.ios_mac/ios/targets/ios/Info.plist")
        set_xcode_property(${APP_NAME} ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon")
        set_xcode_property(${APP_NAME} DEVELOPMENT_TEAM "")
        set_xcode_property(${APP_NAME} CODE_SIGN_IDENTIFY "iPhone Developer")
    endif()

    # For code-signing, set the DEVELOPMENT_TEAM:
    #set_xcode_property(${APP_NAME} DEVELOPMENT_TEAM "GRLXXXX2K9")
elseif(WINDOWS AND (NOT WINRT))
    if(NOT _AX_USE_PREBUILT)
        ax_copy_target_dll(${APP_NAME})
        ax_copy_lua_dlls(${APP_NAME})
    endif()
endif()

if ((NOT APPLE) AND (NOT WINRT))
    ax_sync_target_res(${APP_NAME} LINK_TO "${CMAKE_CURRENT_SOURCE_DIR}/Content/res" SYNC_TARGET_ID "1" FOLDERS "${_AX_ROOT_PATH}/tests/cpp-tests/Content" SYM_LINK 1)

    ax_get_resource_path(APP_RES_DIR ${APP_NAME})
    if (NOT _AX_TOP_LEVEL_APP)
        ax_sync_lua_scripts(${APP_NAME} ${engine_script_folder} "${content_folder}/src/axmol")
    endif()
    ax_sync_target_res(${APP_NAME} LINK_TO ${APP_RES_DIR} FOLDERS ${content_folder} SYM_LINK 1)

    if((WINDOWS AND (NOT (CMAKE_GENERATOR MATCHES "Ninja"))))
        set_property(TARGET ${APP_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${content_folder}")
        if(NOT DEFINED BUILD_ENGINE_DONE)
            set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${APP_NAME})
        endif()
    endif()
elseif(WINRT)
    if(NOT DEFINED BUILD_ENGINE_DONE)
        set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${APP_NAME})
    endif()
endif()

if (_AX_USE_PREBUILT) # support windows and linux
    use_ax_compile_define(${APP_NAME})

    include(${_AX_ROOT_PATH}/cmake/Modules/AXLinkHelpers.cmake)
    ax_link_lua_prebuilt(${APP_NAME} ${_AX_ROOT_PATH} ${AX_PREBUILT_DIR})
endif()

if (NOT DEFINED BUILD_ENGINE_DONE)
    ax_uwp_set_all_targets_deploy_min_version()
endif()
