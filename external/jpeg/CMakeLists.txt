
set(lib_name jpeg)
set(target_name ext_${lib_name})

project(${lib_name})

include(../cmake/CocosExternalConfig.cmake)

add_library(${target_name} STATIC IMPORTED GLOBAL)

set_target_properties(${target_name} PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

if(WINDOWS)
    set_target_properties(${target_name} PROPERTIES
        IMPORTED_LOCATION "${platform_spec_path}/jpeg-static.lib"
    )
else()
    set_target_properties(${target_name} PROPERTIES
        IMPORTED_LOCATION "${platform_spec_path}/libjpeg.a"
    )
endif()
