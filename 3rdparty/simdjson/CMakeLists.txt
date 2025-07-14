
set(lib_name simdjson)
set(target_name ${lib_name})

project(${target_name})

set(target_src_files simdjson.h simdjson.cpp)

add_library(${target_name} STATIC ${target_src_files})
