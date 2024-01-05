set(target_name DrawNodeEx)

FILE(GLOB DRAWNODEEX_SOURCES *.h;*.cpp;./**/*.h;./**/*.cpp)

add_library(${target_name} ${DRAWNODEEX_SOURCES})

setup_ax_extension_config(${target_name})
