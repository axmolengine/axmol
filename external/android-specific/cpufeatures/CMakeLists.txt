
set(lib_name cpufeatures)
set(target_name ${lib_name})

project(${lib_name})

# CMake from android SDK set ANDROID_NDK instead of CMAKE_ANDROID_NDK
if(ANDROID_NDK)
    add_library(${target_name} ${ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c)
    set(cpufeatures_include_dir ${ANDROID_NDK}/sources/android/cpufeatures CACHE INTERNAL "cpufeatures include directory")
else()
    add_library(${target_name} ${CMAKE_ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c)
    set(cpufeatures_include_dir ${CMAKE_ANDROID_NDK}/sources/android/cpufeatures CACHE INTERNAL "cpufeatures include directory")
endif()

set_target_properties(${target_name} PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES ${cpufeatures_include_dir})

target_link_libraries(${target_name} dl)
