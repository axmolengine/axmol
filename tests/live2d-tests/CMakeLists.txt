#/****************************************************************************
# Copyright (c) 2013-2014 cocos2d-x.org
# Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).
#
# https://axmol.dev/
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

cmake_minimum_required(VERSION 3.20)

set(APP_NAME live2d-tests)

project(${APP_NAME})

if(NOT DEFINED BUILD_ENGINE_DONE)
    set(AX_ENABLE_EXT_LIVE2D ON CACHE BOOL "Build extension Live2D" FORCE)

    set(AX_ENABLE_EXT_FAIRYGUI OFF CACHE BOOL "Build extension FairyGUI" FORCE)
    set(AX_ENABLE_EXT_SPINE OFF CACHE BOOL "Build extension spine" FORCE)
    set(AX_ENABLE_EXT_DRAGONBONES OFF CACHE BOOL "Build extension DragonBones" FORCE)
    set(AX_ENABLE_EXT_COCOSTUDIO OFF CACHE BOOL "Build extension cocostudio" FORCE)
    set(AX_ENABLE_EXT_ASSETMANAGER OFF CACHE BOOL "Build extension asset-manager" FORCE)
    set(AX_ENABLE_EXT_PARTICLE3D OFF CACHE BOOL "Build extension Particle3D" FORCE)
    set(AX_ENABLE_EXT_LUA OFF CACHE BOOL "Build lua libraries" FORCE)
    set(AX_ENABLE_EXT_GUI OFF CACHE BOOL "Build extension GUI" FORCE)
    set(AX_ENABLE_EXT_JSONDEFAULT OFF CACHE BOOL "Build extension JSONDefault" FORCE)
    set(AX_ENABLE_EXT_PHYSICS_NODE OFF CACHE BOOL "Build extension physics-nodes" FORCE)

    if(XCODE)
        set(CMAKE_XCODE_GENERATE_TOP_LEVEL_PROJECT_ONLY TRUE)
    endif()

    set(_AX_ROOT "$ENV{AX_ROOT}")
    if(NOT (_AX_ROOT STREQUAL ""))
        file(TO_CMAKE_PATH ${_AX_ROOT} _AX_ROOT) # string(REPLACE "\\" "/" _AX_ROOT ${_AX_ROOT})
        message(STATUS "Using system env var _AX_ROOT=${_AX_ROOT}")
    else()
        set(_AX_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)
    endif()

    set(CMAKE_MODULE_PATH ${_AX_ROOT}/cmake/Modules/)

    include(AXBuildSet)
    add_subdirectory(${_AX_ROOT}/core ${ENGINE_BINARY_PATH}/axmol/core)
endif()

_1kfetch(sample-assets)
_1klink("${sample-assets_SOURCE_DIR}/live2d-tests/Content" "${CMAKE_CURRENT_LIST_DIR}/Content")

