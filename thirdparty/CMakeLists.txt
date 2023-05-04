
project(thirdparty)

if (NOT DEFINED AX_USE_ALSOFT)
    if(NOT APPLE)
        set(AX_USE_ALSOFT ON)
    else()
        set(AX_USE_ALSOFT OFF) 
    endif()
endif()

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
option(AX_WITH_CURL "Build with internal curl support" ON)
option(AX_WITH_UNZIP "Build with internal unzip support" ON)
option(AX_WITH_ASTCENC "Build with internal ASTCENC support" ON)
option(AX_WITH_ALSOFT "Build with internal openal-soft support" ${AX_USE_ALSOFT})
option(AX_WITH_CARES "Build with internal c-ares support" OFF)
option(AX_WITH_LLHTTP "Build with lhttp support" ON)
option(AX_WITH_YAML_CPP "Build with yaml-cpp support" OFF)
option(AX_WITH_KCP "Build with internal kcp support" OFF)

# by default, enable ios,macOS openal-soft framework for legal license LGPL-2.1
option(ALSOFT_OSX_FRAMEWORK "" ON)

set(ANDROID_SHARED_LOADS "" CACHE INTERNAL "The android shared libraries load source code" )
set(ANDROID_SHARED_LOAD_FILE_NAME "SharedLoader.java" CACHE INTERNAL "The android shared load java file name" )
set(ANDROID_SHARED_LOAD_FILE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${ANDROID_SHARED_LOAD_FILE_NAME}.in" CACHE INTERNAL "The android shared libraries load config code file" )

# cmake_policy(SET CMP0079 NEW)
include(CheckIncludeFile)
include(CheckCCompilerFlag)
include(CheckCSourceCompiles)
include(CheckCXXSourceCompiles)

# Detetion SIMD instrinsics

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

if (NOT TVOS)
    # Checking intel SIMD Intrinsics
    if(APPLE)
        set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -mpopcnt")
    endif()
    check_c_source_compiles("#include <immintrin.h>
        #include <stdint.h>
        int main()
        {
            __m256 m = _mm256_set_ps(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f);
            return (int)*(float*)&m;
        }" AX_HAVE_AVX2_INTRINSICS)
    check_c_source_compiles("#include <nmmintrin.h>
        #include <stdint.h>
        int main()
        {
            uint32_t v = 0;
            return (int)_mm_popcnt_u32(v);
        }" AX_HAVE_SSE42_INTRINSICS)
    check_c_source_compiles("#include <smmintrin.h>
        #include <stdint.h>
        int main()
        {
            __m128i shuf = _mm_set_epi8(0,0,0,0, 0,0,0,0, 0,0,0,0, 12,8,4,0);
            return *(int*)&shuf;
        }" AX_HAVE_SSE41_INTRINSICS)

    if (NOT AX_HAVE_SSE2_INTRINSICS)
        check_c_source_compiles("#include <emmintrin.h>
            #include <stdint.h>
            int main()
            {
                __m128d m = _mm_set_sd(0.0);
                return (int)*(double*)&m;
            }" AX_HAVE_SSE2_INTRINSICS)
    endif()

    set(CMAKE_REQUIRED_FLAGS ${OLD_REQUIRED_FLAGS})

    ### Checking ARM SIMD neon
    check_include_file(arm_neon.h AX_HAVE_ARM_NEON_H)
    if(AX_HAVE_ARM_NEON_H)
        check_c_source_compiles("#include <arm_neon.h>
        int main()
        {
            int32x4_t ret4 = vdupq_n_s32(0);
            return vgetq_lane_s32(ret4, 0);
        }" AX_HAVE_NEON_INTRINSICS)
    endif()
else()
    message(AUTHOR_WARNING "Skipping AVX2/SSE4/NEON detection for astc-encoder when build target 'tvos'")
endif()

set(CMAKE_REQUIRED_FLAGS ${OLD_REQUIRED_FLAGS})
unset(OLD_REQUIRED_FLAGS)

### set AX_ISA_SIMD
if(AX_HAVE_AVX2_INTRINSICS)
    set(AX_ISA_SIMD "avx2")
elseif(AX_HAVE_SSE42_INTRINSICS)
    set(AX_ISA_SIMD "sse4.2")
elseif(AX_HAVE_SSE41_INTRINSICS)
    set(AX_ISA_SIMD "sse4.1")
elseif(AX_HAVE_SSE2_INTRINSICS)
    set(AX_ISA_SIMD "sse2")
elseif(AX_HAVE_NEON_INTRINSICS)
    set(AX_ISA_SIMD "neon")
else()
    set(AX_ISA_SIMD "none")
endif()

message(AUTHOR_WARNING "AX_ISA_SIMD=${AX_ISA_SIMD},AX_HAVE_AVX2_INTRINSICS=${AX_HAVE_AVX2_INTRINSICS},AX_HAVE_SSE42_INTRINSICS=${AX_HAVE_SSE42_INTRINSICS},AX_HAVE_SSE41_INTRINSICS=${AX_HAVE_SSE41_INTRINSICS},AX_HAVE_SSE2_INTRINSICS=${AX_HAVE_SSE2_INTRINSICS},AX_HAVE_NEON_INTRINSICS=${AX_HAVE_NEON_INTRINSICS}")

if (WINDOWS)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        if (AX_HAVE_SSE41_INTRINSICS)
            add_compile_options("-msse4.1")
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

add_library(thirdparty STATIC empty.cpp)

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
target_link_libraries(thirdparty fmt-header-only)

# bellow are non header only libs

# cpufeatures
if(ANDROID)
    add_subdirectory(android-specific/cpufeatures)
endif()

add_subdirectory(zlib)
target_link_libraries(thirdparty 
    dep_zlib
)

# libpng
add_subdirectory(png)
target_include_directories(png PRIVATE "zlib/include")
if(ANDROID)
    target_include_directories(png PRIVATE ${cpufeatures_include_dir})
    target_link_libraries(png INTERFACE cpufeatures)
endif()
target_link_libraries(thirdparty png)
configure_target_outdir(png)

set(PNG_PNG_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/png" CACHE STRING "png include dir" FORCE)
set(PNG_LIBRARY "png" CACHE STRING "png include dir" FORCE)

if(AX_WITH_BOX2D)
    set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "Build the Box2D unit tests" FORCE)
    set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "Build the Box2D testbed" FORCE)
    if(OPT_BOX2D_OPTIMIZED)
        add_subdirectory(box2d-optimized)
        configure_target_outdir(box2d-optimized)
        target_link_libraries(thirdparty box2d-optimized)
    else()
        add_subdirectory(box2d)
        configure_target_outdir(box2d)
        target_link_libraries(thirdparty box2d)
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
    target_link_libraries(thirdparty chipmunk)
    configure_target_outdir(chipmunk)
endif(AX_WITH_CHIPMUNK)
if(AX_WITH_FREETYPE)
    if (NOT LINUX)
        set(FT_WITH_ZLIB ON CACHE BOOL "Use system zlib instead of internal library." FORCE)
        set(DISABLE_FORCE_DEBUG_POSTFIX ON CACHE BOOL "" FORCE)
        set(SKIP_INSTALL_ALL TRUE CACHE BOOL "Skip freetype instal." FORCE)
        set(FT_DISABLE_HARFBUZZ TRUE CACHE BOOL "Disable HarfBuzz." FORCE)
        set(FT_DISABLE_BROTLI TRUE CACHE BOOL "Disable Brotli." FORCE)
        set(FT_DISABLE_BZIP2 TRUE CACHE BOOL "Disable bzip2." FORCE)
        add_subdirectory(freetype)
        target_link_libraries(thirdparty freetype)
        configure_target_outdir(freetype)
        if (WINRT)
            target_compile_definitions(freetype PUBLIC "generic=GenericFromFreeTypeLibrary")
        endif()
    endif()
    target_include_directories(thirdparty PUBLIC "freetype/include")
endif(AX_WITH_FREETYPE)
if(AX_WITH_RECAST)
    add_subdirectory(recast)
    target_link_libraries(thirdparty recast)
    configure_target_outdir(recast)
endif(AX_WITH_RECAST)
if(AX_WITH_BULLET)
    add_subdirectory(bullet)
    target_link_libraries(thirdparty bullet)
    configure_target_outdir(bullet)
endif(AX_WITH_BULLET)
if(AX_WITH_JPEG AND NOT WINRT)
    add_subdirectory(jpeg-turbo)
    target_link_libraries(thirdparty dep_jpeg-turbo)
endif()
if(AX_WITH_OPENSSL)
    add_subdirectory(openssl)
    if(ANDROID OR LINUX)
        target_link_libraries(OpenSSL::SSL INTERFACE OpenSSL::Crypto)
        target_link_libraries(thirdparty OpenSSL::SSL)
    else()
        target_link_libraries(thirdparty OpenSSL::Crypto OpenSSL::SSL)
    endif()
    target_compile_definitions(thirdparty PUBLIC OPENSSL_SUPPRESS_DEPRECATED=1)
endif(AX_WITH_OPENSSL)
if(AX_WITH_WEBP)
    add_subdirectory(webp)
    target_link_libraries(thirdparty webp)
    configure_target_outdir(webp)
endif(AX_WITH_WEBP)
if(AX_WITH_PUGIXML)
    add_subdirectory(pugixml)
    target_link_libraries(thirdparty pugixml)
    configure_target_outdir(pugixml)
endif(AX_WITH_PUGIXML)

add_subdirectory(xxhash)
target_link_libraries(thirdparty xxhash)
configure_target_outdir(xxhash)

add_subdirectory(lz4)
target_link_libraries(thirdparty lz4)
configure_target_outdir(lz4)

if(AX_WITH_CLIPPER2)
    add_subdirectory(clipper2)
    target_link_libraries(thirdparty clipper2)
    configure_target_outdir(clipper2)
endif(AX_WITH_CLIPPER2)
if(AX_WITH_CONVERTUTF)
    add_subdirectory(ConvertUTF)
    target_link_libraries(thirdparty ConvertUTF)
    configure_target_outdir(ConvertUTF)
endif(AX_WITH_CONVERTUTF)
if(AX_WITH_POLY2TRI)
    add_subdirectory(poly2tri)
    target_link_libraries(thirdparty poly2tri)  
    # Build static lib
    target_compile_definitions(poly2tri PUBLIC P2T_STATIC_EXPORTS)
    configure_target_outdir(poly2tri)
endif(AX_WITH_POLY2TRI)

if(AX_WITH_ASTCENC)
    add_subdirectory(astcenc)
    target_link_libraries(thirdparty astcenc)
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

if(AX_WITH_CURL)
    add_subdirectory(curl)
    if(ANDROID OR LINUX)
        target_link_libraries(libcurl INTERFACE OpenSSL::SSL)
    endif()
    target_link_libraries(thirdparty libcurl)
endif(AX_WITH_CURL)

# The openal-soft(LGPL 2.1)
if(AX_WITH_ALSOFT)
    set(ALSOFT_DLOPEN OFF CACHE BOOL "Check for the dlopen API for loading optional libs" FORCE)
    set(ALSOFT_UTILS OFF CACHE BOOL "Build utility program" FORCE)
    set(ALSOFT_EXAMPLES OFF CACHE BOOL "Build example programs" FORCE)
    set(ALSOFT_INSTALL OFF CACHE BOOL "Install main library" FORCE)
    if (ANDROID)
        set(ALSOFT_BACKEND_OPENSL OFF CACHE BOOL "Enable OpenSL backend" FORCE)
        set(OBOE_SOURCE "${CMAKE_CURRENT_LIST_DIR}/oboe" CACHE STRING "Source directory for Oboe." FORCE)
    endif()

    if(WINRT)
        set(ALSOFT_BACKEND_WASAPI OFF CACHE BOOL "Enable WASAPI backend" FORCE)
        set(ALSOFT_BACKEND_WAVE OFF CACHE BOOL "Enable Wave Writer backend" FORCE)
        set(ALSOFT_BACKEND_DSOUND OFF CACHE BOOL "Enable DirectSound backend" FORCE)
        set(ALSOFT_BACKEND_WINMM OFF CACHE BOOL "Enable Windows Multimedia backend" FORCE)
    endif()

    add_subdirectory(openal EXCLUDE_FROM_ALL)
    configure_target_outdir(common)
    
    configure_target_outdir(OpenAL)
    target_link_libraries(thirdparty
        OpenAL
    )
    target_compile_definitions(thirdparty PUBLIC AX_USE_ALSOFT=1)

    if(WINRT)
        target_include_directories(thirdparty PUBLIC openal-uwp/include)
    endif()
    
    if(ANDROID)
        set(ANDROID_SHARED_LOADS "${ANDROID_SHARED_LOADS}System.loadLibrary(\"openal\");" CACHE INTERNAL "Android Shared Loads" )
    endif()
endif()

# The ogg decoder
add_subdirectory(ogg)
target_link_libraries(thirdparty ogg)
configure_target_outdir(ogg)

if(WINDOWS OR LINUX OR (ANDROID AND AX_USE_GLAD))
    add_subdirectory(glad)
    target_link_libraries(thirdparty glad)
    configure_target_outdir(glad)
endif()

if((WINDOWS AND NOT WINRT) OR MACOSX OR LINUX)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "Build the GLFW documentation" FORCE)
    add_subdirectory(glfw)
    configure_target_outdir(glfw)
    set_target_properties(glfw PROPERTIES
                      OUTPUT_NAME glfw)
    target_link_libraries(thirdparty glfw)
    if(NOT WINDOWS)
        target_link_libraries(thirdparty pthread)
    endif()
    target_include_directories(thirdparty PUBLIC "glfw/include/GLFW")
endif()

# unzip depend on zlib
if(AX_WITH_UNZIP)
    add_subdirectory(unzip)
    target_link_libraries(thirdparty unzip)
    configure_target_outdir(unzip)
endif(AX_WITH_UNZIP)

if(AX_WITH_CARES)
    set(CARES_STATIC ON CACHE BOOL "Build c-ares as static library" FORCE)
    set(CARES_SHARED OFF CACHE BOOL "Build c-ares as shared library" FORCE)
    set(CARES_BUILD_TOOLS OFF CACHE BOOL "Build c-ares tools" FORCE)
    set(CARES_INSTALL OFF CACHE BOOL "Create installation targets (chain builders may want to disable this)" FORCE)
    add_subdirectory(c-ares)
    target_link_libraries(thirdparty c-ares)
    include_directories($<TARGET_FILE_DIR:c-ares>)
    include_directories("c-ares/include")
    configure_target_outdir(c-ares)
    target_compile_definitions(thirdparty PUBLIC YASIO_USE_CARES=1)
endif(AX_WITH_CARES)

if(AX_WITH_LLHTTP)
    add_subdirectory(llhttp)
    configure_target_outdir(llhttp)
    target_link_libraries(thirdparty llhttp)
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
    target_link_libraries(thirdparty kcp)
endif()

if (WINRT) 
    add_subdirectory(angle)
    target_link_libraries(thirdparty angle::GLESv2 angle::EGL)
endif()

# put "thirdparty" into thirdparty folder, too
configure_target_outdir(thirdparty)

if(MACOSX OR IOS)
    add_subdirectory(glsl-optimizer)
    target_link_libraries(thirdparty 
    dep_glcpp_library
    dep_glsl_optimizer
    dep_libmesa
    )
endif()

if (ANDROID)
    function(config_android_shared_libs package_name target_folder)
        string(REPLACE "." "/" package_path ${package_name})
        set(ANDROID_PACKAGE_NAME ${package_name})
        configure_file(${ANDROID_SHARED_LOAD_FILE_PATH} ${target_folder}/${package_path}/${ANDROID_SHARED_LOAD_FILE_NAME} @ONLY)
    endfunction()
endif()
