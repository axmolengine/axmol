
set(lib_name jpeg-turbo)
set(target_name ${lib_name})

project(${lib_name})

_1kfetch_dist(jpeg-turbo)

add_library(${target_name} STATIC IMPORTED GLOBAL)

set_target_properties(${target_name} PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${jpeg-turbo_INC_DIR}"
)

if(WINDOWS)
  set_target_properties(${target_name} PROPERTIES
    IMPORTED_LOCATION "${jpeg-turbo_LIB_DIR}/jpeg-static.lib"
  )
elseif(APPLE)
  set_target_properties(${target_name} PROPERTIES
    IMPORTED_LOCATION "${jpeg-turbo_LIB_DIR}/jpeg.xcframework"
  )
else()
  set_target_properties(${target_name} PROPERTIES
    IMPORTED_LOCATION "${jpeg-turbo_LIB_DIR}/libjpeg.a"
  )
endif()
