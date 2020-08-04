
set(target_name fairygui)

FILE(GLOB_RECURSE FAIRYGUI_SOURCES *.h;*.cpp)

add_library(${target_name} STATIC ${FAIRYGUI_SOURCES})

target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

setup_cocos_extension_config(${target_name})
