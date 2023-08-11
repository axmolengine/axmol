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
    set(PLATFORM_FOLDER win32)
    if (${CMAKE_SYSTEM_NAME} MATCHES "WindowsStore")
        set(WINRT TRUE CACHE BOOL "" FORCE)
        set(PLATFORM_FOLDER winrt)
    endif()
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Android")
    set(PLATFORM_FOLDER android)
    set(ARCH_ALIAS ${ANDROID_ABI})
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    if(ANDROID)
        set(PLATFORM_FOLDER android)
    else()
        set(LINUX TRUE)
        set(PLATFORM_FOLDER linux)
    endif()
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(APPLE TRUE)
    set(MACOSX TRUE)
    set(PLATFORM_FOLDER mac)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "iOS")
    set(APPLE TRUE)
    set(IOS TRUE)
    set(PLATFORM_FOLDER ios)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "tvOS")
    set(APPLE TRUE)
    set(IOS TRUE)
    set(TVOS TRUE)
    set(PLATFORM_FOLDER tvos)
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
if(IOS)
    if(TVOS)
        set(platform_name tvos)
        set(platform_spec_path tvos)
    else()
        set(platform_name ios)
        set(platform_spec_path ios)
    endif()
elseif(ANDROID)
    set(platform_name android)
    set(platform_spec_path android/${ANDROID_ABI})
elseif(WINDOWS)
    if (NOT WINRT)
        set(platform_name win32)
    else()
        set(platform_name winuwp) 
    endif()
    set(platform_spec_path ${platform_name}/${ARCH_ALIAS})
elseif(MACOSX)
    set(platform_name mac)
    set(platform_spec_path mac)
elseif(LINUX)
    set(platform_name linux)
    set(platform_spec_path linux)
endif()

set(platform_spec_path "${_path_prefix}${platform_spec_path}")


### axpkg url
set (axpkg_ver v56)
set (axpkg_base_url "https://github.com/axmolengine/buildware/releases/download/${axpkg_ver}" CACHE STRING "" FORCE)

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
