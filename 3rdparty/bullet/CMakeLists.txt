
set(lib_name bullet)
set(target_name ${lib_name})

project(${lib_name})

file(GLOB_RECURSE bullet_SOURCES *.h;*.cpp;./**/*.h;./**/*.cpp)

add_library(${target_name} STATIC ${bullet_SOURCES})

target_include_directories(${target_name} PUBLIC .)

if(WIN32)
  target_compile_definitions(${target_name} PUBLIC BT_USE_SSE_IN_API=1)
endif()
