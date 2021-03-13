#/****************************************************************************
# Copyright (c) 2013 cocos2d-x.org
# Copyright (c) 2014 martell malone
# Copyright (c) 2015-2017 Chukong Technologies Inc.
#
# http://www.cocos2d-x.org
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

# this CMakeLists is to generate Cocos2d-x Engine Library
# build libcocos2d
# build luacocos2d if BUILD_LUA_LIBS=ON

# The workaround for solve try_compile failed with code sign
# since cmake-3.18.2, not required
# everyting for cmake toolchain config before project(xxx) is better
set(CMAKE_TRY_COMPILE_PLATFORM_VARIABLES
    "CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED"
    "CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED")
set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED NO)
set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED NO)

# Fix compile failed with armv7 deployment target >= 11.0, xcode clang will report follow error
# clang: error: invalid iOS deployment version '--target=armv7-apple-ios13.6', 
#        iOS 10 is the maximum deployment target for 32-bit targets
# If not defined CMAKE_OSX_DEPLOYMENT_TARGET, cmake will choose latest deployment target
if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    if("${CMAKE_OSX_ARCHITECTURES}" MATCHES ".*armv7.*")
        if(NOT DEFINED CMAKE_OSX_DEPLOYMENT_TARGET 
        OR "${CMAKE_OSX_DEPLOYMENT_TARGET}" VERSION_GREATER "11.0" 
        OR "${CMAKE_OSX_DEPLOYMENT_TARGET}" VERSION_EQUAL "11.0")
            message(STATUS "Sets iOS minimum deployment target to 10.0 for armv7")
            # a. armv7 maximum deployment 10.x
            # b. armv7 TLS require deployment 10.x
            set(CMAKE_OSX_DEPLOYMENT_TARGET "10.0" CACHE STRING "Minimum OS X deployment version")
        endif()
    else()
        if(NOT DEFINED CMAKE_OSX_DEPLOYMENT_TARGET)
            set(CMAKE_OSX_DEPLOYMENT_TARGET "9.0" CACHE STRING "Minimum OS X deployment version")
        endif()
    endif()
endif()

# The version number
set(ENGINEX_VERSION 1.0)

if(NOT DEFINED ENGINEX_CORE_LIB)
    set(ENGINEX_CORE_LIB cocos2d CACHE INTERNAL "The engine-x core lib name" )
endif()

if (NOT DEFINED CC_COMPAT_GL)
    set(CC_COMPAT_GL FALSE CACHE INTERNAL "Enabling CC_COMPAT_GL" )
endif()

project(cocos2d_libs)

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

set(COCOS_HEADER cocos2d.h
    ${COCOS_2D_HEADER}
    ${COCOS_3D_HEADER}
    ${COCOS_PLATFORM_HEADER}
    ${COCOS_PHYSICS_HEADER}
    ${COCOS_PHYSICS3D_HEADER}
    ${COCOS_MATH_HEADER}
    ${COCOS_NAVMESH_HEADER}
    ${COCOS_RENDERER_HEADER}
    ${COCOS_BASE_HEADER}
    ${COCOS_AUDIO_HEADER}
    ${COCOS_UI_HEADER}
    ${COCOS_NETWORK_HEADER}
    )

set(COCOS_SRC cocos2d.cpp
    ${COCOS_2D_SRC}
    ${COCOS_3D_SRC}
    ${COCOS_PLATFORM_SRC}
    ${COCOS_PHYSICS_SRC}
    ${COCOS_PHYSICS3D_SRC}
    ${COCOS_MATH_SRC}
    ${COCOS_NAVMESH_SRC}
    ${COCOS_RENDERER_SRC}
    ${COCOS_BASE_SRC}
    ${COCOS_AUDIO_SRC}
    ${COCOS_UI_SRC}
    ${COCOS_NETWORK_SRC}
    )

list(APPEND COCOS_SRC ${COCOS_HEADER})

add_library(${ENGINEX_CORE_LIB} ${COCOS_SRC})

# if (NOT APPLE)
#   set_source_files_properties("audio/src/AudioEngineImpl.mm" PROPERTIES LANGUAGE CXX)
# endif()

# use external libs
add_subdirectory(${COCOS2DX_ROOT_PATH}/external ${ENGINE_BINARY_PATH}/external)
target_link_libraries(${ENGINEX_CORE_LIB} external)

# add base macro define and compile options
use_cocos2dx_compile_define(${ENGINEX_CORE_LIB})
use_cocos2dx_compile_options(${ENGINEX_CORE_LIB})

# use all platform related system libs
use_cocos2dx_libs_depend(${ENGINEX_CORE_LIB})

target_include_directories(${ENGINEX_CORE_LIB}
    PUBLIC ${COCOS2DX_ROOT_PATH}
    PUBLIC ${COCOS2DX_ROOT_PATH}/external
    PUBLIC ${COCOS2DX_ROOT_PATH}/extensions
    PUBLIC ${COCOS2DX_ROOT_PATH}/cocos
    PUBLIC ${COCOS2DX_ROOT_PATH}/cocos/platform

    INTERFACE ${COCOS2DX_ROOT_PATH}/external
    INTERFACE ${COCOS2DX_ROOT_PATH}/extensions
    INTERFACE ${COCOS2DX_ROOT_PATH}/cocos/base
    INTERFACE ${COCOS2DX_ROOT_PATH}/cocos/audio/include
    INTERFACE ${COCOS2DX_ROOT_PATH}/cocos/platform/${PLATFORM_FOLDER}
)

set_target_properties(${ENGINEX_CORE_LIB}
                      PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                      VERSION "${ENGINEX_VERSION}"
                      FOLDER "Internal"
                      )

## Lua bindings lib
# if(BUILD_LUA_LIBS)
#     add_subdirectory(${COCOS2DX_ROOT_PATH}/extensions/scripting/lua-bindings ${ENGINE_BINARY_PATH}/cocos/lua-bindings)
# endif()

if(XCODE OR VS)
    cocos_mark_code_files("${ENGINEX_CORE_LIB}")
endif()

if(WINDOWS)
    # precompiled header. Compilation time speedup ~4x.
    target_sources(${ENGINEX_CORE_LIB} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/precheader.cpp")
    set_target_properties(${ENGINEX_CORE_LIB} PROPERTIES COMPILE_FLAGS "/Yuprecheader.h /FIprecheader.h")
    set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/precheader.cpp" PROPERTIES COMPILE_FLAGS "/Ycprecheader.h")
    # compile c as c++. needed for precompiled header
    set_source_files_properties(${COCOS_SPINE_SRC} base/ccFPSImages.c PROPERTIES LANGUAGE CXX)
endif()

# engine extensions
add_subdirectory(${COCOS2DX_ROOT_PATH}/extensions ${ENGINE_BINARY_PATH}/extensions)

# copy all external libraries to ${CMAKE_BINARY_DIR}/lib/$<CONFIG>
# if (WINDOWS)
#     copy_thirdparty_dlls(libcocos2d ${CMAKE_BINARY_DIR}/lib/$<CONFIG>)
# endif()

#if(XCODE)
#    # Later versions of Xcode clang want to compile C++17 with aligned allocation turned on and this is only supported on iOS 11.0+
#    # TODO: Only turn this off if ${CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET} < 11.0
#    target_compile_options(${ENGINEX_CORE_LIB} PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-fno-aligned-allocation>)
#endif()
