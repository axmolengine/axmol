cmake_minimum_required(VERSION 3.6)

set(lib_name convertUTF)
set(target_name ${lib_name})

project(${lib_name})

add_library(${target_name} STATIC
  ConvertUTF.c
  ConvertUTFWrapper.cpp
)

target_include_directories(${target_name} PUBLIC .)

set_target_properties(${target_name}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    FOLDER "External"
)

