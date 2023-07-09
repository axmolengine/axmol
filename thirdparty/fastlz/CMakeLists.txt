set(lib_name fastlz)
set(target_name ${lib_name})

project(${lib_name})

file(GLOB ${target_name}_src *.h;*.c)

add_library(${target_name} STATIC
  ${${target_name}_src}
)

target_include_directories(${target_name} INTERFACE "${CMAKE_CURRENT_LIST_DIR}")
