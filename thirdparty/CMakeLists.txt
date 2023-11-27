
project(thirdparty)

option(OPT_BOX2D_OPTIMIZED "" OFF)
option(AX_WITH_BOX2D "Build with internal Box2D support" ON)
option(AX_WITH_CHIPMUNK "Build with internal chipmunk support" ON)
option(AX_WITH_FREETYPE "Build with internal freetype support" ON)
option(AX_WITH_RECAST "Build with internal recast support" ON)
option(AX_WITH_BULLET "Build with internal bullet support" ON)
option(AX_WITH_JPEG "Build with internal jpeg support" ON)
option(AX_WITH_OPENSSL "Build with internal openssl support" ON)
option(AX_WITH_WEBP "Build with internal webp support" ON)
option(AX_WITH_PUGIXML "Build with internal pugixml support" ON)
option(AX_WITH_CLIPPER2 "Build with internal Clipper2 support" ON)
option(AX_WITH_CONVERTUTF "Build with internal ConvertUTF support" ON)
option(AX_WITH_POLY2TRI "Build with internal poly2tri support" ON)
option(AX_WITH_ZLIB "Build with internal zlib support" ON)
option(AX_WITH_FASTLZ "Build with internal fastlz support" ON)
option(AX_WITH_LZ4 "Build with internal lz4 support" OFF)
option(AX_WITH_CURL "Build with internal curl support" ON)
option(AX_WITH_UNZIP "Build with internal unzip support" ON)
option(AX_WITH_ASTCENC "Build with internal ASTCENC support" ON)
option(AX_WITH_CARES "Build with internal c-ares support" OFF)
option(AX_WITH_LLHTTP "Build with lhttp support" ON)
option(AX_WITH_YAML_CPP "Build with yaml-cpp support" OFF)
option(AX_WITH_KCP "Build with internal kcp support" OFF)
option(AX_WITH_OBOE "Build with oboe support" OFF)

# by default, enable ios,macOS openal-soft framework for legal license LGPL-2.1
option(ALSOFT_OSX_FRAMEWORK "" ON)

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

function(configure_target_outdir target)
    set_target_properties(${target} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        FOLDER "ThirdParty"
    )
endfunction()

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

add_library(thirdparty INTERFACE)

macro(ax_add_3rd target_name)
    target_link_libraries(thirdparty INTERFACE ${target_name})
endmacro()

# bellow are header only libs
target_include_directories(thirdparty
    INTERFACE "robin-map/include"
)

if (ANDROID)
    target_include_directories(thirdparty
        INTERFACE "jni.hpp/include"
    )
endif()

add_subdirectory(fmt EXCLUDE_FROM_ALL)

ax_add_3rd(fmt-header-only)
set_target_properties(fmt PROPERTIES
    FOLDER "ThirdParty"
)

# bellow are non header only libs

# cpufeatures
if(ANDROID)
    add_subdirectory(android-specific/cpufeatures)
endif()

if(NOT EMSCRIPTEN)
    add_subdirectory(zlib)
    ax_add_3rd(dep_zlib)
endif(NOT EMSCRIPTEN)

# libpng
add_subdirectory(png)
target_include_directories(png PRIVATE "zlib/include")
if(ANDROID)
    target_include_directories(png PRIVATE ${cpufeatures_include_dir})
    target_link_libraries(png INTERFACE cpufeatures)
endif()
ax_add_3rd(png)
configure_target_outdir(png)

set(PNG_PNG_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/png" CACHE STRING "png include dir" FORCE)
set(PNG_LIBRARY "png" CACHE STRING "png include dir" FORCE)

if(AX_WITH_BOX2D)
    set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "Build the Box2D unit tests" FORCE)
    set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "Build the Box2D testbed" FORCE)
    if(OPT_BOX2D_OPTIMIZED)
        add_subdirectory(box2d-optimized)
        configure_target_outdir(box2d-optimized)
        ax_add_3rd(box2d-optimized)
    else()
        add_subdirectory(box2d)
        configure_target_outdir(box2d)
        ax_add_3rd(box2d)
    endif()
