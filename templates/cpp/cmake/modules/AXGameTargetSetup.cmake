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

target_include_directories(${APP_NAME} PRIVATE ${GAME_INC_DIRS})

if (_AX_TESTS)
  add_executable(${TEST_TARGET_NAME} ${TEST_TARGET_SOURCES})

  if(NOT _AX_USE_PREBUILT)
    target_link_libraries(${TEST_TARGET_NAME} ${_AX_CORE_LIB})
  endif()

  target_include_directories(${TEST_TARGET_NAME} PRIVATE ${TEST_INC_DIRS})
  target_link_libraries(${TEST_TARGET_NAME} doctest)
  target_compile_definitions(${TEST_TARGET_NAME} PRIVATE _AX_TESTS=1)
endif()
