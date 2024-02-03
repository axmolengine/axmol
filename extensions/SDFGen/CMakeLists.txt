set(target_name SDFGen)

FILE(GLOB_RECURSE SDFGEN_SOURCES *.h;*.cpp;./**/*.h;./**/*.cpp)

add_library(${target_name} STATIC ${SDFGEN_SOURCES})
target_include_directories(${target_name} PUBLIC src)
setup_ax_extension_config(${target_name})
