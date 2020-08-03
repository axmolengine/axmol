set(target_name assets-manager)

FILE(GLOB ASSETS_MANAGER_HEADER *.h)
FILE(GLOB ASSETS_MANAGER_SOURCE *.cpp)

# TODO: dll
add_library(${target_name} STATIC 
        ${ASSETS_MANAGER_HEADER} 
        ${ASSETS_MANAGER_SOURCE})

setup_cocos_extension_config(${target_name})

