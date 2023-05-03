## astc-encoder CMakeLists.txt: auto detect SIMD Intrinsics
set(lib_name astcenc)
set(target_name ${lib_name})

project(${lib_name})

# include(CheckIncludeFile)
# include(CheckCCompilerFlag)
# include(CheckCSourceCompiles)
# include(CheckCXXSourceCompiles)

include(AXPlatform)

set(${target_name}_src
    astcenc_averages_and_directions.cpp
    astcenc_block_sizes.cpp
    astcenc_color_quantize.cpp
    astcenc_color_unquantize.cpp
    astcenc_compress_symbolic.cpp
    astcenc_compute_variance.cpp
    astcenc_decompress_symbolic.cpp
    astcenc_diagnostic_trace.cpp
    astcenc_entry.cpp
    astcenc_find_best_partitioning.cpp
    astcenc_ideal_endpoints_and_weights.cpp
    astcenc_image.cpp
    astcenc_integer_sequence.cpp
    astcenc_mathlib.cpp
    astcenc_mathlib_softfloat.cpp
    astcenc_partition_tables.cpp
    astcenc_percentile_tables.cpp
    astcenc_pick_best_endpoint_format.cpp
    astcenc_quantization.cpp
    astcenc_symbolic_physical.cpp
    astcenc_weight_align.cpp
    astcenc_weight_quant_xfer_tables.cpp
)

add_library(${target_name} STATIC
  ${${target_name}_src}
)

# target_compile_definitions(${target_name}
#            PUBLIC ASTCENC_DECOMPRESS_ONLY)
if (NOT DEFINED ASTC_ISA_SIMD)
    if (NOT (ARCH_ALIAS STREQUAL "x86") AND NOT TVOS)
        set(ASTC_HAVE_AVX2_INTRINSICS ${AX_HAVE_AVX2_INTRINSICS})
        set(ASTC_HAVE_SSE42_INTRINSICS ${AX_HAVE_SSE42_INTRINSICS})
        set(ASTC_HAVE_SSE41_INTRINSICS ${AX_HAVE_SSE41_INTRINSICS})
        if (AX_HAVE_NEON_INTRINSICS)
            set(OLD_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS})
            set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -std=c++11")
            check_cxx_source_compiles("#include <arm_neon.h>
                    int main()
                    {
                        int32x4_t ret4 = vdupq_n_s32(0);
                        uint32x4_t v{};
                        float16x4_t f16 = vcvt_f16_f32(v);
                        return vgetq_lane_s32(ret4, 0);
                    }" ASTC_HAVE_NEON_INTRINSICS)
            set(CMAKE_REQUIRED_FLAGS ${OLD_REQUIRED_FLAGS})
            unset(OLD_REQUIRED_FLAGS)
        endif()
    else()
        set(ASTC_HAVE_SSE2_INTRINSICS ${AX_HAVE_SSE2_INTRINSICS})
        message(AUTHOR_WARNING "Skipping AVX2/SSE4/NEON detection for astc-encoder when build target 'x86' and 'tvos'")
    endif()

    ### set ASTC_ISA_SIMD
    if(ASTC_HAVE_AVX2_INTRINSICS)
        set(ASTC_ISA_SIMD "avx2")
    elseif(ASTC_HAVE_SSE42_INTRINSICS)
        set(ASTC_ISA_SIMD "sse4.2")
    elseif(ASTC_HAVE_SSE41_INTRINSICS)
        set(ASTC_ISA_SIMD "sse4.1")
    elseif(ASTC_HAVE_SSE2_INTRINSICS)
        set(ASTC_ISA_SIMD "sse2")
    elseif(ASTC_HAVE_NEON_INTRINSICS)
        set(ASTC_ISA_SIMD "neon")
    else()
        set(ASTC_ISA_SIMD "none")
    endif()

    message(AUTHOR_WARNING "ASTC_ISA_SIMD=${ASTC_ISA_SIMD}")
endif()

# Set up configuration for SIMD ISA builds
if(${ASTC_ISA_SIMD} MATCHES "none")
    target_compile_definitions(${target_name}
        PUBLIC
            ASTCENC_NEON=0
            ASTCENC_SSE=0
            ASTCENC_AVX=0
            ASTCENC_POPCNT=0
            ASTCENC_F16C=0)
elseif(${ASTC_ISA_SIMD} MATCHES "neon")
    target_compile_definitions(${target_name}
        PUBLIC
            ASTCENC_NEON=1
            ASTCENC_SSE=0
            ASTCENC_AVX=0
            ASTCENC_POPCNT=0
            ASTCENC_F16C=0)
elseif(${ASTC_ISA_SIMD} MATCHES "avx2")
    target_compile_definitions(${target_name}
        PUBLIC
            ASTCENC_NEON=0
            ASTCENC_SSE=42
            ASTCENC_AVX=2
            ASTCENC_POPCNT=1
            ASTCENC_F16C=1)

    target_compile_options(${target_name}
        PRIVATE
            $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-mavx2 -mpopcnt -mf16c>
            $<$<CXX_COMPILER_ID:MSVC>:/arch:AVX2>)
elseif(${ASTC_ISA_SIMD} MATCHES "sse4.2")
    target_compile_definitions(${target_name}
        PUBLIC
            ASTCENC_NEON=0
            ASTCENC_SSE=42
            ASTCENC_AVX=0
            ASTCENC_POPCNT=1
            ASTCENC_F16C=0)

    target_compile_options(${target_name}
        PRIVATE
            $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-msse4.2 -mpopcnt>)
elseif(${ASTC_ISA_SIMD} MATCHES "sse4.1")
    target_compile_definitions(${target_name}
        PUBLIC
            ASTCENC_NEON=0
            ASTCENC_SSE=41
            ASTCENC_AVX=0
            ASTCENC_POPCNT=1
            ASTCENC_F16C=0)

    target_compile_options(${target_name}
        PRIVATE
            $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-msse4.1 -mpopcnt>)
elseif(${ASTC_ISA_SIMD} MATCHES "sse2")
    target_compile_definitions(${target_name}
        PUBLIC
            ASTCENC_NEON=0
            ASTCENC_SSE=20
            ASTCENC_AVX=0
            ASTCENC_POPCNT=0
            ASTCENC_F16C=0)
endif()

target_include_directories(${target_name} PUBLIC ..)
