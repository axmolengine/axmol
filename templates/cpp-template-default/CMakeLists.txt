#/****************************************************************************
# Copyright (c) 2013-2014 cocos2d-x.org
# Copyright (c) 2021-2022 Bytedance Inc.
#
# https://adxeproject.github.io
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

set(APP_NAME HelloCpp)

project(${APP_NAME})

if(NOT DEFINED BUILD_ENGINE_DONE) # to test HelloCpp into root project
    if(XCODE)
        set(CMAKE_XCODE_GENERATE_TOP_LEVEL_PROJECT_ONLY TRUE)
    endif()
    
    # config quick starter batch script run.bat for windows
    if(WIN32)
        file(RELATIVE_PATH CMAKE_BUILD_RELATIVE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}/run.bat.in" "${CMAKE_CURRENT_SOURCE_DIR}/run.bat" @ONLY)
    endif()

    set(ADXE_ROOT "$ENV{ADXE_ROOT}")

    if(NOT (ADXE_ROOT STREQUAL ""))
        set(ADXE_ROOT_PATH "${ADXE_ROOT}")
        file(TO_CMAKE_PATH ${ADXE_ROOT_PATH} ADXE_ROOT_PATH) # string(REPLACE "\\" "/" ADXE_ROOT_PATH ${ADXE_ROOT_PATH})
        message(STATUS "Using system env var ADXE_ROOT=${ADXE_ROOT}")
    else()
        message(FATAL_ERROR "Please run setup.py add system env var 'ADXE_ROOT' to specific the engine root")
    endif()

    set(CMAKE_MODULE_PATH ${ADXE_ROOT_PATH}/cmake/Modules/)

    include(CocosBuildSet)
    set(_AX_USE_PREBUILT FALSE)
    if (WIN32 AND DEFINED AX_PREBUILT_DIR AND IS_DIRECTORY ${ADXE_ROOT_PATH}/${AX_PREBUILT_DIR})
        set(_AX_USE_PREBUILT TRUE)
    endif()

    if (NOT _AX_USE_PREBUILT)
        add_subdirectory(${ADXE_ROOT_PATH}/core ${ENGINE_BINARY_PATH}/adxe/core)
    endif()
endif()

# record sources, headers, resources...
set(GAME_SOURCE)
set(GAME_HEADER)

set(GAME_RES_FOLDER
    "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
    )
if(APPLE OR WINDOWS)
    cocos_mark_multi_resources(common_res_files RES_TO "Resources" FOLDERS ${GAME_RES_FOLDER})
endif()

# add cross-platforms source files and header files 
list(APPEND GAME_SOURCE
     Classes/AppDelegate.cpp
     Classes/HelloWorldScene.cpp
     )
list(APPEND GAME_HEADER
     Classes/AppDelegate.h
     Classes/HelloWorldScene.h
     )

if(ANDROID)
    # the APP_NAME should match on AndroidManifest.xml
    list(APPEND GAME_SOURCE
         proj.android/app/jni/hellocpp/main.cpp
         )
elseif(LINUX)
    list(APPEND GAME_SOURCE
         proj.linux/main.cpp
         )
elseif(WINDOWS)
    list(APPEND GAME_HEADER
         proj.win32/main.h
         proj.win32/resource.h
         )
    list(APPEND GAME_SOURCE
         proj.win32/main.cpp
         proj.win32/game.rc
         ${common_res_files}
         )
elseif(APPLE)
    if(IOS)
        list(APPEND GAME_HEADER
             proj.ios_mac/ios/AppController.h
             proj.ios_mac/ios/RootViewController.h
             )
        set(APP_UI_RES
            proj.ios_mac/ios/LaunchScreen.storyboard
            proj.ios_mac/ios/LaunchScreenBackground.png
            proj.ios_mac/ios/Images.xcassets
            )
        list(APPEND GAME_SOURCE
             proj.ios_mac/ios/main.m
             proj.ios_mac/ios/AppController.mm
             proj.ios_mac/ios/RootViewController.mm
             proj.ios_mac/ios/Prefix.pch
             ${APP_UI_RES}
             )
    elseif(MACOSX)
        set(APP_UI_RES
            proj.ios_mac/mac/Icon.icns
            proj.ios_mac/mac/Info.plist
            )
        list(APPEND GAME_SOURCE
             proj.ios_mac/mac/main.cpp
             proj.ios_mac/mac/Prefix.pch
             ${APP_UI_RES}
             )
    endif()
    list(APPEND GAME_SOURCE ${common_res_files})
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
    add_subdirectory(${ADXE_ROOT_PATH}/core/platform/android ${ENGINE_BINARY_PATH}/core/platform)
    target_link_libraries(${APP_NAME} -Wl,--whole-archive cpp_android_spec -Wl,--no-whole-archive)
    config_android_shared_libs("org.cocos2dx.lib" "${CMAKE_CURRENT_SOURCE_DIR}/proj.android/app/src")
