set(target_name spine)

file(GLOB_RECURSE _AX_SPINE_SRC 
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/**/*.cpp
)

file(GLOB_RECURSE _AX_SPINE_HEADER 
    ${CMAKE_CURRENT_LIST_DIR}/*.h
    ${CMAKE_CURRENT_LIST_DIR}/**/*.h
)

add_library(${target_name} ${_AX_SPINE_HEADER} ${_AX_SPINE_SRC})

if(BUILD_SHARED_LIBS)
    target_compile_definitions(${target_name} 
        PRIVATE SPINEPLUGIN_API=DLLEXPORT
        PUBLIC AX_HAVE_EXTENSION_SPINE=1
    )
endif()

target_include_directories(${target_name} PUBLIC "runtime/include")

setup_ax_extension_config(${target_name})
