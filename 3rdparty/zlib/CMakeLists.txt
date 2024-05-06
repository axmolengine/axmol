set(target_name "zlib")
project(${target_name})

_1kfetch_dist(${target_name})

if(WINDOWS)
  add_library(${target_name} SHARED IMPORTED GLOBAL)
else()
  add_library(${target_name} STATIC IMPORTED GLOBAL)
endif()

if(WINDOWS)
  set_target_properties(${target_name} PROPERTIES
    IMPORTED_LOCATION "${zlib_LIB_DIR}/zlib1.dll"
    IMPORTED_IMPLIB "${zlib_LIB_DIR}/zlib.lib"
    INTERFACE_COMPILE_DEFINITIONS ZLIB_DLL=1
  )
elseif(APPLE)
  
  set_target_properties(${target_name} PROPERTIES
    IMPORTED_LOCATION "${zlib_LIB_DIR}/zlib.xcframework"
  )
else()
  set_target_properties(${target_name} PROPERTIES
    IMPORTED_LOCATION "${zlib_LIB_DIR}/libz.a"
  )
endif()

set_target_properties(${target_name} PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${zlib_INC_DIR}"
)

# Export zlib as ZLIB::ZLIB to avoid cmake builtin FindPNG.cmake add target again
add_library(ZLIB::ZLIB ALIAS ${target_name})
set(ZLIB_INCLUDE_DIR "${zlib_INC_DIR}" CACHE STRING "zlib include dir" FORCE)
set(ZLIB_LIBRARY "zlib" CACHE INTERNAL "zlib library" FORCE)
