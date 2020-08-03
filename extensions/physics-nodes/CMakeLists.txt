set(target_name physics-nodes)

FILE(GLOB PHYSICS_NODES_SOURCES *.h;*.cpp;./**/*.h;./**/*.cpp)

add_library(${target_name} STATIC ${PHYSICS_NODES_SOURCES})

target_link_libraries(${target_name} cocos2d)

set(CC_EXTENSION_LIBS "${target_name};${CC_EXTENSION_LIBS}" CACHE INTERNAL "extensions for auto link to target application")

set_target_properties(${target_name}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    FOLDER "Extensions"
)
