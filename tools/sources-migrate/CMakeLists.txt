
set(target_name sources-migrate)
add_executable(${target_name} main.cpp)
target_include_directories(${target_name} 
    PRIVATE ${_AX_ROOT_PATH}/core
    PRIVATE ${_AX_ROOT_PATH}/thirdparty
)
set_target_properties(${target_name}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        FOLDER "Tools"
    )