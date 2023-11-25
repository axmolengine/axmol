#/****************************************************************************
# Copyright (c) 2013 cocos2d-x.org
# Copyright (c) 2014 martell malone
# Copyright (c) 2015-2017 Chukong Technologies Inc.
# Copyright (c) 2021-2023 Bytedance Inc.
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

# include_guard (GLOBAL)

# The version number
set(_AX_VERSION 2.0)

if(NOT DEFINED _AX_CORE_LIB)
    set(_AX_CORE_LIB axmol CACHE INTERNAL "The axmol core lib name" )
endif()

cmake_policy(SET CMP0127 NEW)

project(ax_libs)

if(WIN32)
    # explicit set source charset to utf-8 for windows targets
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()

include(CMakeDependentOption)

# by default: use angle on win32
cmake_dependent_option(AX_USE_COMPAT_GL "Whether use compatibility GL as render backend" ON "WIN32" OFF)

# choosing render backend for all target platforms: win32, winuwp, ios, tvos, android, linux, osx
if(APPLE AND (NOT AX_USE_COMPAT_GL))
    set(AX_USE_METAL ON CACHE BOOL "" FORCE)
    set(AX_USE_GL OFF CACHE BOOL "" FORCE)
else() # win32, winuwp, android, linux: OpenGL
    set(AX_USE_GL ON CACHE BOOL "" FORCE)
    set(AX_USE_METAL OFF CACHE BOOL "" FORCE)
endif()

# macos, linux or (win32, ios, tvos and NOT AX_USE_COMPAT_GL) not use GLES profile
set(_GLES_PROFILE 0) # _GLES_PROFILE is the default GLES profile
if(ANDROID) # android force use GLES profile
    set(_GLES_PROFILE 200)
elseif(WINRT OR WASM) # winuwp/wasm fore use GLES profile
    set(_GLES_PROFILE 300)
elseif((NOT MACOSX) AND (NOT LINUX)) # win32, ios, tvos
    if(AX_USE_COMPAT_GL)
        set(_GLES_PROFILE 300)
    endif()
endif()

if(_GLES_PROFILE)
    set(AX_GLES_PROFILE ${_GLES_PROFILE} CACHE STRING "")
    if (NOT (AX_GLES_PROFILE EQUAL 200 OR AX_GLES_PROFILE EQUAL 300))
       message(FATAL_ERROR "${MACOSX} Invalid GLES profile ${AX_GLES_PROFILE}, must be 200 or 300, default ${_GLES_PROFILE}")
    endif()
else() # force disable GLES profile
    set(AX_GLES_PROFILE 0 CACHE STRING "" FORCE)
endif()

# print renderer backend profile
message(AUTHOR_WARNING "AX_USE_GL=${AX_USE_GL}, AX_USE_METAL=${AX_USE_METAL}, AX_GLES_PROFILE=${AX_GLES_PROFILE}")

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
endif()

cmake_dependent_option(AX_ENABLE_VLC_MEDIA "Enabling vlc media" ON "_AX_HAVE_VLC" OFF)

message(STATUS "AX_ENABLE_VLC_MEDIA=${AX_ENABLE_VLC_MEDIA}")

if(NOT APPLE AND (NOT EMSCRIPTEN))
    set(AX_USE_ALSOFT ON CACHE BOOL "" FORCE)
else()
    option(AX_USE_ALSOFT "Use ALSOFT on apple" OFF)
endif()

option(AX_UPDATE_BUILD_VERSION "Update build version" ON)

# default value for axmol extensions modules to Build
# total supported extensions count: 13
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

cmake_dependent_option(AX_ENABLE_EXT_IMGUI "Build extension ImGui" ON "(WINDOWS AND NOT WINRT) OR MACOSX OR LINUX OR ANDROID OR WASM" OFF)
cmake_dependent_option(AX_ENABLE_EXT_INSPECTOR "Build extension Inspector" ON "(WINDOWS AND NOT WINRT) OR MACOSX OR LINUX OR ANDROID OR WASM" OFF)
cmake_dependent_option(AX_ENABLE_EXT_SDFGEN "Build extension SDFGen" ON "(WINDOWS AND NOT WINRT) OR MACOSX OR LINUX OR WASM" OFF)

option(AX_DISABLE_GLES2 "Whether disable GLES2 support" OFF)

option(AX_ENABLE_EXT_JSONDEFAULT "Build extension JSONDefault" ON)

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

# axmolver.h
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

# early experimental features     
# include(CMakeDependentOption)
# cmake_dependent_option(AX_ENABLE_EARLY_FEATURES
#   "Build the tests when we are the root project" ON
#   "NOT (AX_GIT_BRANCH STREQUAL \"dev\")" OFF)

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

ax_find_shaders(${_AX_ROOT}/core/renderer/shaders BUILTIN_SHADER_SOURCES)
set(ax_builtin_shaders ${BUILTIN_SHADER_SOURCES} CACHE STATIC "" FORCE)

# configure engine builtin light num
if (NOT DEFINED AX_MAX_DIRECTIONAL_LIGHT)
    set(AX_MAX_DIRECTIONAL_LIGHT 1)
endif()
if (NOT DEFINED AX_MAX_POINT_LIGHT)
    set(AX_MAX_POINT_LIGHT 1)
endif()
if (NOT DEFINED AX_MAX_SPOT_LIGHT)
    set (AX_MAX_SPOT_LIGHT 1)
