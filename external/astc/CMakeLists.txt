
set(lib_name astc)
set(target_name ${lib_name})

project(${lib_name})

include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)
include(CheckCSourceCompiles)
include(CheckCXXSourceCompiles)

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
    astcenc_platform_isa_detection.cpp
    astcenc_quantization.cpp
    astcenc_symbolic_physical.cpp
    astcenc_weight_align.cpp
    astcenc_weight_quant_xfer_tables.cpp
)

add_library(${target_name} STATIC
  ${${target_name}_src}
)

target_compile_definitions(${target_name}
            PRIVATE ASTCENC_DECOMPRESS_ONLY)
            
if (NOT DEFINED ISA_SIMD)
    set(ISA_SIMD "none")

    include(CheckTypeSize)
    CHECK_TYPE_SIZE (void* VOIDP_SIZE) #4 if 32bit, 8 if 64bit

    message(STATUS "VOIDP_SIZE=${VOIDP_SIZE}")

    # sse4.1 x64
    check_include_file(smmintrin.h HAVE_SMMINTRIN_H)

    check_c_compiler_flag(-msse2 HAVE_MSSE2_SWITCH)
    if(HAVE_MSSE2_SWITCH)
        set(ISA_SIMD "sse2")
    endif()

    if(HAVE_SMMINTRIN_H AND (VOIDP_SIZE EQUAL 8))
        message(STATUS "Enabling x64 sse4.1 intrinsics...")
        set(ISA_SIMD "sse4.1")
    endif()

    check_include_file(arm_neon.h HAVE_ARM_NEON_H)
    if(HAVE_ARM_NEON_H)
        check_c_source_compiles("#include <arm_neon.h>
                int main()
                {
                    int32x4_t ret4 = vdupq_n_s32(0);
                    return vgetq_lane_s32(ret4, 0);
                }" HAVE_NEON_INTRINSICS)

        
        if(HAVE_NEON_INTRINSICS AND (VOIDP_SIZE EQUAL 8))
            message(STATUS "Enabling arm64 neon intrinsics...")
            set(ISA_SIMD "neon")
        endif()
    endif()
endif()

# Set up configuration for SIMD ISA builds
if(${ISA_SIMD} MATCHES "none")
    target_compile_definitions(${target_name}
        PRIVATE
            ASTCENC_NEON=0
            ASTCENC_SSE=0
            ASTCENC_AVX=0
            ASTCENC_POPCNT=0
            ASTCENC_F16C=0)

elseif(${ISA_SIMD} MATCHES "neon")
    target_compile_definitions(${target_name}
        PRIVATE
            ASTCENC_NEON=1
            ASTCENC_SSE=0
            ASTCENC_AVX=0
            ASTCENC_POPCNT=0
            ASTCENC_F16C=0)

elseif(${ISA_SIMD} MATCHES "sse2")
    target_compile_definitions(${target_name}
        PRIVATE
            ASTCENC_NEON=0
            ASTCENC_SSE=20
            ASTCENC_AVX=0
            ASTCENC_POPCNT=0
            ASTCENC_F16C=0)

elseif(${ISA_SIMD} MATCHES "sse4.1")
    target_compile_definitions(${target_name}
        PRIVATE
            ASTCENC_NEON=0
            ASTCENC_SSE=41
            ASTCENC_AVX=0
            ASTCENC_POPCNT=1
            ASTCENC_F16C=0)

    target_compile_options(${target_name}
        PRIVATE
            $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-msse4.1 -mpopcnt>)

elseif(${ISA_SIMD} MATCHES "avx2")
    target_compile_definitions(${target_name}
        PRIVATE
            ASTCENC_NEON=0
            ASTCENC_SSE=41
            ASTCENC_AVX=2
            ASTCENC_POPCNT=1
            ASTCENC_F16C=1)

    target_compile_options(${target_name}
        PRIVATE
            $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-mavx2 -mpopcnt -mf16c>
            $<$<CXX_COMPILER_ID:MSVC>:/arch:AVX2>)
endif()

target_include_directories(${target_name} PUBLIC ..)

set_target_properties(${target_name}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    FOLDER "External"
)

