
set(lib_name glad)
set(target_name ${lib_name})

project(${target_name})

if(AX_RENDER_API STREQUAL "gl")
  set(target_src_files "include/glad/gl.h" src/gl.c)
elseif(AX_RENDER_API STREQUAL "vk")
  set(target_src_files "include/glad/vulkan.h" src/vulkan.c)
endif()

add_library(${target_name} ${target_src_files})

if(BUILD_SHARED_LIBS)
  target_compile_definitions(${target_name}
    PUBLIC GLAD_API_CALL_EXPORT=1
    PRIVATE GLAD_API_CALL_EXPORT_BUILD=1
  )

  if(NOT WINDOWS AND AX_USE_GL)
    target_link_libraries(${target_name} EGL)
  endif()
endif()

if(ANDROID)
  if(AX_RENDER_API STREQUAL "gl")
    target_compile_definitions(${target_name}
        PUBLIC GLAD_GLES2=1
        PUBLIC GLAD_GLES2_USE_SYSTEM_EGL=1 # refer: https://github.com/Dav1dde/glad/pull/438
    )
  elseif(AX_RENDER_API STREQUAL "vk")
    target_compile_definitions(${target_name}
      PUBLIC VK_USE_PLATFORM_ANDROID_KHR=1
    )
  endif()
endif()

target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
