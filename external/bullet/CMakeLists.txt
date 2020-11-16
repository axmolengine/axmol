cmake_minimum_required(VERSION 3.6)

set(lib_name bullet)
set(target_name ${lib_name})

project(${lib_name})

add_library(${target_name} STATIC
  btBulletCollisionAll.cpp
  btBulletDynamicsAll.cpp
  btLinearMathAll.cpp
)

target_include_directories(${target_name} PUBLIC .)

target_compile_definitions(${target_name} PUBLIC BT_USE_SSE_IN_API=1)

set_target_properties(${target_name}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    FOLDER "External"
)

