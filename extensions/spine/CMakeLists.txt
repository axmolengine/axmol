
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

target_link_libraries(${target_name} cocos2d)

set(CC_EXTENSION_LIBS "${target_name};${CC_EXTENSION_LIBS}" CACHE INTERNAL "extensions for auto link to target application")

set_target_properties(${target_name}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    FOLDER "Extensions"
)
