
set(target_name c-ares)

project(${target_name})

_1kfetch_dist(curl "${PARENT_DIRECTORY}/_deps")

if(WINDOWS)
  add_library(${target_name} SHARED IMPORTED GLOBAL)
else()
  add_library(${target_name} STATIC IMPORTED GLOBAL)
endif()

set_target_properties(${target_name} PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include/"
  INTERFACE_COMPILE_DEFINITIONS CARES_STATICLIB=1
)

if (APPLE)
  set_target_properties(${target_name} PROPERTIES
      INTERFACE_LINK_LIBRARIES resolv
  )
endif()

if(WINDOWS)
  set_target_properties(${target_name} PROPERTIES
    IMPORTED_IMPLIB "${platform_spec_path}/cares.lib"
  )
else()
  set_target_properties(${target_name} PROPERTIES
    IMPORTED_LOCATION "${platform_spec_path}/libcares.a"
  )
endif()

set(CARES_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include" CACHE STRING "c-ares Include Directory" )
