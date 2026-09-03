project(EffekseerExtension)

cmake_minimum_required(VERSION 3.15)

function(clang_format target)
endfunction()

function(FilterFolder srcs)
  foreach(FILE ${srcs})
    get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")
    string(REPLACE "/" "\\" GROUP "${GROUP}")
    source_group("${GROUP}" FILES "${FILE}")
  endforeach()
endfunction()

function(WarnError target)
endfunction()

set(NETWORK_ENABLED OFF CACHE BOOL "" FORCE)
set(BUILD_WITH_EASY_PROFILER OFF CACHE BOOL "" FORCE)
set(USE_LIBPNG_LOADER OFF CACHE BOOL "" FORCE)

# ── Effekseer Built-in shaders ────────────────────────────────────────────
# Uses ax_find_shaders to discover all .hlsl entry shaders in Shaders.
# Utility includes (.hlsli) are NOT compiled — only .hlsl files are.
set(_SHADER_DIR "${CMAKE_CURRENT_LIST_DIR}/Shaders")

ax_find_shaders(${_SHADER_DIR} _shader_src)

# Phase 1: exclude model/advanced shaders (need __INST__ define or not used yet)
list(REMOVE_ITEM _shader_src
  "${_SHADER_DIR}/model_vs.hlsl"
  "${_SHADER_DIR}/ad_model_vs.hlsl"
  "${_SHADER_DIR}/ad_sprite_unlit_vs.hlsl"
  "${_SHADER_DIR}/ad_model_ps.hlsl"
  "${_SHADER_DIR}/ad_model_distortion_ps.hlsl"
)

set_source_files_properties(
  "${_SHADER_DIR}/sprite_unlit_vs.hlsl"
  PROPERTIES
    AXSLCC_VARIANT_DEFINES
      "ENABLE_LIGHTING;ENABLE_DISTORTION"
)

set_source_files_properties(
  "${_SHADER_DIR}/model_ps.hlsl"
  PROPERTIES
    AXSLCC_VARIANT_DEFINES
      "ENABLE_LIGHTING=1"
)

set_source_files_properties(
  "${_SHADER_DIR}/model_distortion_ps.hlsl"
  PROPERTIES
    AXSLCC_VARIANT_DEFINES
      "DISABLED_SOFT_PARTICLE"
)

# ── Effekseer GPU particle shaders ──────────────────────────────────────────
# Compute shaders and the GPU render vertex shader read storage buffers, which
# requires GL 4.3 / GLES 3.1 on the OpenGL backends. Compile them with the
# compute-capable GLSL/ESSL profiles; ordinary shaders keep gl-330 / essl-300.
set(_GP_TARGETS "")
if(AX_ENABLE_GL)
  if(AX_GLES_PROFILE)
    list(APPEND _GP_TARGETS "gles-310")
  else()
    list(APPEND _GP_TARGETS "gl-430")
  endif()
endif()
if(AX_ENABLE_D3D11)
  list(APPEND _GP_TARGETS "d3d11")
endif()
if(AX_ENABLE_D3D12)
  list(APPEND _GP_TARGETS "d3d12")
endif()
if(AX_ENABLE_MTL)
  list(APPEND _GP_TARGETS "mtl")
endif()
if(AX_ENABLE_VK)
  list(APPEND _GP_TARGETS "vk")
endif()

set_source_files_properties(
  "${_SHADER_DIR}/gpu_particles_clear_cs.hlsl"
  "${_SHADER_DIR}/gpu_particles_spawn_cs.hlsl"
  "${_SHADER_DIR}/gpu_particles_update_cs.hlsl"
  "${_SHADER_DIR}/gpu_particles_render_vs.hlsl"
  "${_SHADER_DIR}/gpu_particles_render_ps.hlsl"
  PROPERTIES
    AXSLCC_TARGETS "${_GP_TARGETS}"
)

ax_add_shader_target(Effekseer_shaders FILES ${_shader_src})

add_library(EffekseerAxmolSIMDOptions INTERFACE)
target_compile_definitions(EffekseerAxmolSIMDOptions INTERFACE EFK_SIMD_LEVEL=${AX_ISA_LEVEL})

if(WASM)
  if(AX_WASM_ISA_SIMD MATCHES "sse")
    target_compile_definitions(EffekseerAxmolSIMDOptions INTERFACE __SSE__=1 __SSE2__=1 __SSE3__=1 __SSSE3__=1)

    if(AX_ISA_LEVEL GREATER_EQUAL 2)
      target_compile_definitions(EffekseerAxmolSIMDOptions INTERFACE __SSE4_1__=1)
    endif()

    target_compile_options(EffekseerAxmolSIMDOptions INTERFACE -msimd128)
    target_link_options(EffekseerAxmolSIMDOptions INTERFACE -msimd128)
  elseif(AX_WASM_ISA_SIMD MATCHES "neon")
    target_compile_definitions(EffekseerAxmolSIMDOptions INTERFACE __ARM_NEON__=1)
    target_compile_options(EffekseerAxmolSIMDOptions INTERFACE -mfpu=neon -msimd128)
    target_link_options(EffekseerAxmolSIMDOptions INTERFACE -msimd128)
  endif()
elseif(LINUX AND AX_ISA_SIMD MATCHES "sse4")
  target_compile_options(EffekseerAxmolSIMDOptions INTERFACE -msse4.1)
endif()

add_subdirectory(Effekseer)
add_subdirectory(EffekseerRendererCommon)
add_subdirectory(EffekseerAxmol)

target_compile_definitions(EffekseerRendererCommon PUBLIC __DISABLED_DEFAULT_TEXTURE_LOADER__)

ax_configure_target_output(Effekseer STATIC_LIBRARY Extensions/Effekseer)
ax_configure_target_output(EffekseerRendererCommon STATIC_LIBRARY Extensions/Effekseer)

if(ANDROID)
  ax_setup_ext_config(EffekseerAxmol FOLDER "Extensions/Effekseer" LINK_SCOPE PRIVATE)
else()
  ax_setup_ext_config(EffekseerAxmol FOLDER "Extensions/Effekseer")
endif()
