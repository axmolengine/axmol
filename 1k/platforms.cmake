# Please use them everywhere
# WINDOWS   =   Windows Desktop
# ANDROID    =  Android
# IOS    =  iOS
# MACOSX    =  MacOS X
# LINUX      =   Linux

# ############################
# cmake commands:
# win32: cmake -B build -A x64
# winrt: cmake -B build -A x64 -DCMAKE_SYSTEM_NAME=WindowsStore "-DCMAKE_SYSTEM_VERSION=10.0"
#

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(WINDOWS TRUE)

  include(${CMAKE_CURRENT_LIST_DIR}/windows.cmake)

  if(CMAKE_GENERATOR_PLATFORM)
    string(TOLOWER "${CMAKE_GENERATOR_PLATFORM}" _target_cpu)
  elseif(CLANG_TARGET_TRIPLE)
    set(_target_cpu ${CLANG_TARGET_TRIPLE})
  endif()

  if(_target_cpu)
    if("${_target_cpu}" MATCHES "win32|i686")
      set(WIN32 TRUE)
      set(ARCH_ALIAS "x86")
    elseif("${_target_cpu}" MATCHES "arm64|aarch64")
      set(WIN64 TRUE)
      set(ARCH_ALIAS "arm64")
    else()
      set(WIN64 TRUE)
      set(ARCH_ALIAS "x64")
    endif()
  else()
    # https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_COMPILER_ARCHITECTURE_ID.html
    string(TOLOWER "${CMAKE_C_COMPILER_ARCHITECTURE_ID}" ARCH_ALIAS)

    if(ARCH_ALIAS MATCHES "64")
      set(WIN64 TRUE)
    endif()
  endif()

  set(PLATFORM_NAME win32)

  if(${CMAKE_SYSTEM_NAME} MATCHES "WindowsStore")
    set(WINRT TRUE CACHE BOOL "" FORCE)
    set(PLATFORM_NAME winrt)
  endif()
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Android")
  set(PLATFORM_NAME android)
  set(ARCH_ALIAS ${ANDROID_ABI})

  # refer
  # - https://developer.android.com/about/versions/15/behavior-changes-all#16-kb
  # - https://developer.android.google.cn/about/versions/15/behavior-changes-all?hl=zh-cn#16-kb
  set(_16KPAGE_SIZE_LD_FLAGS "-Wl,-z,max-page-size=16384")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(LINUX TRUE)
  set(PLATFORM_NAME linux)

  if(NOT DEFINED ARCH_ALIAS)
    message(STATUS "Detecting build target triple ...")
    execute_process(
      COMMAND ${CMAKE_CXX_COMPILER} -dumpmachine
      OUTPUT_VARIABLE _target_cpu
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    message(STATUS "Detected build target triple: ${_target_cpu}")

    if(_target_cpu MATCHES "x86_64")
      set(ARCH_ALIAS "x64")
    elseif(_target_cpu MATCHES "arm64|aarch64")
      set(ARCH_ALIAS "arm64")
    else()
      message(FATAL_ERROR "Unsupported platform: ${_target_cpu}")
    endif()
  endif()
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
  set(WASM TRUE)
  set(EMSCRIPTEN TRUE)

  if("${CMAKE_LIBRARY_ARCHITECTURE}" MATCHES "64")
    set(PLATFORM_NAME wasm64)
  else()
    set(PLATFORM_NAME wasm)
  endif()
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(APPLE TRUE)
  set(MACOSX TRUE)
  set(PLATFORM_NAME mac)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "iOS")
  set(APPLE TRUE)
  set(IOS TRUE)
  set(PLATFORM_NAME ios)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "tvOS")
  set(APPLE TRUE)
  set(IOS TRUE)
  set(TVOS TRUE)
  set(PLATFORM_NAME tvos)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "OHOS")
  set(OHOS TRUE)
  set(PLATFORM_NAME ohos)
else()
  message(AUTHOR_WARNING "Unhandled platform: ${CMAKE_SYSTEM_NAME}")
endif()

if(NOT "${ARCH_ALIAS}" STREQUAL "")
  set(ARCH_ALIAS ${ARCH_ALIAS} CACHE STRING "" FORCE)
else()
  # CMAKE_SYSTEM_PROCESSOR: AMD64, ARM64, aarch64, x86_64, i686(android x86 already handled)
  string(TOLOWER ${CMAKE_SYSTEM_PROCESSOR} _target_cpu)

  if(_target_cpu MATCHES "x86_64|amd64")
    set(ARCH_ALIAS x64 CACHE STRING "" FORCE)
  elseif(_target_cpu MATCHES "arm64|aarch64")
    set(ARCH_ALIAS arm64 CACHE STRING "" FORCE)
  elseif(_target_cpu MATCHES "armv7")
    set(ARCH_ALIAS armv7 CACHE STRING "" FORCE)
  else()
    message(WARNING "Unknown architecture: ${_target_cpu}")
  endif()
endif()

message(STATUS "🔧 Evaluated ARCH_ALIAS=${ARCH_ALIAS}")

if(NOT DEFINED WIN32)
  set(WIN32 FALSE)
endif()

if(NOT DEFINED WASM)
  set(WASM FALSE)
endif()

# compiler id
if(NOT WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(FULL_CLANG TRUE)
endif()

# generators that are capable of organizing into a hierarchy of folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# simplify generator condition, please use them everywhere
if(CMAKE_GENERATOR STREQUAL Xcode)
  set(XCODE TRUE)
elseif(CMAKE_GENERATOR MATCHES Visual)
  set(VS TRUE)
endif()

# The global rpath settings
if(LINUX OR APPLE)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE CACHE BOOL "" FORCE)

  if(LINUX)
    set(CMAKE_INSTALL_RPATH ".:\$ORIGIN:\$ORIGIN/../lib:${CMAKE_BINARY_DIR}/lib")
  elseif(APPLE)
    set(CMAKE_SKIP_BUILD_RPATH TRUE CACHE BOOL "" FORCE)

    if(IOS)
      set(CMAKE_INSTALL_RPATH "/usr/lib/swift" "@executable_path/Frameworks")
    else()
      set(CMAKE_INSTALL_RPATH "@executable_path/../Frameworks")
    endif()
  endif()
endif()

function(_1k_deprecated_32bit project_name release_ver)
  if(("${ARCH_ALIAS}" STREQUAL "x86") OR("${ARCH_ALIAS}" MATCHES "armeabi-v7a"))
    message(AUTHOR_WARNING "Building 32-bit[${ARCH_ALIAS}] ${project_name} is deprecated, and will be removed in next release ${release_ver}")
  endif()
endfunction()
