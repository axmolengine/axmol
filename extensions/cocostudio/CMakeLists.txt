set(target_name cocostudio)

FILE(GLOB_RECURSE CCS_SOURCES ./*.h;./*.cpp)

add_library(${target_name} ${CCS_SOURCES})

if(BUILD_SHARED_LIBS)
    target_compile_definitions(${target_name} 
                        PRIVATE _USRCCS_DLL=1
                        )
else()
    target_compile_definitions(${target_name} 
                        PUBLIC AX_STATIC=1
                        )
endif()

target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_LIST_DIR})

setup_ax_extension_config(${target_name})