endif(AX_WITH_BOX2D)

if(AX_WITH_CHIPMUNK)
    set(CP_BUILD_SHARED OFF CACHE BOOL "Build chipmunk as shared library" FORCE)
    set(CP_BUILD_STATIC ON CACHE BOOL "Build chipmunk as static library" FORCE)
    set(CP_BUILD_DEMOS OFF CACHE BOOL "Build chipmunk demos" FORCE)
    set(CP_INSTALL_STATIC OFF CACHE BOOL "Install chipmunk static" FORCE)
    add_subdirectory(chipmunk)
    set_target_properties(chipmunk PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/chipmunk/include"
    )

    # !important axmol not use double precision
    target_compile_definitions(chipmunk PUBLIC CP_USE_CGTYPES=0)
    target_compile_definitions(chipmunk PUBLIC CP_USE_DOUBLES=0)

    #~ include_directories("${CMAKE_CURRENT_SOURCE_DIR}/chipmunk/include")
    ax_add_3rd(chipmunk)
    configure_target_outdir(chipmunk)
endif(AX_WITH_CHIPMUNK)
if(AX_WITH_FREETYPE)
    set(FT_WITH_ZLIB ON CACHE BOOL "Use system zlib instead of internal library." FORCE)
    set(DISABLE_FORCE_DEBUG_POSTFIX ON CACHE BOOL "" FORCE)
    set(SKIP_INSTALL_ALL TRUE CACHE BOOL "Skip freetype instal." FORCE)
    set(FT_DISABLE_HARFBUZZ TRUE CACHE BOOL "Disable HarfBuzz." FORCE)
    set(FT_DISABLE_BROTLI TRUE CACHE BOOL "Disable Brotli." FORCE)
    set(FT_DISABLE_BZIP2 TRUE CACHE BOOL "Disable bzip2." FORCE)
    add_subdirectory(freetype)
    ax_add_3rd(freetype)
    configure_target_outdir(freetype)
    if (WINRT)
        target_compile_definitions(freetype PUBLIC "generic=GenericFromFreeTypeLibrary")
    endif()
    target_include_directories(thirdparty INTERFACE "freetype/include")
endif()
if(AX_WITH_RECAST)
    add_subdirectory(recast)
    ax_add_3rd(recast)
    configure_target_outdir(recast)
endif(AX_WITH_RECAST)
if(AX_WITH_BULLET)
    add_subdirectory(bullet)
    ax_add_3rd(bullet)
    configure_target_outdir(bullet)
endif(AX_WITH_BULLET)

if(AX_WITH_JPEG AND NOT WINRT AND NOT EMSCRIPTEN)
    add_subdirectory(jpeg-turbo)
    ax_add_3rd(dep_jpeg-turbo)
endif()

if(AX_WITH_OPENSSL)
    add_subdirectory(openssl)
    if(ANDROID OR LINUX)
        target_link_libraries(OpenSSL::SSL INTERFACE OpenSSL::Crypto)
        ax_add_3rd(OpenSSL::SSL)
    else()
        target_link_libraries(thirdparty
                           INTERFACE OpenSSL::Crypto
                           INTERFACE OpenSSL::SSL)
    endif()
    target_compile_definitions(thirdparty INTERFACE OPENSSL_SUPPRESS_DEPRECATED=1)
endif()

if(AX_WITH_WEBP)
    add_subdirectory(webp)
    ax_add_3rd(webp)
    configure_target_outdir(webp)
endif(AX_WITH_WEBP)

if(AX_WITH_PUGIXML)
    add_subdirectory(pugixml)
    ax_add_3rd(pugixml)
    configure_target_outdir(pugixml)
endif(AX_WITH_PUGIXML)

