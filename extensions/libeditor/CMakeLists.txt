set(target_name libeditor)

FILE(GLOB_RECURSE EDITOR_SOURCES ./*.h;./*.cpp)

add_library(${target_name} ${EDITOR_SOURCES})

if(BUILD_SHARED_LIBS)
    target_compile_definitions(${target_name} 
                        PRIVATE _USRCCS_DLL=1
                        PRIVATE SPINEPLUGIN_API=DLLIMPORT
                        )
else()
    target_compile_definitions(${target_name} 
                        PRIVATE CC_STATIC=1
                        )
endif()

setup_cocos_extension_config(${target_name})
