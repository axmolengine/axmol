# config quick starter batch script run.bat for windows
if(WIN32)
    file(RELATIVE_PATH CMAKE_BUILD_RELATIVE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/run.bat.in" "${CMAKE_CURRENT_SOURCE_DIR}/run.bat" @ONLY)
endif()

if(WINDOWS)
    if(NOT _AX_USE_PREBUILT)
        ax_sync_target_dlls(${APP_NAME})
    endif()
endif()