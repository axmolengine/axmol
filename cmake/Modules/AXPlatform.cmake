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
    if("${CMAKE_GENERATOR_PLATFORM}" STREQUAL "Win32")
        set(WIN32 TRUE)
        set(ARCH_ALIAS "x86")
    elseif("${CMAKE_GENERATOR_PLATFORM}" STREQUAL "arm64")
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
    if(ANDROID)
        set(PLATFORM_NAME android)
    else()
        set(LINUX TRUE)
        set(PLATFORM_NAME linux)
    endif()
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
     set(WASM TRUE)
     set(EMSCRIPTEN TRUE)
     set(PLATFORM_NAME wasm)
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
else()
    message(FATAL_ERROR "Unsupported platform, CMake will exit")
    return()
endif()

# generators that are capable of organizing into a hierarchy of folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
# simplify generator condition, please use them everywhere
if(CMAKE_GENERATOR STREQUAL Xcode)
    set(XCODE TRUE)
elseif(CMAKE_GENERATOR MATCHES Visual)
    set(VS TRUE)
endif()

# set platform specific path
set(_path_prefix ${CMAKE_CURRENT_SOURCE_DIR}/prebuilt/)
set(platform_spec_path "${_path_prefix}${PLATFORM_NAME}")
if(ANDROID OR WIN32)
    set(platform_spec_path "${platform_spec_path}/${ARCH_ALIAS}")
endif()

### axpkg url
set (axpkg_ver v60)
set (axpkg_base_url "https://github.com/axmolengine/build1k/releases/download/${axpkg_ver}" CACHE STRING "" FORCE)

function(axpkg_require package_name dir)
    if(NOT IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/prebuilt)
        set (package_url "${axpkg_base_url}/${package_name}.zip")
        message(AUTHOR_WARNING "Downloading ${package_url}")
        if (NOT EXISTS ${dir}/${package_name}.zip)
            file(DOWNLOAD ${package_url} ${dir}/${package_name}.zip)
        endif()
        file(ARCHIVE_EXTRACT INPUT ${dir}/${package_name}.zip DESTINATION ${dir}/)
        file(RENAME ${dir}/${package_name}/include ${dir}/include)
        file(RENAME ${dir}/${package_name}/prebuilt ${dir}/prebuilt)
        file(REMOVE_RECURSE ${dir}/${package_name})
        # file(REMOVE ${CMAKE_CURRENT_LIST_DIR}/${package_name}.zip)
    endif()
endfunction()
