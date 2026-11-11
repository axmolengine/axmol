
set(lib_name vkma)
set(target_name ${lib_name})

project(${lib_name})

set(${target_name}_src
  vk_mem_alloc.h
  vk_mem_alloc.cpp
)

if(MSVC_IDE)
  list(APPEND ${target_name}_src vk_mem_alloc.natvis)
endif()

add_library(${target_name}
  ${${target_name}_src}
)

if(WIN32 AND BUILD_SHARED_LIBS)
  target_compile_definitions(${target_name}
    PRIVATE VMA_CALL_PRE=__declspec\(dllexport\)
    INTERFACE VMA_CALL_PRE=__declspec\(dllimport\)
  )
endif()

target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${target_name} glad)
