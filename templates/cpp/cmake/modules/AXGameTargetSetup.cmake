if(NOT ANDROID)
  add_executable(${APP_NAME} ${APP_SOURCES})
else()
  add_library(${APP_NAME} SHARED ${APP_SOURCES})

  # whole archive for jni when not building engine as shared libs, otherwise libaxmol.so is archived with it.
  if(NOT BUILD_SHARED_LIBS)
    target_link_libraries(${APP_NAME} -Wl,--whole-archive cpp_android_spec -Wl,--no-whole-archive)
  endif()

  config_android_shared_libs("dev.axmol.lib" "${CMAKE_CURRENT_SOURCE_DIR}/proj.android/app/src")
endif()

if(NOT _AX_USE_PREBUILT)
  target_link_libraries(${APP_NAME} ${_AX_CORE_LIB})
endif()

if(APPLE)
  set_property(TARGET ${APP_NAME} PROPERTY XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC "YES")
  target_compile_options(${APP_NAME} PRIVATE
    "$<$<COMPILE_LANGUAGE:OBJC>:-fobjc-arc>"
    "$<$<COMPILE_LANGUAGE:OBJCXX>:-fobjc-arc>")
endif()

target_include_directories(${APP_NAME} PRIVATE ${GAME_INC_DIRS})
