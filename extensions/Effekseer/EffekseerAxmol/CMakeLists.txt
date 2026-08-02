project(EffekseerAxmol)

set(_src
  EffekseerAxmol.h
  EffekseerAxmol.cpp
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
    EffekseerRendererAxmol
    axmol
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