add_subdirectory(xxhash)
ax_add_3rd(xxhash)
configure_target_outdir(xxhash)

if (AX_WITH_FASTLZ)
    add_subdirectory(fastlz)
    ax_add_3rd(fastlz)
    configure_target_outdir(fastlz)
endif()

if(AX_WITH_LZ4)
    add_subdirectory(lz4)
    ax_add_3rd(lz4)
    configure_target_outdir(lz4)
endif()

if(AX_WITH_CLIPPER2)
    add_subdirectory(clipper2)
    ax_add_3rd(clipper2)
    configure_target_outdir(clipper2)
endif(AX_WITH_CLIPPER2)
if(AX_WITH_CONVERTUTF)
    add_subdirectory(ConvertUTF)
    ax_add_3rd(ConvertUTF)
    configure_target_outdir(ConvertUTF)
endif(AX_WITH_CONVERTUTF)
if(AX_WITH_POLY2TRI)
    add_subdirectory(poly2tri)
    ax_add_3rd(poly2tri)
    # Build static lib
    target_compile_definitions(poly2tri PUBLIC P2T_STATIC_EXPORTS)
    configure_target_outdir(poly2tri)
endif(AX_WITH_POLY2TRI)

if(AX_WITH_ASTCENC)
    add_subdirectory(astcenc)
    ax_add_3rd(astcenc)
    configure_target_outdir(astcenc)
endif(AX_WITH_ASTCENC)

# use lua/js specific libs by property to prevent conflict
if(AX_ENABLE_EXT_LUA)
    add_subdirectory(lua/${AX_LUA_ENGINE})
    add_subdirectory(lua/tolua)
    add_subdirectory(lua/lua-cjson)
    set_property(TARGET thirdparty APPEND PROPERTY
        AX_LUA_DEPEND ${AX_LUA_ENGINE} tolua lua-cjson
    )
    if (AX_USE_LUAJIT)
        target_compile_definitions(lua-cjson PRIVATE USING_LUAJIT=1)
    endif()
endif()

if(AX_WITH_CURL AND NOT EMSCRIPTEN)
    add_subdirectory(curl)
    if(ANDROID OR LINUX)
        target_link_libraries(libcurl INTERFACE OpenSSL::SSL)
    endif()
    ax_add_3rd(libcurl)
endif()

# The openal-soft(LGPL 2.1)
if(AX_USE_ALSOFT AND NOT EMSCRIPTEN)
    set(ALSOFT_DLOPEN OFF CACHE BOOL "Check for the dlopen API for loading optional libs" FORCE)
    set(ALSOFT_UTILS OFF CACHE BOOL "Build utility program" FORCE)
    set(ALSOFT_EXAMPLES OFF CACHE BOOL "Build example programs" FORCE)
    set(ALSOFT_INSTALL OFF CACHE BOOL "Install main library" FORCE)
    if (ANDROID)
        if(AX_WITH_OBOE)
            set(ALSOFT_BACKEND_OPENSL OFF CACHE BOOL "Enable OpenSL backend" FORCE)
            set(OBOE_SOURCE "${CMAKE_CURRENT_LIST_DIR}/oboe" CACHE STRING "Source directory for Oboe." FORCE)
        endif()
        set(ALSOFT_RTKIT OFF CACHE BOOL "Enable RTKit support" FORCE)
    endif()

    if(WINDOWS)
        set(ALSOFT_BACKEND_WASAPI ON CACHE BOOL "Enable WASAPI backend" FORCE)
        set(ALSOFT_BACKEND_WAVE OFF CACHE BOOL "Enable Wave Writer backend" FORCE)
        set(ALSOFT_BACKEND_DSOUND OFF CACHE BOOL "Enable DirectSound backend" FORCE)
        set(ALSOFT_BACKEND_WINMM OFF CACHE BOOL "Enable Windows Multimedia backend" FORCE)
    endif()

    target_include_directories(thirdparty INTERFACE openal)

    add_subdirectory(openal EXCLUDE_FROM_ALL)
    configure_target_outdir(alcommon)
    configure_target_outdir(OpenAL)
    ax_add_3rd(OpenAL)
    target_compile_definitions(thirdparty INTERFACE AX_USE_ALSOFT=1)

    set_target_properties(alcommon PROPERTIES CXX_STANDARD ${_AX_CXX_STD})
    set_target_properties(OpenAL PROPERTIES CXX_STANDARD ${_AX_CXX_STD})

    if(ANDROID)
        set(ANDROID_SHARED_LOADS "${ANDROID_SHARED_LOADS}System.loadLibrary(\"openal\");" CACHE INTERNAL "Android Shared Loads" )
    endif()
