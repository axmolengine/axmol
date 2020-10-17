set(target_name spine)

file(GLOB_RECURSE COCOS_SPINE_SRC 
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/**/*.cpp
)

file(GLOB_RECURSE COCOS_SPINE_HEADER 
    ${CMAKE_CURRENT_LIST_DIR}/*.h
    ${CMAKE_CURRENT_LIST_DIR}/**/*.h
)

# add_subdirectory(runtime)
# list(REMOVE_ITEM COCOS_SPINE_SRC "${CMAKE_CURRENT_LIST_DIR}/main.cpp")

add_library(${target_name} ${COCOS_SPINE_HEADER} ${COCOS_SPINE_SRC})

if(BUILD_SHARED_LIBS)
    target_compile_definitions(${target_name} 
        PRIVATE SPINEPLUGIN_API=DLLEXPORT
        PUBLIC CC_HAVE_EXTENSION_SPINE=1
    )
endif()

target_include_directories(${target_name} PUBLIC "runtime/include")

# set_target_properties(spine-cpp
#      PROPERTIES
#      ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
#      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
#      FOLDER "Extensions")

# since 3.6/3.7 c runtime support build as DLL
# since 3.8 cpp runtime is preferred for cocos2d-x
# if(BUILD_EXT_SPINE_CPP)
#     add_subdirectory(spine-cpp)
#     target_link_libraries(external spine-cpp)
#     set_target_properties(spine-cpp
#     PROPERTIES
#     ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
#     LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
#     FOLDER "External"
# )
# 
# endif(BUILD_EXT_SPINE_CPP)

setup_cocos_extension_config(${target_name})
