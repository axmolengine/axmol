
set(lib_name png)
set(target_name ${lib_name})

project(${lib_name})

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

if (NOT WASM)
  set(_PNG_ISA_SIMD ${AX_ISA_SIMD})
else()
  set(_PNG_ISA_SIMD OFF)
endif()

if (_PNG_ISA_SIMD STREQUAL "neon")
  set(libpng_arm_sources
    arm/arm_init.c
    arm/filter_neon_intrinsics.c
    arm/palette_neon_intrinsics.c)
  # notes:
  #  - android armv7: needs PNG_ARM_NEON_CHECK_SUPPORTED for very older devices, but axmol no longer support them
  # PNG_ARM_NEON_IMPLEMENTATION:
  #    1: use instrinsics
  #    2: use handcode
  set(libpng_optimize_flag "PNG_ARM_NEON_OPT=2")
elseif(_PNG_ISA_SIMD MATCHES "sse")
  set(libpng_intel_sources
    intel/intel_init.c
    intel/filter_sse2_intrinsics.c)
  set(libpng_optimize_flag "PNG_INTEL_SSE_OPT=1")
endif()

add_library(${target_name} STATIC
  ${${target_name}_src}
  ${libpng_arm_sources}
  ${libpng_intel_sources}
)

# cmake builtin FindPNG.cmake detect cached variable PNG_PNG_INCLUDE_DIR
set(PNG_PNG_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE INTERNAL "png include dir" FORCE)

set(PNG_INCLUDE_DIR "${PNG_PNG_INCLUDE_DIR}" CACHE INTERNAL "" FORCE)
target_include_directories(${target_name} PUBLIC "${PNG_INCLUDE_DIR}")

if(_PNG_ISA_SIMD)
    target_compile_definitions(${target_name} PUBLIC ${libpng_optimize_flag})
endif()

target_include_directories(${target_name} PRIVATE "${ZLIB_INCLUDE_DIR}")
if(ANDROID)
    target_include_directories(${target_name} PRIVATE ${cpufeatures_include_dir})
    target_link_libraries(${target_name} INTERFACE cpufeatures)
endif()

# Export png as PNG::PNG to avoid cmake builtin FindPNG.cmake add target again
add_library(PNG::PNG ALIAS ${target_name})

set(PNG_LIBRARY "png" CACHE INTERNAL "png include dir" FORCE)
