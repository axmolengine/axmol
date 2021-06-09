
set(lib_name edtaa3func)
set(target_name ${lib_name})

project(${lib_name})

add_library(${target_name} STATIC
  edtaa3func.cpp
)

target_include_directories(${target_name} PUBLIC .)

