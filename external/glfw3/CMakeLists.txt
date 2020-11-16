cmake_minimum_required(VERSION 3.6)

set(lib_name glfw3)
set(target_name ext_${lib_name})

project(${lib_name})

include(../cmake/CocosExternalConfig.cmake)

add_library(${target_name} STATIC IMPORTED GLOBAL)

set_target_properties(${target_name} PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include/${platform_name}"
)
if(LINUX)
  set_target_properties(${target_name} PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/prebuilt/linux/lib${lib_name}.a"
  )
elseif(WINDOWS)
  set_target_properties(${target_name} PROPERTIES
    IMPORTED_LOCATION "${platform_spec_path}/${lib_name}.lib"
  )
else()
  set_target_properties(${target_name} PROPERTIES
    IMPORTED_LOCATION "${platform_spec_path}/lib${lib_name}.a"
  )
endif()

if(IOS OR ANDROID)
  message(FATAL_ERROR "It's a error to use ${lib_name} on iOS or Android")
endif()
