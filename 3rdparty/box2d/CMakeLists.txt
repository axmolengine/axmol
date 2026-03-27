
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

if(_simdc_defines)
  target_compile_definitions(${target_name} PRIVATE ${_simdc_defines})

  if(_simdc_options)
    target_compile_options(${target_name} PRIVATE ${_simdc_options})
  endif()
else()
  target_compile_definitions(${target_name} PUBLIC BOX2D_DISABLE_SIMD=1)
endif()

target_include_directories(${target_name}
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
