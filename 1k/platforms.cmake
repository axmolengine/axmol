#Please use them everywhere
#WINDOWS   =   Windows Desktop
#ANDROID    =  Android
#IOS    =  iOS
#MACOSX    =  MacOS X
#LINUX      =   Linux

#############################
# cmake commands:
# win32: cmake -B build -A x64
# winrt: cmake -B build -A x64 -DCMAKE_SYSTEM_NAME=WindowsStore "-DCMAKE_SYSTEM_VERSION=10.0"
#

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(WINDOWS TRUE)
    string(TOLOWER "${CMAKE_GENERATOR_PLATFORM}" _gp_lcase)
    if("${_gp_lcase}" MATCHES "win32")
        set(WIN32 TRUE)
        set(ARCH_ALIAS "x86")
    elseif("${_gp_lcase}" STREQUAL "arm64")
        set(WIN64 TRUE)
        set(ARCH_ALIAS "arm64")
    else()
        set(WIN64 TRUE)
        set(ARCH_ALIAS "x64")
    endif()
    set(PLATFORM_NAME win32)
    if (${CMAKE_SYSTEM_NAME} MATCHES "WindowsStore")
        set(WINRT TRUE CACHE BOOL "" FORCE)
        set(PLATFORM_NAME winrt)
    endif()
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Android")
    set(PLATFORM_NAME android)
    set(ARCH_ALIAS ${ANDROID_ABI})
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(LINUX TRUE)
    set(PLATFORM_NAME linux)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(WASM TRUE)
    set(EMSCRIPTEN TRUE)
    if ("${CMAKE_LIBRARY_ARCHITECTURE}" MATCHES "64")
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

if (NOT DEFINED WIN32)
    set(WIN32 FALSE)
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
    if(("${ARCH_ALIAS}" STREQUAL "x86") OR ("${ARCH_ALIAS}" MATCHES "armeabi-v7a"))
        message(WARNING "Building 32-bit[${ARCH_ALIAS}] ${project_name} is deprecated, and will be removed in next release ${release_ver}")
    endif()
endfunction()
