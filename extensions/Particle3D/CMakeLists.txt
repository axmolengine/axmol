set(target_name particle3d)

FILE(GLOB_RECURSE PARTICLE3D_SOURCES *.h;*.cpp;./**/*.h;./**/*.cpp)

add_library(${target_name} ${PARTICLE3D_SOURCES})
target_include_directories(${target_name} PUBLIC src)

ax_setup_ext_config(${target_name})
