include(CMakeParseArguments)

find_program(PWSH_PROG NAMES pwsh powershell NO_PACKAGE_ROOT_PATH NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH NO_CMAKE_FIND_ROOT_PATH)

if(NOT PWSH_PROG)
    message("powershell not found.")
    message(FATAL_ERROR "Please install it https://learn.microsoft.com/en-us/powershell/scripting/install/installing-powershell, and run CMake again.")
endif()

if(NOT DEFINED WASM)
    set(WASM FALSE CACHE BOOL "")
endif()

# copy resource `FILES` and `FOLDERS` to TARGET_FILE_DIR/Resources
function(ax_sync_target_res ax_target)
    set(options SYM_LINK)
    set(oneValueArgs LINK_TO SYNC_TARGET_ID)
    set(multiValueArgs FOLDERS)
    cmake_parse_arguments(opt "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
   
    if (NOT DEFINED opt_SYNC_TARGET_ID)
        set(sync_target_name "SYNC_RESOURCE-${ax_target}")
    else()
        set(sync_target_name "SYNC_RESOURCE-${ax_target}-${opt_SYNC_TARGET_ID}")
    endif()

    ax_def_sync_resource_target(${ax_target} ${sync_target_name})
    
    if(NOT TARGET ${sync_target_name})
        message(WARNING "SyncResource targe for ${ax_target} is not defined")
        return()
    endif()

    # linking folders
    if((NOT WASM AND NOT ANDROID) OR NOT opt_SYM_LINK)
        foreach(cc_folder ${opt_FOLDERS})
            #get_filename_component(link_folder ${opt_LINK_TO} DIRECTORY)
            get_filename_component(link_folder_abs ${opt_LINK_TO} ABSOLUTE)
            add_custom_command(TARGET ${sync_target_name} POST_BUILD
                COMMAND ${PWSH_PROG} ARGS ${_AX_ROOT}/1k/fsync.ps1
                    -s ${cc_folder} -d ${link_folder_abs} -l ${opt_SYM_LINK}
            )
        endforeach()
    endif()
endfunction()

if (NOT COMMAND set_xcode_property)
    # This little macro lets you set any XCode specific property, from ios.toolchain.cmake
    function(set_xcode_property TARGET XCODE_PROPERTY XCODE_VALUE)
        set_property(TARGET ${TARGET} PROPERTY XCODE_ATTRIBUTE_${XCODE_PROPERTY} ${XCODE_VALUE})
    endfunction(set_xcode_property)
endif()

## create a virtual target SYNC_RESOURCE-${ax_target}
## Update resource files in Resources/ folder everytime when `Run/Debug` target.
function(ax_def_sync_resource_target ax_target sync_target_name)
    add_custom_target(${sync_target_name} ALL
        COMMAND ${CMAKE_COMMAND} -E echo "Syncing resources for ${ax_target} ..."
    )
    add_dependencies(${ax_target} ${sync_target_name})
    set_target_properties(${sync_target_name} PROPERTIES
        FOLDER Utils
    )
endfunction()


function(ax_sync_lua_scripts ax_target src_dir dst_dir)
    set(luacompile_target COPY_LUA-${ax_target})
    if(NOT TARGET ${luacompile_target})
        add_custom_target(${luacompile_target} ALL
            COMMAND ${CMAKE_COMMAND} -E echo "Syncing lua scripts ..."
        )
        add_dependencies(${ax_target} ${luacompile_target})
        set_target_properties(${luacompile_target} PROPERTIES
            FOLDER Utils
        )
    endif()
    if(MSVC)
        add_custom_command(TARGET ${luacompile_target} POST_BUILD
            COMMAND ${PWSH_PROG} ARGS ${_AX_ROOT}/1k/fsync.ps1
                -s ${src_dir} -d ${dst_dir}
        )
    else()
        if("${CMAKE_BUILD_TYPE}" STREQUAL "")
            add_custom_command(TARGET ${luacompile_target} POST_BUILD
                COMMAND ${PWSH_PROG} ARGS ${_AX_ROOT}/1k/fsync.ps1
                -s ${src_dir} -d ${dst_dir}
            )
        else()
            add_custom_command(TARGET ${luacompile_target} POST_BUILD
                COMMAND ${PWSH_PROG} ARGS ${_AX_ROOT}/1k/fsync.ps1
                    -s ${src_dir} -d ${dst_dir}
            )
        endif()
    endif()
endfunction()


function(ax_get_resource_path output ax_target)
    get_target_property(rt_output ${ax_target} RUNTIME_OUTPUT_DIRECTORY)
    set(${output} "${rt_output}/${CMAKE_CFG_INTDIR}/Content" PARENT_SCOPE)
endfunction()


# mark `FILES` and files in `FOLDERS` as resource files, the destination is `RES_TO` folder
# save all marked files in `res_out`
function(ax_mark_multi_resources res_out)
    set(oneValueArgs RES_TO)
    set(multiValueArgs FILES FOLDERS)
    cmake_parse_arguments(opt "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(tmp_file_list)
    foreach(cc_file ${opt_FILES})
        get_filename_component(cc_file_abs ${cc_file} ABSOLUTE)
        get_filename_component(file_dir ${cc_file_abs} DIRECTORY)
        ax_mark_resources(FILES ${cc_file_abs} BASEDIR ${file_dir} RESOURCEBASE ${opt_RES_TO})
    endforeach()
    list(APPEND tmp_file_list ${opt_FILES})

    foreach(cc_folder ${opt_FOLDERS})
        file(GLOB_RECURSE folder_files FOLLOW_SYMLINKS "${cc_folder}/*")
        list(APPEND tmp_file_list ${folder_files})
        ax_mark_resources(FILES ${folder_files} BASEDIR ${cc_folder} RESOURCEBASE ${opt_RES_TO})
    endforeach()
    set(${res_out} ${tmp_file_list} PARENT_SCOPE)
endfunction()

# get all linked libraries including transitive ones, recursive
function(search_depend_libs_recursive ax_target all_depends_out)
    set(all_depends_inner)
    set(targets_prepare_search ${ax_target})
    while(true)
        foreach(tmp_target ${targets_prepare_search})
            get_target_property(target_type ${tmp_target} TYPE)
            if(${target_type} STREQUAL "SHARED_LIBRARY" OR ${target_type} STREQUAL "STATIC_LIBRARY" OR ${target_type} STREQUAL "MODULE_LIBRARY" OR ${target_type} STREQUAL "EXECUTABLE")
                get_target_property(tmp_depend_libs ${tmp_target} LINK_LIBRARIES)
                list(REMOVE_ITEM targets_prepare_search ${tmp_target})
                list(APPEND tmp_depend_libs ${tmp_target})
                foreach(depend_lib ${tmp_depend_libs})
                    if(TARGET ${depend_lib})
                        list(APPEND all_depends_inner ${depend_lib})
                        if(NOT (depend_lib STREQUAL tmp_target))
                            list(APPEND targets_prepare_search ${depend_lib})
                        endif()
                    endif()
                endforeach()
            else()
                list(REMOVE_ITEM targets_prepare_search ${tmp_target})
            endif()
        endforeach()
        list(LENGTH targets_prepare_search targets_prepare_search_size)
        if(targets_prepare_search_size LESS 1)
            break()
        endif()
    endwhile(true)
    list(REMOVE_DUPLICATES all_depends_inner)
    set(${all_depends_out} ${all_depends_inner} PARENT_SCOPE)
endfunction()

# get `ax_target` depend all dlls, save the result in `all_depend_dlls_out`
function(get_target_depends_ext_dlls ax_target all_depend_dlls_out)

    set(depend_libs)
    set(all_depend_ext_dlls)
    search_depend_libs_recursive(${ax_target} depend_libs)
    foreach(depend_lib ${depend_libs})
        if(TARGET ${depend_lib})
            get_target_property(target_type ${depend_lib} TYPE)
            if(${target_type} STREQUAL "SHARED_LIBRARY" OR ${target_type} STREQUAL "STATIC_LIBRARY" OR ${target_type} STREQUAL "MODULE_LIBRARY" OR ${target_type} STREQUAL "EXECUTABLE")
                get_target_property(found_shared_lib ${depend_lib} IMPORTED_IMPLIB)
                if(found_shared_lib)
                    get_target_property(tmp_dlls ${depend_lib} IMPORTED_LOCATION)
                    list(APPEND all_depend_ext_dlls ${tmp_dlls})
                endif()
            endif()
        endif()
    endforeach()
    list(REMOVE_DUPLICATES all_depend_ext_dlls)
    set(${all_depend_dlls_out} ${all_depend_ext_dlls} PARENT_SCOPE)
endfunction()


# sync the `ax_target` depended(prebuilt) dlls into TARGET_FILE_DIR
function(ax_sync_target_dlls ax_target)
    set(options LUA)
    cmake_parse_arguments(opt "${options}" "" "" ${ARGN})

    # winrt mark dll as deploy item in ax_setup_winrt_sources
    if(WINRT)
        return()
    endif()

    get_target_depends_ext_dlls(${ax_target} all_depend_dlls)

    # remove repeat items
    if(all_depend_dlls)
        list(REMOVE_DUPLICATES all_depend_dlls)
    endif()
    foreach(cc_dll_file ${all_depend_dlls})
        get_filename_component(cc_dll_name ${cc_dll_file} NAME)
        add_custom_command(TARGET ${ax_target} POST_BUILD
            #COMMAND ${CMAKE_COMMAND} -E echo "copy dll into target file dir: ${cc_dll_name} ..."
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${cc_dll_file} "$<TARGET_FILE_DIR:${ax_target}>/${cc_dll_name}"
        )
    endforeach()

    # copy 3rdparty dlls to target bin dir
    if(NOT CMAKE_GENERATOR MATCHES "Ninja")
        set(BUILD_CONFIG_DIR "\$\(Configuration\)/")
    endif()

    add_custom_command(TARGET ${ax_target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${ax_target}> $<TARGET_FILE_DIR:${ax_target}>
        COMMAND_EXPAND_LISTS
    )

    # Copy windows angle binaries
    if (WIN32 AND AX_GLES_PROFILE)
        add_custom_command(TARGET ${ax_target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${_AX_ROOT}/${_AX_THIRDPARTY_NAME}/angle/_x/lib/${PLATFORM_NAME}/${ARCH_ALIAS}/libGLESv2.dll
            ${_AX_ROOT}/${_AX_THIRDPARTY_NAME}/angle/_x/lib/${PLATFORM_NAME}/${ARCH_ALIAS}/libEGL.dll
            ${_AX_ROOT}/${_AX_THIRDPARTY_NAME}/angle/_x/lib/${PLATFORM_NAME}/${ARCH_ALIAS}/d3dcompiler_47.dll
            $<TARGET_FILE_DIR:${ax_target}>
        )
    endif()

    # Copy webview2 for ninja
    if(AX_ENABLE_MSEDGE_WEBVIEW2)
        if(CMAKE_GENERATOR MATCHES "Ninja")
            add_custom_command(TARGET ${ax_target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_BINARY_DIR}/packages/Microsoft.Web.WebView2/build/native/${ARCH_ALIAS}/WebView2Loader.dll"
                $<TARGET_FILE_DIR:${ax_target}>)
        endif()
    endif()

    # copy libvlc plugins dir for windows
    if(AX_ENABLE_VLC_MEDIA)
        add_custom_command(TARGET ${ax_target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${_AX_ROOT}/cache/vlc/lib/vlc/plugins
        $<TARGET_FILE_DIR:${ax_target}>/plugins
        )
    endif()

    # if lua
    if(opt_LUA AND NOT AX_USE_LUAJIT)
        if(NOT CMAKE_GENERATOR MATCHES "Ninja")
            set(BUILD_CONFIG_DIR "\$\(Configuration\)/")
        endif()
        if (MSVC)
            add_custom_command(TARGET ${ax_target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_BINARY_DIR}/bin/${BUILD_CONFIG_DIR}plainlua.dll"
                $<TARGET_FILE_DIR:${ax_target}>)
        endif()
    endif()
endfunction()

# mark `FILES` as resources, files will be put into sub-dir tree depend on its absolute path
function(ax_mark_resources)
    set(oneValueArgs BASEDIR RESOURCEBASE)
    set(multiValueArgs FILES)
    cmake_parse_arguments(opt "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT opt_RESOURCEBASE)
        set(opt_RESOURCEBASE Resources)
    endif()

    if(opt_BASEDIR)
        get_filename_component(BASEDIR_ABS ${opt_BASEDIR} ABSOLUTE)
    else()
        set(BASEDIR_ABS "")
    endif()
    foreach(RES_FILE ${opt_FILES} ${opt_UNPARSED_ARGUMENTS})

        if(BASEDIR_ABS)
            get_filename_component(RES_FILE_ABS ${RES_FILE} ABSOLUTE)
            file(RELATIVE_PATH RES ${BASEDIR_ABS} ${RES_FILE_ABS})
            get_filename_component(RES_LOC ${RES} PATH)
        else()
            set(RES_LOC "")
        endif()

        if (APPLE)
            set_source_files_properties(${RES_FILE} PROPERTIES
                                        MACOSX_PACKAGE_LOCATION "${opt_RESOURCEBASE}/${RES_LOC}"
                                        HEADER_FILE_ONLY 1
                                        )
        elseif(WINRT)
            # MakeAppx.exe require deployment location path rule
            #   - must full quailfied windows style path
            #   - can't start with .\xxx.txt, must be xxx.txt
            # 
            # Otherwise, will fail with:
            #  MakeAppx : error : 0x8007007b - The filename, directory name, or volume label syntax is incorrect.
            if (opt_RESOURCEBASE STREQUAL ".")
                set(basedir "")
            else()
                set(basedir "${opt_RESOURCEBASE}\\")
            endif()
            get_filename_component(RES_EXTENSION ${RES_FILE} LAST_EXT)
            string(TOLOWER "${RES_EXTENSION}" RES_EXTENSION)
            if (RES_EXTENSION STREQUAL ".obj")
                # refer to: https://gitlab.kitware.com/cmake/cmake/issues/18820
                get_property(__res_loc SOURCE ${RES_FILE} PROPERTY LOCATION)
                set_property(SOURCE ${RES_FILE} PROPERTY EXTERNAL_OBJECT 0)
            endif()
            string(REPLACE "/" "\\" VSDEPLOY_LOC "${basedir}${RES_LOC}")
            # dir path can have one trailing / -> remove
            string(REGEX REPLACE "(.)\\\\$" "\\1" VSDEPLOY_LOC "${VSDEPLOY_LOC}")
            set_source_files_properties(${RES_FILE} PROPERTIES
                                        VS_DEPLOYMENT_CONTENT 1
                                        VS_DEPLOYMENT_LOCATION "${VSDEPLOY_LOC}"
            )
        else()
            set_source_files_properties(${RES_FILE} PROPERTIES
                                        HEADER_FILE_ONLY 1
                                        )
        endif()

        if(XCODE OR VS)
            string(REPLACE "/" "\\" ide_source_group "${opt_RESOURCEBASE}/${RES_LOC}")
            source_group("${ide_source_group}" FILES ${RES_FILE})
        endif()
    endforeach()
endfunction()

# mark the code sources of `ax_target` into sub-dir tree
function(ax_mark_code_files ax_target)
    set(oneValueArgs GROUPBASE)
    cmake_parse_arguments(opt "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    if(NOT opt_GROUPBASE)
        set(root_dir ${CMAKE_CURRENT_SOURCE_DIR})
    else()
        set(root_dir ${opt_GROUPBASE})
        message(STATUS "target ${ax_target} code group base is: ${root_dir}")
    endif()

    # message(STATUS "ax_mark_code_files: ${ax_target}")

    get_property(file_list TARGET ${ax_target} PROPERTY SOURCES)

    foreach(single_file ${file_list})
        source_group_single_file(${single_file} GROUP_TO "Source Files" BASE_PATH "${root_dir}")
    endforeach()

endfunction()

# source group one file
# cut the `single_file` absolute path from `BASE_PATH`, then mark file to `GROUP_TO`
function(source_group_single_file single_file)
    set(oneValueArgs GROUP_TO BASE_PATH)
    cmake_parse_arguments(opt "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    # get relative_path
    get_filename_component(abs_path ${single_file} ABSOLUTE)
    file(RELATIVE_PATH relative_path_with_name ${opt_BASE_PATH} ${abs_path})
    get_filename_component(relative_path ${relative_path_with_name} PATH)
    # set source_group, consider sub source group
    string(REPLACE "/" "\\" ide_file_group "${opt_GROUP_TO}/${relative_path}")
    source_group("${ide_file_group}" FILES ${single_file})
endfunction()

# Gets a list of all target libraries that a given target depends on. By target libraries
# it is meant that libraries themselves are CMake targets.
function(get_target_all_library_targets output_list target)
    list(APPEND visited_targets ${target})
    get_target_property(imported ${target} IMPORTED)
    if (imported)
        get_target_property(libs ${target} INTERFACE_LINK_LIBRARIES)
    else()
        get_target_property(libs ${target} LINK_LIBRARIES)
    endif()
    set(lib_targets "")
    foreach(lib ${libs})
        if(TARGET ${lib})
            list(FIND visited_targets ${lib} visited)
            if (${visited} EQUAL -1)
                get_target_all_library_targets(link_lib_targets ${lib})
                list(APPEND lib_targets ${lib} ${link_lib_targets})
            endif()
        endif()
    endforeach()
    set(visited_targets ${visited_targets} PARENT_SCOPE)
    set(${output_list} ${lib_targets} PARENT_SCOPE)
endfunction()

# Gets a list of all compiled shaders that a given target depends on.
#
# A list of all compiled shaders that the executable or library target builds is kept in
# `AX_COMPILED_SHADERS` property on the target. This function uses this property to gather
# the list of all shaders from this target and all libraries that this target depends on.
function(get_target_compiled_shaders output_list target)
    get_target_all_library_targets(libs ${target})
    list(APPEND libs ${target})
    set(shaders)
    foreach(lib ${libs})
        get_target_property(target_shaders ${lib} AX_COMPILED_SHADERS)
        if(target_shaders)
            list(APPEND shaders ${target_shaders})
        endif()
    endforeach()
    set(${output_list} ${shaders} PARENT_SCOPE)
endfunction()

# setup a ax application
function(ax_setup_app_config app_name)
    set(options CONSOLE)
    set(oneValueArgs RUNTIME_OUTPUT_DIR)
    cmake_parse_arguments(opt "${options}" "${oneValueArgs}" ""
                          "" ${ARGN} )
    if (WINRT)
        target_include_directories(${app_name}
            PRIVATE "proj.winrt"
        )
    endif()
    if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        target_link_options(${app_name} PRIVATE "/STACK:4194304")
    endif()
    if(NOT opt_RUNTIME_OUTPUT_DIR)
        # put all output app into bin/${app_name}
        set(opt_RUNTIME_OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin/${app_name}")
    endif()
    set_target_properties(${app_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${opt_RUNTIME_OUTPUT_DIR}")
    if(APPLE)
        # output macOS/iOS .app
        set_target_properties(${app_name} PROPERTIES MACOSX_BUNDLE 1)

        # set codesign
        if(IOS AND (NOT ("${CMAKE_OSX_SYSROOT}" MATCHES ".*simulator.*")))
            set_xcode_property(${app_name} CODE_SIGNING_REQUIRED "YES")
            set_xcode_property(${app_name} CODE_SIGNING_ALLOWED "YES")
        else()
            # By default, explicit disable codesign for macOS or ios Simulator
            set_xcode_property(${app_name} CODE_SIGN_IDENTITY "")
            set_xcode_property(${app_name} CODE_SIGNING_ALLOWED "NO")
        endif()
    elseif(WINDOWS)
        # windows: visual studio/LLVM-clang default is Console app, but we need Windows app
        set(_win32_linker_flags "")
        set(_win32_console_app FALSE)
        
        if (NOT opt_CONSOLE OR WINRT)
            set(_win32_linker_flags "/SUBSYSTEM:WINDOWS")
        else()
            set(_win32_linker_flags "/SUBSYSTEM:CONSOLE")
            set(_win32_console_app TRUE)
        endif()

        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            set(_win32_linker_flags "-Xlinker ${_win32_linker_flags}")
        endif()

        set_property(TARGET ${app_name} APPEND PROPERTY LINK_FLAGS "${_win32_linker_flags}")
        if (_win32_console_app)
            set_source_files_properties(proj.win32/main.cpp PROPERTIES COMPILE_DEFINITIONS _CONSOLE=1)
        endif()
    endif()
    # auto mark code files for IDE when mark app
    if(XCODE OR VS)
        ax_mark_code_files(${app_name})
    endif()

    if (XCODE)
        ax_config_app_xcode_property(${app_name})
    endif()

    if(BUILD_SHARED_LIBS)
        target_compile_definitions(${app_name} PRIVATE SPINEPLUGIN_API=DLLIMPORT) # spine dll
    endif()
    target_link_libraries(${app_name} ${_AX_EXTENSION_LIBS})

    if(XCODE AND AX_ENABLE_AUDIO AND AX_USE_ALSOFT AND ALSOFT_OSX_FRAMEWORK)
        # Embedded soft_oal embedded framework
        # XCODE_LINK_BUILD_PHASE_MODE BUILT_ONLY
        # ???CMake BUG: XCODE_EMBED_FRAMEWORKS_CODE_SIGN_ON_COPY works for first app
        message(STATUS "Embedding framework soft_oal to ${app_name}...")
        set_target_properties(${app_name} PROPERTIES
            XCODE_LINK_BUILD_PHASE_MODE KNOWN_LOCATION
            XCODE_EMBED_FRAMEWORKS OpenAL
            XCODE_EMBED_FRAMEWORKS_CODE_SIGN_ON_COPY ON
            XCODE_EMBED_FRAMEWORKS_REMOVE_HEADERS_ON_COPY ON
        )
    endif()

    # auto looking app shaders source dir and add to glslcc compile-list
    get_target_property(_APP_SOURCE_DIR ${app_name} SOURCE_DIR)
    set(app_shaders_dir "${_APP_SOURCE_DIR}/Source/shaders")

    ax_find_shaders(${app_shaders_dir} app_shaders RECURSE)
    if (app_shaders)
        list(LENGTH app_shaders app_shaders_count)
        message(STATUS "${app_shaders_count} shader sources found in ${app_shaders_dir}")
        # compile app shader to ${CMAKE_BINARY_DIR}/runtime/axslc/custom/
        ax_target_compile_shaders(${app_name} FILES ${app_shaders} CUSTOM)
        source_group("Source Files/Source/shaders" FILES ${app_shaders})
    endif()

    if (IS_DIRECTORY ${GLSLCC_OUT_DIR})
        get_target_property(rt_output ${app_name} RUNTIME_OUTPUT_DIRECTORY)
        if ((WIN32 AND (NOT WINRT)) OR LINUX)
            if (NOT DEFINED AX_PREBUILT_DIR)
                ax_sync_target_res(${app_name} LINK_TO "${rt_output}/${CMAKE_CFG_INTDIR}/axslc" FOLDERS ${GLSLCC_OUT_DIR} SYM_LINK 1 SYNC_TARGET_ID axslc)
            else() # linking with prebuilt, can't use symlink
                ax_sync_target_res(${app_name} LINK_TO "${rt_output}/${CMAKE_CFG_INTDIR}/axslc" FOLDERS ${GLSLCC_OUT_DIR} SYNC_TARGET_ID axslc)
            endif()
        elseif(APPLE)
            if (CMAKE_GENERATOR MATCHES "Xcode")
                set_target_properties(${app_name} PROPERTIES XCODE_EMBED_RESOURCES ${GLSLCC_OUT_DIR})
            else()
                get_target_compiled_shaders(shaders ${app_name})
                ax_mark_resources(FILES ${shaders} BASEDIR ${GLSLCC_OUT_DIR} RESOURCEBASE "Resources/axslc")
                target_sources(${app_name} PRIVATE ${shaders})
            endif()
        elseif(WINRT OR WASM)
            set(app_all_shaders)
            list(APPEND app_all_shaders ${ax_builtin_shaders})
            list(APPEND app_all_shaders ${app_shaders})
            if (WINRT)
                ax_target_embed_compiled_shaders(${app_name} ${rt_output} FILES ${app_all_shaders})
            else()
                # --preload-file
                # refer to: https://emscripten.org/docs/porting/files/packaging_files.html
                target_link_options(${app_name} PRIVATE "--preload-file" ${GLSLCC_OUT_DIR}@axslc/)
            endif()
        endif()
    endif()
endfunction()

set(AX_WASM_SHELL_FILE "${_AX_ROOT}/core/platform/wasm/shell_minimal.html" CACHE STRING "The path of wasm shell file")

option(AX_WASM_ENABLE_DEVTOOLS "Enable wasm devtools" ON)

set(_AX_WASM_EXPORTS "_main")
if(AX_WASM_ENABLE_DEVTOOLS)
    set(_AX_WASM_EXPORTS "${_AX_WASM_EXPORTS},_axmol_director_pause,_axmol_director_resume,_axmol_director_step")
endif()
set(AX_WASM_EXPORTS "${_AX_WASM_EXPORTS}" CACHE STRING "" FORCE)

# stupid & pitfall: function not emcc not output .html
macro (ax_setup_app_props app_name)
    if(WINRT)
        set_target_properties(
            ${APP_NAME}
            PROPERTIES
            # ----- C++/WinRT -----
            # VS_PACKAGE_REFERENCES "Microsoft.Windows.CppWinRT_${AX_CPPWINRT_VERSION}"
            VS_PROJECT_IMPORT "${CMAKE_BINARY_DIR}/CppWinRT.props"
            VS_GLOBAL_CppWinRTOptimized true
            VS_GLOBAL_CppWinRTRootNamespaceAutoMerge true
            VS_GLOBAL_CppWinRTGenerateWindowsMetadata true
            VS_GLOBAL_MinimalCoreWin true
            VS_GLOBAL_AppContainerApplication true
            VS_GLOBAL_RootNameSpace "AxmolAppWinRT" # this is important for cppwinrt to fix Generated Files\XamlMetaDataProvider.idl(4): error MIDL2025: [msg]syntax error [context]: expecting NamespaceTag near "{"
        )
        target_link_libraries(${APP_NAME} ${CMAKE_BINARY_DIR}/packages/Microsoft.Windows.CppWinRT/build/native/Microsoft.Windows.CppWinRT.targets)
    elseif(WASM)
        get_target_property(_APP_SOURCE_DIR ${app_name} SOURCE_DIR)
        set(CMAKE_EXECUTABLE_SUFFIX ".html")
        target_link_options(${app_name} PRIVATE
                            "-sEXPORTED_FUNCTIONS=[${AX_WASM_EXPORTS}]"
                            "-sEXPORTED_RUNTIME_METHODS=[ccall,cwrap]"
                            )
        set(EMSCRIPTEN_LINK_FLAGS "-lidbfs.js -s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 -s STACK_SIZE=4mb -s INITIAL_MEMORY=512MB --shell-file ${AX_WASM_SHELL_FILE} --use-preload-cache")
        # Disable wasm, generate js build?
        # string(APPEND EMSCRIPTEN_LINK_FLAGS " -s WASM=0")
        # string(APPEND EMSCRIPTEN_LINK_FLAGS " -s SEPARATE_DWARF_URL=https://xxx:8080/axmolwasm/axmolwasm/build/HelloLua.debug.wasm")
        # string(APPEND EMSCRIPTEN_LINK_FLAGS " -gseparate-dwarf=HelloLua.debug.wasm")

        set(_APP_RES_FOLDER "${_APP_SOURCE_DIR}/Content")
        foreach(FOLDER IN LISTS _APP_RES_FOLDER)
            string(APPEND EMSCRIPTEN_LINK_FLAGS " --preload-file ${FOLDER}/@/")
        endforeach()
        
        set_target_properties(${app_name} PROPERTIES LINK_FLAGS "${EMSCRIPTEN_LINK_FLAGS}")
    endif()
endmacro()

# if cc_variable not set, then set it cc_value
macro(ax_set_default_value cc_variable cc_value)
    if(NOT DEFINED ${cc_variable})
        set(${cc_variable} ${cc_value})
    endif()
endmacro()

macro(ax_setup_winrt_sources )
    set_property(SOURCE "proj.winrt/App.xaml" PROPERTY VS_XAML_TYPE "ApplicationDefinition")

    ax_mark_multi_resources(platform_content_files RES_TO "Content" FOLDERS "${CMAKE_CURRENT_SOURCE_DIR}/proj.winrt/Content")

    get_target_depends_ext_dlls(3rdparty prebuilt_dlls)

    if (NOT prebuilt_dlls) 
        set(prebuilt_dlls
            ${_AX_ROOT}/${_AX_THIRDPARTY_NAME}/zlib/_x/lib/${PLATFORM_NAME}/${ARCH_ALIAS}/zlib1.dll
            ${_AX_ROOT}/${_AX_THIRDPARTY_NAME}/openssl/_x/lib/${PLATFORM_NAME}/${ARCH_ALIAS}/libssl-3-x64.dll
            ${_AX_ROOT}/${_AX_THIRDPARTY_NAME}/openssl/_x/lib/${PLATFORM_NAME}/${ARCH_ALIAS}/libcrypto-3-x64.dll
            ${_AX_ROOT}/${_AX_THIRDPARTY_NAME}/curl/_x/lib/${PLATFORM_NAME}/${ARCH_ALIAS}/libcurl.dll
            ${_AX_ROOT}/${_AX_THIRDPARTY_NAME}/angle/_x/lib/${PLATFORM_NAME}/${ARCH_ALIAS}/libGLESv2.dll
            ${_AX_ROOT}/${_AX_THIRDPARTY_NAME}/angle/_x/lib/${PLATFORM_NAME}/${ARCH_ALIAS}/libEGL.dll
            ${_AX_ROOT}/${_AX_THIRDPARTY_NAME}/angle/_x/lib/${PLATFORM_NAME}/${ARCH_ALIAS}/d3dcompiler_47.dll)
    endif()
    ax_mark_multi_resources(prebuilt_dlls RES_TO "." FILES ${prebuilt_dlls})

    list(APPEND PLATFORM_SOURCES
        proj.winrt/App.xaml
        proj.winrt/App.idl
        proj.winrt/App.h
        proj.winrt/App.cpp
        proj.winrt/Package.appxmanifest
        ${_AX_ROOT}/core/platform/winrt/xaml/OpenGLESPage.xaml
        ${_AX_ROOT}/core/platform/winrt/xaml/OpenGLESPage.idl
        ${_AX_ROOT}/core/platform/winrt/xaml/OpenGLESPage.h
        ${_AX_ROOT}/core/platform/winrt/xaml/OpenGLESPage.cpp
        ${_AX_ROOT}/core/platform/winrt/xaml/OpenGLES.h
        ${_AX_ROOT}/core/platform/winrt/xaml/OpenGLES.cpp
        ${_AX_ROOT}/core/platform/winrt/xaml/AxmolRenderer.h
        ${_AX_ROOT}/core/platform/winrt/xaml/AxmolRenderer.cpp
    )

    file(TO_NATIVE_PATH "${CMAKE_CURRENT_LIST_DIR}/proj.winrt/App.xaml" APP_XAML_FULL_PATH)
    set_property(
        SOURCE proj.winrt/App.h proj.winrt/App.cpp proj.winrt/App.idl
        PROPERTY VS_SETTINGS
        "DependentUpon=${APP_XAML_FULL_PATH}"
    )
    file(TO_NATIVE_PATH "${_AX_ROOT}/core/platform/winrt/xaml/OpenGLESPage.xaml" MAINPAGE_XAML_FULL_PATH)
    set_property(
        SOURCE ${_AX_ROOT}/core/platform/winrt/xaml/OpenGLESPage.h ${_AX_ROOT}/core/platform/winrt/xaml/OpenGLESPage.cpp ${_AX_ROOT}/core/platform/winrt/xaml/OpenGLESPage.idl
        PROPERTY VS_SETTINGS
        "DependentUpon=${MAINPAGE_XAML_FULL_PATH}"
    )

    list(APPEND GAME_INC_DIRS ${_AX_ROOT}/core/platform/winrt/xaml)

    list(APPEND GAME_HEADER
        ${PLATFORM_HEADERS}
    )
    list(APPEND GAME_SOURCE
        ${PLATFORM_SOURCES}
        ${platform_content_files}
        ${prebuilt_dlls}
        )
endmacro()

# must call last line
function(get_all_targets var)
    set(targets)
    get_all_targets_recursive(targets ${CMAKE_CURRENT_SOURCE_DIR})
    set(${var} ${targets} PARENT_SCOPE)
endfunction()

macro(get_all_targets_recursive targets dir)
    get_property(subdirectories DIRECTORY ${dir} PROPERTY SUBDIRECTORIES)
    foreach(subdir ${subdirectories})
        get_all_targets_recursive(${targets} ${subdir})
    endforeach()

    get_property(current_targets DIRECTORY ${dir} PROPERTY BUILDSYSTEM_TARGETS)
    list(APPEND ${targets} ${current_targets})
endmacro()

# set Xcode property for application, include all depend target
macro(ax_config_app_xcode_property ax_app)
    set(depend_libs)
    search_depend_libs_recursive(${ax_app} depend_libs)
    foreach(depend_lib ${depend_libs})
        if(TARGET ${depend_lib})
            ax_config_target_xcode_property(${depend_lib})
        endif()
    endforeach()
endmacro()

# custom Xcode property for iOS target
macro(ax_config_target_xcode_property ax_target)
    if(IOS)
        set(real_target)
        get_property(real_target TARGET ${ax_target} PROPERTY ALIASED_TARGET)
        if (NOT real_target)
            set(real_target ${ax_target})
        endif()
        set_xcode_property(${real_target} ENABLE_BITCODE "NO")
        set_xcode_property(${real_target} ONLY_ACTIVE_ARCH "YES")
    endif()
endmacro()

# works same as find_package, but do additional care to properly find
macro(ax_find_package pkg_name pkg_prefix)
    if(NOT ${pkg_prefix}_FOUND)
        find_package(${pkg_name} ${ARGN})
    endif()
    if(NOT ${pkg_prefix}_INCLUDE_DIRS AND ${pkg_prefix}_INCLUDE_DIR)
        set(${pkg_prefix}_INCLUDE_DIRS ${${pkg_prefix}_INCLUDE_DIR})
    endif()
    if(NOT ${pkg_prefix}_LIBRARIES AND ${pkg_prefix}_LIBRARY)
        set(${pkg_prefix}_LIBRARIES ${${pkg_prefix}_LIBRARY})
    endif()

    message(STATUS "${pkg_name} include dirs: ${${pkg_prefix}_INCLUDE_DIRS}")
endmacro()

# ax_use_pkg(pkg) function.
# This function applies standard package variables (after find_package(pkg) call) to current scope
# Recognized variables: <pkg>_INCLUDE_DIRS, <pkg>_LIBRARIES, <pkg>_LIBRARY_DIRS
# Also if BUILD_SHARED_LIBS variable off, it is try to use <pkg>_STATIC_* vars before
function(ax_use_pkg target pkg)
    set(prefix ${pkg})

    set(_include_dirs)
    if(NOT _include_dirs)
        set(_include_dirs ${${prefix}_INCLUDE_DIRS})
    endif()
    if(NOT _include_dirs)
        # backward compat with old package-find scripts
        set(_include_dirs ${${prefix}_INCLUDE_DIR})
    endif()
    if(_include_dirs)
        include_directories(${_include_dirs})
        message(STATUS "${pkg} add to include_dirs: ${_include_dirs}")
    endif()

    set(_library_dirs)
    if(NOT _library_dirs)
        set(_library_dirs ${${prefix}_LIBRARY_DIRS})
    endif()
    if(_library_dirs)
        link_directories(${_library_dirs})
        # message(STATUS "${pkg} add to link_dirs: ${_library_dirs}")
    endif()

    set(_libs)
    if(NOT _libs)
        set(_libs ${${prefix}_LIBRARIES})
    endif()
    if(NOT _libs)
        set(_libs ${${prefix}_LIBRARY})
    endif()
    if(_libs)
        target_link_libraries(${target} ${_libs})
        message(STATUS "${pkg} libs added to '${target}': ${_libs}")
    endif()

    set(_defs)
    if(NOT _defs)
        set(_defs ${${prefix}_DEFINITIONS})
    endif()
    if(_defs)
        add_definitions(${_defs})
        message(STATUS "${pkg} add definitions: ${_defs}")
    endif()

    set(_dlls)
    if(NOT _dlls)
        set(_dlls ${${prefix}_DLLS})
    endif()
    if(_dlls)
        if(MSVC)
            # message(STATUS "${target} add dll: ${_dlls}")
            get_property(pre_dlls
                         TARGET ${target}
                         PROPERTY AX_DEPEND_DLLS)
            if(pre_dlls)
                set(_dlls ${pre_dlls} ${_dlls})
            endif()
            set_property(TARGET ${target}
                         PROPERTY
                         AX_DEPEND_DLLS ${_dlls}
                         )
        endif()
    endif()

endfunction()

# The axmol preprocessors config helper macro
macro(ax_config_pred target_name pred)
    if(${pred})
        target_compile_definitions(${target_name} PUBLIC ${pred}=1)
    endif()
endmacro()

macro(source_group_by_dir proj_dir source_files)
    if(MSVC OR APPLE)
        get_filename_component(sgbd_cur_dir ${proj_dir} ABSOLUTE)
        foreach(sgbd_file ${${source_files}})
            get_filename_component(sgbd_abs_file ${sgbd_file} ABSOLUTE)
            file(RELATIVE_PATH sgbd_fpath ${sgbd_cur_dir} ${sgbd_abs_file})
            string(REGEX REPLACE "\(.*\)/.*" \\1 sgbd_group_name ${sgbd_fpath})
            string(COMPARE EQUAL ${sgbd_fpath} ${sgbd_group_name} sgbd_nogroup)
            string(REPLACE "/" "\\" sgbd_group_name ${sgbd_group_name})
            if(sgbd_nogroup)
                set(sgbd_group_name "\\")
            endif(sgbd_nogroup)
            source_group(${sgbd_group_name} FILES ${sgbd_file})
        endforeach(sgbd_file)
    endif(MSVC OR APPLE)
endmacro(source_group_by_dir)
