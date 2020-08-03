set(target_name assets-manager)

FILE(GLOB ASSETS_MANAGER_HEADER *.h)
FILE(GLOB ASSETS_MANAGER_SOURCE *.cpp)

# TODO: dll
add_library(${target_name} STATIC 
        ${ASSETS_MANAGER_HEADER} 
        ${ASSETS_MANAGER_SOURCE})

target_link_libraries(${target_name} cocos2d)
set(CC_EXTENSION_LIBS "${target_name};${CC_EXTENSION_LIBS}" CACHE INTERNAL "extensions for auto link to target application")

set_target_properties(${target_name}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    FOLDER "Extensions"
)

