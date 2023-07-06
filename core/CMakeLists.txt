#/****************************************************************************
# Copyright (c) 2013 cocos2d-x.org
# Copyright (c) 2014 martell malone
# Copyright (c) 2015-2017 Chukong Technologies Inc.
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

# this CMakeLists is to generate Axmol Engine Library
# build ax
# build axlua if AX_ENABLE_EXT_LUA=ON

# The version number
set(_AX_VERSION 1.0)

if(NOT DEFINED _AX_CORE_LIB)
    set(_AX_CORE_LIB axmol CACHE INTERNAL "The axmol core lib name" )
endif()

project(ax_libs)

if (NOT WINRT)
    option (AX_USE_ANGLE "Use google angle renderer" OFF)
else()
    set (AX_USE_ANGLE TRUE CACHE BOOL "Use google angle renderer" FORCE)
endif()

if (NOT AX_USE_ANGLE)  # check deprecated option: AX_USE_COMPAT_GL
    if (NOT WINRT)
        option (AX_USE_COMPAT_GL "Use google angle renderer" OFF)
    else()
        set (AX_USE_COMPAT_GL TRUE CACHE BOOL "Use google angle renderer" ON)
    endif()
    set (AX_USE_ANGLE ${AX_USE_COMPAT_GL} CACHE BOOL "Use google angle renderer" FORCE)
endif()

message(STATUS "WINRT=${WINRT}")

option (AX_ENABLE_MFMEDIA "Enabling microsoft media foundation" ON)

if (NOT WINRT)
    option (AX_ENABLE_MSEDGE_WEBVIEW2 "Use Microsoft Edge webview2" ON)
else() # we not support webview2 and vidio player for WINRT target
    set(AX_ENABLE_MSEDGE_WEBVIEW2 OFF CACHE BOOL "Use Microsoft Edge webview2" FORCE)
    # set(AX_ENABLE_MFMEDIA OFF CACHE BOOL "Enabling microsoft media foundation for windows video player" FORCE)
endif()

if (LINUX)
    include(CheckIncludeFile)
    check_include_file(vlc/vlc.h _AX_HAVE_VLC)
    option (AX_ENABLE_VLC_MEDIA "Enabling vlc media on linux" ON)
elseif(WINDOWS AND (NOT WINRT))
    option (AX_ENABLE_VLC_MEDIA "Enabling vlc media on windows" OFF)
endif()

message(STATUS "AX_ENABLE_VLC_MEDIA=${AX_ENABLE_VLC_MEDIA}")

if(NOT APPLE)
    set(AX_USE_ALSOFT ON CACHE BOOL "" FORCE)
else()
    option(AX_USE_ALSOFT "Use ALSOFT on apple" OFF)
endif()

option(AX_UPDATE_BUILD_VERSION "Update build version" ON)


# default value for axmol extensions modules to Build
# total supported extensions count: 12
# extensions dependicies: COCOSTUDIO may depend on spine & dragonBones if they are present in buildset

option(AX_ENABLE_EXT_GUI "Build extension GUI" ON)

option(AX_ENABLE_EXT_ASSETMANAGER "Build extension asset-manager" ON)
option(AX_ENABLE_EXT_PARTICLE3D "Build extension Particle3D" ON)
option(AX_ENABLE_EXT_PHYSICS_NODE "Build extension physics-nodes" ON)

option(AX_ENABLE_EXT_SPINE "Build extension spine" ON)
option(AX_ENABLE_EXT_DRAGONBONES "Build extension DragonBones" ON)

option(AX_ENABLE_EXT_COCOSTUDIO "Build extension cocostudio" ON)

option(AX_ENABLE_EXT_FAIRYGUI "Build extension FairyGUI" ON)

option(AX_ENABLE_EXT_LIVE2D "Build extension Live2D" OFF)
option(AX_ENABLE_EXT_EFFEKSEER "Build extension Effekseer" OFF)

if((WINDOWS AND NOT WINRT) OR MACOSX OR LINUX OR ANDROID)
  option(AX_ENABLE_EXT_IMGUI "Build extension ImGui" ON)
else()
  set(AX_ENABLE_EXT_IMGUI OFF)
endif()

if (ANDROID)
    set(AX_USE_GLAD TRUE CACHE BOOL "Use glad for android GLESv3 support" FORCE)
    add_definitions(-DAX_USE_GLAD=1)
endif()

