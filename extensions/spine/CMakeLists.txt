
set(target_name spine)

file(GLOB_RECURSE COCOS_SPINE_SRC 
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/**/*.cpp
)

file(GLOB_RECURSE COCOS_SPINE_HEADER 
    ${CMAKE_CURRENT_LIST_DIR}/*.h
    ${CMAKE_CURRENT_LIST_DIR}/**/*.h
)

add_library(${target_name} STATIC ${COCOS_SPINE_HEADER} ${COCOS_SPINE_SRC})

setup_cocos_extension_config(${target_name})
