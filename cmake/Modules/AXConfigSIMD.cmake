# axmol math simd intrinsics detection
# Note: we set default AX_ISA_LEVEL to 2 for sse4.1 for axmol app can runs on large amount devices
# If you want axmol app runs on more old devices, you can specify in cmake cmdline `-DAX_ISA_LEVEL=1`,
# otherwise, host compiler generated instructions will crash on old devices which not support high level
# SIMD instructions.

include(CheckIncludeFile)
include(CheckCCompilerFlag)
include(CheckCSourceCompiles)
include(CheckCXXSourceCompiles)
set(AX_ISA_LEVEL 2 CACHE STRING "SIMD Instructions Acceleration Level")

# wasm skip dynamic check
if(WASM)
  # wasm requires user specify SIMD intrinsics manually
  set(AX_WASM_ISA_SIMD "sse4.1" CACHE STRING "")
  string(TOLOWER ${AX_WASM_ISA_SIMD} AX_WASM_ISA_SIMD)
  set(AX_ISA_SIMD ${AX_WASM_ISA_SIMD} CACHE STRING "" FORCE)
elseif(AX_ISA_LEVEL)
  # SIMD instrinsics detetion when AX_ISA_LEVEL not 0

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
  check_include_file(arm_neon.h AX_HAVE_ARM_NEON_H)
  if(AX_HAVE_ARM_NEON_H)
    check_c_source_compiles("#include <arm_neon.h>
        int main()
        {
            int32x4_t ret4 = vdupq_n_s32(0);
            return vgetq_lane_s32(ret4, 0);
        }" AX_HAVE_NEON_INTRINSICS)
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

  if (WINDOWS)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      if (AX_HAVE_SSE41_INTRINSICS AND AX_ISA_LEVEL GREATER_EQUAL 2)
        add_compile_options("-msse4.1")
      endif()
    endif()
  endif()
endif()

message(AUTHOR_WARNING "AX_ISA_SIMD=${AX_ISA_SIMD}")
