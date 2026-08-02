project(EffekseerRendererAxmol)

set(_src
  EffekseerRendererAxmol.h
  EffekseerRendererAxmol.cpp
)

add_library(${PROJECT_NAME} STATIC ${_src})

target_include_directories(${PROJECT_NAME}
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC
    Effekseer
    EffekseerRendererCommon
    axmol
)

ax_add_shader_dependencies(${PROJECT_NAME} Effekseer_shaders)

# target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
