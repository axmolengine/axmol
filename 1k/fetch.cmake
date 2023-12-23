# 
# the 1k fetch functions
# require predefine variable:
#   _1kfetch_cache_dir
#   _1kfetch_manifest
# 

### 1kdist url
find_program(PWSH_COMMAND NAMES pwsh powershell NO_PACKAGE_ROOT_PATH NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH NO_CMAKE_FIND_ROOT_PATH REQUIRED)

function(_1kfetch_init)
    execute_process(COMMAND ${PWSH_COMMAND} ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/fetchurl.ps1
        -name "1kdist"
        -cfg ${_1kfetch_manifest}
        OUTPUT_VARIABLE _1kdist_url
    )
    string(REPLACE "#" ";" _1kdist_url ${_1kdist_url})
    list(GET _1kdist_url 0 _1kdist_base_url)
    list(GET _1kdist_url 1 _1kdist_ver)
    set(_1kdist_base_url "${_1kdist_base_url}/v${_1kdist_ver}" PARENT_SCOPE)
    set(_1kdist_ver ${_1kdist_ver} PARENT_SCOPE)
endfunction()

# fetch prebuilt from 1kdist
# param package_name
function(_1kfetch_dist package_name)
    set(_prebuilt_root ${CMAKE_CURRENT_LIST_DIR}/_d)
    if(NOT IS_DIRECTORY ${_prebuilt_root})
        set (package_store "${_1kfetch_cache_dir}/1kdist/v${_1kdist_ver}/${package_name}.zip")
        if (NOT EXISTS ${package_store})
            set (package_url "${_1kdist_base_url}/${package_name}.zip")
            message(AUTHOR_WARNING "Downloading ${package_url}")
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
function(_1kfetch name)
    set(_pkg_store "${_1kfetch_cache_dir}/${name}")
    execute_process(COMMAND ${PWSH_COMMAND} ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/fetch.ps1 
        -name "${name}"
        -dest "${_pkg_store}"
        -cfg ${_1kfetch_manifest}
        RESULT_VARIABLE _errorcode
        )
    if (_errorcode)
        message(FATAL_ERROR "aborted")
    endif()
    set(${name}_SOURCE_DIR ${_pkg_store} PARENT_SCOPE)
endfunction()

function(_1klink src dest)
    file(TO_NATIVE_PATH "${src}" _srcDir)
    file(TO_NATIVE_PATH "${dest}" _dstDir)
    execute_process(COMMAND ${PWSH_COMMAND} ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/fsync.ps1 -s "${_srcDir}" -d "${_dstDir}" -l 1)
endfunction()

_1kfetch_init()
