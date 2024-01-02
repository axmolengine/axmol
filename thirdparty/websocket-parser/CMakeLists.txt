
set(lib_name websocket-parser)
set(target_name ${lib_name})

project(${lib_name})

FILE(GLOB_RECURSE ${target_name}_src *.h;*.c)

add_library(${target_name} STATIC
  ${${target_name}_src}
)

target_include_directories(${target_name} PUBLIC "${CMAKE_CURRENT_LIST_DIR}") 

