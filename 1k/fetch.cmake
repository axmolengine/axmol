# 
# the 1k fetch functions
# require predefine variable:
#   _1kfetch_cache_dir
#   _1kfetch_manifest
# 
cmake_minimum_required(VERSION 3.23)

### 1kdist url
find_program(PWSH_PROG NAMES pwsh powershell NO_PACKAGE_ROOT_PATH NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH NO_CMAKE_FIND_ROOT_PATH)
find_program(GIT_PROG NAMES git NO_PACKAGE_ROOT_PATH NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH NO_CMAKE_FIND_ROOT_PATH)

function(_1kfetch_init)
    if(NOT _1kfetch_cache_dir)
        file(REAL_PATH "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../cache" _1kfetch_cache_dir)
        set(_1kfetch_cache_dir "${_1kfetch_cache_dir}" CACHE STRING "" FORCE)
    endif()
    if(NOT _1kfetch_manifest)
        file(REAL_PATH "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../manifest.json" _1kfetch_manifest)
        set(_1kfetch_manifest "${_1kfetch_manifest}" CACHE STRING "" FORCE)
    endif()

    if(NOT EXISTS ${PWSH_PROG}) # try again
        unset(PWSH_PROG CACHE)
        find_program(PWSH_PROG NAMES pwsh powershell NO_PACKAGE_ROOT_PATH NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH NO_CMAKE_FIND_ROOT_PATH)
    endif()

    execute_process(COMMAND ${PWSH_PROG} ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/resolv-uri.ps1
        -name "1kdist"
        -manifest ${_1kfetch_manifest}
        OUTPUT_VARIABLE _1kdist_url
        RESULT_VARIABLE _1kdist_error
    )

    if(_1kdist_url)
        string(REPLACE "#" ";" _1kdist_url ${_1kdist_url})
        list(GET _1kdist_url 0 _1kdist_base_url)
        list(GET _1kdist_url 1 _1kdist_ver)
        set(_1kdist_base_url "${_1kdist_base_url}/${_1kdist_ver}" PARENT_SCOPE)
        set(_1kdist_ver ${_1kdist_ver} PARENT_SCOPE)
    else()
        message(WARNING "Resolve 1kdist uri fail, ${_1kdist_error}, the _1kfetch_dist will not work")
    endif()
endfunction()

# fetch prebuilt from 1kdist
# param package_name
function(_1kfetch_dist package_name)
    set(_prebuilt_root ${CMAKE_CURRENT_LIST_DIR}/_x)
    if(_1KFETCH_UPGRADE AND IS_DIRECTORY ${_prebuilt_root})
        file(REMOVE_RECURSE ${_prebuilt_root})
    endif()
    if(NOT IS_DIRECTORY ${_prebuilt_root})
        set (package_store "${_1kfetch_cache_dir}/1kdist/${_1kdist_ver}/${package_name}.zip")
        if (NOT EXISTS ${package_store})
            set (package_url "${_1kdist_base_url}/${package_name}.zip")
            message(AUTHOR_WARNING "Downloading ${package_url}")
            file(DOWNLOAD ${package_url} ${package_store} STATUS _status LOG _logs SHOW_PROGRESS)
            list(GET _status 0 status_code)
            list(GET _status 1 status_string)
            if(NOT status_code EQUAL 0)
                file(REMOVE ${package_store})
                message(FATAL_ERROR "Download ${package_url} fail, ${status_string}, logs: ${_logs}")
            endif()
        endif()
        file(ARCHIVE_EXTRACT INPUT ${package_store} DESTINATION ${CMAKE_CURRENT_LIST_DIR}/)
        if (IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/${package_name})
            file(RENAME ${CMAKE_CURRENT_LIST_DIR}/${package_name} ${_prebuilt_root})
        else() # download may fail
            file(REMOVE ${package_store})
            message(FATAL_ERROR "The package ${package_store} is malformed, please try again!")
        endif()
    endif()

    # set platform specific path, PLATFORM_NAME provided by user: win32,winrt,mac,ios,android,tvos,watchos,linux
    if(APPLE) # since 1kiss_dist v80+, xcframework don't require platform spec sub folder
        set(_prebuilt_lib_dir "${_prebuilt_root}/lib")
    else()
        set(_prebuilt_lib_dir "${_prebuilt_root}/lib/${PLATFORM_NAME}")
        if(ANDROID OR WIN32)
            set(_prebuilt_lib_dir "${_prebuilt_lib_dir}/${ARCH_ALIAS}")
        endif()
    endif()
    set(${package_name}_INC_DIR ${_prebuilt_root}/include PARENT_SCOPE)
    set(${package_name}_LIB_DIR ${_prebuilt_lib_dir} PARENT_SCOPE)
