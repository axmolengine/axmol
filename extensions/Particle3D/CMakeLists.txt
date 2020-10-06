set(target_name particle3d)

FILE(GLOB PARTICLE3D_SOURCES *.h;*.cpp;./**/*.h;./**/*.cpp)

add_library(${target_name} ${PARTICLE3D_SOURCES})

setup_cocos_extension_config(${target_name})
