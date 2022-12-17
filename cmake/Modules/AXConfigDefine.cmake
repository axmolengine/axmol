include(CheckIncludeFile)
include(CheckCCompilerFlag)
include(CheckCSourceCompiles)
include(CheckCXXSourceCompiles)

If(APPLE)
    if(${CMAKE_VERSION} VERSION_LESS "3.14")
        message(FATAL_ERROR "Please use CMake 3.14 or newer for Apple platform (macOS, iOS, tvOS or watchOS)")
    endif()
endif()

 #Please use them everywhere
 #WINDOWS   =   Windows Desktop
 #ANDROID    =  Android
 #IOS    =  iOS
 #MACOSX    =  MacOS X
 #LINUX      =   Linux
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(WINDOWS TRUE)
    if(NOT ("${CMAKE_GENERATOR_PLATFORM}" STREQUAL "Win32"))
        set(WIN64 TRUE)
        set(ARCH_ALIAS "x64")
     else()
        set(WIN32 TRUE)
        set(ARCH_ALIAS "x86")
     endif()
    set(PLATFORM_FOLDER win32)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Android")
    set(PLATFORM_FOLDER android)
    set(ARCH_ALIAS ${ANDROID_ABI})
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    if(ANDROID)
        set(PLATFORM_FOLDER android)
    else()
        set(LINUX TRUE)
        set(PLATFORM_FOLDER linux)
    endif()
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(APPLE TRUE)
    set(MACOSX TRUE)
    set(PLATFORM_FOLDER mac)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "iOS")
    set(APPLE TRUE)
    set(IOS TRUE)
    set(PLATFORM_FOLDER ios)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "tvOS")
    set(APPLE TRUE)
    set(IOS TRUE)
    set(TVOS TRUE)
    set(PLATFORM_FOLDER tvos)
else()
    message(FATAL_ERROR "Unsupported platform, CMake will exit")
    return()
endif()

# generators that are capable of organizing into a hierarchy of folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
# simplify generator condition, please use them everywhere
if(CMAKE_GENERATOR STREQUAL Xcode)
    set(XCODE TRUE)
elseif(CMAKE_GENERATOR MATCHES Visual)
    set(VS TRUE)
endif()
message(STATUS "CMAKE_GENERATOR: ${CMAKE_GENERATOR}")

# custom target property for lua/js link
define_property(TARGET
    PROPERTY AX_LUA_DEPEND
    BRIEF_DOCS "axmol lua depend libs"
    FULL_DOCS "use to save depend libs of axmol lua project"
)

# config c standard
if (NOT WINDOWS)
    if(NOT DEFINED C_STD)
        set(C_STD 11)
    endif()
    message(STATUS "C_STD=${C_STD}")
    set(CMAKE_C_STANDARD ${C_STD})
    set(CMAKE_C_STANDARD_REQUIRED ON)
endif()

# config c++ standard
if(NOT DEFINED CXX_STD)
    set(CXX_STD 20)
