set(target_name NodeExplorer)

FILE(GLOB NODE_EXPLORER_SOURCES *.h;*.cpp;./**/*.h;./**/*.cpp)

add_library(${target_name} ${NODE_EXPLORER_SOURCES})

setup_ax_extension_config(${target_name})
