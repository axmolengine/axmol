# Detects Windows SDK installation path

function(find_windows_sdk OUT_VAR)
  if(NOT WIN32)
    message(FATAL_ERROR "find_windows_sdk only works on Windows.")
    return()
  endif()

  # Get SDK version from CMake (auto-detected by Visual Studio generator)
  if(NOT DEFINED CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION)
    message(FATAL_ERROR "CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION is not defined.")
    return()
  endif()

  # Check if SDK root is already cached
  if(NOT DEFINED WINDOWS_SDK_ROOT)
    # Query registry for KitsRoot10
    # Try main registry key first
    execute_process(
      COMMAND reg query "HKLM\\SOFTWARE\\Microsoft\\Windows Kits\\Installed Roots" /v KitsRoot10
      OUTPUT_VARIABLE SDK_PATH_RAW
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_QUIET
    )

    # Fallback to WOW6432Node if needed
    if("${SDK_PATH_RAW}" STREQUAL "")
      execute_process(
        COMMAND reg query "HKLM\\SOFTWARE\\WOW6432Node\\Microsoft\\Windows Kits\\Installed Roots" /v KitsRoot10
        OUTPUT_VARIABLE SDK_PATH_RAW
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
      )
    endif()

    string(REGEX MATCH "KitsRoot10[ \t]+REG_SZ[ \t]+(.+)" _match "${SDK_PATH_RAW}")
    set(SDK_ROOT "${CMAKE_MATCH_1}")
    file(TO_CMAKE_PATH "${SDK_ROOT}" SDK_ROOT)

    if(EXISTS "${SDK_ROOT}")
      set(WINDOWS_SDK_ROOT "${SDK_ROOT}" CACHE STRING "Windows SDK root path")
      message(STATUS "Detected Windows SDK root: ${WINDOWS_SDK_ROOT}")
    else()
      message(FATAL_ERROR "Windows SDK root not found in registry.")
    endif()
  endif()

  set(${OUT_VAR} "${WINDOWS_SDK_ROOT}" PARENT_SCOPE)
endfunction()

function (find_windows_sdk_bin OUT_VAR ARCH)
  # Supported architectures
  set(SUPPORTED_ARCHS x86 x64 arm64)
  list(FIND SUPPORTED_ARCHS "${ARCH}" ARCH_INDEX)

  if(ARCH_INDEX EQUAL -1)
    message(FATAL_ERROR "Unsupported architecture: ${ARCH}")
    return()
  endif()

  # find windows install dir
  find_windows_sdk(_sdk_root)

  # Construct bin path for the given architecture
  set(SDK_VERSION "${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")
  set(SDK_BIN_PATH "${_sdk_root}/bin/${SDK_VERSION}/${ARCH}")

  if(EXISTS "${SDK_BIN_PATH}")
    set(${OUT_VAR} "${SDK_BIN_PATH}" PARENT_SCOPE)
  else()
    message(FATAL_ERROR "SDK bin path does not exist: ${SDK_BIN_PATH}")
    set(${OUT_VAR} "" PARENT_SCOPE)
  endif()
endfunction()
