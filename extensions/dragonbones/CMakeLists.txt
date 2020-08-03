set(target_name dragonbones)

FILE(GLOB_RECURSE DRAGONBONES_SOURCES ./*.h;./*.cpp)

add_library(${dragonbones} STATIC ${DRAGONBONES_SOURCES})

target_link_libraries(${target_name} cocos2d)
set(CC_EXTENSION_LIBS "${target_name};${CC_EXTENSION_LIBS}" CACHE INTERNAL "extensions for auto link to target application")

set_target_properties(${target_name}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    FOLDER "Extensions"
)

