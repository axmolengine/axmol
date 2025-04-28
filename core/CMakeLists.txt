#/****************************************************************************
# Copyright (c) 2013 cocos2d-x.org
# Copyright (c) 2014 martell malone
# Copyright (c) 2015-2017 Chukong Technologies Inc.
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

# this CMakeLists is to generate Axmol Engine Library
# build ax
# build axlua if AX_ENABLE_EXT_LUA=ON

# include_guard (GLOBAL)

# The version number
set(_AX_VERSION 2.6)

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
cmake_dependent_option(AX_USE_COMPAT_GL "Whether use compatibility GL as render backend" ${WIN32} "WIN32 OR APPLE" OFF)

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

if (LINUX)
    include(CheckIncludeFile)
    check_include_file(vlc/vlc.h _AX_HAVE_VLC)
    cmake_dependent_option(AX_ENABLE_VLC_MEDIA "Enabling vlc media" ON "_AX_HAVE_VLC" OFF)
endif()

cmake_dependent_option(AX_ENABLE_MSEDGE_WEBVIEW2 "Use Microsoft Edge webview2" ON "WIN32 AND NOT WINRT" OFF)

cmake_dependent_option(AX_ENABLE_MFMEDIA "Enabling microsoft media foundation" ON "WIN32" OFF)
cmake_dependent_option(AX_ENABLE_VLC_MEDIA "Enabling vlc media" OFF "(WIN32 AND NOT WINRT) OR LINUX" OFF)

if(NOT APPLE AND (NOT EMSCRIPTEN))
    set(AX_USE_ALSOFT ON CACHE BOOL "" FORCE)
else()
    option(AX_USE_ALSOFT "Use ALSOFT on apple" OFF)
endif()

option(AX_ENABLE_PHYSICS "Build Physics support" ON)
cmake_dependent_option(AX_ENABLE_MEDIA "Build media support" ON "AX_ENABLE_MFMEDIA OR AX_ENABLE_VLC_MEDIA OR APPLE OR ANDROID" OFF)
option(AX_ENABLE_AUDIO "Build audio support" ON)
option(AX_ENABLE_OPUS "Build with opus support" ON)
option(AX_ENABLE_CONSOLE "Build axmol debug tool: console support" ON)

option(AX_ENABLE_3D "Build 3D support" ON)
cmake_dependent_option(AX_ENABLE_3D_PHYSICS "Build 3D Physics support" ON "AX_ENABLE_3D" OFF)
cmake_dependent_option(AX_ENABLE_NAVMESH "Build NavMesh support" ON "AX_ENABLE_3D" OFF)

option(AX_UPDATE_BUILD_VERSION "Update build version" ON)
option(AX_DISABLE_GLES2 "Whether disable GLES2 support" OFF)
option(AX_CORE_PROFILE "Whether strip deprecated features" OFF)

# default value for axmol extensions modules to Build
# total supported extensions count: 13
# extensions dependicies: COCOSTUDIO may depend on spine & dragonBones if they are present in buildset

option(AX_EXT_HINT "The default extensions hint" ON)

option(AX_ENABLE_EXT_LUA "Build lua libraries" ${AX_EXT_HINT})
option(AX_ENABLE_EXT_GUI "Build extension GUI" ${AX_EXT_HINT})
option(AX_ENABLE_EXT_ASSETMANAGER "Build extension asset-manager" ${AX_EXT_HINT})
option(AX_ENABLE_EXT_SPINE "Build extension spine" ${AX_EXT_HINT})
option(AX_ENABLE_EXT_DRAGONBONES "Build extension DragonBones" ${AX_EXT_HINT})
option(AX_ENABLE_EXT_COCOSTUDIO "Build extension cocostudio" ${AX_EXT_HINT})
option(AX_ENABLE_EXT_FAIRYGUI "Build extension FairyGUI" ${AX_EXT_HINT})
option(AX_ENABLE_EXT_LIVE2D "Build extension Live2D" OFF)
option(AX_ENABLE_EXT_EFFEKSEER "Build extension Effekseer" OFF)

cmake_dependent_option(AX_ENABLE_EXT_PARTICLE3D "Build extension Particle3D" ${AX_EXT_HINT} "AX_ENABLE_3D" OFF)
cmake_dependent_option(AX_ENABLE_EXT_PHYSICS_NODE "Build extension physics-nodes" ${AX_EXT_HINT} "AX_ENABLE_PHYSICS" OFF)
cmake_dependent_option(AX_ENABLE_EXT_IMGUI "Build extension ImGui" ${AX_EXT_HINT} "(WINDOWS AND NOT WINRT) OR MACOSX OR LINUX OR ANDROID OR WASM" OFF)
cmake_dependent_option(AX_ENABLE_EXT_INSPECTOR "Build extension Inspector" ${AX_EXT_HINT} "AX_ENABLE_EXT_IMGUI" OFF)
cmake_dependent_option(AX_ENABLE_EXT_SDFGEN "Build extension SDFGen" ${AX_EXT_HINT} "AX_ENABLE_EXT_IMGUI" OFF)

