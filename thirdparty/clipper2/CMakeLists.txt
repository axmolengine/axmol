
set(lib_name clipper2)
set(target_name ${lib_name})

project(${lib_name})

file(GLOB_RECURSE CLIPPER_SOURCES  *.h;*.cpp)

add_library(${target_name} STATIC ${CLIPPER_SOURCES})

target_include_directories(${target_name} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")
