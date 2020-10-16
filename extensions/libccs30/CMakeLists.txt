set(target_name libccs30)

FILE(GLOB_RECURSE CCS30_SOURCES ./*.h;./*.cpp)

add_library(${target_name} ${CCS30_SOURCES})

if(BUILD_SHARED_LIBS)
    target_compile_definitions(${target_name} 
                        PRIVATE _USRCCS_LDR_DLL=1
                        )
endif()

setup_cocos_extension_config(${target_name})
