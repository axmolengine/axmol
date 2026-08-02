set(target_name SDFGen)

FILE(GLOB_RECURSE SDFGEN_SOURCES *.h;*.cpp;./**/*.h;./**/*.cpp)

add_library(${target_name} STATIC ${SDFGEN_SOURCES})
target_include_directories(${target_name} PUBLIC src)
ax_setup_ext_config(${target_name})
