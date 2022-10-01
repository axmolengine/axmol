target_sources(${LIB_NAME}
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismRenderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismRenderer.hpp
)

if(NOT DEFINED FRAMEWORK_SOURCE)
  message(WARNING
    "[${LIB_NAME}] Set 'FRAMEWORK_SOURCE' variable to 'axmol' when using axmol rendering."
  )
  set(FRAMEWORK_SOURCE axmol)
endif()

# # Add specified rendering directory.
add_subdirectory(${FRAMEWORK_SOURCE})

# Add include path set in application (Deprecated).
set(RENDER_INCLUDE_PATH
  ${FRAMEWORK_DX9_INCLUDE_PATH}
  ${FRAMEWORK_DX11_INCLUDE_PATH}
  ${FRAMEWORK_GLEW_PATH}
  ${FRAMEWORK_GLFW_PATH}
  PARENT_SCOPE
)
