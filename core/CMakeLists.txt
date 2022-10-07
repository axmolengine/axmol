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

if (NOT DEFINED AX_USE_COMPAT_GL)
    set(AX_USE_COMPAT_GL FALSE CACHE INTERNAL "Enabling AX_USE_COMPAT_GL" )
endif()

if (NOT DEFINED AX_ENABLE_MSEDGE_WEBVIEW2)
    set(AX_ENABLE_MSEDGE_WEBVIEW2 TRUE CACHE INTERNAL "Enabling windows webview2" )
endif()

if (NOT DEFINED AX_ENABLE_MFMEDIA)
    set(AX_ENABLE_MFMEDIA TRUE CACHE INTERNAL "Enabling microsoft media foundation for windows video player" )
endif()

project(ax_libs)

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
    )

list(APPEND _AX_SRC ${_AX_HEADER})

add_library(${_AX_CORE_LIB} ${_AX_SRC})

# use thirdparty libs
add_subdirectory(${_AX_ROOT_PATH}/thirdparty ${ENGINE_BINARY_PATH}/thirdparty)
target_link_libraries(${_AX_CORE_LIB} thirdparty)

# add base macro define and compile options
use_ax_compile_define(${_AX_CORE_LIB})
use_ax_compile_options(${_AX_CORE_LIB})

# use all platform related system libs
use_ax_depend(${_AX_CORE_LIB})

target_include_directories(${_AX_CORE_LIB}
    PUBLIC ${_AX_ROOT_PATH}
    PUBLIC ${_AX_ROOT_PATH}/thirdparty
    PUBLIC ${_AX_ROOT_PATH}/extensions
    PUBLIC ${_AX_ROOT_PATH}/core
    PUBLIC ${_AX_ROOT_PATH}/core/platform

    INTERFACE ${_AX_ROOT_PATH}/thirdparty
    INTERFACE ${_AX_ROOT_PATH}/extensions
    INTERFACE ${_AX_ROOT_PATH}/core/base
    INTERFACE ${_AX_ROOT_PATH}/core/audio
    INTERFACE ${_AX_ROOT_PATH}/core/platform/${PLATFORM_FOLDER}
)

set_target_properties(${_AX_CORE_LIB}
                      PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
                      VERSION "${_AX_VERSION}"
                      FOLDER "Internal"
                      )

## Lua bindings lib
# if(AX_ENABLE_EXT_LUA)
#     add_subdirectory(${_AX_ROOT_PATH}/extensions/scripting/lua-bindings ${ENGINE_BINARY_PATH}/core/lua-bindings)
# endif()

if(XCODE OR VS)
    ax_mark_code_files("${_AX_CORE_LIB}")
endif()

if(NOT IOS)
    target_precompile_headers(${_AX_CORE_LIB} PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:cocos2d.h>")
endif()

if(WINDOWS)
    if(AX_ENABLE_MSEDGE_WEBVIEW2)
        find_program(NUGET_EXE NAMES nuget 
            PATHS ${_AX_ROOT_PATH}/tools/external/nuget)
            
        if(NOT NUGET_EXE)
            message("NUGET.EXE not found.")
            message(FATAL_ERROR "Please run setup.py again to download NUGET.EXE, and run CMake again.")
        endif()
     
        exec_program(${NUGET_EXE}
            ARGS install "Microsoft.Web.WebView2" -Version 1.0.992.28 -ExcludeVersion -OutputDirectory ${CMAKE_BINARY_DIR}/packages)

        if(CMAKE_GENERATOR STREQUAL "Ninja")
            target_link_libraries(${_AX_CORE_LIB} ${CMAKE_BINARY_DIR}/packages/Microsoft.Web.WebView2/build/native/${ARCH_ALIAS}/WebView2Loader.dll.lib)
            target_include_directories(${_AX_CORE_LIB} PUBLIC ${CMAKE_BINARY_DIR}/packages/Microsoft.Web.WebView2/build/native/include)
        else()
            target_link_libraries(${_AX_CORE_LIB} ${CMAKE_BINARY_DIR}/packages/Microsoft.Web.WebView2/build/native/Microsoft.Web.WebView2.targets)
        endif()
        target_compile_definitions(${_AX_CORE_LIB} PUBLIC _AX_HAVE_WEBVIEW2)
    endif()
endif()

# engine extensions
add_subdirectory(${_AX_ROOT_PATH}/extensions ${ENGINE_BINARY_PATH}/extensions)

if(MSVC)
    target_sources(${_AX_CORE_LIB} PRIVATE ../thirdparty/yasio/yasio.natvis)
    target_sources(${_AX_CORE_LIB} PRIVATE ../thirdparty/robin-map/tsl-robin-map.natvis)
    target_compile_options(${_AX_CORE_LIB} PUBLIC "/Zm2000")
endif()

# copy all thirdparty libraries to ${CMAKE_BINARY_DIR}/lib/$<CONFIG>
# if (WINDOWS)
#     copy_thirdparty_dlls(libcocos2d ${CMAKE_BINARY_DIR}/lib/$<CONFIG>)
# endif()

#if(XCODE)
#    # Later versions of Xcode clang want to compile C++17 with aligned allocation turned on and this is only supported on iOS 11.0+
#    # TODO: Only turn this off if ${CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET} < 11.0
#    target_compile_options(${_AX_CORE_LIB} PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-fno-aligned-allocation>)
#endif()
