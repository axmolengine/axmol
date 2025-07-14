set(target_name cocostudio)

FILE(GLOB_RECURSE CCS_SOURCES *.h;*.cpp;./**/*.h;./**/*.cpp)

add_library(${target_name} ${CCS_SOURCES})

if(BUILD_SHARED_LIBS)
  target_compile_definitions(${target_name} PRIVATE CCS_DLLEXPORT INTERFACE CCS_DLLIMPORT)
endif()

target_include_directories(${target_name} PUBLIC src)

setup_ax_extension_config(${target_name})
