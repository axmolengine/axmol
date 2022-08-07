
set(lib_name cpufeatures)
set(target_name ${lib_name})

project(${lib_name})

add_library(${target_name} ${CMAKE_ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c)
set(cpufeatures_include_dir ${CMAKE_ANDROID_NDK}/sources/android/cpufeatures CACHE INTERNAL "cpufeatures include directory")

set_target_properties(${target_name} PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES ${cpufeatures_include_dir})

target_link_libraries(${target_name} dl)
