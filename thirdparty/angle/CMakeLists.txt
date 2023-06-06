
set(lib_name angle)

project(${lib_name})

include(AXPlatform)

# -----macro: set openssl sub target-----
macro(set_openssl_sub_target sub_target_name sub_lib_name)
  add_library(${sub_target_name} SHARED IMPORTED GLOBAL)

  set_target_properties(${sub_target_name} PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
  )

  set_target_properties(${sub_target_name} PROPERTIES
    IMPORTED_LOCATION "${platform_spec_path}/lib${sub_lib_name}.dll"
    IMPORTED_IMPLIB "${platform_spec_path}/lib${sub_lib_name}.dll.lib"
  )
endmacro()

# -----sub target 1: GLESv2-----
set(target_name angle::GLESv2 GLESv2)
set_openssl_sub_target(${target_name})

# -----sub target 2: EGL-----
set(target_name angle::EGL EGL)
set_openssl_sub_target(${target_name})

