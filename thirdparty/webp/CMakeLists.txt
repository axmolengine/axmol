
set(lib_name webp)
set(target_name ${lib_name})

project(${lib_name})
include(../cmake/CocosExternalConfig.cmake)

FILE(GLOB_RECURSE ${target_name}_src *.h;*.c)

add_library(${target_name} STATIC
  ${${target_name}_src}
)

target_include_directories(${target_name} PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
target_include_directories(${target_name} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/src/webp")
