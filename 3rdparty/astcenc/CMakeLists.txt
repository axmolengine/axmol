## astc-encoder CMakeLists.txt: auto detect SIMD Intrinsics
set(lib_name astcenc)
set(target_name ${lib_name})

project(${lib_name})

# include(CheckIncludeFile)
# include(CheckCCompilerFlag)
# include(CheckCSourceCompiles)
# include(CheckCXXSourceCompiles)

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

if(NOT WASM)
    if (NOT (ARCH_ALIAS STREQUAL "x86") AND NOT TVOS AND NOT (ARCH_ALIAS MATCHES "arm.*v7"))
        set(ASTC_ISA_SIMD ${AX_ISA_SIMD})
    else() # astcenc not support sse4/avx tvos/x86/armv7
        message(AUTHOR_WARNING "Skipping AVX2/SSE4/NEON detection for astc-encoder when build tvos/x86/armv7")
        if(AX_HAVE_SSE2_INTRINSICS)
            set(ASTC_ISA_SIMD "sse2")
        else()
            set(ASTC_ISA_SIMD "null")
        endif()
    endif()
else()
    set(ASTC_ISA_SIMD "null")
endif()

message(AUTHOR_WARNING "ASTC_ISA_SIMD=${ASTC_ISA_SIMD}")

# Set up configuration for SIMD ISA builds
if(ASTC_ISA_SIMD STREQUAL "neon")
    target_compile_definitions(${target_name}
        PUBLIC
            ASTCENC_NEON=1
            ASTCENC_SSE=0
            ASTCENC_AVX=0
            ASTCENC_POPCNT=0
            ASTCENC_F16C=0)
elseif(ASTC_ISA_SIMD STREQUAL "avx2")
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
elseif(ASTC_ISA_SIMD STREQUAL "sse4.2")
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
elseif(ASTC_ISA_SIMD STREQUAL "sse4.1")
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
elseif(ASTC_ISA_SIMD STREQUAL "sse2")
    target_compile_definitions(${target_name}
        PUBLIC
            ASTCENC_NEON=0
            ASTCENC_SSE=20
            ASTCENC_AVX=0
            ASTCENC_POPCNT=0
            ASTCENC_F16C=0)
else() # null
    target_compile_definitions(${target_name}
        PUBLIC
            ASTCENC_NEON=0
            ASTCENC_SSE=0
            ASTCENC_AVX=0
            ASTCENC_POPCNT=0
            ASTCENC_F16C=0)
endif()

target_include_directories(${target_name} PUBLIC ..)
