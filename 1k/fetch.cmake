# 
# the 1k fetch functions
# require predefine variable:
#   _1kfetch_cache_dir
#   _1kfetch_manifest
# 

### 1kdist url
file(READ "${_1kfetch_manifest}" _manifest_content)
string(JSON _1kdist_ver GET "${_manifest_content}" 1kdist)
set(_1kdist_ver v${_1kdist_ver})
set(_1kdist_base_url "https://github.com/axmolengine/1k/releases/download/${_1kdist_ver}" CACHE STRING "" FORCE)

# fetch prebuilt from 1kdist
# param package_name
function(_1kfetch_dist package_name)
    set(_prebuilt_root ${CMAKE_CURRENT_LIST_DIR}/_d)
    if(NOT IS_DIRECTORY ${_prebuilt_root})
        set (package_url "${_1kdist_base_url}/${package_name}.zip")
        set (package_store "${_1kfetch_cache_dir}/${package_name}.${_1kdist_ver}.zip")
        message(AUTHOR_WARNING "Downloading ${package_url}")
        if (NOT EXISTS ${package_store})
            file(DOWNLOAD ${package_url} ${package_store} STATUS _status LOG _logs SHOW_PROGRESS)
            list(GET _status 0 status_code)
            list(GET _status 1 status_string)
            if(NOT status_code EQUAL 0)
                message(FATAL_ERROR "Download ${package_url} fail, ${status_string}, logs: ${_logs}")
            endif()
        endif()
        file(ARCHIVE_EXTRACT INPUT ${package_store} DESTINATION ${CMAKE_CURRENT_LIST_DIR}/)
        file(RENAME ${CMAKE_CURRENT_LIST_DIR}/${package_name} ${_prebuilt_root})
    endif()

    # set platform specific path, PLATFORM_NAME provided by user: win32,winrt,mac,ios,android,tvos,watchos,linux
    set(_prebuilt_lib_dir "${_prebuilt_root}/lib/${PLATFORM_NAME}")
    if(ANDROID OR WIN32)
        set(_prebuilt_lib_dir "${_prebuilt_lib_dir}/${ARCH_ALIAS}")
    endif()
    set(${package_name}_INC_DIR ${_prebuilt_root}/include PARENT_SCOPE)
    set(${package_name}_LIB_DIR ${_prebuilt_lib_dir} PARENT_SCOPE)
endfunction()

# params: name, url
function(_1kfetch name url)
    if(NOT url) 
        message(FATAL_ERROR "Missing url for package ${name}")
    endif()
    set(_pkg_store "${_1kfetch_cache_dir}/${name}")
    find_program(PWSH_COMMAND NAMES pwsh powershell NO_PACKAGE_ROOT_PATH NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH NO_CMAKE_FIND_ROOT_PATH REQUIRED)
    execute_process(COMMAND ${PWSH_COMMAND} ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/fetch.ps1 
        -url "${url}"
        -name "${name}"
        -dest "${_pkg_store}"
        -cfg ${_1kfetch_manifest})
    set(${name}_SOURCE_DIR ${_pkg_store} PARENT_SCOPE)
endfunction()

function(_1klink source dest)
    if(UNIX)
        execute_process(COMMAND "${CMAKE_COMMAND}" -E create_symlink "${source}" "${dest}")
    else()
        file(TO_NATIVE_PATH "${dest}" _dstDir)
        file(TO_NATIVE_PATH "${source}" _srcDir)
        execute_process(COMMAND cmd.exe /c mklink /J "${_dstDir}" "${_srcDir}")
    endif()
endfunction()
