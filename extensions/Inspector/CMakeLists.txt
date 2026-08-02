set(target_name Inspector)

FILE(GLOB_RECURSE INSPECTOR_SOURCES *.h;*.cpp;./**/*.h;./**/*.cpp)

add_library(${target_name} STATIC ${INSPECTOR_SOURCES})
target_include_directories(
  ${target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
)
ax_setup_ext_config(${target_name})
