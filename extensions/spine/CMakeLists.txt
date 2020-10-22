set(target_name spine)

file(GLOB_RECURSE COCOS_SPINE_SRC 
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/**/*.cpp
)

file(GLOB_RECURSE COCOS_SPINE_HEADER 
    ${CMAKE_CURRENT_LIST_DIR}/*.h
    ${CMAKE_CURRENT_LIST_DIR}/**/*.h
)

add_library(${target_name} ${COCOS_SPINE_HEADER} ${COCOS_SPINE_SRC})

if(BUILD_SHARED_LIBS)
    target_compile_definitions(${target_name} 
        PRIVATE SPINEPLUGIN_API=DLLEXPORT
        PUBLIC CC_HAVE_EXTENSION_SPINE=1
    )
endif()

target_include_directories(${target_name} PUBLIC "runtime/include")

setup_cocos_extension_config(${target_name})