# record sources, headers, resources...
file(GLOB GAME_SOURCE Source/*.cpp)
file(GLOB GAME_HEADER Source/*.h)

set(GAME_INC_DIRS
  "${CMAKE_CURRENT_SOURCE_DIR}/Source"
)

set(content_folder
    "${CMAKE_CURRENT_SOURCE_DIR}/Content"
    )
if(APPLE)
    ax_mark_multi_resources(common_content_files RES_TO "Resources" FOLDERS ${content_folder})
elseif(WINDOWS)
    ax_mark_multi_resources(common_content_files RES_TO "Content" FOLDERS ${content_folder})
endif()

if(ANDROID)
    list(APPEND GAME_SOURCE
         proj.android/app/jni/main.cpp
         )
elseif(LINUX)
    list(APPEND GAME_SOURCE
         proj.linux/main.cpp
         )
    list(APPEND GAME_SOURCE ${common_content_files})
elseif(EMSCRIPTEN)
    list(APPEND GAME_SOURCE
         proj.wasm/main.cpp
         )
    list(APPEND GAME_SOURCE ${common_content_files})
elseif(WINDOWS)
    if(NOT WINRT)
        list(APPEND GAME_HEADER
             proj.win32/main.h
             )
        list(APPEND GAME_SOURCE
             proj.win32/main.cpp
             ${common_content_files}
             )
    else()
        ax_setup_winrt_sources()
    endif()
    list(APPEND GAME_SOURCE ${common_content_files})
elseif(APPLE)
    if(IOS)
        list(APPEND GAME_HEADER
             proj.ios/AppController.h
             proj.ios/RootViewController.h
             )

        if (TVOS)
            set(APP_UI_RES
                proj.ios/LaunchScreenBackground.png
                proj.ios/targets/tvos/LaunchScreen.storyboard
                proj.ios/targets/tvos/Images.xcassets
                )
        else()
            set(APP_UI_RES
                proj.ios/LaunchScreenBackground.png
                proj.ios/targets/ios/LaunchScreen.storyboard
                proj.ios/targets/ios/Images.xcassets
                )
        endif()

        list(APPEND GAME_SOURCE
             proj.ios/main.m
             proj.ios/AppController.mm
             proj.ios/RootViewController.mm
             ${APP_UI_RES}
             )
    elseif(MACOSX)
        set(APP_UI_RES
            proj.mac/Icon.icns
            proj.mac/Info.plist
            proj.mac/en.lproj/InfoPlist.strings
            )
        list(APPEND GAME_SOURCE
             proj.mac/main.cpp
             ${APP_UI_RES}
             )
    endif()
    list(APPEND GAME_SOURCE ${common_content_files})
endif()

# mark app complie info and libs info
set(all_code_files
    ${GAME_HEADER}
    ${GAME_SOURCE}
    )

if(NOT ANDROID)
    add_executable(${APP_NAME} ${all_code_files})
else()
    add_library(${APP_NAME} SHARED ${all_code_files})

    # whole archive for jni
    target_link_libraries(${APP_NAME} -Wl,--whole-archive cpp_android_spec -Wl,--no-whole-archive)

    config_android_shared_libs("org.axmol.lib" "${CMAKE_CURRENT_SOURCE_DIR}/proj.android/app/src")
endif()

target_link_libraries(${APP_NAME} ${_AX_CORE_LIB})

target_include_directories(${APP_NAME} PRIVATE ${GAME_INC_DIRS})

# mark app resources
ax_setup_app_config(${APP_NAME})

if(APPLE)
    set_target_properties(${APP_NAME} PROPERTIES RESOURCE "${APP_UI_RES}")
    set_xcode_property(${APP_NAME} INSTALL_PATH "\$(LOCAL_APPS_DIR)")
    set_xcode_property(${APP_NAME} PRODUCT_BUNDLE_IDENTIFIER "org.axmol.${APP_NAME}")

    if(MACOSX)
        set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/proj.mac/Info.plist")
    elseif(TVOS)
        set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios/targets/tvos/Info.plist")
        set_xcode_property(${APP_NAME} ASSETCATALOG_COMPILER_APPICON_NAME "Brand Assets")
    elseif(IOS)
        set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios/targets/ios/Info.plist")
        set_xcode_property(${APP_NAME} ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon")
    endif()

    # For code-signing, set the DEVELOPMENT_TEAM:
    #set_xcode_property(${APP_NAME} DEVELOPMENT_TEAM "GRLXXXX2K9")
elseif(WINDOWS)
    if(NOT _AX_USE_PREBUILT)
        ax_sync_target_dlls(${APP_NAME})
    endif()
endif()

if ((NOT APPLE) AND (NOT WINRT))
    ax_get_resource_path(APP_RES_DIR ${APP_NAME})
    ax_sync_target_res(${APP_NAME} LINK_TO ${APP_RES_DIR} FOLDERS ${content_folder} SYM_LINK 1)
    if(WINDOWS)
        set_property(TARGET ${APP_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${content_folder}")
    endif()
endif()

if((NOT IOS) AND (NOT WINRT))
    message("CMake ${APP_NAME} target_precompile_headers")
    target_precompile_headers(${APP_NAME} PRIVATE
      "$<$<COMPILE_LANGUAGE:CXX>:axmol.h>"
    )
endif()

ax_setup_app_props(${APP_NAME})
