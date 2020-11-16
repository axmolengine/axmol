cmake_minimum_required(VERSION 3.6)

set(lib_name md5)
set(target_name ${lib_name})

project(${lib_name})

set(${target_name}_src
  md5.c
)

add_library(${target_name} STATIC
  ${${target_name}_src}
)

target_include_directories(${target_name} PUBLIC ..)

set_target_properties(${target_name}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    FOLDER "External"
)

