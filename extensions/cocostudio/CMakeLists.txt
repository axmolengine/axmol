set(target_name cocostudio)

FILE(GLOB_RECURSE CCS_SOURCES ./*.h;./*.cpp)

add_library(${target_name} ${CCS_SOURCES})

if(BUILD_SHARED_LIBS)
    target_compile_definitions(${target_name} 
                        PRIVATE _USRCCS_DLL=1
                        )
else()
    target_compile_definitions(${target_name} 
                        PUBLIC CC_STATIC=1
                        )
endif()

target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_LIST_DIR})

setup_cocos_extension_config(${target_name})
