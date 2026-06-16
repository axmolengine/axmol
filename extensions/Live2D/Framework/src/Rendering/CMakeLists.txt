target_sources(${LIB_NAME}
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/csmBlendMode.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/csmBlendMode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismClippingManager.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismClippingManager.tpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismRenderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismRenderer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ICubismOffscreenManager.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ICubismOffscreenRenderTarget.hpp
)

# Add specified rendering directory.
add_subdirectory(${FRAMEWORK_SOURCE})

# Add include path set in application (Deprecated).
set(RENDER_INCLUDE_PATH
  ${FRAMEWORK_DX9_INCLUDE_PATH}
  ${FRAMEWORK_DX11_INCLUDE_PATH}
  ${FRAMEWORK_GLEW_PATH}
  ${FRAMEWORK_GLFW_PATH}
  PARENT_SCOPE
)
