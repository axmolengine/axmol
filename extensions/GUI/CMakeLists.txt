set(target_name GUI)

FILE(GLOB GUI_CONTROL_SOURCES ControlExtension/*.h;ControlExtension/*.cpp)
FILE(GLOB GUI_SCROLLVIEW_SOURCES ScrollView/*.h;ScrollView/*.cpp)

SOURCE_GROUP("ControlExtension" FILES ${GUI_CONTROL_SOURCES})
SOURCE_GROUP("ScrollView" FILES ${GUI_SCROLLVIEW_SOURCES})

add_library(${target_name} 
        ${GUI_CONTROL_SOURCES} 
        ${GUI_SCROLLVIEW_SOURCES})

target_include_directories(${target_name} INTERFACE ${CMAKE_CURRENT_LIST_DIR})
        
setup_ax_extension_config(${target_name})