option(AX_ENABLE_EXT_JSONDEFAULT "Build extension JSONDefault" ${AX_EXT_HINT})

set(_AX_THIRDPARTY_NAME "3rdparty" CACHE INTERNAL "" )

if (WIN32)
    option(WIN32_PATH_USE_ACP "" OFF)
    if(NOT WIN32_PATH_USE_ACP)
        add_definitions(-DNTCVT_CP_DEFAULT=CP_UTF8)
    else()
        add_definitions(-DNTCVT_CP_DEFAULT=CP_ACP)
    endif()
endif()

# axmolver.h
find_program(GIT_PROG NAMES git)
if(AX_UPDATE_BUILD_VERSION AND GIT_PROG AND EXISTS "${_AX_ROOT}/.git")
    # Get the current working branch and its latest abbreviated commit hash
    execute_process(COMMAND ${GIT_PROG} -C "${_AX_ROOT}" rev-list --count HEAD
        TIMEOUT 5
        OUTPUT_VARIABLE AX_BUILD_NUM
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND ${GIT_PROG} -C "${_AX_ROOT}" branch --show-current
        TIMEOUT 5
        OUTPUT_VARIABLE AX_GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND ${GIT_PROG} -C "${_AX_ROOT}" rev-parse --short=7 HEAD
        TIMEOUT 5
        OUTPUT_VARIABLE AX_GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(AX_GIT_PRESENT 1)
else()
    set(AX_GIT_BRANCH "UNKNOWN")
    set(AX_GIT_COMMIT_HASH "unknown")
    set(AX_GIT_PRESENT 0)
endif()
configure_file(
        "${CMAKE_CURRENT_LIST_DIR}/axmolver.h.in"
        "${CMAKE_CURRENT_LIST_DIR}/axmolver.h" @ONLY)

# early experimental features
# include(CMakeDependentOption)
# cmake_dependent_option(AX_ENABLE_EARLY_FEATURES
#   "Build the tests when we are the root project" ON
#   "NOT (AX_GIT_BRANCH STREQUAL \"dev\")" OFF)

include(2d/CMakeLists.txt)
if(AX_ENABLE_3D)
    include(3d/CMakeLists.txt)
endif()
include(platform/CMakeLists.txt)
if(AX_ENABLE_PHYSICS)
    include(physics/CMakeLists.txt)
endif()
if(AX_ENABLE_3D_PHYSICS)
    include(physics3d/CMakeLists.txt)
endif()
include(math/CMakeLists.txt)
if(AX_ENABLE_NAVMESH)
    include(navmesh/CMakeLists.txt)
endif()
include(renderer/CMakeLists.txt)
include(base/CMakeLists.txt)
include(ui/CMakeLists.txt)
include(network/CMakeLists.txt)
if(AX_ENABLE_AUDIO)
    include(audio/CMakeLists.txt)
endif()

if(AX_ENABLE_MEDIA)
    file(GLOB_RECURSE _AX_MEDIA_HEADER media/*.h)
    file(GLOB_RECURSE _AX_MEDIA_SRC media/*.cpp)

    if (APPLE)
        list(APPEND _AX_MEDIA_SRC media/AvfMediaEngine.mm)
        set_source_files_properties(media/MediaEngine.cpp PROPERTIES LANGUAGE OBJCXX)
    endif()
endif()

set(_AX_HEADER axmol.h
    ${_AX_2D_HEADER}
    ${_AX_PLATFORM_HEADER}
    ${_AX_MATH_HEADER}
    ${_AX_RENDERER_HEADER}
    ${_AX_BASE_HEADER}
    ${_AX_UI_HEADER}
    ${_AX_NETWORK_HEADER}
    )

set(_AX_SRC axmol.cpp
    ${_AX_2D_SRC}
    ${_AX_PLATFORM_SRC}
    ${_AX_MATH_SRC}
    ${_AX_RENDERER_SRC}
    ${_AX_BASE_SRC}
    ${_AX_UI_SRC}
    ${_AX_NETWORK_SRC}
    )

if(AX_ENABLE_3D)
    list(APPEND _AX_HEADER ${_AX_3D_HEADER})
    list(APPEND _AX_SRC ${_AX_3D_SRC})
endif()
if(AX_ENABLE_PHYSICS)
    list(APPEND _AX_HEADER ${_AX_PHYSICS_HEADER})
    list(APPEND _AX_SRC ${_AX_PHYSICS_SRC})
endif()
if(AX_ENABLE_3D_PHYSICS)
    list(APPEND _AX_HEADER ${_AX_PHYSICS3D_HEADER})
    list(APPEND _AX_SRC ${_AX_PHYSICS3D_SRC})
endif()
if(AX_ENABLE_NAVMESH)
    list(APPEND _AX_HEADER ${_AX_NAVMESH_HEADER})
    list(APPEND _AX_SRC ${_AX_NAVMESH_SRC})
endif()
if(AX_ENABLE_MEDIA)
    list(APPEND _AX_HEADER ${_AX_MEDIA_HEADER})
    list(APPEND _AX_SRC ${_AX_MEDIA_SRC})
endif()
if(AX_ENABLE_AUDIO)
    list(APPEND _AX_HEADER ${_AX_AUDIO_HEADER})
    list(APPEND _AX_SRC ${_AX_AUDIO_SRC})
endif()

list(APPEND _AX_SRC ${_AX_HEADER})

add_library(${_AX_CORE_LIB} ${_AX_SRC})

if(APPLE)
    set_xcode_property(${_AX_CORE_LIB} CLANG_ENABLE_OBJC_WEAK "YES")
endif()

if(WASM)
  if(${EMSCRIPTEN_VERSION} VERSION_GREATER_EQUAL "3.1.51")
      target_link_options(${_AX_CORE_LIB} PUBLIC -sGL_ENABLE_GET_PROC_ADDRESS)
  endif()
  target_link_options(${_AX_CORE_LIB} PUBLIC -lwebsocket.js)
elseif(AX_ENABLE_MFMEDIA AND NOT WINRT AND FULL_MSVC)
  target_link_options(${_AX_CORE_LIB} PUBLIC "/DELAYLOAD:mf.dll" "/DELAYLOAD:mfplat.dll")
endif()

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
    PROPERTIES AXSLCC_DEFINES
    "MAX_DIRECTIONAL_LIGHT_NUM=${AX_MAX_DIRECTIONAL_LIGHT},MAX_POINT_LIGHT_NUM=${AX_MAX_POINT_LIGHT},MAX_SPOT_LIGHT_NUM=${AX_MAX_SPOT_LIGHT}"
)
set_source_files_properties(
    ${_AX_ROOT}/core/renderer/shaders/colorNormalTexture.frag
    ${_AX_ROOT}/core/renderer/shaders/positionNormalTexture.vert
    ${_AX_ROOT}/core/renderer/shaders/skinPositionNormalTexture.vert
    PROPERTIES AXSLCC_OUTPUT1 "USE_NORMAL_MAPPING=1"
)
ax_target_compile_shaders(${_AX_CORE_LIB} FILES ${BUILTIN_SHADER_SOURCES})

ax_config_pred(${_AX_CORE_LIB} AX_ENABLE_MFMEDIA)
ax_config_pred(${_AX_CORE_LIB} AX_ENABLE_MSEDGE_WEBVIEW2)
ax_config_pred(${_AX_CORE_LIB} AX_ENABLE_PHYSICS)
ax_config_pred(${_AX_CORE_LIB} AX_ENABLE_3D)
ax_config_pred(${_AX_CORE_LIB} AX_ENABLE_3D_PHYSICS)
ax_config_pred(${_AX_CORE_LIB} AX_ENABLE_NAVMESH)
ax_config_pred(${_AX_CORE_LIB} AX_ENABLE_MEDIA)
ax_config_pred(${_AX_CORE_LIB} AX_ENABLE_AUDIO)
ax_config_pred(${_AX_CORE_LIB} AX_ENABLE_CONSOLE)
ax_config_pred(${_AX_CORE_LIB} AX_CORE_PROFILE)

# use 3rdparty libs
add_subdirectory(${_AX_ROOT}/3rdparty ${ENGINE_BINARY_PATH}/3rdparty)
target_link_libraries(${_AX_CORE_LIB} 3rdparty)

# add base macro define and compile options
use_ax_compile_define(${_AX_CORE_LIB})
use_ax_compile_options(${_AX_CORE_LIB})

# use all platform related system libs
use_ax_depend(${_AX_CORE_LIB})

target_include_directories(${_AX_CORE_LIB}
    PUBLIC ${_AX_ROOT}
    PUBLIC ${_AX_ROOT}/3rdparty
    PUBLIC ${_AX_ROOT}/extensions/scripting
    PUBLIC ${_AX_ROOT}/core
    PUBLIC ${_AX_ROOT}/core/platform

    INTERFACE ${_AX_ROOT}/3rdparty
    INTERFACE ${_AX_ROOT}/extensions/scripting
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

message(STATUS "CMake precompile headers for ${_AX_CORE_LIB}")
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
                install "Microsoft.Windows.CppWinRT" -Version ${AX_CPPWINRT_VERSION} -ExcludeVersion -OutputDirectory "${CMAKE_BINARY_DIR}/packages")
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

# axmol math simd intrinsics support
set(_simdc_defines)
set(_simdc_options)
if (NOT WASM) # native platforms auto detect from cmake or preprocessor check
    if (AX_ISA_SIMD MATCHES "sse")
        list(APPEND _simdc_defines AX_SSE_INTRINSICS=1)
        if (AX_ISA_SIMD MATCHES "sse4")
            list(APPEND _simdc_defines __SSE4_1__=1)
            if (LINUX)
                list(APPEND _simdc_options -msse4.1)
            endif()
        endif()
    endif()
else() # wasm requires user specify SIMD intrinsics manually
    set(AX_WASM_ISA_SIMD "none" CACHE STRING "")
    string(TOLOWER ${AX_WASM_ISA_SIMD} AX_WASM_ISA_SIMD)
    if(AX_WASM_ISA_SIMD MATCHES "sse")
        message(AUTHOR_WARNING "Using SSE intrinsics for WASM ...")
        list(APPEND _simdc_defines AX_SSE_INTRINSICS=1 __SSE__=1 __SSE2__=1 __SSE3__=1 __SSSE3__=1)
        if(AX_ISA_LEVEL GREATER_EQUAL 2)
            list(APPEND _simdc_defines __SSE4_1__=1)
        endif()
        list(APPEND _simdc_options -msimd128)
    elseif(AX_WASM_ISA_SIMD MATCHES "neon")
        message(AUTHOR_WARNING "Using NEON intrinsics for WASM ...")
        list(APPEND _simdc_defines AX_NEON_INTRINSICS=1)
        list(APPEND _simdc_options -mfpu=neon -msimd128)
    endif()
endif()

if(_simdc_defines)
    target_compile_definitions(${_AX_CORE_LIB} PUBLIC ${_simdc_defines})
    if(_simdc_options)
        target_compile_options(${_AX_CORE_LIB} PUBLIC ${_simdc_options})
    endif()
endif()

# engine extensions
add_subdirectory(${_AX_ROOT}/extensions ${ENGINE_BINARY_PATH}/extensions)

if(MSVC)
    target_sources(${_AX_CORE_LIB} PRIVATE ./axmol.natvis)
    target_sources(${_AX_CORE_LIB} PRIVATE ../3rdparty/robin-map/tsl-robin-map.natvis)
    target_compile_options(${_AX_CORE_LIB} PUBLIC "/Zm2000")
endif()

if(ANDROID)
    add_subdirectory(${_AX_ROOT}/core/platform/android ${ENGINE_BINARY_PATH}/core/cpp-android)
    if (BUILD_SHARED_LIBS)
        # whole archive for jni
        target_link_libraries(${_AX_CORE_LIB} -Wl,--whole-archive cpp_android_spec -Wl,--no-whole-archive)
        if (_AX_LUA_PROJ)
            add_subdirectory(${_AX_ROOT}/extensions/scripting/lua-bindings/proj.android ${ENGINE_BINARY_PATH}/extensions/lua-android)
            target_link_libraries(${_AX_CORE_LIB} -Wl,--whole-archive lua_android_spec -Wl,--no-whole-archive)
        endif()

        if (_AX_LUA_PROJ OR AX_ENABLE_EXT_LUA)
            target_link_libraries(${_AX_CORE_LIB} ${AX_LUA_ENGINE})
        endif()
    endif()
endif()

if(LINUX)
    find_package(X11 REQUIRED)
    target_include_directories(${_AX_CORE_LIB} PUBLIC "${X11_X11_INCLUDE_PATH}")
    # X11 gets linked by cmake/Modules/AXLinkHelpers.cmake

    # including GTK3.0 and WebKit2Gtk4.x
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GKT3 REQUIRED gtk+-3.0)

    set(_webkit2gtk_vers "4.1;4.0")
    foreach(_ver ${_webkit2gtk_vers})
        pkg_check_modules(WEBKIT2GTK webkit2gtk-${_ver})
        if (WEBKIT2GTK_LIBRARY_DIRS)
            break()
        endif()
    endforeach()

    target_include_directories(${_AX_CORE_LIB} PUBLIC ${GTK3_INCLUDE_DIRS} ${WEBKIT2GTK_INCLUDE_DIRS})
    target_link_directories(${_AX_CORE_LIB} PRIVATE ${GTK3_LIBRARY_DIRS} ${WEBKIT2GTK_LIBRARY_DIRS})
    target_link_libraries(${_AX_CORE_LIB} ${GTK3_LIBRARIES} ${WEBKIT2GTK_LIBRARIES})
endif()

#if(XCODE)
#    # Later versions of Xcode clang want to compile C++17 with aligned allocation turned on and this is only supported on iOS 11.0+
#    # TODO: Only turn this off if ${CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET} < 11.0
#    target_compile_options(${_AX_CORE_LIB} PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-fno-aligned-allocation>)
#endif()
