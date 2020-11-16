cmake_minimum_required(VERSION 3.6)

set(lib_name glad)
set(target_name ${lib_name})

project(${target_name})

include(../cmake/CocosExternalConfig.cmake)

add_library(${target_name}
      ${${target_name}_src}
      include/glad/glad.h
      src/glad.c
    )

#~ set_target_properties(${target_name} PROPERTIES
#~   INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include
#~ )

if(BUILD_SHARED_LIBS)
  target_compile_definitions(${target_name}
    PUBLIC GLAD_GLAPI_EXPORT=1
    PRIVATE GLAD_GLAPI_EXPORT_BUILD=1
  )
endif()

target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(${target_name}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    FOLDER "External"
)