endif()

target_link_libraries(${APP_NAME} ${ADXE_CORE_LIB})

target_include_directories(${APP_NAME}
        PRIVATE Classes
        PRIVATE ${ADXE_ROOT_PATH}/core/audio
)

# mark app resources
setup_cocos_app_config(${APP_NAME})
if(APPLE)
    set_target_properties(${APP_NAME} PROPERTIES RESOURCE "${APP_UI_RES}")
    set_xcode_property(${APP_NAME} INSTALL_PATH "\$(LOCAL_APPS_DIR)")
    set_xcode_property(${APP_NAME} PRODUCT_BUNDLE_IDENTIFIER "org.adxe.hellocpp")

    if(MACOSX)
        set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios_mac/mac/Info.plist")
    elseif(IOS)
        set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios_mac/ios/Info.plist")
        set_xcode_property(${APP_NAME} ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon")
    endif()

    # For code-signing, set the DEVELOPMENT_TEAM:
    #set_xcode_property(${APP_NAME} DEVELOPMENT_TEAM "GRLXXXX2K9")
elseif(WINDOWS)
    if(NOT _AX_USE_PREBUILT)
        cocos_copy_target_dll(${APP_NAME})
    endif()
endif()

if((WINDOWS AND (CMAKE_GENERATOR STREQUAL "Ninja")) OR LINUX)
    cocos_get_resource_path(APP_RES_DIR ${APP_NAME})
    cocos_copy_target_res(${APP_NAME} LINK_TO ${APP_RES_DIR} FOLDERS ${GAME_RES_FOLDER})
elseif(WINDOWS)
    set_property(TARGET ${APP_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${GAME_RES_FOLDER}")
    if(NOT DEFINED BUILD_ENGINE_DONE)
        set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${APP_NAME})
    endif()
endif()

# The optional thirdparties(not dependent by engine)
if (AX_WITH_YAML_CPP)
    target_include_directories(${APP_NAME} PRIVATE ${ADXE_ROOT_PATH}/thirdparty/yaml-cpp/include)
    target_link_libraries(${APP_NAME} yaml-cpp)
endif()

if (_AX_USE_PREBUILT) # support windows only

    option(AX_ENABLE_MSEDGE_WEBVIEW2 "Enable msedge webview2" TRUE)

    if(NOT ("${CMAKE_GENERATOR_PLATFORM}" STREQUAL "Win32"))
        set(WIN64 TRUE)
        set(ARCH_ALIAS "x64")
    else()
       set(WIN32 TRUE)
       set(ARCH_ALIAS "x86")
    endif()

    if (NOT AX_USE_SHARED_PREBUILT)
        target_compile_definitions(${APP_NAME}
            PRIVATE CC_STATIC=1
        )
    endif()

    target_include_directories(${APP_NAME}
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/lua
        PRIVATE ${ADXE_ROOT_PATH}/extensions/scripting/lua-bindings/manual
        PRIVATE ${ADXE_ROOT_PATH}
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty
        PRIVATE ${ADXE_ROOT_PATH}/extensions
        PRIVATE ${ADXE_ROOT_PATH}/core
        PRIVATE ${ADXE_ROOT_PATH}/core/platform
        PRIVATE ${ADXE_ROOT_PATH}/core/base
        PRIVATE ${ADXE_ROOT_PATH}/core/audio
        PRIVATE ${ADXE_ROOT_PATH}/core/platform/win32
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/fmt/include
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/robin-map/include
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/freetype/include
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/glfw/include/GLFW
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/box2d/include
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/chipmunk/include
        PRIVATE ${ADXE_ROOT_PATH}/${AX_PREBUILT_DIR}/engine/thirdparty/freetype/include
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/recast/..
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/bullet/.
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/webp/src/webp
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/pugixml
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/xxhash/.
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/lz4/.
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/clipper/.
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/ConvertUTF/.
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/poly2tri/..
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/astc/..
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/openal/include
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/ogg/include
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/glad/include
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/glfw/include
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/png
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/unzip/.
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/llhttp/include
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/lua/plainlua
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/lua/tolua/.
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/lua/lua-cjson/.
        PRIVATE ${ADXE_ROOT_PATH}/extensions/cocostudio
        PRIVATE ${ADXE_ROOT_PATH}/extensions/spine/runtime/include
        PRIVATE ${ADXE_ROOT_PATH}/extensions/fairygui
        PRIVATE ${ADXE_ROOT_PATH}/extensions/GUI
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/zlib/include
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/jpeg/include
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/openssl/include
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/curl/include
    )
    
    target_link_directories(${APP_NAME}
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/openssl/prebuilt/windows/${ARCH_ALIAS}
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/zlib/prebuilt/windows/${ARCH_ALIAS}
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/jpeg/prebuilt/windows/${ARCH_ALIAS}
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/curl/prebuilt/windows/${ARCH_ALIAS}
        PRIVATE ${ADXE_ROOT_PATH}/${AX_PREBUILT_DIR}/lib/Debug
    )
    
    # Linking OS libs
    target_link_libraries(${APP_NAME} winmm Version)
    
    # Linking engine and thirdparty libs
    target_link_libraries(${APP_NAME}
        spine.lib
        particle3d.lib
        assets-manager.lib
        cocostudio.lib
        DragonBones.lib
        adxe.lib
        box2d.lib
        chipmunk.lib
        freetype.lib
        zlib.lib
        recast.lib
        bullet.lib
        jpeg-static.lib
        libcrypto.lib
        libssl.lib
        webp.lib
        pugixml.lib
        xxhash.lib
        lz4.lib
        clipper.lib
        ConvertUTF.lib
        poly2tri.lib
        astc.lib
        libcurl_imp.lib
        OpenAL32.lib
        ogg.lib
        glad.lib
        glfw.lib
        png.lib
        unzip.lib
        llhttp.lib
        physics-nodes.lib
    )
    
    # Copy dlls to app bin dir
        # copy thirdparty dlls to target bin dir
    # copy_thirdparty_dlls(${APP_NAME} $<TARGET_FILE_DIR:${APP_NAME}>)
    set(ssl_dll_suffix "")
    if(WIN64)
        set(ssl_dll_suffix "-${ARCH_ALIAS}")
    endif()
    if(NOT CMAKE_GENERATOR STREQUAL "Ninja")
        set(THIRD_PARTY_ARCH "\$\(Configuration\)/")
    endif()
    add_custom_command(TARGET ${APP_NAME} POST_BUILD
       COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${ADXE_ROOT_PATH}/thirdparty/openssl/prebuilt/windows/${ARCH_ALIAS}/libcrypto-3${ssl_dll_suffix}.dll"
        "${ADXE_ROOT_PATH}/thirdparty/openssl/prebuilt/windows/${ARCH_ALIAS}/libssl-3${ssl_dll_suffix}.dll"
        "${ADXE_ROOT_PATH}/thirdparty/curl/prebuilt/windows/${ARCH_ALIAS}/libcurl.dll"
        "${ADXE_ROOT_PATH}/thirdparty/zlib/prebuilt/windows/${ARCH_ALIAS}/zlib1.dll"
        "${ADXE_ROOT_PATH}/${AX_PREBUILT_DIR}/bin/${THIRD_PARTY_ARCH}OpenAL32.dll"
         $<TARGET_FILE_DIR:${APP_NAME}>)

    # Copy windows angle binaries
    if (AX_USE_COMPAT_GL)
        add_custom_command(TARGET ${APP_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${ADXE_ROOT_PATH}/thirdparty/angle/prebuilt/${ARCH_ALIAS}/libGLESv2.dll
            ${ADXE_ROOT_PATH}/thirdparty/angle/prebuilt/${ARCH_ALIAS}/libEGL.dll
            ${ADXE_ROOT_PATH}/thirdparty/angle/prebuilt/${ARCH_ALIAS}/d3dcompiler_47.dll
            $<TARGET_FILE_DIR:${APP_NAME}>
        )
    endif()
    
    if (AX_ENABLE_MSEDGE_WEBVIEW2)
        if(CMAKE_GENERATOR STREQUAL "Ninja")
            target_link_libraries(${ADXE_CORE_LIB} ${CMAKE_BINARY_DIR}/packages/Microsoft.Web.WebView2/build/native/${ARCH_ALIAS}/WebView2Loader.dll.lib)
            target_include_directories(${ADXE_CORE_LIB} PUBLIC ${CMAKE_BINARY_DIR}/packages/Microsoft.Web.WebView2/build/native/include)
            add_custom_command(TARGET ${cocos_target} POST_BUILD
               COMMAND ${CMAKE_COMMAND} -E copy_if_different
               "${CMAKE_BINARY_DIR}/packages/Microsoft.Web.WebView2/build/native/${ARCH_ALIAS}/WebView2Loader.dll"
                $<TARGET_FILE_DIR:${APP_NAME}>
            )
        else()
            target_link_libraries(${ADXE_CORE_LIB} ${CMAKE_BINARY_DIR}/packages/Microsoft.Web.WebView2/build/native/Microsoft.Web.WebView2.targets)
        endif()
    endif()
endif()