endfunction()

function(_1kfetch uri)
    set(oneValueArgs NAME REV)
    cmake_parse_arguments(opt "" "${oneValueArgs}" "" ${ARGN})

    _1kparse_name(${uri} "${opt_NAME}")

    set(_pkg_store "${_1kfetch_cache_dir}/${_pkg_name}")

    get_property(_fetched GLOBAL PROPERTY "${_pkg_name}_fetched")
    if (NOT _fetched)
        set(_fetch_args 
            -uri "${uri}"
            -prefix "${_1kfetch_cache_dir}"
            -manifest "${_1kfetch_manifest}"
            -name "${_pkg_name}"
        )
        # rev: the explicit rev to checkout, i.e. git release tag name
        if(opt_REV)
            list(APPEND _fetch_args -rev ${opt_REV})
        endif()

        if(_1KFETCH_UPGRADE)
            list(APPEND _fetch_args -pull_branch)
        endif()

        execute_process(COMMAND ${PWSH_PROG} ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/fetch.ps1
            ${_fetch_args}
            RESULT_VARIABLE _errorcode
            )
        if (_errorcode)
            message(FATAL_ERROR "fetch content ${uri} failed")
        endif()

        set_property(GLOBAL PROPERTY "${_pkg_name}_fetched" TRUE)
    endif()

    set(${_pkg_name}_SOURCE_DIR ${_pkg_store} PARENT_SCOPE)
    set(source_dir ${_pkg_store} PARENT_SCOPE)
endfunction()

# developing, not available yet
function(_1kfetch_fast uri)
    _1kperf_start("_1kfetch: ${uri}")

    set(oneValueArgs NAME REV)
    cmake_parse_arguments(opt "" "${oneValueArgs}" "" ${ARGN})

    _1kparse_name(${uri} "${opt_NAME}")
    
    set(_pkg_store "${_1kfetch_cache_dir}/${_pkg_name}")

    set(_sentry_file "${_pkg_store}/_1kiss")

    if(NOT _manifest_conf)
        file(READ "${_1kfetch_manifest}" _manifest_conf)
    endif()
    string(JSON _url GET "${_manifest_conf}" "mirrors" "github" "${_pkg_name}")
    string(JSON _version GET "${_manifest_conf}" "versions" "${_pkg_name}")
    string(PREPEND _url "https://github.com/")
    if(NOT EXISTS "${_sentry_file}")
        execute_process(COMMAND ${GIT_PROG} clone --progress ${_url} "${_pkg_store}" RESULT_VARIABLE _errorcode)
        file(WRITE "${_sentry_file}" "ver: ${_version}")
    endif()

    if(EXISTS "${_sentry_file}")
        execute_process(COMMAND ${GIT_PROG} -C ${_pkg_store} checkout ${_version} RESULT_VARIABLE _errorcode)
        if(_errorcode)
            execute_process(COMMAND ${GIT_PROG} -C ${_pkg_store} checkout v${_version} RESULT_VARIABLE _errorcode)
        endif()
    else()
        message(FATAL_ERROR "fetch repo ${uri} fail, try again")
    endif()

    set(${_pkg_name}_SOURCE_DIR ${_pkg_store} PARENT_SCOPE)
    set(source_dir ${_pkg_store} PARENT_SCOPE)

    _1kperf_end("_1kfetch")
endfunction()

