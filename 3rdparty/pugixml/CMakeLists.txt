
set(lib_name pugixml)
set(target_name ${lib_name})

project(${lib_name})

set(${target_name}_src
   pugixml.cpp
  )
  
if(WINDOWS)
  add_library(${target_name} ${${target_name}_src})
  if(BUILD_SHARED_LIBS)
    target_compile_definitions(${target_name} PRIVATE $<$<CXX_COMPILER_ID:MSVC>:PUGIXML_API=__declspec\(dllexport\)>)
  endif()
else()
  add_library(${target_name} STATIC
    ${${target_name}_src}
  )
  target_include_directories(${target_name} PRIVATE .)
endif()

set_target_properties(${target_name} PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}"
)

if(BUILD_SHARED_LIBS AND MSVC)
    set_target_properties(${target_name} PROPERTIES
        INTERFACE_COMPILE_DEFINITIONS PUGIXML_API=__declspec\(dllimport\)
    )
endif()
