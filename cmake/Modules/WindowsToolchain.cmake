# WindowsToolchain.cmake not a general toolchain file, just include as early as possible
include_guard(GLOBAL)

if(NOT WIN32)
  return()
endif()

cmake_minimum_required(VERSION 3.27...4.1)

# Determine Compiler
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  message(STATUS "Using Windows clang generate axmol project, CLANG_VERSION: ${CLANG_VERSION_STRING}")
  set(FUZZ_CLANG TRUE) # clang-cl or clang++

  if(NOT MSVC)
    set(FULL_CLANG TRUE) # clang++
  else()
    set(FUZZ_MSVC TRUE) # clang-cl
  endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  # Visual Studio 2015, MSVC_VERSION 1900      (v140 toolset)
  # Visual Studio 2017, MSVC_VERSION 1910-1919 (v141 toolset)
  set(FUZZ_MSVC TRUE)
  set(FULL_MSVC TRUE)

  if(${MSVC_VERSION} EQUAL 1900 OR ${MSVC_VERSION} GREATER 1900)
    message(STATUS "Using Windows MSVC generate axmol project, MSVC_VERSION:${MSVC_VERSION}")
  else()
    message(FATAL_ERROR "Using Windows MSVC generate axmol project, MSVC_VERSION:${MSVC_VERSION} lower than needed")
  endif()
else()
  message(FATAL_ERROR "Please using Windows MSVC/LLVM-Clang compile axmol project")
endif()

# Determine Windows SDK version
if(FUZZ_MSVC)
  # don't support override windows sdk version when compiler is msvc, just detect
  if(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION)
    set(WINDOWS_SDK_VERSION "${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}" CACHE STRING "" FORCE)
  else() # Generator not Visual Studio
    set(INCLUDE_ENV "$ENV{INCLUDE}")
    string(TOLOWER "${INCLUDE_ENV}" INCLUDE_ENV_LOWER)
    string(REPLACE "\\" "/" INCLUDE_ENV_LOWER "${INCLUDE_ENV_LOWER}")
    string(REGEX MATCH "include/([0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+)/ucrt"
      _match "${INCLUDE_ENV_LOWER}")
    if(CMAKE_MATCH_1)
      set(WINDOWS_SDK_VERSION "${CMAKE_MATCH_1}" CACHE STRING "Detected Windows SDK version" FORCE)
      message(STATUS "🔍 Detected Windows SDK version: ${WINDOWS_SDK_VERSION}")
    else()
      message(FATAL_ERROR "⚠️ Could not detect Windows SDK version from INCLUDE env: ${INCLUDE_ENV}")
    endif()
  endif()
