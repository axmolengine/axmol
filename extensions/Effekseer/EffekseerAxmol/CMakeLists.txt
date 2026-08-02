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

target_link_libraries(${PROJECT_NAME}
    Effekseer
    EffekseerRendererCommon
)

ax_add_shader_dependencies(${PROJECT_NAME} Effekseer_shaders)
