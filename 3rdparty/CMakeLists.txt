
project(3rdparty)

option(AX_WITH_JPEG "Build with internal jpeg support" ON)
option(AX_WITH_WEBP "Build with internal webp support" ON)
option(AX_WITH_CLIPPER2 "Build with internal Clipper2 support" ON)
option(AX_WITH_POLY2TRI "Build with internal poly2tri support" ON)
option(AX_WITH_FASTLZ "Build with internal fastlz support" ON)
option(AX_WITH_LZ4 "Build with internal lz4 support" OFF)
option(AX_WITH_CURL "Build with internal curl support" ON)
option(AX_WITH_UNZIP "Build with internal unzip support" ON)
option(AX_WITH_ASTCENC "Build with internal ASTCENC support" ON)
option(AX_WITH_CARES "Build with internal c-ares support" OFF)
option(AX_WITH_YAML_CPP "Build with yaml-cpp support" OFF)
option(AX_WITH_KCP "Build with internal kcp support" OFF)
option(AX_WITH_OBOE "Build with oboe support" OFF)
option(AX_WITH_LLHTTP "Build with internal LLHTTP support" ON)

# by default, enable ios,macOS openal-soft framework for legal license LGPL-2.1
option(ALSOFT_OSX_FRAMEWORK "" ON)

# Enable static library for openal-soft. This has an impact on the
# license of the final product since the source of the whole game must
# then be available under terms compatible with the LGPL (e.g. GPL,
# MPL). Consequently, we set it to OFF by default.
option(
  AX_USE_ALSOFT_STATIC
  "Link with OpenAL as a static library (LGPL-compatible apps only)."
  OFF
)

