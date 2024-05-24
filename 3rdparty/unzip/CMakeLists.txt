
set(lib_name unzip)
set(target_name ${lib_name})

project(${lib_name})

set(${target_name}_src
  ioapi.c
  unzip.c
)

add_library(${target_name} STATIC
  ${${target_name}_src}
)

target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_compile_definitions(${target_name} PUBLIC NOUNCRYPT=1)

get_target_property(zlib_header zlib INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(${target_name} PRIVATE ${zlib_header})
