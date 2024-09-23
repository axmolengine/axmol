
set(lib_name lua-cjson)
set(target_name ${lib_name})

project(${lib_name})

set(${target_name}_src
  fpconv.c
  strbuf.c
  lua_cjson.c
)

add_library(${target_name} STATIC
  ${${target_name}_src}
)

if(NOT AX_USE_LUAJIT)
  target_include_directories(${target_name} PRIVATE ../${AX_LUA_ENGINE})
else()
  target_include_directories(${target_name} PRIVATE ../${AX_LUA_ENGINE}/_x/include)
endif()
target_include_directories(${target_name} PUBLIC .)

set_target_properties(${target_name}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    FOLDER "3rdparty"
)
