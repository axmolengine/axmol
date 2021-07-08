set(lib_name kcp)
set(target_name ${lib_name})

project(${lib_name})

add_library(${target_name} STATIC
  ikcp.c
)

target_include_directories(${target_name} PUBLIC .)