endif()

# The ogg decoder
add_subdirectory(ogg)
ax_add_3rd(ogg)
configure_target_outdir(ogg)

if(WINDOWS OR LINUX OR ANDROID OR WASM)
    add_subdirectory(glad)
    ax_add_3rd(glad)
    configure_target_outdir(glad)
endif()

if((WINDOWS AND NOT WINRT) OR MACOSX OR LINUX)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "Build the GLFW documentation" FORCE)
    add_subdirectory(glfw)
    configure_target_outdir(glfw)
    set_target_properties(glfw PROPERTIES
                      OUTPUT_NAME glfw)
    ax_add_3rd(glfw)
    if(NOT WINDOWS)
        ax_add_3rd(pthread)
    endif()
    target_include_directories(thirdparty INTERFACE "glfw/include/GLFW")
endif()

# unzip depend on zlib
if(AX_WITH_UNZIP)
    add_subdirectory(unzip)
    ax_add_3rd(unzip)
    configure_target_outdir(unzip)
endif(AX_WITH_UNZIP)

if(AX_WITH_CARES)
    add_subdirectory(c-ares)
    ax_add_3rd(c-ares)
    target_compile_definitions(thirdparty INTERFACE YASIO_USE_CARES=1)
endif(AX_WITH_CARES)

if(AX_WITH_LLHTTP)
    add_subdirectory(llhttp)
    configure_target_outdir(llhttp)
    ax_add_3rd(llhttp)
endif()

# libvlc
if (AX_ENABLE_VLC_MEDIA AND (NOT _AX_HAVE_VLC))
    message(STATUS "Using axmol prebuilt libvlc")
    add_subdirectory(vlc)
    ax_add_3rd(VLC::vlc)
    ax_add_3rd(VLC::vlccore)
endif()

# yaml
if(AX_WITH_YAML_CPP)
    set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
    set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "" FORCE)
    set(YAML_CPP_INSTALL OFF CACHE BOOL "" FORCE)
    set(YAML_CPP_CLANG_FORMAT_EXE OFF CACHE BOOL "" FORCE)
    # set(YAML_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    add_subdirectory(yaml-cpp)
    configure_target_outdir(yaml-cpp)
    add_dependencies(thirdparty yaml-cpp)
endif()

# kcp
if(AX_WITH_KCP)
    add_subdirectory(kcp)
    configure_target_outdir(kcp)
    ax_add_3rd(kcp)
endif()

if (WINRT)
    add_subdirectory(angle)
    ax_add_3rd(angle::GLESv2)
    ax_add_3rd(angle::EGL)
endif()

add_subdirectory(simdjson)
configure_target_outdir(simdjson)
ax_add_3rd(simdjson)

# put "thirdparty" into thirdparty folder, too
configure_target_outdir(thirdparty)

if (ANDROID)
    function(config_android_shared_libs package_name target_folder)
        string(REPLACE "." "/" package_path ${package_name})
        set(ANDROID_PACKAGE_NAME ${package_name})
        configure_file(${ANDROID_SHARED_LOAD_FILE_PATH} ${target_folder}/${package_path}/${ANDROID_SHARED_LOAD_FILE_NAME} @ONLY)
    endfunction()
endif()
