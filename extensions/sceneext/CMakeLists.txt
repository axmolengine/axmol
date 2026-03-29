set(target_name sceneext)

FILE(GLOB_RECURSE sceneext_SOURCES ./*.h;./*.cpp)

add_library(${target_name} ${sceneext_SOURCES})

if(BUILD_SHARED_LIBS)
  target_compile_definitions(${target_name}
    PRIVATE SCNEXT_EXPORTS=1
    INTERFACE SCNEXT_IMPORTS=1
    PRIVATE SPINEPLUGIN_API=DLLIMPORT
  )
endif()

if(AX_ENABLE_EXT_SPINE)
  target_compile_definitions(${target_name} PUBLIC AX_ENABLE_EXT_SPINE=1)
  if(BUILD_SHARED_LIBS)
    target_link_libraries(${target_name} spine)
  else()
    target_include_directories(${target_name} PUBLIC "${_AX_ROOT}/extensions/spine/runtime/include" "${_AX_ROOT}/extensions/spine/src")
  endif()
endif()

target_include_directories(${target_name} PUBLIC src)
setup_ax_extension_config(${target_name})
