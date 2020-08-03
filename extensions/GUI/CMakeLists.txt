set(target_name GUI)

FILE(GLOB GUI_CONTROL_SOURCES CCControlExtension/*.h;CCControlExtension/*.cpp)
FILE(GLOB GUI_SCROLLVIEW_SOURCES CCScrollView/*.h;CCScrollView/*.cpp)

SOURCE_GROUP("CCControlExtension" FILES ${GUI_CONTROL_SOURCES})
SOURCE_GROUP("CCScrollView" FILES ${GUI_SCROLLVIEW_SOURCES})

# TODO: dll
add_library(${target_name} STATIC 
        ${GUI_CONTROL_SOURCES} 
        ${GUI_SCROLLVIEW_SOURCES})
        
target_link_libraries(${target_name} cocos2d)

set(CC_EXTENSION_LIBS "${target_name};${CC_EXTENSION_LIBS}" CACHE INTERNAL "extensions for auto link to target application")

set_target_properties(${target_name}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    FOLDER "Extensions"
)

        
