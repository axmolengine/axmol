set(target_name physics-nodes)

FILE(GLOB PHYSICS_NODES_SOURCES *.h;*.cpp;./**/*.h;./**/*.cpp)

add_library(${target_name} ${PHYSICS_NODES_SOURCES})

setup_ax_extension_config(${target_name})
