
set(lib_name poly2tri)
set(target_name ${lib_name})

project(${lib_name})

set(${target_name}_src
  common/shapes.cc
  sweep/sweep.cc
  sweep/sweep_context.cc
  sweep/cdt.cc
  sweep/advancing_front.cc
)

add_library(${target_name} STATIC
  ${${target_name}_src}
)

target_include_directories(${target_name} PUBLIC ..)
