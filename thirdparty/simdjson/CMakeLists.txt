
set(lib_name simdjson)
set(target_name ${lib_name})

project(${target_name})

set(target_src_files simdjson.h simdjson.cpp)

add_library(${target_name} STATIC ${target_src_files})

# notworking properly: "double __cdecl simdjson::internal::from_chars(char const *)
# if(BUILD_SHARED_LIBS)
#     target_compile_definitions(${target_name} PRIVATE SIMDJSON_BUILDING_WINDOWS_DYNAMIC_LIBRARY=1)
#     target_compile_definitions(${target_name} INTERFACE SIMDJSON_USING_WINDOWS_DYNAMIC_LIBRARY=1)
# endif()
