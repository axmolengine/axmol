
set(lib_name png)
set(target_name ${lib_name})

project(${lib_name})
include(AXPlatform)

set(${target_name}_src
    png.c
    pngerror.c
    pngget.c
    pngmem.c
    pngpread.c
    pngread.c
    pngrio.c
    pngrtran.c
    pngrutil.c
    pngset.c
    pngtrans.c
    pngwio.c
    pngwrite.c
    pngwtran.c
    pngwutil.c
)

if (AX_ISA_SIMD STREQUAL "neon")
  set(libpng_arm_sources
    arm/arm_init.c
    arm/filter_neon_intrinsics.c
    arm/palette_neon_intrinsics.c)
  # notes:
  #  - android armv7: needs PNG_ARM_NEON_CHECK_SUPPORTED for very older devices, but axmol no longer support them
  set(libpng_optimize_flag "PNG_ARM_NEON_OPT=2")
elseif(NOT (AX_ISA_SIMD STREQUAL "none"))
  set(libpng_intel_sources
    intel/intel_init.c
    intel/filter_sse2_intrinsics.c)
  set(libpng_optimize_flag "PNG_INTEL_SSE_OPT=1")
else()
  set(libpng_optimize_flag "PNG_ARM_NEON_OPT=0")
  set(libpng_optimize_flag "PNG_INTEL_SSE_OPT=0")
endif()

add_library(${target_name} STATIC
  ${${target_name}_src}
  ${libpng_arm_sources}
  ${libpng_intel_sources}
)

target_include_directories(${target_name} PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
target_compile_definitions(${target_name} PUBLIC ${libpng_optimize_flag})
