set(lib_name tracy)
set(target_name ${lib_name})

project(${lib_name})

add_library(${target_name} STATIC
    TracyClient.cpp
)

target_include_directories(${target_name}
    INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}
)