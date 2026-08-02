
set(target_name fairygui)

FILE(GLOB_RECURSE FAIRYGUI_SOURCES *.h;*.cpp)

add_library(${target_name} STATIC ${FAIRYGUI_SOURCES})

target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(${target_name}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/fairygui

  INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src
  INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src/fairygui
)

ax_setup_ext_config(${target_name})
