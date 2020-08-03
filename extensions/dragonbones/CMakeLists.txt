set(target_name dragonbones)

FILE(GLOB_RECURSE DRAGONBONES_SOURCES ./*.h;./*.cpp)

add_library(${dragonbones} STATIC ${DRAGONBONES_SOURCES})

setup_cocos_extension_config(${target_name})
