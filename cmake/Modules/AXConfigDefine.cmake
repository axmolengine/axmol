include(${_AX_ROOT}/1k/platforms.cmake)

# custom target property for lua/js link
define_property(TARGET
  PROPERTY AX_LUA_DEPEND
  BRIEF_DOCS "axmol lua depend libs"
  FULL_DOCS "use to save depend libs of axmol lua project"
)

if(WINDOWS)
  cmake_minimum_required(VERSION 3.27...4.1)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:Debug,RelWithDebInfo>:Embedded>")

  set(_NUGET_PACKAGE_DIR "${_AX_ROOT}/cache/packages" CACHE INTERNAL "" FORCE)
  file(TO_NATIVE_PATH ${_NUGET_PACKAGE_DIR} _NUGET_PACKAGE_DIR_N)
  set(_NUGET_PACKAGE_DIR_N "${_NUGET_PACKAGE_DIR_N}" CACHE INTERNAL "" FORCE)

  if(${MSVC_VERSION} LESS 1900)
    message(FATAL_ERROR "MSVC_VER=1900 required, your version is:${MSVC_VERSION}")
  endif()
endif()

# UWP min deploy target support, VS property: targetPlatformMinVersion
if(WINRT)
  # The minmal deploy target version: Windows 10, version 1809 (Build 10.0.17763) for building msix package
  # refer to: https://learn.microsoft.com/en-us/windows/msix/supported-platforms?source=recommendations
  set(CMAKE_VS_WINDOWS_TARGET_PLATFORM_MIN_VERSION "10.0.17763" CACHE STRING "")
  set(AX_CPPWINRT_VERSION "2.0.250303.1" CACHE STRING "")

  # For axmol deprecated policy, we need disable /sdl checks explicitly to avoid compiler traits invoking deprecated functions as error
  set(CMAKE_C_FLAGS "/sdl- ${CMAKE_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "/sdl- ${CMAKE_CXX_FLAGS}")
elseif(WIN32)
  set(AX_MSEDGE_WEBVIEW2_VERSION "1.0.3485.44" CACHE STRING "")
endif()

if(ANDROID OR LINUX)
  set(CMAKE_C_FLAGS "-fPIC ${CMAKE_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "-fPIC ${CMAKE_CXX_FLAGS}")
endif()

# config c standard
if(WINDOWS)
  # CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION aka selected windows sdk version
  if(${WINDOWS_SDK_VERSION} VERSION_GREATER_EQUAL "10.0.22000.0")
    set(CMAKE_C_STANDARD 11)
  else()
    # windows sdk < 10.0.22000.0, The c11 header stdalign.h was missing, so workaroud fallback C standard to 99
    # refer to:
    # - https://github.com/axmolengine/axmol/issues/991
    # - https://github.com/axmolengine/axmol/issues/1246
    message(AUTHOR_WARNING "Forcing set CMAKE_C_STANDARD to 99 when winsdk < 10.0.22000.0")
    set(CMAKE_C_STANDARD 99)
  endif()
else()
  if(NOT DEFINED CMAKE_C_STANDARD)
    set(CMAKE_C_STANDARD 11)
  endif()
endif()

message(STATUS "CMAKE_C_STANDARD=${CMAKE_C_STANDARD}")

if(NOT DEFINED CMAKE_C_STANDARD_REQUIRED)
  set(CMAKE_C_STANDARD_REQUIRED ON)
endif()

# config c++ standard, minimal require c++23
set(_AX_MIN_CXX_STD 23)

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD ${_AX_MIN_CXX_STD})
endif()

if(CMAKE_CXX_STANDARD GREATER_EQUAL ${_AX_MIN_CXX_STD})
  message(STATUS "Building axmol with c++${CMAKE_CXX_STANDARD}")
else()
  message(STATUS "Building axmol require c++ std >= ${_AX_MIN_CXX_STD}")
endif()

# used to set 3rdparty c++ standard same with axmol
set(_AX_CXX_STD ${CMAKE_CXX_STANDARD} CACHE STRING "" FORCE)

if(NOT DEFINED CMAKE_CXX_STANDARD_REQUIRED)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

if(NOT DEFINED CMAKE_CXX_EXTENSIONS)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()

# Axmol not use c++20 modules, so disable cmake scan for modules
# benefits:
# 1. speed up build time
# 2. fix wasm build fail on windows host due to emsdk has bugs not trim some
# emsdk spec flags, i.e. -sUSE_LIBJPEG -msse2, -mss4.1 ...
# remark: The feature scan for modules was added in cmake 3.28
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

if(EMSCRIPTEN_VERSION)
  message(STATUS "Using emsdk, version: ${EMSCRIPTEN_VERSION}")
endif()

# Set macro definitions for special platforms
function(use_ax_compile_define target)
  target_compile_definitions(${target} PUBLIC $<$<CONFIG:Debug>:_AX_DEBUG=1>)

  # !important axmol not use double precision
  # target_compile_definitions(${target} PUBLIC CP_USE_CGTYPES=0)
  # target_compile_definitions(${target} PUBLIC CP_USE_DOUBLES=0)
  if(LINUX)
    ax_config_pred(${target} AX_ENABLE_VLC_MEDIA)
    target_compile_definitions(${target} PUBLIC _GNU_SOURCE)
  elseif(WINDOWS)
    ax_config_pred(${target} AX_ENABLE_VLC_MEDIA)
    target_compile_definitions(${target}
      PUBLIC WIN32
      PUBLIC _WIN32
      PUBLIC _WINDOWS
      PUBLIC UNICODE
      PUBLIC _UNICODE
      PUBLIC _CRT_SECURE_NO_WARNINGS
      PUBLIC _SCL_SECURE_NO_WARNINGS
      # PUBLIC GLAD_GLAPI_EXPORT
    )

    if(BUILD_SHARED_LIBS)
      target_compile_definitions(${target} PRIVATE AX_DLLEXPORT INTERFACE AX_DLLIMPORT)
    endif()
  endif()

  # render api
  if(AX_RENDER_API STREQUAL "gl")
    target_compile_definitions(${target} PUBLIC AX_RENDER_API=1)
    if(APPLE)
      target_compile_definitions(${target} PUBLIC GL_SILENCE_DEPRECATION=1)
    endif()
    if(AX_GLES_PROFILE)
      target_compile_definitions(${target} PUBLIC AX_GLES_PROFILE=${AX_GLES_PROFILE})
    else()
      target_compile_definitions(${target} PUBLIC AX_GLES_PROFILE=0)
    endif()
  elseif(AX_RENDER_API STREQUAL "mtl")
    target_compile_definitions(${target} PUBLIC AX_RENDER_API=2)
  elseif(AX_RENDER_API STREQUAL "d3d")
    target_compile_definitions(${target} PUBLIC AX_RENDER_API=3)
  endif()
endfunction()

# Set compiler options for engine lib: axmol
function(use_ax_compile_options target)
  if(FULL_MSVC)
    # Enable msvc multi-process building
    target_compile_options(${target} PUBLIC /MP)
  endif()

  if(WASM)
    # refer to: https://github.com/emscripten-core/emscripten/blob/main/src/settings.js
    target_link_options(${target} PUBLIC -sFORCE_FILESYSTEM=1 -sFETCH=1 -sUSE_GLFW=3)
  elseif(LINUX)
    target_link_options(${target} PUBLIC "-lpthread")
  endif()
endfunction()

# ----------- begin of axmol compile and link flags ===========
set(_ax_compile_opts)  # list: common compile options for all languages
set(_ax_link_opts)     # list: common link options for all languages
set(_ax_cxx_flags)     # list: extra compile flags for C++ only
set(_ax_c_flags)

if(FUZZ_MSVC)
  list(APPEND _ax_compile_opts /GF)
  list(APPEND _ax_cxx_flags "/Zc:char8_t-")
else() # others
  list(APPEND _ax_cxx_flags "-fno-char8_t")
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    list(APPEND _ax_compile_opts "-Wno-unknown-attributes" "-Wno-deprecated-literal-operator")
  elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    list(APPEND _ax_compile_opts "-Wno-attributes")
  else()
    message(AUTHOR_WARNING "Unknown compiler, may not works")
  endif()
endif()

if(APPLE)
  list(APPEND _ax_compile_opts
    "$<$<COMPILE_LANGUAGE:OBJC>:-Werror=objc-method-access>"
    "$<$<COMPILE_LANGUAGE:OBJCXX>:-Werror=objc-method-access>"
  )
endif()

if(EMSCRIPTEN)
  # Tell emcc build port libjpeg(not in sysroot)
  list(APPEND _ax_c_flags "-sUSE_LIBJPEG=1")

  # fix build fail on windows host when cmake invoking emscan-deps (raise unknown options)
  list(APPEND _ax_link_opts  "-ljpeg")

  # list(APPEND _ax_link_opts "-sASSERTIONS=1")

  set(AX_WASM_THREADS "4" CACHE STRING "Wasm threads count")
  set(_threads_hint "")

  if(AX_WASM_THREADS STREQUAL "auto") # not empty string or not 0
    # Enable pthread support globally
    set(_threads_hint "(auto)")
    include(ProcessorCount)
    set(_AX_WASM_THREADS_INT 0)
    ProcessorCount(_AX_WASM_THREADS_INT)
    set(AX_WASM_THREADS "${_AX_WASM_THREADS_INT}" CACHE STRING "Wasm threads count" FORCE)
  endif()

  message(STATUS "AX_WASM_THREADS=${AX_WASM_THREADS}${_threads_hint}")

  if((AX_WASM_THREADS MATCHES "^([0-9]+)$" AND AX_WASM_THREADS GREATER 0) OR AX_WASM_THREADS STREQUAL "navigator.hardwareConcurrency")
    list(APPEND _ax_compile_opts -pthread)
    list(APPEND _ax_link_opts -pthread -sPTHREAD_POOL_SIZE=${AX_WASM_THREADS})
  endif()

  option(AX_WASM_ALLOW_MEMORY_GROWTH "Allow wasm memory growth" ON)
  if(AX_WASM_ALLOW_MEMORY_GROWTH)
    set(AX_WASM_INITIAL_MEMORY "128MB" CACHE STRING "")
    list(APPEND _ax_link_opts -sALLOW_MEMORY_GROWTH=1)
  else()
    set(AX_WASM_INITIAL_MEMORY "1024MB" CACHE STRING "")
  endif()

  list(APPEND _ax_link_opts -sINITIAL_MEMORY=${AX_WASM_INITIAL_MEMORY})
endif()

# apply axmol spec compile options
if(_ax_compile_opts)
  add_compile_options(${_ax_compile_opts})
endif()

if(_ax_link_opts)
  add_link_options(${_ax_link_opts})
endif()

if(_ax_c_flags)
  string(JOIN " " _ax_c_flags "${_ax_c_flags}")
  string(APPEND CMAKE_C_FLAGS " ${_ax_c_flags}")
endif()

if(_ax_cxx_flags)
  string(JOIN " " _ax_cxx_flags "${_ax_cxx_flags}")
  string(APPEND CMAKE_CXX_FLAGS " ${_ax_cxx_flags}")
endif()

set(CMAKE_DEBUG_POSTFIX "" CACHE STRING "Library postfix for debug builds. Normally left blank." FORCE)
set(CMAKE_PLATFORM_NO_VERSIONED_SONAME TRUE CACHE BOOL "Disable dynamic libraries symblink." FORCE)

if(ANDROID)
  # Ensure fseeko available on ndk > 23
  math(EXPR _ARCH_BITS "${CMAKE_SIZEOF_VOID_P} * 8")
  add_definitions(-D_FILE_OFFSET_BITS=${_ARCH_BITS})

  # set hash style to both for android old device compatible
  # see also: https://github.com/axmolengine/axmol/discussions/614
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--hash-style=both")
endif()

# =========== end fo axmol compile and link flags ===========

if(APPLE)
  enable_language(C CXX OBJC OBJCXX)
else()
  enable_language(C CXX)
endif()

# Try enable asm & nasm compiler support
if(MSVC)
  enable_language(ASM_MASM OPTIONAL)
else()
  enable_language(ASM OPTIONAL)
endif()

enable_language(ASM_NASM OPTIONAL)

# we don't need cmake BUILD_TESTING feature
set(BUILD_TESTING FALSE CACHE BOOL "" FORCE)
