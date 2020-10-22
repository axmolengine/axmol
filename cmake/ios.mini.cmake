# The workaround for solve try_compile failed with code sign
# since cmake-3.18.2, not required
# everyting for cmake toolchain config before project(xxx) is better

set(CMAKE_SYSTEM_NAME "iOS" CACHE STRING "The CMake system name for iOS")

set(CMAKE_TRY_COMPILE_PLATFORM_VARIABLES
    "CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED"
    "CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED")
set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED NO)
set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED NO)

# Fix compile failed with armv7 deployment target >= 11.0, xcode clang will report follow error
# clang: error: invalid iOS deployment version '--target=armv7-apple-ios13.6', 
#        iOS 10 is the maximum deployment target for 32-bit targets
# If not defined CMAKE_OSX_DEPLOYMENT_TARGET, cmake will choose latest deployment target
if("${CMAKE_OSX_ARCHITECTURES}" MATCHES ".*armv7.*")
    if(NOT DEFINED CMAKE_OSX_DEPLOYMENT_TARGET 
    OR "${CMAKE_OSX_DEPLOYMENT_TARGET}" VERSION_GREATER "11.0" 
    OR "${CMAKE_OSX_DEPLOYMENT_TARGET}" VERSION_EQUAL "11.0")
        message(STATUS "Sets iOS minimum deployment target to 10.0 for armv7")
        # a. armv7 maximum deployment 10.x
        # b. armv7 TLS require deployment 10.x
        set(CMAKE_OSX_DEPLOYMENT_TARGET "10.0" CACHE STRING "Minimum OS X deployment version")
    endif()
else()
    if(NOT DEFINED CMAKE_OSX_DEPLOYMENT_TARGET)
         message(STATUS "The CMAKE_OSX_DEPLOYMENT_TARGET not defined, sets iOS minimum deployment target to 9.0")
        set(CMAKE_OSX_DEPLOYMENT_TARGET "9.0" CACHE STRING "Minimum OS X deployment version")
    endif()
endif()

if(NOT DEFINED CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET)
    set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET ${CMAKE_OSX_DEPLOYMENT_TARGET} CACHE STRING "Minimum iphoneos deployment version")
endif()
