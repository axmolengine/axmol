
set(target_name c-ares)

project(${target_name})

_1kfetch_dist(cares)

if(WINDOWS)
  add_library(${target_name} SHARED IMPORTED GLOBAL)
else()
  add_library(${target_name} STATIC IMPORTED GLOBAL)
endif()

set_target_properties(${target_name} PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${cares_INC_DIR}"
  INTERFACE_COMPILE_DEFINITIONS CARES_STATICLIB=1
)

if (APPLE)
  set_target_properties(${target_name} PROPERTIES
      INTERFACE_LINK_LIBRARIES resolv
  )
endif()

if(WINDOWS)
  set_target_properties(${target_name} PROPERTIES
    IMPORTED_IMPLIB "${cares_LIB_DIR}/cares.lib"
  )
elseif(APPLE)
  set_target_properties(${target_name} PROPERTIES
    IMPORTED_LOCATION "${cares_LIB_DIR}/cares.xcframework"
  )
else()
  set_target_properties(${target_name} PROPERTIES
    IMPORTED_LOCATION "${cares_LIB_DIR}/libcares.a"
  )
endif()

set(CARES_INCLUDE_DIR "${cares_INC_DIR}" CACHE STRING "c-ares Include Directory" )
