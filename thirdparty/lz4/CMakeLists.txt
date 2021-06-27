set(lib_name lz4)
set(target_name ${lib_name})

project(${lib_name})

add_library(${target_name} STATIC
  lz4.c
)

target_include_directories(${target_name} PUBLIC .)