endif()
message(STATUS "CXX_STD=${CXX_STD}")
set(CMAKE_CXX_STANDARD ${CXX_STD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/GF)
endif()

set(CMAKE_DEBUG_POSTFIX "" CACHE STRING "Library postfix for debug builds. Normally left blank." FORCE)

# set hash style to both for android old device compatible
# see also: https://github.com/axmolengine/axmol/discussions/614
if (ANDROID)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--hash-style=both")
endif()

# check visual studio version
if(WINDOWS)
    # not support other compile tools except MSVC for now
    if(MSVC)
        # Visual Studio 2015, MSVC_VERSION 1900      (v140 toolset)
        # Visual Studio 2017, MSVC_VERSION 1910-1919 (v141 toolset)
        if(${MSVC_VERSION} EQUAL 1900 OR ${MSVC_VERSION} GREATER 1900)
            message(STATUS "using Windows MSVC generate axmol project, MSVC_VERSION:${MSVC_VERSION}")
        else()
            message(FATAL_ERROR "using Windows MSVC generate axmol project, MSVC_VERSION:${MSVC_VERSION} lower than needed")
        endif()
    else()
        message(FATAL_ERROR "please using Windows MSVC compile axmol project, support other compile tools not yet")
    endif()
endif()

# Set macro definitions for special platforms
function(use_ax_compile_define target)
    target_compile_definitions(${target} PUBLIC $<$<CONFIG:Debug>:_AX_DEBUG=1>)

    # !important axmol not use double precision
    # target_compile_definitions(${target} PUBLIC CP_USE_CGTYPES=0)
    # target_compile_definitions(${target} PUBLIC CP_USE_DOUBLES=0)

    if(APPLE)
        target_compile_definitions(${target} PUBLIC __APPLE__)
        target_compile_definitions(${target} PUBLIC USE_FILE32API)
        if(AX_USE_COMPAT_GL)
            target_compile_definitions(${target}
                PUBLIC AX_USE_COMPAT_GL=1
                PUBLIC GL_SILENCE_DEPRECATION=1
            )
        endif()

        if(IOS)
            if(TVOS)
                target_compile_definitions(${target} PUBLIC AX_TARGET_OS_TVOS)
            else()
                target_compile_definitions(${target} PUBLIC AX_TARGET_OS_IPHONE)
            endif()
        endif()

    elseif(LINUX)
        target_compile_definitions(${target} PUBLIC _GNU_SOURCE)
    elseif(ANDROID)
        target_compile_definitions(${target} PUBLIC USE_FILE32API)
    elseif(WINDOWS)
        if(AX_USE_COMPAT_GL)
            target_compile_definitions(${target} PUBLIC AX_USE_COMPAT_GL=1)
        endif()
        target_compile_definitions(${target}
            PUBLIC WIN32
            PUBLIC _WIN32
            PUBLIC _WINDOWS
            PUBLIC UNICODE
            PUBLIC _UNICODE
            PUBLIC _CRT_SECURE_NO_WARNINGS
            PUBLIC _SCL_SECURE_NO_WARNINGS
            # PUBLIC GLAD_GLAPI_EXPORT
        )
        if(BUILD_SHARED_LIBS)
            target_compile_definitions(${target}
                PRIVATE _USRDLL
                PRIVATE _USEGUIDLL # ui
            )
        else()
            target_compile_definitions(${target} PUBLIC AX_STATIC)
        endif()
    endif()
endfunction()

# Set compiler options
function(use_ax_compile_options target)
    if(MSVC)
        target_compile_options(${target}
            PUBLIC /MP
        )
    endif()
endfunction()

# softfp for android armv7a?
# if(ANDROID)
# 	if(${ANDROID_ABI} STREQUAL "armeabi-v7a")
#         set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfloat-abi=softfp")
#         set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfloat-abi=softfp")
# 	endif()
# endif()

# Try enable asm & nasm compiler support
set(can_use_assembler TRUE)
enable_language(ASM)
enable_language(ASM_NASM OPTIONAL)
message(STATUS "The nasm compiler speed up libraries: jpeg(libjpeg-turbo)")

if(NOT EXISTS "${CMAKE_ASM_NASM_COMPILER}")
   set(CMAKE_ASM_NASM_COMPILER_LOADED FALSE CACHE BOOL "Does cmake asm nasm compiler loaded" FORCE)
   message(WARNING "The nasm compiler doesn't present on your system PATH, please download from: https://www.nasm.us/pub/nasm/releasebuilds/2.15.05/")
endif()

# Detetion SIMD instrinsics
if (NOT DEFINED AX_ISA_SIMD)
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
    set(CMAKE_REQUIRED_FLAGS ${OLD_REQUIRED_FLAGS})
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
            set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -std=c++11")
            check_cxx_source_compiles("#include <arm_neon.h>
                    int main()
                    {
                        int32x4_t ret4 = vdupq_n_s32(0);
                        uint32x4_t v{};
                        float16x4_t f16 = vcvt_f16_f32(v);
                        return vgetq_lane_s32(ret4, 0);
                    }" AX_HAVE_NEON_INTRINSICS)
            set(CMAKE_REQUIRED_FLAGS ${OLD_REQUIRED_FLAGS})
        endif()
    else()
        message(AUTHOR_WARNING "Skipping AVX2/SSE4/NEON detection for astc-encoder when build target 'tvos'")
    endif()

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
endif()
