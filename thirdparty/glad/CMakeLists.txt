
set(lib_name glad)
set(target_name ${lib_name})

project(${target_name})

add_library(${target_name} 
    "include/glad/gl.h" 
    src/gl.c)

if(BUILD_SHARED_LIBS)
  target_compile_definitions(${target_name}
    PUBLIC GLAD_API_CALL_EXPORT=1
    PRIVATE GLAD_API_CALL_EXPORT_BUILD=1
  )
endif()

target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
