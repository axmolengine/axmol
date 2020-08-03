set(target_name GUI)

FILE(GLOB GUI_CONTROL_SOURCES CCControlExtension/*.h;CCControlExtension/*.cpp)
FILE(GLOB GUI_SCROLLVIEW_SOURCES CCScrollView/*.h;CCScrollView/*.cpp)

SOURCE_GROUP("CCControlExtension" FILES ${GUI_CONTROL_SOURCES})
SOURCE_GROUP("CCScrollView" FILES ${GUI_SCROLLVIEW_SOURCES})

# TODO: dll
add_library(${target_name} STATIC 
        ${GUI_CONTROL_SOURCES} 
        ${GUI_SCROLLVIEW_SOURCES})
        
setup_cocos_extension_config(${target_name})
