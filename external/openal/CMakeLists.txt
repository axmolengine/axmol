cmake_minimum_required(VERSION 3.6)

set(lib_name openal)
set(target_name ext_${lib_name})

project(${lib_name})

include(../cmake/CocosExternalConfig.cmake)

if(WINDOWS)
    add_library(${target_name} SHARED IMPORTED GLOBAL)
    set_target_properties(${target_name} PROPERTIES
       IMPORTED_LOCATION "${platform_spec_path}/OpenAL32.dll"
       IMPORTED_IMPLIB "${platform_spec_path}/OpenAL32.lib"
    )
elseif(APPLE)
    # TODO: switch to dynamic framework bundle after cmake-3.19 ready
    add_library(${target_name} STATIC IMPORTED GLOBAL)
    set_target_properties(${target_name} PROPERTIES
      IMPORTED_LOCATION "${platform_spec_path}/lib${lib_name}.a"
    )
else()
    add_library(${target_name} SHARED IMPORTED GLOBAL)
    set_target_properties(${target_name} PROPERTIES
      IMPORTED_LOCATION "${platform_spec_path}/lib${lib_name}.so"
    )
    if(ANDROID)
        # Notes: unused since we upgrade android gradle plugin to 4.0.1+
        # set(OPENAL_SO_PATH "${platform_spec_path}/lib${lib_name}.so" CACHE INTERNAL "OpenAL Library location" )
        # set(OPENAL_SO_NAME "lib${lib_name}.so" CACHE INTERNAL "OpenAL Library name" )
        set(ANDROID_SHARED_LOADS "${ANDROID_SHARED_LOADS}System.loadLibrary(\"openal\");" CACHE INTERNAL "Android Shared Loads" )
    endif()
endif()

set_target_properties(${target_name} PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include
)
