# 
# the 1k fetch functions
# require predefine variable:
#   _1kfetch_cache_dir
#   _1kfetch_manifest
# 

### 1kdist url
find_program(PWSH_COMMAND NAMES pwsh powershell NO_PACKAGE_ROOT_PATH NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH NO_CMAKE_FIND_ROOT_PATH)

function(_1kfetch_init)
    execute_process(COMMAND ${PWSH_COMMAND} ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/fetchurl.ps1
        -name "1kdist"
        -manifest ${_1kfetch_manifest}
        OUTPUT_VARIABLE _1kdist_url
    )
    string(REPLACE "#" ";" _1kdist_url ${_1kdist_url})
    list(GET _1kdist_url 0 _1kdist_base_url)
    list(GET _1kdist_url 1 _1kdist_ver)
    set(_1kdist_base_url "${_1kdist_base_url}/v${_1kdist_ver}" PARENT_SCOPE)
    set(_1kdist_ver ${_1kdist_ver} PARENT_SCOPE)
    if(NOT _1kfetch_cache_dir)
        file(REAL_PATH "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../cache" _1kfetch_cache_dir)
        set(_1kfetch_cache_dir "${_1kfetch_cache_dir}" CACHE STRING "" FORCE)
    endif()
    if(NOT _1kfetch_manifest)
        file(REAL_PATH "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../manifest.json" _1kfetch_manifest)
        set(_1kfetch_manifest "${_1kfetch_manifest}" CACHE STRING "" FORCE)
    endif()
endfunction()

# fetch prebuilt from 1kdist
# param package_name
function(_1kfetch_dist package_name)
    set(_prebuilt_root ${CMAKE_CURRENT_LIST_DIR}/_x)
    if(NOT IS_DIRECTORY ${_prebuilt_root})
        set (package_store "${_1kfetch_cache_dir}/1kdist/v${_1kdist_ver}/${package_name}.zip")
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
    set(_prebuilt_lib_dir "${_prebuilt_root}/lib/${PLATFORM_NAME}")
    if(ANDROID OR WIN32)
        set(_prebuilt_lib_dir "${_prebuilt_lib_dir}/${ARCH_ALIAS}")
    endif()
    set(${package_name}_INC_DIR ${_prebuilt_root}/include PARENT_SCOPE)
    set(${package_name}_LIB_DIR ${_prebuilt_lib_dir} PARENT_SCOPE)
endfunction()

function(_1kfetch uri)
    set(oneValueArgs NAME)
    cmake_parse_arguments(opt "" "${oneValueArgs}" "" ${ARGN})

    set(_pkg_name)
    if(opt_NAME)
        set(_pkg_name ${opt_NAME})
    else()
        # parse pkg name for pkg_store due to we can't get from execute_process properly
        string(REGEX REPLACE "#.*" "" _trimmed_uri ${uri})
        get_filename_component(_pkg_name ${_trimmed_uri} NAME_WE)
    endif()

    set(_pkg_store "${_1kfetch_cache_dir}/${_pkg_name}")
    execute_process(COMMAND ${PWSH_COMMAND} ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/fetch.ps1
        -uri "${uri}"
        -prefix "${_1kfetch_cache_dir}"
        -manifest "${_1kfetch_manifest}"
        -name "${_pkg_name}"
        RESULT_VARIABLE _errorcode
        )
    if (_errorcode)
        message(FATAL_ERROR "fetch content ${uri} failed")
    endif()
    set(${_pkg_name}_SOURCE_DIR ${_pkg_store} PARENT_SCOPE)
    set(source_dir ${_pkg_store} PARENT_SCOPE)
endfunction()

# for example: _1kcm_add_pkg("gh:yasio/yasio#4.2.1")
function(_1kcm_add_pkg uri)
    _1kfetch(${uri} ${ARGN})

    set(optValueArgs EXCLUDE_FROM_ALL)
    set(oneValueArgs OPTIONS)
    cmake_parse_arguments(opt "${optValueArgs}" "${oneValueArgs}" "" ${ARGN})
    foreach(OPTION ${opt_OPTIONS})
        _1k_parse_option("${OPTION}")
        set(${OPTION_KEY} "${OPTION_VALUE}" CACHE BOOL "" FORCE)
    endforeach()
    set(binary_dir "")
    if(IS_ABSOLUTE ${source_dir})
        # string(LENGTH "${_1kfetch_cache_dir}/" _offset)
        # string(LENGTH "${source_dir}" _len)
        # math(EXPR _len "${_len} - ${_offset}" OUTPUT_FORMAT DECIMAL)
        # string(SUBSTRING ${source_dir} ${_offset} ${_len} _path)
        get_filename_component(_path ${source_dir} NAME)
        set(binary_dir "${CMAKE_BINARY_DIR}/1kiss/${_path}")
    endif()

    if (opt_EXCLUDE_FROM_ALL)
        add_subdirectory(${source_dir} ${binary_dir} EXCLUDE_FROM_ALL)
    else()
        add_subdirectory(${source_dir} ${binary_dir})
    endif()
endfunction()

function(_1klink src dest)
    file(TO_NATIVE_PATH "${src}" _srcDir)
    file(TO_NATIVE_PATH "${dest}" _dstDir)
    execute_process(COMMAND ${PWSH_COMMAND} ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/fsync.ps1 -s "${_srcDir}" -d "${_dstDir}" -l 1)
endfunction()

function(_1k_parse_option OPTION)
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

if(PWSH_COMMAND)
    _1kfetch_init()
else()
    message(WARNING "fetch.cmake: PowerShell is missing, the fetch functions not work, please install from https://github.com/PowerShell/PowerShell/releases")
endif()
