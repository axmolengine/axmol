set(target_name cocostudio)

FILE(GLOB_RECURSE CCS_SOURCES ./*.h;./*.cpp)

add_library(${target_name} STATIC ${CCS_SOURCES})

setup_cocos_extension_config(${target_name})