else()
  # Clang detecting SDK
  if(NOT WINDOWS_SDK_VERSION_DEFAULT)
    message(STATUS "Detecting Windows SDK version via clang ...")
    execute_process(
      COMMAND ${CMAKE_CXX_COMPILER} -v -E -x c++ NUL
      OUTPUT_VARIABLE CLANG_OUT
      ERROR_VARIABLE CLANG_ERR
    )
    set(CLANG_OUTPUT_ALL "${CLANG_OUT}\n${CLANG_ERR}")
    string(REGEX MATCH "Include[/\\][ \t]*([0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+)[/\\][ \t]*ucrt" _match "${CLANG_OUTPUT_ALL}")

    if(CMAKE_MATCH_1)
      set(WINDOWS_SDK_VERSION_DEFAULT "${CMAKE_MATCH_1}" CACHE STRING "Detected Windows SDK version" FORCE)
    else()
      message(FATAL_ERROR "Failed to detect Windows SDK version from clang++ output." "<CLANG_OUTPUT_ALL=${CLANG_OUTPUT_ALL}>")
    endif()
  endif()

  set(WINDOWS_SDK_VERSION ${WINDOWS_SDK_VERSION_DEFAULT} CACHE STRING "Windows SDK version")

  if(NOT DEFINED CLANG_TARGET_TRIPLE_DEFAULT)
    message(STATUS "Detecting Clang default target triple ...")
    execute_process(
      COMMAND ${CMAKE_CXX_COMPILER} -dumpmachine
      OUTPUT_VARIABLE CLANG_TRIPLE
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    # string(REGEX MATCH "^[^\\-]+" CLANG_COMPILER_TARGET_ARCH "${CLANG_TRIPLE}")
    set(CLANG_TARGET_TRIPLE_DEFAULT ${CLANG_TRIPLE} CACHE STRING "" FORCE)
    message(STATUS "🧠 Detected Clang default target triple: ${CLANG_TARGET_TRIPLE_DEFAULT}")
  endif()

  # x86_64-pc-windows-msvc
  # i686-pc-windows-msvc
  # aarch64-pc-windows-msvc
  set(TARGET_ARCH "x64" CACHE STRING "")

  if(TARGET_ARCH STREQUAL "x64")
    set(CLANG_TARGET_TRIPLE "x86_64-pc-windows-msvc" CACHE STRING "" FORCE)
    set(CMAKE_SIZEOF_VOID_P 8)
  elseif(TARGET_ARCH STREQUAL "x86")
    set(CLANG_TARGET_TRIPLE "i686-pc-windows-msvc" CACHE STRING "" FORCE)
    set(CMAKE_SIZEOF_VOID_P 4)
  elseif(TARGET_ARCH STREQUAL "arm64")
    set(CLANG_TARGET_TRIPLE "aarch64-pc-windows-msvc" CACHE STRING "" FORCE)
    set(CMAKE_SIZEOF_VOID_P 8)
  else()
    message(FATAL_ERROR "Unsupported target architecture: ${TARGET_ARCH}")
  endif()
endif()

function(find_windows_sdk OUT_VAR)
  if(NOT WIN32)
    message(FATAL_ERROR "find_windows_sdk only works on Windows.")
    return()
  endif()

  # Get SDK version from CMake (auto-detected by Visual Studio generator)
  if(NOT DEFINED WINDOWS_SDK_VERSION)
    message(FATAL_ERROR "WINDOWS_SDK_VERSION is not defined.")
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

function(find_windows_sdk_bin OUT_VAR ARCH)
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
  set(SDK_VERSION "${WINDOWS_SDK_VERSION}")
  set(SDK_BIN_PATH "${_sdk_root}/bin/${SDK_VERSION}/${ARCH}")

  if(EXISTS "${SDK_BIN_PATH}")
    set(${OUT_VAR} "${SDK_BIN_PATH}" PARENT_SCOPE)
  else()
    message(FATAL_ERROR "SDK bin path does not exist: ${SDK_BIN_PATH}")
    set(${OUT_VAR} "" PARENT_SCOPE)
  endif()
endfunction()

if(FULL_CLANG)
  add_compile_options(-Wno-nonportable-include-path)

  if(NOT CLANG_TARGET_TRIPLE STREQUAL CLANG_TARGET_TRIPLE_DEFAULT)
    message(STATUS "🔧 Using Clang with user-specified target: ${CLANG_TARGET_TRIPLE}")

    # add_compile_options("--target=${CLANG_TARGET_TRIPLE}")
    # add_link_options("--target=${CLANG_TARGET_TRIPLE}")
    # since cmake-3.20
    set(CMAKE_C_COMPILER_TARGET ${CLANG_TARGET_TRIPLE})
    set(CMAKE_CXX_COMPILER_TARGET ${CLANG_TARGET_TRIPLE})
  endif()

  if(NOT(WINDOWS_SDK_VERSION STREQUAL WINDOWS_SDK_VERSION_DEFAULT))
    find_windows_sdk(SDK_ROOT)
    message(STATUS "🔧 Using Clang with user-specified Windows SDK version: ${WINDOWS_SDK_VERSION}")
    message(STATUS "📁 Resolved Windows SDK root path: ${SDK_ROOT}")
    message(STATUS "📂 Resolved Windows SDK include path: ${SDK_ROOT}/Include/${WINDOWS_SDK_VERSION}")
    message(STATUS "📂 Resolved Windows SDK lib path: ${SDK_ROOT}/Lib/${WINDOWS_SDK_VERSION} for architecture: ${TARGET_ARCH}")

    set(SDK_INC "${SDK_ROOT}/Include/${WINDOWS_SDK_VERSION}")
    set(SDK_LIB "${SDK_ROOT}/Lib/${WINDOWS_SDK_VERSION}")

    include_directories(
      "${SDK_INC}/ucrt"
      "${SDK_INC}/shared"
      "${SDK_INC}/um"
      "${SDK_INC}/winrt"
    )

    link_directories(
      "${SDK_LIB}/ucrt/${TARGET_ARCH}"
      "${SDK_LIB}/um/${TARGET_ARCH}"
    )

    message(STATUS "✅ Windows SDK include/lib paths configured successfully.")
  endif()
endif()

message(STATUS "🔧 WINDOWS_SDK_VERSION:" "${WINDOWS_SDK_VERSION}")
message(STATUS "🔧 CMAKE_SYSTEM_VERSION:" "${CMAKE_SYSTEM_VERSION}")
message(STATUS "🔧 CMAKE_HOST_SYSTEM_VERSION:" "${CMAKE_HOST_SYSTEM_VERSION}")
message(STATUS "🔧 CMAKE_SIZEOF_VOID_P:" "${CMAKE_SIZEOF_VOID_P}")
message(STATUS "🔧 CMAKE_C_COMPILER_ARCHITECTURE_ID:" "${CMAKE_C_COMPILER_ARCHITECTURE_ID}")
message(STATUS "🔧 CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION:" "${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")

if(DEFINED CMAKE_VS_WINDOWS_TARGET_PLATFORM_MIN_VERSION)
  message(STATUS "🔧 CMAKE_VS_WINDOWS_TARGET_PLATFORM_MIN_VERSION: ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_MIN_VERSION}")
endif()
