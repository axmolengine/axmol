
set(lib_name box2d)
set(target_name ${lib_name})

project(${lib_name})

file(GLOB_RECURSE box2d_sources *.h;*.c)

add_library(${target_name} ${box2d_sources})

if(MSVC)
  if(BUILD_SHARED_LIBS)
    # this is needed by DLL users to import Box2D symbols
    target_compile_definitions(box2d INTERFACE BOX2D_DLL)
  endif()
endif()

# Deterministic math
# https://box2d.org/posts/2024/08/determinism/
if (MINGW OR APPLE OR UNIX)
  message(AUTHOR_WARNING "Compiling box2d with -ffp-contract=off for non-windows platform")
  target_compile_options(${target_name} PRIVATE -ffp-contract=off)
endif()

# Atomics are still considered experimental in Visual Studio 17.8
if(FULL_MSVC)
  target_compile_options(box2d PRIVATE /experimental:c11atomics)
endif()

set_target_properties(${target_name} PROPERTIES C_STANDARD 17 C_STANDARD_REQUIRED TRUE)

# Match upstream Box2D SIMD configuration. Axmol's _simdc_defines describe
# Axmol intrinsics and do not select Box2D's SIMD implementation.
option(BOX2D_DISABLE_SIMD "Disable SIMD math (slower)" OFF)
if(BOX2D_DISABLE_SIMD)
  target_compile_definitions(${target_name} PRIVATE BOX2D_DISABLE_SIMD)
elseif(EMSCRIPTEN)
  # Box2D maps WebAssembly to its SSE2 path.
  target_compile_options(${target_name} PRIVATE -msimd128 -msse2)
endif()

# Upstream compiles only the width-8 solver with AVX2 on MSVC, leaving the
# rest of Box2D on the baseline instruction set.
if(NOT BOX2D_DISABLE_SIMD AND MSVC
   AND NOT CMAKE_C_COMPILER_ID STREQUAL "Clang"
   AND CMAKE_C_COMPILER_ARCHITECTURE_ID MATCHES "^(x64|X86)$")
  set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/contact_solver_w8.c"
    PROPERTIES COMPILE_OPTIONS "/arch:AVX2")
endif()

target_include_directories(${target_name}
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
