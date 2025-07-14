
set(lib_name luajit)
set(target_name ${lib_name})
project(${lib_name})

_1kfetch_dist(luajit)

if(WINDOWS)
  add_library(${target_name} SHARED IMPORTED GLOBAL)
else()
  add_library(${target_name} STATIC IMPORTED GLOBAL)
endif()

set_target_properties(${target_name} PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${luajit_INC_DIR}"
  INTERFACE_COMPILE_DEFINITIONS USING_LUAJIT=1 # for lua-cjson
)

if(WINDOWS)
  set_target_properties(${target_name} PROPERTIES
    IMPORTED_LOCATION "${luajit_LIB_DIR}/lua51.dll"
    IMPORTED_IMPLIB "${luajit_LIB_DIR}/lua51.lib"
  )
elseif(APPLE)
  set_target_properties(${target_name} PROPERTIES
    IMPORTED_LOCATION "${luajit_LIB_DIR}/${lib_name}.xcframework"
  )
else()
  set_target_properties(${target_name} PROPERTIES
    IMPORTED_LOCATION "${luajit_LIB_DIR}/lib${lib_name}.a"
  )
endif()
