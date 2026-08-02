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

ax_add_shader_target(Effekseer_shaders FILES ${_shader_src})

add_subdirectory(Effekseer)
add_subdirectory(EffekseerRendererCommon)
add_subdirectory(EffekseerRendererAxmol)
add_subdirectory(EffekseerAxmol)

target_compile_definitions(EffekseerRendererCommon PUBLIC __DISABLED_DEFAULT_TEXTURE_LOADER__)

set_target_properties(Effekseer PROPERTIES FOLDER Extensions/Effekseer)
set_target_properties(EffekseerRendererCommon PROPERTIES FOLDER Extensions/Effekseer)
set_target_properties(EffekseerRendererAxmol PROPERTIES FOLDER Extensions/Effekseer)
set_target_properties(EffekseerAxmol PROPERTIES FOLDER Extensions/Effekseer)
