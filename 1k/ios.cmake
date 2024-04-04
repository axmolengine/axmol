#
# The minimal ios toolchain file: https://github.com/simdsoft/1kiss/blob/dev/1k/ios.cmake
# version: 4.1.3
#
# The supported params:
#   PLAT: iOS, tvOS, default: iOS
#   ARCHS: arm64, x86_64, default: arm64
#   DEPLOYMENT_TARGET: default: iOS=11.0/12.0, tvOS=15.0, watchOS=8.0
#   SIMULATOR: TRUE, FALSE, UNDEFINED(auto determine by archs) 
#   ENABLE_BITCODE: FALSE(default)
# 
# !!!Note: iOS simulator, there is no xcode General tab, and we must set
# CMAKE_OSX_SYSROOT properly for simulator, otherwise will lead cmake based
# project detect C/C++ header from device sysroot which is not present in 
# simulator sysroot, then cause compiling errors
#
#

# PLAT
if(NOT DEFINED PLAT)
    set(PLAT iOS CACHE STRING "" FORCE)
endif()

# ARCHS
if(NOT DEFINED ARCHS)
    set(ARCHS "arm64" CACHE STRING "" FORCE)
endif()

# DEPLOYMENT_TARGET
if(NOT DEFINED DEPLOYMENT_TARGET)
    if (PLAT STREQUAL "iOS")
        if("${ARCHS}" MATCHES ".*armv7.*")
           set(DEPLOYMENT_TARGET "10.0" CACHE STRING "" FORCE)
        else()
           if (XCODE_VERSION LESS "14.3.0")
               set(DEPLOYMENT_TARGET "11.0" CACHE STRING "" FORCE)
           else() # xcode 14.3+ require 12.0 for c++ std::get
               set(DEPLOYMENT_TARGET "12.0" CACHE STRING "" FORCE)
           endif()
        endif()
    elseif (PLAT STREQUAL "tvOS")
        set(DEPLOYMENT_TARGET "15.0" CACHE STRING "" FORCE)
    elseif (PLAT STREQUAL "watchOS")
        set(DEPLOYMENT_TARGET "8.0" CACHE STRING "" FORCE)
    else()
        message(FATAL_ERROR "PLAT=${PLAT} unsupported!")
    endif()
endif()

# SIMULATOR, regards x86_64 as simulator if SIMULATOR not defined
if((NOT DEFINED SIMULATOR) AND ("${ARCHS}" STREQUAL "x86_64"))
    set(SIMULATOR TRUE CACHE BOOL "" FORCE)
endif()

# ENABLE_BITCODE, default OFF, xcode14: Building with bitcode is deprecated. Please update your project and/or target settings to disable bitcode
if(NOT DEFINED ENABLE_BITCODE)
    set(ENABLE_BITCODE FALSE CACHE BOOL "" FORCE)
endif()

# apply params
set(CMAKE_SYSTEM_NAME ${PLAT} CACHE STRING "")
set(CMAKE_OSX_ARCHITECTURES ${ARCHS} CACHE STRING "")
set(CMAKE_OSX_DEPLOYMENT_TARGET ${DEPLOYMENT_TARGET} CACHE STRING "")

# The best solution for fix try_compile failed with code sign currently
# since cmake-3.18.2, not required
# everyting for cmake toolchain config before project(xxx) is better
set(CMAKE_TRY_COMPILE_PLATFORM_VARIABLES
    ${CMAKE_TRY_COMPILE_PLATFORM_VARIABLES}
    "CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED"
    "CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED")
set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED NO)
set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED NO)

# set(CMAKE_BUILD_WITH_INSTALL_RPATH YES)
# set(CMAKE_INSTALL_RPATH "@executable_path/Frameworks")
# set(CMAKE_XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "@executable_path/Frameworks" ${CMAKE_XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS})

# Fix compile failed with armv7 deployment target >= 11.0, xcode clang will report follow error
# clang: error: invalid iOS deployment version '--target=armv7-apple-ios13.6', 
#        iOS 10 is the maximum deployment target for 32-bit targets
# If not defined CMAKE_OSX_DEPLOYMENT_TARGET, cmake will choose latest deployment target
if(NOT DEFINED CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET)
    set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET ${CMAKE_OSX_DEPLOYMENT_TARGET} CACHE STRING "")
