set(target_name particle3d)

FILE(GLOB_RECURSE PARTICLE3D_SOURCES *.h;*.cpp;./**/*.h;./**/*.cpp)

add_library(${target_name} ${PARTICLE3D_SOURCES})
target_include_directories(${target_name} PUBLIC src)

setup_ax_extension_config(${target_name})