# simple cmake pkg management:
# for example: _1kadd_pkg("gh:yasio/yasio#4.2.1")
function(_1kadd_pkg uri)
    set(optValueArgs EXCLUDE_FROM_ALL)
    set(oneValueArgs BINARY_DIR NAME)
    set(multiValueArgs OPTIONS)
    cmake_parse_arguments(opt "${optValueArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    _1kparse_name(${uri} "${opt_NAME}")

    if(NOT TARGET ${_pkg_name})
        _1kfetch(${uri} ${ARGN} NAME ${_pkg_name})

        foreach(OPTION ${opt_OPTIONS})
            _1kparse_option("${OPTION}")
            set(${OPTION_KEY} "${OPTION_VALUE}" CACHE BOOL "" FORCE)
        endforeach()

        if(opt_BINARY_DIR)
            set(binary_dir "${opt_BINARY_DIR}/${_pkg_name}")
        else()
            set(binary_dir "${CMAKE_BINARY_DIR}/1kiss/${_pkg_name}")
        endif()
        
        if (opt_EXCLUDE_FROM_ALL)
            add_subdirectory(${source_dir} ${binary_dir} EXCLUDE_FROM_ALL)
        else()
            add_subdirectory(${source_dir} ${binary_dir})
        endif()
    endif()
endfunction()

function(_1klink src dest)
    file(TO_NATIVE_PATH "${src}" _srcDir)
    file(TO_NATIVE_PATH "${dest}" _dstDir)
    execute_process(COMMAND ${PWSH_PROG} ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/fsync.ps1 -s "${_srcDir}" -d "${_dstDir}" -l 1)
endfunction()

function(_1kparse_option OPTION)
  string(REGEX MATCH "^[^ ]+" OPTION_KEY "${OPTION}")
  string(LENGTH "${OPTION}" OPTION_LENGTH)
  string(LENGTH "${OPTION_KEY}" OPTION_KEY_LENGTH)
  if(OPTION_KEY_LENGTH STREQUAL OPTION_LENGTH)
    # no value for key provided, assume user wants to set option to "ON"
    set(OPTION_VALUE "ON")
  else()
    math(EXPR OPTION_KEY_LENGTH "${OPTION_KEY_LENGTH}+1")
    string(SUBSTRING "${OPTION}" "${OPTION_KEY_LENGTH}" "-1" OPTION_VALUE)
  endif()
  set(OPTION_KEY
      "${OPTION_KEY}"
      PARENT_SCOPE
  )
  set(OPTION_VALUE
      "${OPTION_VALUE}"
      PARENT_SCOPE
  )
endfunction()

macro(_1kparse_name uri opt_NAME)
    if(opt_NAME)
        set(_pkg_name ${opt_NAME})
    else()
        set(_trimmed_uri "")
        # parse pkg name for pkg_store due to we can't get from execute_process properly
        string(REGEX REPLACE "#.*" "" _trimmed_uri "${uri}")
        get_filename_component(_pkg_name ${_trimmed_uri} NAME_WE)
        set(_pkg_name ${_pkg_name})
    endif()
endmacro()

macro(_1kperf_start tag)
    string(TIMESTAMP _current_sec "%s" UTC)
    string(TIMESTAMP _current_usec "%f" UTC)
    math(EXPR _fetch_start_msec "${_current_sec} * 1000 + ${_current_usec} / 1000" OUTPUT_FORMAT DECIMAL)
    message(STATUS "[${_fetch_start_msec}ms][1kperf] start of ${tag} ..." )
endmacro()

macro(_1kperf_end tag)
    string(TIMESTAMP _current_sec "%s" UTC)
    string(TIMESTAMP _current_usec "%f" UTC)
    math(EXPR _fetch_end_msec "${_current_sec} * 1000 + ${_current_usec} / 1000" OUTPUT_FORMAT DECIMAL)
    math(EXPR _fetch_cost_msec "${_fetch_end_msec} - ${_fetch_start_msec}")
    message(STATUS "[${_fetch_end_msec}ms][1kperf] end of ${tag}, cost: ${_fetch_cost_msec}ms" )
endmacro()

if(PWSH_PROG)
    _1kfetch_init()
else()
    message(WARNING "fetch.cmake: PowerShell is missing, the fetch functions not work, please install from https://github.com/PowerShell/PowerShell/releases")
endif()
