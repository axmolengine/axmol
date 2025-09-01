set(lib_name curl)
set(target_name ${lib_name})

project(${lib_name})

_1kfetch_dist(curl)

if(WINDOWS)
  add_library(${target_name} SHARED IMPORTED GLOBAL)
else()
  add_library(${target_name} STATIC IMPORTED GLOBAL)
endif()

set_target_properties(${target_name} PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${curl_INC_DIR}"
)

if(WINDOWS)
  set_target_properties(${target_name} PROPERTIES
    IMPORTED_LOCATION "${curl_LIB_DIR}/lib${lib_name}.dll"
    IMPORTED_IMPLIB "${curl_LIB_DIR}/lib${lib_name}_imp.lib"
  )
elseif(APPLE)
  set_target_properties(${target_name} PROPERTIES
    IMPORTED_LOCATION "${curl_LIB_DIR}/${lib_name}.xcframework"
  )
else()
  set_target_properties(${target_name} PROPERTIES
    IMPORTED_LOCATION "${curl_LIB_DIR}/lib${lib_name}.a"
  )
endif()

if(ANDROID OR LINUX)
  target_link_libraries(curl INTERFACE OpenSSL::SSL)
endif()