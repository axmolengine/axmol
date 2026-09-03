project(EffekseerAxmol)

set(_src
  EffekseerAxmol.h
  EffekseerAxmol.cpp
  EffekseerRendererAxmol.h
  EffekseerRendererAxmol.cpp
)

add_library(${PROJECT_NAME} STATIC ${_src})

target_include_directories(${PROJECT_NAME}
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../EffekseerRendererAxmol
)

if(ANDROID)
  set(_LINK_SCOPE PUBLIC)
else()
  set(_LINK_SCOPE)
endif()
target_link_libraries(${PROJECT_NAME}
  ${_LINK_SCOPE}
  Effekseer
  EffekseerRendererCommon
)

ax_add_shader_dependencies(${PROJECT_NAME} Effekseer_shaders)
