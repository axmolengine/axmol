
set(lib_name box2d)
set(target_name ${lib_name})

project(${lib_name})

FILE(GLOB_RECURSE box2d_sources *.h;*.c)

add_library(${target_name} STATIC ${box2d_sources})

# Atomics are still considered experimental in Visual Studio 17.8
if (FULL_MSVC)
    target_compile_options(box2d PRIVATE /experimental:c11atomics)
endif()

set_target_properties(${target_name} PROPERTIES C_STANDARD 17 C_STANDARD_REQUIRED TRUE)

target_include_directories(${target_name} 
    PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
