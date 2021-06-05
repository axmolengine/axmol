
set(lib_name astc)
set(target_name ${lib_name})

project(${lib_name})

include(CheckIncludeFile)
include(CheckCCompilerFlag)
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
    ### check -msse2 flag
    set(ASTC_SSE2_SWITCH "")

    set(OLD_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS})
    if(MSVC)
        set(CMAKE_REQUIRED_FLAGS "${OLD_REQUIRED_FLAGS} /WX")
        check_c_compiler_flag("/arch:SSE2" ASTC_HAVE_MSSE2_SWITCH)  
    else()
        set(CMAKE_REQUIRED_FLAGS "${OLD_REQUIRED_FLAGS} -Werror")
        check_c_compiler_flag(-msse2 ASTC_HAVE_MSSE2_SWITCH)
    endif()
    ### end check -msse2 flag

    ### check SSE4.1
    check_include_file(smmintrin.h ASTC_HAVE_SMMINTRIN_H)
    if(ASTC_HAVE_SMMINTRIN_H)
        if(APPLE)
            set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -mpopcnt")
        endif()
        check_c_source_compiles("#include <immintrin.h>
                int main()
                {
                    uint64_t v = 0;
                    return (int)_mm_popcnt_u64(v);
                }" ASTC_HAVE_SSE4_INTRINSICS)
    endif()

    ### restore CMAKE_REQUIRED_FLAGS
    set(CMAKE_REQUIRED_FLAGS ${OLD_REQUIRED_FLAGS})
    unset(OLD_REQUIRED_FLAGS)

    ### check neon
    check_include_file(arm_neon.h ASTC_HAVE_ARM_NEON_H)
    if(ASTC_HAVE_ARM_NEON_H)
        check_cxx_source_compiles("#include <arm_neon.h>
                int main()
                {
                    int32x4_t ret4 = vdupq_n_s32(0);
                    uint32x4_t v{};
                    float16x4_t f16 = vcvt_f16_f32(v);
                    return vgetq_lane_s32(ret4, 0);
                }" ASTC_HAVE_NEON_INTRINSICS)
    endif()

    ### set ISA_SIMD
    if(ASTC_HAVE_MSSE2_SWITCH)
        set(ISA_SIMD "sse2")
        if(ASTC_HAVE_SSE4_INTRINSICS)
            set(ISA_SIMD "sse4.1")
        endif()
    elseif(ASTC_HAVE_NEON_INTRINSICS)
        set(ISA_SIMD "neon")
    else()
        set(ISA_SIMD "none")
    endif()

    message(AUTHOR_WARNING "ISA_SIMD=${ISA_SIMD},ASTC_HAVE_MSSE2_SWITCH=${ASTC_HAVE_MSSE2_SWITCH},ASTC_HAVE_SSE4_INTRINSICS=${ASTC_HAVE_SSE4_INTRINSICS},ASTC_HAVE_NEON_INTRINSICS=${ASTC_HAVE_NEON_INTRINSICS}")
endif()

# Set up configuration for SIMD ISA builds
if(${ISA_SIMD} MATCHES "none")
    target_compile_definitions(${target_name}
        PUBLIC
            ASTCENC_NEON=0
            ASTCENC_SSE=0
            ASTCENC_AVX=0
            ASTCENC_POPCNT=0
            ASTCENC_F16C=0)

elseif(${ISA_SIMD} MATCHES "neon")
    target_compile_definitions(${target_name}
        PUBLIC
            ASTCENC_NEON=1
            ASTCENC_SSE=0
            ASTCENC_AVX=0
            ASTCENC_POPCNT=0
            ASTCENC_F16C=0)

elseif(${ISA_SIMD} MATCHES "sse2")
    target_compile_definitions(${target_name}
        PUBLIC
            ASTCENC_NEON=0
            ASTCENC_SSE=20
            ASTCENC_AVX=0
            ASTCENC_POPCNT=0
            ASTCENC_F16C=0)

elseif(${ISA_SIMD} MATCHES "sse4.1")
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

elseif(${ISA_SIMD} MATCHES "avx2")
    target_compile_definitions(${target_name}
        PUBLIC
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

