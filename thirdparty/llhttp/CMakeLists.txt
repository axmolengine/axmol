set(lib_name llhttp)
set(target_name ${lib_name})

project(${lib_name})

file(GLOB_RECURSE ${target_name}_src *.h;*.c)

add_library(${target_name} STATIC
  ${${target_name}_src}
)

target_include_directories(${target_name} PUBLIC ..)