endif()
configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/renderer/RenderConsts.h.in"
    "${CMAKE_CURRENT_LIST_DIR}/renderer/RenderConsts.h" @ONLY)
# SKINPOSITION_NORMAL_TEXTURE_3D:       skinPositionNormalTexture.vert,     colorNormalTexture.frag, LightDefs
# POSITION_NORMAL_TEXTURE_3D:           skinPositionNormalTexture.vert,     colorNormalTexture.frag, LightDefs
# POSITION_NORMAL_3D:                   positionNormalTexture.vert,         colorNormal.frag,        LightDefs
# POSITION_BUMPEDNORMAL_TEXTURE_3D:     positionNormalTexture.vert,         colorNormalTexture.frag, lightNormMapDef
# SKINPOSITION_BUMPEDNORMAL_TEXTURE_3D: skinPositionNormalTexture_vert,     colorNormalTexture.frag, lightNormMapDef
set_source_files_properties(
    ${_AX_ROOT}/core/renderer/shaders/colorNormal.frag
    ${_AX_ROOT}/core/renderer/shaders/colorNormalTexture.frag
    ${_AX_ROOT}/core/renderer/shaders/positionNormalTexture.vert
    ${_AX_ROOT}/core/renderer/shaders/skinPositionNormalTexture.vert
    PROPERTIES GLSLCC_DEFINES
    "MAX_DIRECTIONAL_LIGHT_NUM=${AX_MAX_DIRECTIONAL_LIGHT},MAX_POINT_LIGHT_NUM=${AX_MAX_POINT_LIGHT},MAX_SPOT_LIGHT_NUM=${AX_MAX_SPOT_LIGHT}"
)
set_source_files_properties(
    ${_AX_ROOT}/core/renderer/shaders/colorNormalTexture.frag
    ${_AX_ROOT}/core/renderer/shaders/positionNormalTexture.vert
    ${_AX_ROOT}/core/renderer/shaders/skinPositionNormalTexture.vert
    PROPERTIES GLSLCC_OUTPUT1 "USE_NORMAL_MAPPING=1"
)
ax_target_compile_shaders(${_AX_CORE_LIB} FILES ${BUILTIN_SHADER_SOURCES})

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
    INTERFACE ${_AX_ROOT}/core/platform/${PLATFORM_NAME}
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
    # cppwinrt or webview2 require nuget
    if(WINDOWS_STORE OR AX_ENABLE_MSEDGE_WEBVIEW2)
        find_program(NUGET_EXE NAMES nuget 
            PATHS ${_AX_ROOT}/tools/external/nuget)
            
        if(NOT NUGET_EXE)
            message("NUGET.EXE not found.")
            message(FATAL_ERROR "Please run setup.ps1 again to download NUGET.EXE, and run CMake again.")
        endif()
    endif()

    if(WINDOWS_STORE)
        file(TO_NATIVE_PATH ${CMAKE_BINARY_DIR} AX_TOP_SOLUTION_DIR)
        configure_file(${_AX_ROOT}/cmake/CppWinRT.props.in ${CMAKE_BINARY_DIR}/CppWinRT.props @ONLY)

        # intsall cppwinrt package for xaml apps
        execute_process(COMMAND "${NUGET_EXE}"
                install "Microsoft.Windows.CppWinRT" -Version ${AX_CPPWINRT_VERISON} -ExcludeVersion -OutputDirectory "${CMAKE_BINARY_DIR}/packages")
        set_target_properties(${_AX_CORE_LIB} PROPERTIES
            VS_PROJECT_IMPORT ${CMAKE_BINARY_DIR}/packages/Microsoft.Windows.CppWinRT/build/native/Microsoft.Windows.CppWinRT.props
        )
        target_link_libraries(${_AX_CORE_LIB} ${CMAKE_BINARY_DIR}/packages/Microsoft.Windows.CppWinRT/build/native/Microsoft.Windows.CppWinRT.targets)
    endif()
    if(AX_ENABLE_MSEDGE_WEBVIEW2)
        execute_process(COMMAND "${NUGET_EXE}"
            install "Microsoft.Web.WebView2" -Version 1.0.992.28 -ExcludeVersion -OutputDirectory "${CMAKE_BINARY_DIR}/packages")

        if(CMAKE_GENERATOR MATCHES "Ninja")
            target_link_libraries(${_AX_CORE_LIB} ${CMAKE_BINARY_DIR}/packages/Microsoft.Web.WebView2/build/native/${ARCH_ALIAS}/WebView2Loader.dll.lib)
            target_include_directories(${_AX_CORE_LIB} PUBLIC ${CMAKE_BINARY_DIR}/packages/Microsoft.Web.WebView2/build/native/include)
        else()
            target_link_libraries(${_AX_CORE_LIB} ${CMAKE_BINARY_DIR}/packages/Microsoft.Web.WebView2/build/native/Microsoft.Web.WebView2.targets)
        endif()
    endif()
endif()

# AX_USE_SSE
if (AX_ISA_SIMD MATCHES "sse")
    target_compile_definitions(${_AX_CORE_LIB} PUBLIC AX_USE_SSE=1)
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

#if(XCODE)
#    # Later versions of Xcode clang want to compile C++17 with aligned allocation turned on and this is only supported on iOS 11.0+
#    # TODO: Only turn this off if ${CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET} < 11.0
#    target_compile_options(${_AX_CORE_LIB} PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-fno-aligned-allocation>)
#endif()
