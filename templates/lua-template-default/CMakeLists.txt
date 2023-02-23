#/****************************************************************************
# Copyright (c) 2013-2014 cocos2d-x.org
# Copyright (c) 2021-2022 Bytedance Inc.
#
# https://axmolengine.github.io/
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
# ****************************************************************************/

cmake_minimum_required(VERSION 3.10)

set(APP_NAME HelloLua)

project(${APP_NAME})

set(_AX_TOP_LEVEL_APP FALSE)
if(NOT DEFINED BUILD_ENGINE_DONE) # to test HelloLua into root project
    set(_AX_TOP_LEVEL_APP TRUE)
    if(XCODE)
        set(CMAKE_XCODE_GENERATE_TOP_LEVEL_PROJECT_ONLY TRUE)
    endif()

    # config quick starter batch script run.bat for windows
    if(WIN32)
        file(RELATIVE_PATH CMAKE_BUILD_RELATIVE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}/run.bat.in" "${CMAKE_CURRENT_SOURCE_DIR}/run.bat" @ONLY)
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
    if ((WIN32 OR LINUX) AND DEFINED AX_PREBUILT_DIR AND IS_DIRECTORY ${_AX_ROOT_PATH}/${AX_PREBUILT_DIR})
        set(_AX_USE_PREBUILT TRUE)
    endif()

    if (NOT _AX_USE_PREBUILT)
        add_subdirectory(${_AX_ROOT_PATH}/core ${ENGINE_BINARY_PATH}/axmol/core)
    endif()
endif()

