set(target_name DragonBones)

FILE(GLOB_RECURSE DRAGONBONES_SOURCES *.h;*.cpp;./**/*.h;./**/*.cpp)

add_library(${target_name} STATIC ${DRAGONBONES_SOURCES})
target_include_directories(${target_name} PUBLIC src)

setup_ax_extension_config(${target_name})
