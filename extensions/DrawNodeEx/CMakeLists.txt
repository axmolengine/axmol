set(target_name DrawNodeEx)

FILE(GLOB_RECURSE DRAWNODEEX_SOURCES *.h;*.cpp;./**/*.h;./**/*.cpp)

add_library(${target_name} ${DRAWNODEEX_SOURCES})
target_include_directories(${target_name} PUBLIC src)

setup_ax_extension_config(${target_name})