set(ANDROID_SHARED_LOADS "" CACHE INTERNAL "The android shared libraries load source code" )
set(ANDROID_SHARED_LOAD_FILE_NAME "SharedLoader.java" CACHE INTERNAL "The android shared load java file name" )
set(ANDROID_SHARED_LOAD_FILE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${ANDROID_SHARED_LOAD_FILE_NAME}.in" CACHE INTERNAL "The android shared libraries load config code file" )

cmake_policy(SET CMP0079 NEW)
include(CheckIncludeFile)
include(CheckCCompilerFlag)
include(CheckCSourceCompiles)
include(CheckCXXSourceCompiles)

# Note: we set default AX_ISA_LEVEL to 2 for sse4.1 for axmol app can runs on large amount devices
# If you want axmol app runs on more old devices, you can specify in cmake cmdline `-DAX_ISA_LEVEL=1`,
# otherwise, host compiler generated instructions will crash on old devices which not support high level
# SIMD instructions.
set(AX_ISA_LEVEL 2 CACHE STRING "SIMD Instructions Acceleration Level")

# SIMD instrinsics detetion when AX_ISA_LEVEL not 0
if(AX_ISA_LEVEL)
    ### check -msse2 flag
    set(OLD_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS})
    if(MSVC)
        set(CMAKE_REQUIRED_FLAGS "${OLD_REQUIRED_FLAGS} /WX")
        check_c_compiler_flag("/arch:SSE2" AX_HAVE_SSE2_SWITCH)
    else()
        set(CMAKE_REQUIRED_FLAGS "${OLD_REQUIRED_FLAGS} -Werror")
        check_c_compiler_flag(-msse2 AX_HAVE_SSE2_SWITCH)
    endif()
    if (AX_HAVE_SSE2_SWITCH)
        set(AX_HAVE_SSE2_INTRINSICS 1)
    endif()
    ### end check -msse2 flag

    macro(ax_check_c_source source outputVar)
        if (NOT CMAKE_CROSSCOMPILING)
            check_c_source_runs("${source}" ${outputVar})
        else()
            check_c_source_compiles("${source}" ${outputVar})
        endif()
    endmacro(ax_check_c_source source var)

    # Checking intel SIMD Intrinsics
    include(CheckCSourceRuns)
    if(APPLE)
        set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -mpopcnt")
    endif()
    ax_check_c_source("#include <immintrin.h>
        int main()
        {
            __m256 m = _mm256_set_ps(0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
            return (int)*(float*)&m;
        }" AX_HAVE_AVX2_INTRINSICS)
    ax_check_c_source("#include <nmmintrin.h>
        int main()
        {
            unsigned int v = 0;
            return (int)_mm_popcnt_u32(v);
        }" AX_HAVE_SSE42_INTRINSICS)
    ax_check_c_source("#include <smmintrin.h>
        int main()
        {
            __m128i shuf = _mm_set_epi8(0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
            return *(int*)&shuf;
        }" AX_HAVE_SSE41_INTRINSICS)

    if (NOT AX_HAVE_SSE2_INTRINSICS)
        ax_check_c_source("#include <emmintrin.h>
            int main()
            {
                __m128d m = _mm_set_sd(0.0);
                return (int)*(double*)&m;
            }" AX_HAVE_SSE2_INTRINSICS)
    endif()

    set(CMAKE_REQUIRED_FLAGS ${OLD_REQUIRED_FLAGS})

    ### Checking ARM SIMD neon
    if (NOT WASM) # wasm neon stupid not work, so skipped
        check_include_file(arm_neon.h AX_HAVE_ARM_NEON_H)
        if(AX_HAVE_ARM_NEON_H)
            check_c_source_compiles("#include <arm_neon.h>
            int main()
            {
                int32x4_t ret4 = vdupq_n_s32(0);
                return vgetq_lane_s32(ret4, 0);
            }" AX_HAVE_NEON_INTRINSICS)
        endif()
    endif()

    set(CMAKE_REQUIRED_FLAGS ${OLD_REQUIRED_FLAGS})
    unset(OLD_REQUIRED_FLAGS)

    ### set AX_ISA_SIMD
    if(AX_HAVE_AVX2_INTRINSICS AND AX_ISA_LEVEL GREATER_EQUAL 4)
        set(AX_ISA_SIMD "avx2" CACHE STRING "" FORCE)
    elseif(AX_HAVE_SSE42_INTRINSICS AND AX_ISA_LEVEL GREATER_EQUAL 3)
        set(AX_ISA_SIMD "sse4.2" CACHE STRING "" FORCE)
    elseif(AX_HAVE_SSE41_INTRINSICS AND AX_ISA_LEVEL GREATER_EQUAL 2)
        set(AX_ISA_SIMD "sse4.1" CACHE STRING "" FORCE)
    elseif(AX_HAVE_SSE2_INTRINSICS AND AX_ISA_LEVEL)
        set(AX_ISA_SIMD "sse2" CACHE STRING "" FORCE)
    elseif(AX_HAVE_NEON_INTRINSICS AND AX_ISA_LEVEL)
        set(AX_ISA_SIMD "neon" CACHE STRING "" FORCE)
    else()
        set(AX_ISA_SIMD "null")
    endif()

    message(AUTHOR_WARNING "AX_ISA_SIMD=${AX_ISA_SIMD}")

    if (WINDOWS)
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            if (AX_HAVE_SSE41_INTRINSICS AND AX_ISA_LEVEL GREATER_EQUAL 2)
                add_compile_options("-msse4.1")
            endif()
        endif()
    endif()
endif()

if (WINRT)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
endif()

if(NOT DEFINED AX_USE_LUAJIT)
    set(AX_USE_LUAJIT FALSE CACHE INTERNAL "Use luajit instead plainlua")
endif()

if(NOT AX_USE_LUAJIT)
    message(STATUS "Using plainlua https://lua.org")
    set (AX_LUA_ENGINE "plainlua"  CACHE INTERNAL "")
else()
    message(STATUS "Using luajit https://luajit.org")
    set (AX_LUA_ENGINE "luajit"  CACHE INTERNAL "")
endif()

add_library(3rdparty INTERFACE)

set(_1kfetch_cache_dir "${_AX_ROOT}/cache" CACHE STRING "" FORCE)
set(_1kfetch_manifest "${_AX_ROOT}/manifest.json" CACHE STRING "" FORCE)

# fetch prebuilts of 1kdist and git sources
include(${_AX_ROOT}/1k/fetch.cmake)

function(ax_add_3rd source_dir)
    set(options EXCLUDE_FROM_ALL NO_LINK)
    set(multiValueArgs TARGETS OPTIONS)
    cmake_parse_arguments(opt "${options}" "" "${multiValueArgs}" ${ARGN})

    foreach(OPTION ${opt_OPTIONS})
        _1kparse_option("${OPTION}")
        set(${OPTION_KEY} "${OPTION_VALUE}" CACHE BOOL "" FORCE)
    endforeach()

    set(binary_dir "")
    if(IS_ABSOLUTE ${source_dir})
        string(LENGTH "${_AX_ROOT}/cache/" _offset)
        string(LENGTH ${source_dir} _len)
        math(EXPR _len "${_len} - ${_offset}" OUTPUT_FORMAT DECIMAL)
        string(SUBSTRING ${source_dir} ${_offset} ${_len} _path)
        set(binary_dir "${ENGINE_BINARY_PATH}/3rdparty/${_path}")
    endif()

    if (opt_EXCLUDE_FROM_ALL)
        add_subdirectory(${source_dir} ${binary_dir} EXCLUDE_FROM_ALL)
    else()
        add_subdirectory(${source_dir} ${binary_dir})
    endif()

    if(NOT opt_TARGETS) # if opt_TARGETS not specified use source_dir as target name
        list(APPEND opt_TARGETS ${source_dir})
    endif()
    foreach(tgt ${opt_TARGETS})
        get_target_property(tgt_type ${tgt} TYPE)
        if (NOT ${opt_NO_LINK})
            if(tgt_type STREQUAL "STATIC_LIBRARY" OR tgt_type STREQUAL "SHARED_LIBRARY" OR tgt_type STREQUAL "INTERFACE_LIBRARY")
                target_link_libraries(3rdparty INTERFACE ${tgt})
            endif()
        else()
            add_dependencies(3rdparty ${tgt})
        endif()
        get_target_property(is_imported_lib ${tgt} IMPORTED)
        if ((NOT is_imported_lib) AND (NOT (tgt_type STREQUAL "INTERFACE_LIBRARY")))
            set_target_properties(${tgt} PROPERTIES
                ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
                FOLDER "3rdparty"
            )
        endif()
    endforeach()
endfunction()

# bellow are header only libs
target_include_directories(3rdparty
    INTERFACE "robin-map/include"
)

if (ANDROID)
    target_include_directories(3rdparty
        INTERFACE "jni.hpp/include"
    )
endif()

ax_add_3rd(fmt)

# bellow are non header only libs

# cpufeatures
if(ANDROID)
    add_subdirectory(android-specific/cpufeatures)
endif()

ax_add_3rd(zlib)

if(NOT EMSCRIPTEN)
    option(AX_WITH_WEBSOCKET_PARSER "Build with websocket parser" ON)
    if (AX_WITH_WEBSOCKET_PARSER)
        ax_add_3rd(websocket-parser)
    endif()
endif()

# libpng
ax_add_3rd(png)

if(AX_ENABLE_PHYSICS)
    set(box2d_opts "BOX2D_BUILD_UNIT_TESTS OFF" "BOX2D_BUILD_TESTBED OFF")
    ax_add_3rd(box2d OPTIONS ${box2d_opts})
endif()

if(AX_ENABLE_PHYSICS)
    ax_add_3rd(chipmunk OPTIONS
        "CP_BUILD_SHARED OFF"
        "CP_BUILD_STATIC ON"
        "CP_BUILD_DEMOS OFF"
        "CP_INSTALL_STATIC OFF"
    )
    set_target_properties(chipmunk PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/chipmunk/include"
    )

    # !important axmol not use double precision
    target_compile_definitions(chipmunk PUBLIC CP_USE_CGTYPES=0)
    target_compile_definitions(chipmunk PUBLIC CP_USE_DOUBLES=0)
endif()

ax_add_3rd(freetype OPTIONS
    "DISABLE_FORCE_DEBUG_POSTFIX ON"
    "SKIP_INSTALL_ALL TRUE"
    "FT_DISABLE_HARFBUZZ TRUE"
    "FT_DISABLE_BROTLI TRUE"
    "FT_DISABLE_BZIP2 TRUE"
)
target_include_directories(freetype PRIVATE "${PNG_INCLUDE_DIR}")
if (WINRT)
    target_compile_definitions(freetype PUBLIC "generic=GenericFromFreeTypeLibrary")
endif()
target_include_directories(3rdparty INTERFACE "freetype/include")

if(AX_ENABLE_NAVMESH)
    ax_add_3rd(recast)
endif()

if(AX_ENABLE_3D_PHYSICS)
    ax_add_3rd(bullet)
endif()

if(AX_WITH_JPEG AND NOT WINRT AND NOT EMSCRIPTEN)
    ax_add_3rd(jpeg-turbo)
endif()

add_subdirectory(openssl)
if(ANDROID OR LINUX)
    target_link_libraries(OpenSSL::SSL INTERFACE OpenSSL::Crypto)
    target_link_libraries(3rdparty INTERFACE OpenSSL::SSL)
else()
    target_link_libraries(3rdparty
                        INTERFACE OpenSSL::Crypto
                        INTERFACE OpenSSL::SSL)
endif()
target_compile_definitions(3rdparty INTERFACE OPENSSL_SUPPRESS_DEPRECATED=1)

if(AX_WITH_WEBP)
    ax_add_3rd(webp)
endif()

ax_add_3rd(pugixml)
ax_add_3rd(xxhash)

if (AX_WITH_FASTLZ)
    ax_add_3rd(fastlz)
endif()

if(AX_WITH_LZ4)
    if (NOT BUILD_SHARED_LIBS)
        set(lz4_tgt lz4_static)
    else()
        set(lz4_tgt lz4_shared)
    endif()
    _1kfetch(lz4)
    ax_add_3rd(${lz4_SOURCE_DIR}/build/cmake TARGETS ${lz4_tgt} OPTIONS
      "LZ4_BUILD_CLI OFF"
      "LZ4_BUILD_LEGACY_LZ4C OFF"
    )
endif()

if(AX_WITH_CLIPPER2)
    ax_add_3rd(clipper2)
endif()

ax_add_3rd(ConvertUTF)

if(AX_WITH_POLY2TRI)
    ax_add_3rd(poly2tri)
    target_compile_definitions(poly2tri PUBLIC P2T_STATIC_EXPORTS)
endif()

if(AX_WITH_ASTCENC)
    ax_add_3rd(astcenc)
endif()

# use lua/js specific libs by property to prevent conflict
if(AX_ENABLE_EXT_LUA)
    add_subdirectory(lua/${AX_LUA_ENGINE})
    add_subdirectory(lua/tolua)
    add_subdirectory(lua/lua-cjson)
    set_property(TARGET 3rdparty APPEND PROPERTY
        AX_LUA_DEPEND ${AX_LUA_ENGINE} tolua lua-cjson
    )
    if (AX_USE_LUAJIT)
        target_compile_definitions(lua-cjson PRIVATE USING_LUAJIT=1)
    endif()
endif()

if(AX_WITH_CURL AND NOT EMSCRIPTEN)
    ax_add_3rd(curl)
    if(ANDROID OR LINUX)
        target_link_libraries(curl INTERFACE OpenSSL::SSL)
    endif()
endif()

if (AX_ENABLE_AUDIO)
    # The openal-soft(LGPL 2.1)
    if(AX_USE_ALSOFT AND NOT EMSCRIPTEN)
        set(alsoft_opts "ALSOFT_DLOPEN OFF" "ALSOFT_UTILS OFF" "ALSOFT_EXAMPLES OFF" "ALSOFT_INSTALL OFF")

        if (AX_USE_ALSOFT_STATIC)
            list(APPEND alsoft_opts "LIBTYPE STATIC")
        endif()

        if (ANDROID)
            if(AX_WITH_OBOE)
                _1kfetch(oboe)
                set(ALSOFT_BACKEND_OPENSL OFF CACHE BOOL "Enable OpenSL backend" FORCE)
                set(OBOE_SOURCE "${oboe_SOURCE_DIR}" CACHE STRING "Source directory for Oboe." FORCE)
            endif()
            list(APPEND alsoft_opts "ALSOFT_RTKIT OFF")
            set(ALSOFT_RTKIT OFF CACHE BOOL "Enable RTKit support" FORCE)
        endif()

        if(WINDOWS)
            list(APPEND alsoft_opts "ALSOFT_BACKEND_WASAPI ON" "ALSOFT_BACKEND_WAVE OFF" "ALSOFT_BACKEND_DSOUND OFF" "ALSOFT_BACKEND_WINMM OFF")
        endif()

        if(WINRT)
            set(ALSOFT_CPPWINRT_VERSION ${AX_CPPWINRT_VERSION} CACHE STRING "" FORCE)
        endif()

        ax_add_3rd(openal EXCLUDE_FROM_ALL TARGETS alcommon;OpenAL OPTIONS ${alsoft_opts})

        target_include_directories(3rdparty INTERFACE openal)
        target_compile_definitions(3rdparty INTERFACE AX_USE_ALSOFT=1)
        set_target_properties(OpenAL alcommon PROPERTIES CXX_STANDARD ${_AX_CXX_STD})

        if (AX_USE_ALSOFT_STATIC)
            target_compile_definitions(3rdparty INTERFACE AL_LIBTYPE_STATIC=1)
        elseif(ANDROID)
            set(ANDROID_SHARED_LOADS "${ANDROID_SHARED_LOADS}System.loadLibrary(\"openal\");" CACHE INTERNAL "Android Shared Loads" )
        endif()
    endif()


    # The ogg decoder
    ax_add_3rd(ogg)
endif()

if(WINDOWS OR LINUX OR ANDROID OR WASM)
    ax_add_3rd(glad)
endif()

if((WINDOWS AND NOT WINRT) OR MACOSX OR LINUX)
    if(NOT LINUX)
        ax_add_3rd(glfw OPTIONS
          "GLFW_BUILD_DOCS OFF"
          "GLFW_INSTALL OFF"
        )
    else()
        ax_add_3rd(glfw OPTIONS
          "GLFW_BUILD_X11 ON"
          "GLFW_BUILD_WAYLAND OFF"
          "GLFW_BUILD_DOCS OFF"
          "GLFW_INSTALL OFF"
        )
    endif()
    set_target_properties(glfw PROPERTIES OUTPUT_NAME glfw)
    if(NOT WINDOWS)
        target_link_libraries(3rdparty INTERFACE pthread)
    endif()
    target_include_directories(3rdparty INTERFACE "glfw/include/GLFW")
endif()

# unzip depend on zlib
if(AX_WITH_UNZIP)
    ax_add_3rd(unzip)
endif()

if(AX_WITH_CARES)
    ax_add_3rd(c-ares)
endif()

if(AX_WITH_LLHTTP)
    ax_add_3rd(llhttp)
endif()

# libvlc
if (AX_ENABLE_VLC_MEDIA AND (NOT _AX_HAVE_VLC))
    message(STATUS "Using axmol prebuilt libvlc")
    ax_add_3rd(vlc TARGETS VLC::vlc;VLC::vlccore)
endif()

# yaml
if(AX_WITH_YAML_CPP)
    _1kfetch(yaml-cpp)
    ax_add_3rd(${yaml-cpp_SOURCE_DIR} NO_LINK
      TARGETS yaml-cpp
      OPTIONS
      "YAML_CPP_BUILD_TESTS OFF"
      "YAML_CPP_BUILD_TOOLS OFF"
      "YAML_CPP_BUILD_CONTRIB OFF"
      "YAML_CPP_INSTALL OFF"
      "YAML_CPP_CLANG_FORMAT_EXE OFF"
    )
endif()

# kcp
if(AX_WITH_KCP)
    _1kfetch(kcp)
    ax_add_3rd(${kcp_SOURCE_DIR} TARGETS kcp)
endif()

if (WIN32)
    if (WINRT)
        ax_add_3rd(angle TARGETS angle::GLESv2;angle::EGL)
    else()
        add_subdirectory(angle)
    endif()
endif()

# yasio
ax_add_3rd(yasio)

# simdjson
ax_add_3rd(simdjson)

if (ANDROID)
    function(config_android_shared_libs package_name target_folder)
        string(REPLACE "." "/" package_path ${package_name})
        set(ANDROID_PACKAGE_NAME ${package_name})
        configure_file(${ANDROID_SHARED_LOAD_FILE_PATH} ${target_folder}/${package_path}/${ANDROID_SHARED_LOAD_FILE_NAME} @ONLY)
    endfunction()
endif()
