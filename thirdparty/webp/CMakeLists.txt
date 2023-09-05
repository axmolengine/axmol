
set(lib_name webp)
set(target_name ${lib_name})

project(${lib_name})
include(AXPlatform)

FILE(GLOB_RECURSE ${target_name}_src *.h;*.c)

add_library(${target_name} STATIC
  ${${target_name}_src}
)

message(STATUS "Fuck")

target_include_directories(${target_name} PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
target_include_directories(${target_name} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/src/webp")
target_compile_definitions(${target_name} PRIVATE HAVE_CONFIG_H=1)

if (NOT WASM)
  set(_WEBP_ISA_SIMD ${AX_ISA_SIMD})
else()
  set(_WEBP_ISA_SIMD "none")
endif()

if (_WEBP_ISA_SIMD MATCHES "neon")
  target_compile_definitions(${target_name} PRIVATE WEBP_HAVE_NEON=1)
elseif(_WEBP_ISA_SIMD MATCHES "avx2" OR _WEBP_ISA_SIMD MATCHES "sse4.2" OR _WEBP_ISA_SIMD MATCHES "sse4.1")
  target_compile_definitions(${target_name} PRIVATE WEBP_HAVE_SSE2=1)
  target_compile_definitions(${target_name} PRIVATE WEBP_HAVE_SSE41=1)
elseif(_WEBP_ISA_SIMD MATCHES "sse2")
  target_compile_definitions(${target_name} PRIVATE WEBP_HAVE_SSE2=1)
endif()
