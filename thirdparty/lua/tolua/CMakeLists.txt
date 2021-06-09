
set(lib_name tolua)
set(target_name ${lib_name})

project(${lib_name})

set(${target_name}_src
  tolua_event.c
  tolua_is.c
  tolua_map.c
  tolua_push.c
  tolua_to.c
)

add_library(${target_name} STATIC
  ${${target_name}_src}
)

if(LUA_ENGINE STREQUAL "plainlua")
  target_include_directories(${target_name} PRIVATE ../${LUA_ENGINE}/lua-${LUA_VERSION}/src)
else()
  target_include_directories(${target_name} PRIVATE ../${LUA_ENGINE}/include)
endif()
target_include_directories(${target_name} PUBLIC .)

set_target_properties(${target_name}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    FOLDER "ThirdParty"
)
