set(target_name "zlib")
project(${target_name})

_1kfetch_dist(zlib)

if(WINDOWS)
  add_library(${target_name} SHARED IMPORTED GLOBAL)
else()
  add_library(${target_name} STATIC IMPORTED GLOBAL)
endif()

set(ZLIB_INCLUDE_DIR "${zlib_INC_DIR}" CACHE STRING "zlib include dir" FORCE)

if(NOT WINDOWS)
  set(ZLIB_LIBRARY "${zlib_LIB_DIR}/libz.a" CACHE STRING "zlib library" FORCE)
  set_target_properties(${target_name} PROPERTIES
    IMPORTED_LOCATION "${zlib_LIB_DIR}/libz.a"
  )
elseif(APPLE)
  set(ZLIB_LIBRARY "${zlib_LIB_DIR}/zlib.xcframework" CACHE STRING "zlib library" FORCE)
  set_target_properties(${target_name} PROPERTIES
    IMPORTED_LOCATION "${zlib_LIB_DIR}/zlib.xcframework"
  )
else()
  set(ZLIB_LIBRARY "${zlib_LIB_DIR}/zlib.lib" CACHE STRING "zlib library" FORCE)
  set_target_properties(${target_name} PROPERTIES
    IMPORTED_LOCATION "${zlib_LIB_DIR}/zlib1.dll"
    IMPORTED_IMPLIB "${zlib_LIB_DIR}/zlib.lib"
    INTERFACE_COMPILE_DEFINITIONS ZLIB_DLL=1
  )
endif()

set_target_properties(${target_name} PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${zlib_INC_DIR}"
)

# Avoid cmake builtin FindPNG.cmake add target again
add_library(ZLIB::ZLIB ALIAS zlib)