set(RUNTIME_SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# script and source files info, not need to compile
set(content_folder "${CMAKE_CURRENT_SOURCE_DIR}/Content")
set(engine_script_folder "${_AX_ROOT_PATH}/extensions/scripting/lua-bindings/script")
if(APPLE OR VS)
    ax_mark_multi_resources(content_files RES_TO "Content" FOLDERS ${content_folder})
    set(common_content_files ${content_files})
    if (NOT _AX_TOP_LEVEL_APP)
        ax_mark_multi_resources(engine_script_files RES_TO "Content/src/axmol" FOLDERS ${content_folder})
        set(common_content_files ${common_content_files} ${engine_script_files})
    endif()
endif()

# record sources, headers
file(GLOB_RECURSE GAME_HEADER ${RUNTIME_SRC_ROOT}/Source/*.h)
file(GLOB_RECURSE GAME_SOURCE ${RUNTIME_SRC_ROOT}/Source/*.cpp)
# sources need to compile info, include dirs and source files

if(ANDROID)
    # the APP_NAME should match on AndroidManifest.xml
    list(APPEND GAME_SOURCE ${RUNTIME_SRC_ROOT}/proj.android/app/jni/main.cpp)
elseif(LINUX)
    list(APPEND GAME_SOURCE ${RUNTIME_SRC_ROOT}/proj.linux/main.cpp)
elseif(WINDOWS)
    list(APPEND GAME_HEADER
         ${RUNTIME_SRC_ROOT}/proj.win32/main.h
         )
    list(APPEND GAME_SOURCE
         ${RUNTIME_SRC_ROOT}/proj.win32/main.cpp
         ${common_content_files}
         )
elseif(APPLE)
    if(IOS)
        list(APPEND GAME_HEADER
             ${RUNTIME_SRC_ROOT}/proj.ios_mac/ios/AppController.h
             ${RUNTIME_SRC_ROOT}/proj.ios_mac/ios/RootViewController.h
             )

        if (TVOS)
            set(APP_UI_RES
                ${RUNTIME_SRC_ROOT}/proj.ios_mac/ios/LaunchScreenBackground.png
                ${RUNTIME_SRC_ROOT}/proj.ios_mac/ios/targets/tvos/LaunchScreen.storyboard
                ${RUNTIME_SRC_ROOT}/proj.ios_mac/ios/targets/tvos/Images.xcassets
            )
        else()
            set(APP_UI_RES
                ${RUNTIME_SRC_ROOT}/proj.ios_mac/ios/LaunchScreenBackground.png
                ${RUNTIME_SRC_ROOT}/proj.ios_mac/ios/targets/ios/LaunchScreen.storyboard
                ${RUNTIME_SRC_ROOT}/proj.ios_mac/ios/targets/ios/Images.xcassets
            )
        endif()

        list(APPEND GAME_SOURCE
             ${RUNTIME_SRC_ROOT}/proj.ios_mac/ios/main.m
             ${RUNTIME_SRC_ROOT}/proj.ios_mac/ios/AppController.mm
             ${RUNTIME_SRC_ROOT}/proj.ios_mac/ios/RootViewController.mm
             ${RUNTIME_SRC_ROOT}/proj.ios_mac/ios/Prefix.pch
             ${RUNTIME_SRC_ROOT}/proj.ios_mac/ios/build-cfg.json
             ${APP_UI_RES}
             )
    elseif(MACOSX)
        list(APPEND GAME_HEADER
             )
        set(APP_UI_RES
            ${RUNTIME_SRC_ROOT}/proj.ios_mac/mac/Icon.icns
            ${RUNTIME_SRC_ROOT}/proj.ios_mac/mac/Info.plist
            )
        list(APPEND GAME_SOURCE
             ${RUNTIME_SRC_ROOT}/proj.ios_mac/mac/main.cpp
             ${RUNTIME_SRC_ROOT}/proj.ios_mac/mac/Prefix.pch
             ${APP_UI_RES}
             )
    endif()
    list(APPEND GAME_SOURCE ${common_content_files})
endif()

set(APP_SRC ${GAME_HEADER} ${GAME_SOURCE})

# mark app complie info and libs info
if(NOT ANDROID)
    add_executable(${APP_NAME} ${APP_SRC})
else()
    add_library(${APP_NAME} SHARED ${APP_SRC})

    # whole archive for jni
    target_link_libraries(${APP_NAME} -Wl,--whole-archive cpp_android_spec -Wl,--no-whole-archive)
    add_subdirectory(${_AX_ROOT_PATH}/extensions/scripting/lua-bindings/proj.android ${ENGINE_BINARY_PATH}/extensions/lua-android)
    target_link_libraries(${APP_NAME} -Wl,--whole-archive lua_android_spec -Wl,--no-whole-archive)

    config_android_shared_libs("org.axmol.lib" "${CMAKE_CURRENT_SOURCE_DIR}/proj.android/app/src")
endif()

if (NOT _AX_USE_PREBUILT)
    target_link_libraries(${APP_NAME} ${_AX_LUA_LIB})
endif()

target_include_directories(${APP_NAME} PRIVATE ${RUNTIME_SRC_ROOT}/Source)

# mark app resources, resource will be copy auto after mark
setup_ax_app_config(${APP_NAME})
if(APPLE)
    set_target_properties(${APP_NAME} PROPERTIES RESOURCE "${APP_UI_RES}")
    set_xcode_property(${APP_NAME} INSTALL_PATH "\$(LOCAL_APPS_DIR)")
    set_xcode_property(${APP_NAME} PRODUCT_BUNDLE_IDENTIFIER "org.axmol.hellolua")
    if(MACOSX)
        set_target_properties(${APP_NAME} PROPERTIES
                              MACOSX_BUNDLE_INFO_PLIST "${RUNTIME_SRC_ROOT}/proj.ios_mac/mac/Info.plist"
                              )
        set_target_properties(${APP_NAME} PROPERTIES
            LINK_FLAGS "-pagezero_size 10000 -image_base 100000000"
        )
    elseif(TVOS)
        set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${RUNTIME_SRC_ROOT}/proj.ios_mac/ios/targets/tvos/Info.plist")
        set_xcode_property(${APP_NAME} ASSETCATALOG_COMPILER_APPICON_NAME "Brand Assets")
        set_xcode_property(${APP_NAME} DEVELOPMENT_TEAM "")
        set_xcode_property(${APP_NAME} CODE_SIGN_IDENTITY "iPhone Developer")
    elseif(IOS)
        set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${RUNTIME_SRC_ROOT}/proj.ios_mac/ios/targets/ios/Info.plist")
        set_xcode_property(${APP_NAME} ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon")
        set_xcode_property(${APP_NAME} DEVELOPMENT_TEAM "")
        set_xcode_property(${APP_NAME} CODE_SIGN_IDENTITY "iPhone Developer")
    endif()
elseif(WINDOWS)
    if(NOT _AX_USE_PREBUILT)
        ax_copy_target_dll(${APP_NAME})
        ax_copy_lua_dlls(${APP_NAME})
    endif()
endif()

if (NOT APPLE)
    ax_get_resource_path(APP_RES_DIR ${APP_NAME})
    if (NOT _AX_TOP_LEVEL_APP)
        ax_sync_lua_scripts(${APP_NAME} ${engine_script_folder} "${content_folder}/src/axmol")
    endif()
    ax_sync_target_res(${APP_NAME} LINK_TO ${APP_RES_DIR} FOLDERS ${content_folder} SYM_LINK 1)
    
    if((WINDOWS AND (NOT (CMAKE_GENERATOR STREQUAL "Ninja"))))
        set_property(TARGET ${APP_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${content_folder}")
        if(NOT DEFINED BUILD_ENGINE_DONE)
            set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${APP_NAME})
        endif()
    endif()
endif()

# The optional thirdparties(not dependent by engine)
if (AX_WITH_YAML_CPP)
    target_include_directories(${APP_NAME} PRIVATE ${_AX_ROOT_PATH}/thirdparty/yaml-cpp/include)
    target_link_libraries(${APP_NAME} yaml-cpp)
endif()

if (_AX_USE_PREBUILT) # support windows and linux
    use_ax_compile_define(${APP_NAME})

    include(${_AX_ROOT_PATH}/cmake/Modules/AXLinkHelpers.cmake)
    ax_link_lua_prebuilt(${APP_NAME} ${_AX_ROOT_PATH} ${AX_PREBUILT_DIR})
endif()
