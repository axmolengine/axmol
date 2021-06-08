
set(lib_name ConvertUTF)
set(target_name ${lib_name})

project(${lib_name})

add_library(${target_name} STATIC
  ConvertUTF.cpp
)

target_include_directories(${target_name} PUBLIC .)