if(XCODE)
    # set(CMAKE_BUILD_WITH_INSTALL_RPATH YES)
    # set(CMAKE_INSTALL_RPATH "@executable_path/Frameworks")
    set(CMAKE_XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "@executable_path/Frameworks" CACHE INTERNAL "")
endif()

set(_AX_THIRDPARTY_NAME "thirdparty" CACHE INTERNAL "" )

if (WIN32)
    option(WIN32_PATH_USE_ACP "" OFF)
    if(NOT WIN32_PATH_USE_ACP)
        add_definitions(-DNTCVT_CP_DEFAULT=CP_UTF8)
    else()
        add_definitions(-DNTCVT_CP_DEFAULT=CP_ACP)
    endif()
endif()

include(2d/CMakeLists.txt)
include(3d/CMakeLists.txt)
include(platform/CMakeLists.txt)
include(physics/CMakeLists.txt)
include(physics3d/CMakeLists.txt)
include(math/CMakeLists.txt)
include(navmesh/CMakeLists.txt)
include(renderer/CMakeLists.txt)
include(base/CMakeLists.txt)
include(ui/CMakeLists.txt)
include(network/CMakeLists.txt)
include(audio/CMakeLists.txt)

file(GLOB_RECURSE _AX_MEDIA_HEADER media/*.h)
file(GLOB_RECURSE _AX_MEDIA_SRC media/*.cpp)

if (APPLE)
    list(APPEND _AX_MEDIA_SRC media/AvfMediaEngine.mm)
    set_source_files_properties(media/MediaEngine.cpp PROPERTIES LANGUAGE OBJCXX)
endif()

set(_AX_HEADER axmol.h
    ${_AX_2D_HEADER}
    ${_AX_3D_HEADER}
    ${_AX_PLATFORM_HEADER}
    ${_AX_PHYSICS_HEADER}
    ${_AX_PHYSICS3D_HEADER}
    ${_AX_MATH_HEADER}
    ${_AX_NAVMESH_HEADER}
    ${_AX_RENDERER_HEADER}
    ${_AX_BASE_HEADER}
    ${_AX_AUDIO_HEADER}
    ${_AX_UI_HEADER}
    ${_AX_NETWORK_HEADER}
    ${_AX_MEDIA_HEADER}
    )

set(_AX_SRC axmol.cpp
    ${_AX_2D_SRC}
    ${_AX_3D_SRC}
    ${_AX_PLATFORM_SRC}
    ${_AX_PHYSICS_SRC}
    ${_AX_PHYSICS3D_SRC}
    ${_AX_MATH_SRC}
    ${_AX_NAVMESH_SRC}
    ${_AX_RENDERER_SRC}
    ${_AX_BASE_SRC}
    ${_AX_AUDIO_SRC}
    ${_AX_UI_SRC}
    ${_AX_NETWORK_SRC}
    ${_AX_MEDIA_SRC}
    )

list(APPEND _AX_SRC ${_AX_HEADER})

add_library(${_AX_CORE_LIB} ${_AX_SRC})

if (WINRT)
    target_compile_options(${_AX_CORE_LIB} PUBLIC /ZW)
endif()

file(GLOB SHADER_SOURCES "renderer/shaders/*.vert" "renderer/shaders/*.frag")
ax_target_compile_shaders(${_AX_CORE_LIB} FILES ${SHADER_SOURCES} RUNTIME)

ax_config_pred(${_AX_CORE_LIB} AX_ENABLE_MFMEDIA)
ax_config_pred(${_AX_CORE_LIB} AX_ENABLE_MSEDGE_WEBVIEW2)

# use thirdparty libs
add_subdirectory(${_AX_ROOT}/thirdparty ${ENGINE_BINARY_PATH}/thirdparty)
target_link_libraries(${_AX_CORE_LIB} thirdparty)

# add base macro define and compile options
use_ax_compile_define(${_AX_CORE_LIB})
use_ax_compile_options(${_AX_CORE_LIB})

# use all platform related system libs
use_ax_depend(${_AX_CORE_LIB})

target_include_directories(${_AX_CORE_LIB}
    PUBLIC ${_AX_ROOT}
    PUBLIC ${_AX_ROOT}/thirdparty
    PUBLIC ${_AX_ROOT}/extensions
    PUBLIC ${_AX_ROOT}/core
    PUBLIC ${_AX_ROOT}/core/platform

    INTERFACE ${_AX_ROOT}/thirdparty
    INTERFACE ${_AX_ROOT}/extensions
    INTERFACE ${_AX_ROOT}/core/base
    INTERFACE ${_AX_ROOT}/core/audio
    INTERFACE ${_AX_ROOT}/core/platform/${PLATFORM_FOLDER}
)

set_target_properties(${_AX_CORE_LIB}
                      PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
                      VERSION "${_AX_VERSION}"
                      FOLDER "Internal"
                      )

if(XCODE OR VS)
    ax_mark_code_files("${_AX_CORE_LIB}")
endif()

message("CMake ${_AX_CORE_LIB} target_precompile_headers")
target_precompile_headers(${_AX_CORE_LIB} PRIVATE 
  "$<$<COMPILE_LANGUAGE:CXX>:axmol.h>"
)

if(WINDOWS)
    if(AX_ENABLE_MSEDGE_WEBVIEW2)
        find_program(NUGET_EXE NAMES nuget 
            PATHS ${_AX_ROOT}/tools/external/nuget)
            
        if(NOT NUGET_EXE)
            message("NUGET.EXE not found.")
            message(FATAL_ERROR "Please run setup.ps1 again to download NUGET.EXE, and run CMake again.")
        endif()
     
        exec_program(${NUGET_EXE}
            ARGS install "Microsoft.Web.WebView2" -Version 1.0.992.28 -ExcludeVersion -OutputDirectory ${CMAKE_BINARY_DIR}/packages)

        if(CMAKE_GENERATOR MATCHES "Ninja")
            target_link_libraries(${_AX_CORE_LIB} ${CMAKE_BINARY_DIR}/packages/Microsoft.Web.WebView2/build/native/${ARCH_ALIAS}/WebView2Loader.dll.lib)
            target_include_directories(${_AX_CORE_LIB} PUBLIC ${CMAKE_BINARY_DIR}/packages/Microsoft.Web.WebView2/build/native/include)
        else()
            target_link_libraries(${_AX_CORE_LIB} ${CMAKE_BINARY_DIR}/packages/Microsoft.Web.WebView2/build/native/Microsoft.Web.WebView2.targets)
        endif()
        target_compile_definitions(${_AX_CORE_LIB} PUBLIC _AX_HAVE_WEBVIEW2)
    endif()
endif()

# engine extensions
add_subdirectory(${_AX_ROOT}/extensions ${ENGINE_BINARY_PATH}/extensions)

if(MSVC)
    target_sources(${_AX_CORE_LIB} PRIVATE ./axmol.natvis)
    target_sources(${_AX_CORE_LIB} PRIVATE ../thirdparty/yasio/yasio.natvis)
    target_sources(${_AX_CORE_LIB} PRIVATE ../thirdparty/robin-map/tsl-robin-map.natvis)
    target_compile_options(${_AX_CORE_LIB} PUBLIC "/Zm2000")
endif()

if(ANDROID)
    add_subdirectory(${_AX_ROOT}/core/platform/android ${ENGINE_BINARY_PATH}/core/cpp-android)
endif()

find_package(Git)
if(AX_UPDATE_BUILD_VERSION AND GIT_FOUND AND EXISTS "${_AX_ROOT}/.git")
    # Get the current working branch and its latest abbreviated commit hash
    EXECUTE_PROCESS(COMMAND ${GIT_EXECUTABLE} -C "${_AX_ROOT}" rev-list --count HEAD
        TIMEOUT 5
        OUTPUT_VARIABLE AX_BUILD_NUM
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    EXECUTE_PROCESS(COMMAND ${GIT_EXECUTABLE} -C "${_AX_ROOT}" branch --show-current
        TIMEOUT 5
        OUTPUT_VARIABLE AX_GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    EXECUTE_PROCESS(COMMAND ${GIT_EXECUTABLE} -C "${_AX_ROOT}" rev-parse --short=7 HEAD
        TIMEOUT 5
        OUTPUT_VARIABLE AX_GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE)
else()
    set(AX_GIT_BRANCH "UNKNOWN")
    set(AX_GIT_COMMIT_HASH "unknown")
endif()
configure_file(
        "${CMAKE_CURRENT_LIST_DIR}/axmolver.h.in"
        "${CMAKE_CURRENT_LIST_DIR}/axmolver.h")

#if(XCODE)
#    # Later versions of Xcode clang want to compile C++17 with aligned allocation turned on and this is only supported on iOS 11.0+
#    # TODO: Only turn this off if ${CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET} < 11.0
#    target_compile_options(${_AX_CORE_LIB} PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-fno-aligned-allocation>)
#endif()
