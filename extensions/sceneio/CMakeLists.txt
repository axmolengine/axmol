set(target_name sceneio)

FILE(GLOB_RECURSE sceneio_SOURCES ./*.h;./*.cpp)

add_library(${target_name} ${sceneio_SOURCES})

if(BUILD_SHARED_LIBS)
    target_compile_definitions(${target_name}
                        PRIVATE SCNIO_EXPORTS=1
                        INTERFACE SCNIO_IMPORTS=1
                        )
    target_link_libraries(sceneio sceneext)
else()
    target_include_directories(${target_name} PUBLIC "${_AX_ROOT}/extensions/sceneext/src")
endif()

if(AX_ENABLE_EXT_SPINE)
    target_compile_definitions(${target_name} PUBLIC AX_ENABLE_EXT_SPINE=1)
    if(BUILD_SHARED_LIBS)
        target_link_libraries(${target_name} spine)
    else()
        target_include_directories(${target_name} PUBLIC "${_AX_ROOT}/extensions/spine/runtime/include" "${_AX_ROOT}/extensions/spine/src")
    endif()
endif()

if(AX_ENABLE_EXT_PARTICLE3D)
    target_compile_definitions(${target_name} PUBLIC AX_ENABLE_EXT_PARTICLE3D=1)
    if(BUILD_SHARED_LIBS)
        target_link_libraries(${target_name} particle3d)
    else()
        target_include_directories(${target_name} PUBLIC "${_AX_ROOT}/extensions/Particle3D/src")
    endif()
endif()

target_include_directories(${target_name} PUBLIC src)

setup_ax_extension_config(${target_name})
