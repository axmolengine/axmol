set(target_name Inspector)

FILE(GLOB INSPECTOR_SOURCES *.h;*.cpp;./**/*.h;./**/*.cpp)

add_library(${target_name} ${INSPECTOR_SOURCES})

setup_ax_extension_config(${target_name})
