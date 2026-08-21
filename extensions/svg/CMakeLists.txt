set(target_name svg)

add_library(${target_name} STATIC
  src/SVGImageDecoder.h
  src/SVGImageDecoder.cpp
)

target_include_directories(${target_name}
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src
)

target_link_libraries(${target_name} lunasvg)

ax_setup_ext_config(${target_name})
