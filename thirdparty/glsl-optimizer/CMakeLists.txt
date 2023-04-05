
set(lib_name glsl_optimizer)
set(target_name dep_${lib_name})

project(${lib_name})

include(AXPlatform)

add_library(dep_glcpp_library STATIC IMPORTED GLOBAL)
add_library(dep_glsl_optimizer STATIC IMPORTED GLOBAL)
add_library(dep_libmesa STATIC IMPORTED GLOBAL)


set_target_properties(${target_name} PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

set_target_properties(dep_glcpp_library PROPERTIES
  IMPORTED_LOCATION "${platform_spec_path}/libglcpp-library.a"
)
set_target_properties(dep_glsl_optimizer PROPERTIES
  IMPORTED_LOCATION "${platform_spec_path}/libglsl_optimizer.a"
)
set_target_properties(dep_libmesa PROPERTIES
  IMPORTED_LOCATION "${platform_spec_path}/libmesa.a"
)
