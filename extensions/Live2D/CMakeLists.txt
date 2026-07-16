set(LIB_NAME Live2D)

# Force static library.
add_library(${LIB_NAME} STATIC)

set(FRAMEWORK_SOURCE Axmol)
add_subdirectory(Framework/src)

# Add include path.
target_include_directories(${LIB_NAME}
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/Framework/src
)

# Deprecated functions
# The following expressions are written for compatibility
# and will be removed in a future release.

# Add core include.
target_include_directories(${LIB_NAME}
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/include
)

set(live2d_core_tgt Live2DCubismCore)
include(live2d-core.cmake)

if(ANDROID)
  target_link_libraries(${LIB_NAME} INTERFACE ${live2d_core_tgt})
else()
  target_link_libraries(${LIB_NAME} ${live2d_core_tgt})
endif()

ax_find_shaders("${CMAKE_CURRENT_LIST_DIR}/Framework/src/Rendering/Axmol/shaders" live2d_shader_sources)

ax_add_shader_target_for(${LIB_NAME} FILES ${live2d_shader_sources})

if(ANDROID)
  setup_ax_extension_config(${LIB_NAME} LINK_SCOPE PRIVATE)
else()
  setup_ax_extension_config(${LIB_NAME})
endif()

unset(LIB_NAME)
