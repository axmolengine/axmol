set(target_name GUI)

FILE(GLOB_RECURSE GUI_CONTROL_SOURCES src/GUI/ControlExtension/*.h;src/GUI/ControlExtension/*.cpp)
FILE(GLOB_RECURSE GUI_SCROLLVIEW_SOURCES src/GUI/ScrollView/*.h;src/GUI/ScrollView/*.cpp)

SOURCE_GROUP("ControlExtension" FILES ${GUI_CONTROL_SOURCES})
SOURCE_GROUP("ScrollView" FILES ${GUI_SCROLLVIEW_SOURCES})

add_library(${target_name}
  ${GUI_CONTROL_SOURCES}
  ${GUI_SCROLLVIEW_SOURCES})

target_include_directories(${target_name}
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src
  INTERFACE ${CMAKE_CURRENT_LIST_DIR}/src/GUI
)

setup_ax_extension_config(${target_name})