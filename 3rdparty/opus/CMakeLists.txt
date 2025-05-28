set(target_name "opus")
project(${target_name})

_1kfetch_dist(${target_name})

add_library(${target_name} STATIC IMPORTED GLOBAL)

if(WINDOWS)
  set_target_properties(${target_name} PROPERTIES
    IMPORTED_LOCATION "${opus_LIB_DIR}/${target_name}.lib"
  )
elseif(APPLE)
  set_target_properties(${target_name} PROPERTIES
    IMPORTED_LOCATION "${opus_LIB_DIR}/${target_name}.xcframework"
  )
else()
  set_target_properties(${target_name} PROPERTIES
    IMPORTED_LOCATION "${opus_LIB_DIR}/lib${target_name}.a"
  )
endif()

set_target_properties(${target_name} PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${opus_INC_DIR}"
)

add_library(Opus::opus ALIAS ${target_name})
set(OPUS_INCLUDE_DIR "${opus_INC_DIR}" CACHE STRING "opus include dir" FORCE)
set(OPUS_LIBRARY "opus" CACHE INTERNAL "opus library" FORCE)
