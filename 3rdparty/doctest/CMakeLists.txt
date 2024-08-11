set(lib_name doctest)
set(target_name ${lib_name})

project(${lib_name})

add_library(${target_name} STATIC
  doctest.cpp
)

target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
