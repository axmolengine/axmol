
set(lib_name glad)
set(target_name ${lib_name})

project(${target_name})

set(target_src_files)

# glad used on platforms: WINDOWS, LINUX, ANDROID, WASM
if(AX_ENABLE_GL)
  if (AX_GLES_PROFILE)
    list(APPEND target_src_files "include/glad/gles2.h" src/egl.c src/gles2.c)
    if (NOT ANDROID AND NOT WASM) # Win32, Andorid use system EGL
      list(APPEND target_src_files "include/glad/egl.h" src/egl.c src/egl.c)
    endif()
  else()
    list(APPEND target_src_files "include/glad/gl.h" src/gl.c)
  endif()
endif()

if(AX_ENABLE_VK)
  list(APPEND target_src_files "include/glad/vulkan.h" src/vulkan.c)
endif()

add_library(${target_name} ${target_src_files})

if(BUILD_SHARED_LIBS)
  target_compile_definitions(${target_name}
    PUBLIC GLAD_API_CALL_EXPORT=1
    PRIVATE GLAD_API_CALL_EXPORT_BUILD=1
  )

  if(NOT WINDOWS AND AX_ENABLE_GL)
    target_link_libraries(${target_name} EGL)
  endif()
endif()

if(AX_ENABLE_GL)
  if(ANDROID OR WASM)
    target_compile_definitions(${target_name}
      PUBLIC GLAD_GLES2_USE_SYSTEM_EGL=1 # refer: https://github.com/Dav1dde/glad/pull/438
    )
  endif()
endif()

if(ANDROID)
  if(AX_ENABLE_VK)
    target_compile_definitions(${target_name}
      PUBLIC VK_USE_PLATFORM_ANDROID_KHR=1
    )
  endif()
endif()

target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
