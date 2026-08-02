set(target_name physics-nodes)

FILE(GLOB_RECURSE PHYSICS_NODES_SOURCES *.h;*.cpp;./**/*.h;./**/*.cpp)

add_library(${target_name} ${PHYSICS_NODES_SOURCES})
target_include_directories(
  ${target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
)

ax_setup_ext_config(${target_name})