endif()

if (NOT SIMULATOR)
    if(PLAT STREQUAL "iOS")
        set (CMAKE_XCODE_EFFECTIVE_PLATFORMS "-iphoneos")
    elseif(PLAT STREQUAL "tvOS")
        set (CMAKE_XCODE_EFFECTIVE_PLATFORMS "-appletvos")
    elseif(PLAT STREQUAL "watchOS")
        set (CMAKE_XCODE_EFFECTIVE_PLATFORMS "-watchos")
    endif()
else()
    if (PLAT STREQUAL "iOS")
        set(_SDK_NAME "iphonesimulator")
        set (CMAKE_XCODE_EFFECTIVE_PLATFORMS "-iphonesimulator")
    elseif(PLAT STREQUAL "tvOS")
        set(_SDK_NAME "appletvsimulator")
        set (CMAKE_XCODE_EFFECTIVE_PLATFORMS "-tvsimulator")

    elseif(PLAT STREQUAL "watchOS")
        set(_SDK_NAME "watchsimulator")
        set (CMAKE_XCODE_EFFECTIVE_PLATFORMS "-watchsimulator")
    else()
        message(FATAL_ERROR "PLAT=${PLAT} unsupported!")
    endif()
    
    find_program(XCODEBUILD_PROG xcodebuild)
    if(NOT XCODEBUILD_PROG)
        message(FATAL_ERROR "xcodebuild not found. Please install either the standalone commandline tools or Xcode.")
    endif()
    execute_process(COMMAND ${XCODEBUILD_PROG} -version -sdk ${_SDK_NAME} SDKVersion
          OUTPUT_VARIABLE _SDK_VER
          ERROR_QUIET
          OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(CMAKE_OSX_SYSROOT "${_SDK_NAME}${_SDK_VER}" CACHE STRING "")
endif() 

# Since xcode14, the bitcode was marked deprecated, so we disable by default
if(ENABLE_BITCODE)
    set(CMAKE_XCODE_ATTRIBUTE_BITCODE_GENERATION_MODE "bitcode")
    set(CMAKE_XCODE_ATTRIBUTE_ENABLE_BITCODE "YES")
else()
    set(CMAKE_XCODE_ATTRIBUTE_ENABLE_BITCODE "NO")
endif()

# Set find path mode properly for cross-compiling
# refer to: https://discourse.cmake.org/t/find-package-stops-working-when-cmake-system-name-ios/4609/6
# BUT: CMAKE_FIND_ROOT_PATH is preferred for additional search directories when cross-compiling
# set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH CACHE STRING "")
# set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH CACHE STRING "")
# set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH CACHE STRING "")

# by default, we want find host program only when cross-compiling
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER CACHE STRING "")

# Sets CMAKE_SYSTEM_PROCESSOR properly
if(ARCHS MATCHES "((arm64|arm64e|x86_64)(^|;|, )?)+")
  set(CMAKE_C_SIZEOF_DATA_PTR 8)
  set(CMAKE_CXX_SIZEOF_DATA_PTR 8)
  if(ARCHS MATCHES "((arm64|arm64e)(^|;|, )?)+")
    set(CMAKE_SYSTEM_PROCESSOR "arm64")
  else()
    set(CMAKE_SYSTEM_PROCESSOR "x86_64")
  endif()
endif()

# This little function lets you set any XCode specific property, refer to: ios.toolchain.cmake
function(set_xcode_property TARGET XCODE_PROPERTY XCODE_VALUE)
    if (ARGC LESS 4 OR ARGV3 STREQUAL "All")
        set_property(TARGET ${TARGET} PROPERTY XCODE_ATTRIBUTE_${XCODE_PROPERTY} ${XCODE_VALUE})
    else()
        set(XCODE_RELVERSION ${ARGV3})
        set_property(TARGET ${TARGET} PROPERTY XCODE_ATTRIBUTE_${XCODE_PROPERTY}[variant=${XCODE_RELVERSION}] "${XCODE_VALUE}")
    endif()
endfunction()
