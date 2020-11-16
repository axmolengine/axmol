cmake_minimum_required(VERSION 3.6)

set(lib_name iconv)
set(target_name ext_${lib_name})

project(${lib_name})

include(../../cmake/CocosExternalConfig.cmake)

add_library(${target_name} SHARED IMPORTED GLOBAL)

set_target_properties(${target_name} PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(${target_name} PROPERTIES
  IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/prebuilt/libiconv.dll"
  IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/prebuilt/libiconv.lib"
)
