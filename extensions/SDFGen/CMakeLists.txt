set(target_name SDFGen)

FILE(GLOB SDFGEN_SOURCES *.h;*.cpp;./**/*.h;./**/*.cpp)

add_library(${target_name} ${SDFGEN_SOURCES})

setup_ax_extension_config(${target_name})
