cmake_minimum_required(VERSION 3.6)

set(lib_name freetype)
set(target_name ext_${lib_name})

project(${lib_name})

include(../cmake/CocosExternalConfig.cmake)

add_library(${target_name} STATIC IMPORTED GLOBAL)

if(WIN32)
  set_target_properties(${target_name} PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include/win32"
  )
else()
  set_target_properties(${target_name} PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include/unix"
  )
endif()

if(WINDOWS)
  set_target_properties(${target_name} PROPERTIES
    IMPORTED_LOCATION "${platform_spec_path}/${lib_name}.lib"
  )
else()
  set_target_properties(${target_name} PROPERTIES
    IMPORTED_LOCATION "${platform_spec_path}/lib${lib_name}.a"
  )
endif()
