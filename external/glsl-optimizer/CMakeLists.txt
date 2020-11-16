cmake_minimum_required(VERSION 3.6)

set(lib_name glsl_optimizer)
set(target_name ext_${lib_name})

project(${lib_name})

include(../cmake/CocosExternalConfig.cmake)

add_library(ext_glcpp_library STATIC IMPORTED GLOBAL)
add_library(ext_glsl_optimizer STATIC IMPORTED GLOBAL)
add_library(ext_libmesa STATIC IMPORTED GLOBAL)


set_target_properties(${target_name} PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

set_target_properties(ext_glcpp_library PROPERTIES
  IMPORTED_LOCATION "${platform_spec_path}/libglcpp-library.a"
)
set_target_properties(ext_glsl_optimizer PROPERTIES
  IMPORTED_LOCATION "${platform_spec_path}/libglsl_optimizer.a"
)
set_target_properties(ext_libmesa PROPERTIES
  IMPORTED_LOCATION "${platform_spec_path}/libmesa.a"
)
