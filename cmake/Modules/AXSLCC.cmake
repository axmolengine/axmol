cmake_policy(PUSH)
cmake_policy(SET CMP0057 NEW)

# #############################################################
# #  enable shader lang by shader compiler: axslcc
macro(axslcc_option variable value)
  if(NOT DEFINED ${variable})
    set(${variable} ${value} CACHE STATIC "" FORCE)
  endif()
endmacro()

axslcc_option(AXSLCC_FRAG_SOURCE_FILE_EXTENSIONS ".frag;.fsh;.fs")
axslcc_option(AXSLCC_VERT_SOURCE_FILE_EXTENSIONS ".vert;.vsh;.vs")
axslcc_option(AXSLCC_OUT_DIR ${CMAKE_BINARY_DIR}/runtime/axslc)
axslcc_option(AXSLCC_FIND_PROG_ROOT "")

find_program(AXSLCC_EXE NAMES axslcc
  PATHS ${AXSLCC_FIND_PROG_ROOT}
)

if(NOT AXSLCC_EXE)
  message(STATUS, "axslcc not found.")
  message(FATAL_ERROR "Please run setup.ps1 again to download axslcc, and run CMake again.")
endif()

message(STATUS "AXSLCC_OUT_DIR=${AXSLCC_OUT_DIR}")
message(STATUS "AXSLCC_FIND_PROG_ROOT=${AXSLCC_FIND_PROG_ROOT}")
message(STATUS "AXSLCC_FRAG_SOURCE_FILE_EXTENSIONS=${AXSLCC_FRAG_SOURCE_FILE_EXTENSIONS}")
message(STATUS "AXSLCC_VERT_SOURCE_FILE_EXTENSIONS=${AXSLCC_VERT_SOURCE_FILE_EXTENSIONS}")

# PROPERTY: include direcotries (optional)
define_property(SOURCE PROPERTY AXSLCC_INCLUDE_DIRS
  BRIEF_DOCS "Compiled shader include directories"
  FULL_DOCS "Compiled shader include directories, seperated with comma")

# PROPERTY: defines (optional) TODO: rename to PREPROCESSOR_LIST
define_property(SOURCE PROPERTY AXSLCC_DEFINES
  BRIEF_DOCS "Compiled shader defines"
  FULL_DOCS "Compiled shader defines, seperated with comma")

# PROPERTY: output1 (optional) TODO: rename to PREPROCESSOR_LIST
define_property(SOURCE PROPERTY AXSLCC_OUTPUT1
  BRIEF_DOCS "Compiled shader output1 additional defines"
  FULL_DOCS "Compiled shader output1 additional defines, seperated with comma")

# PROPERTY: glscc output (optional)
define_property(SOURCE PROPERTY AXSLCC_OUTPUT
  BRIEF_DOCS "The compiled sources shader output path list"
  FULL_DOCS "The compiled shaders output list, seperated with comma")

define_property(TARGET PROPERTY SHADER_DEPENDS
  BRIEF_DOCS "The shader depends of normal target"
  FULL_DOCS "The shader depends of normal target, seperated with comma")

# Find shader sources in specified directory
# syntax: ax_find_shaders(dir shader_sources [RECURSE])
# examples:
# - ax_find_shaders("${CMAKE_CURRENT_LIST_DIR}/axmol/renderer/shaders" runtime_shader_sources)
# - ax_find_shaders("${CMAKE_CURRENT_LIST_DIR}/Source" custom_shader_sources RECURSE)
function(ax_find_shaders dir varName)
  set(options RECURSE)
  cmake_parse_arguments(opt "${options}" "" "" ${ARGN})

  set(SC_FILTERS "")

  foreach(fileext ${AXSLCC_FRAG_SOURCE_FILE_EXTENSIONS})
    list(APPEND SC_FILTERS "${dir}/*${fileext}")
  endforeach()

  foreach(fileext ${AXSLCC_VERT_SOURCE_FILE_EXTENSIONS})
    list(APPEND SC_FILTERS "${dir}/*${fileext}")
  endforeach()

  if(opt_RECURSE)
    file(GLOB_RECURSE out_files ${SC_FILTERS})
  else()
    file(GLOB out_files ${SC_FILTERS})
  endif()

  set(${varName} ${out_files} PARENT_SCOPE)
endfunction()

# This function allow make shader files (.frag, .vert) compiled with axslcc
# usage:
# - ax_add_shader_target(shader_target FILES source_files BUILTIN): output compiled shader to ${CMAKE_BINARY_DIR}/runtime/axslc/xxx_fs
# - ax_add_shader_target(shader_target FILES source_files):         output compiled shader to ${CMAKE_BINARY_DIR}/runtime/axslc/custom/xxx_fs
# - ax_add_shader_target(shader_target FILES source_files CVAR): the shader will compiled to c hex header for embed include by C/C++ use
# Use global variable to control shader file extension:
# - AXSLCC_FRAG_SOURCE_FILE_EXTENSIONS: default is .frag;.fsh
# - AXSLCC_VERT_SOURCE_FILE_EXTENSIONS: default is .vert;.vsh
#
# CUSTOM operation is deprecated
#
function(ax_add_shader_target target_name)
  set(options BUILTIN CVAR CUSTOM)
  set(oneValueArgs PATH)
  set(multiValueArgs FILES)
  cmake_parse_arguments(opt "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT TARGET ${target_name})
    message(STATUS "Add shader build target: ${target_name}, BUILTIN:${opt_BUILTIN}")
    add_custom_target(${target_name})
  endif()

  set(_all_shader_files)

  if(opt_PATH)
    if(NOT opt_FILES)
      set(opt_FILES "")
    endif()
    ax_find_shaders(${opt_PATH} opt_FILES)
  endif()

  set(compiled_shaders)

  foreach(SC_FILE ${opt_FILES})
    get_filename_component(FILE_EXT ${SC_FILE} LAST_EXT)
    get_filename_component(FILE_NAME ${SC_FILE} NAME_WE)
    string(TOLOWER "${FILE_EXT}" FILE_EXT)

    set(SC_DEFINES "")

    # silent when compile shader success
    set(SC_FLAGS "--silent" "--err-format=msvc")

    # shader lang
    set(SC_PROFILE "")

    if(AX_RENDER_API STREQUAL "gl")
      if(AX_GLES_PROFILE)
        # version 300 es
        set(OUT_LANG "ESSL")
        if(AX_GLES_PROFILE GREATER_EQUAL 300)
          set(SC_PROFILE "300")
          set(SC_DEFINES "AXSLC_TARGET_GLES")
        else()
          # GLSL2 use glsl100 syntax es profile aka essl100
          set(SC_PROFILE "100")
          set(SC_DEFINES "AXSLC_TARGET_GLES2")
        endif()
        set(SC_DEFINES "AXSLC_TARGET_GLSL,${SC_DEFINES}")
        list(APPEND SC_FLAGS "--lang=gles" "--profile=${SC_PROFILE}")
      else()
        # version 330
        set(OUT_LANG "GLSL")
        set(SC_PROFILE "330")
        set(SC_DEFINES "AXSLC_TARGET_GLSL")
        list(APPEND SC_FLAGS "--lang=glsl" "--profile=${SC_PROFILE}")
      endif()
    elseif(AX_RENDER_API STREQUAL "d3d")
      set(OUT_LANG "HLSL")
      set(SC_PROFILE "50") # D3D11 HLSL 5.0
      set(SC_DEFINES "AXSLC_TARGET_HLSL")
      list(APPEND SC_FLAGS "--lang=hlsl" "--profile=${SC_PROFILE}")
    elseif(AX_RENDER_API STREQUAL "mtl")
      set(OUT_LANG "MSL")
      set(SC_DEFINES "AXSLC_TARGET_MSL")
      list(APPEND SC_FLAGS "--lang=msl")
    endif()

    # no-suffix since 1.18.1 released by axmolengine
    list(APPEND SC_FLAGS "--no-suffix")

    # The follow flags was separated from --automap since axslcc-1.13.1, many exists shaders not specify it binding index for uniform blocks,
    # so add auto map bindings suppress errors for exists shaders. in future if we migrate all
    # shaders, these flags can be removed
    # Note: axmol only use binding_index=0 for uniform blocks because axmol limit support per-uniform-block/per-stage
    list(APPEND SC_FLAGS "--auto-map-bindings")
    list(APPEND SC_FLAGS "--auto-map-locations")

    # defines
    get_source_file_property(SOURCE_SC_DEFINES ${SC_FILE} AXSLCC_DEFINES)

    if(NOT(SOURCE_SC_DEFINES STREQUAL "NOTFOUND"))
      set(SC_DEFINES "${SC_DEFINES},${SOURCE_SC_DEFINES}")
    endif()

    if(SC_DEFINES)
      list(APPEND SC_FLAGS "\"--defines=${SC_DEFINES}\"")
    endif()

    # includes
    get_source_file_property(INC_DIRS ${SC_FILE} AXSLCC_INCLUDE_DIRS)

    if(INC_DIRS STREQUAL "NOTFOUND")
      set(INC_DIRS "")
    endif()

    list(APPEND INC_DIRS "${_AX_ROOT}/axmol/renderer/shaders")
    list(APPEND SC_FLAGS "--include-dirs=${INC_DIRS}")

    if(opt_CVAR)
      list(APPEND SC_FLAGS "--cvar=shader_rt_${FILE_NAME}")
    endif()

    # sgs, because Apple Metal lack of shader uniform reflect and d3d reflect only support semantic name, so use --sgs --refelect
    if(AX_RENDER_API STREQUAL "mtl" OR AX_RENDER_API STREQUAL "d3d")
      list(APPEND SC_FLAGS "--sgs" "--reflect")
    endif()

    # input
    if(${FILE_EXT} IN_LIST AXSLCC_FRAG_SOURCE_FILE_EXTENSIONS)
      list(APPEND SC_FLAGS "--frag=${SC_FILE}")
      set(SC_TYPE "fs")
    elseif(${FILE_EXT} IN_LIST AXSLCC_VERT_SOURCE_FILE_EXTENSIONS)
      set(SC_TYPE "vs")
      list(APPEND SC_FLAGS "--vert=${SC_FILE}")
    else()
      message(FATAL_ERROR "Invalid shader source, the file extesion must be one of .frag;.vert")
    endif()

    # output
    set(OUT_DIR ${AXSLCC_OUT_DIR})

    if(NOT opt_BUILTIN)
      set(OUT_DIR "${OUT_DIR}/custom")
    endif()

    if(NOT(IS_DIRECTORY ${OUT_DIR}))
      file(MAKE_DIRECTORY ${OUT_DIR})
    endif()

    set(SC_OUTPUT "${OUT_DIR}/${FILE_NAME}_${SC_TYPE}")

    file(TO_CMAKE_PATH "${SC_OUTPUT}" SC_OUTPUT)
    set(SC_COMMENT "[${OUT_LANG}${SC_PROFILE}] Compiling shader ${SC_FILE} to ${SC_OUTPUT} ...")

    get_source_file_property(SOURCE_SC_OUTPUT1 ${SC_FILE} AXSLCC_OUTPUT1)

    string(REPLACE ";" " " FULL_COMMAND_LINE "${AXSLCC_EXE};${SC_FLAGS} ...")

    if(SOURCE_SC_OUTPUT1 STREQUAL "NOTFOUND") # single output
      list(APPEND SC_FLAGS "--output=${SC_OUTPUT}")
      set_source_files_properties(${SC_FILE} DIRECTORY ${CMAKE_BINARY_DIR} PROPERTIES AXSLCC_OUTPUT ${SC_OUTPUT})
      add_custom_command(
        MAIN_DEPENDENCY ${SC_FILE} OUTPUT ${SC_OUTPUT} COMMAND ${AXSLCC_EXE} ${SC_FLAGS}
        COMMENT "${SC_COMMENT}"
      )
      list(APPEND compiled_shaders ${SC_OUTPUT})
    else() # dual outputs
      set(SC_DEFINES1 "${SC_DEFINES},${SOURCE_SC_OUTPUT1}")

      set(SC_FLAGS1 ${SC_FLAGS})
      list(REMOVE_ITEM SC_FLAGS1 "\"--defines=${SC_DEFINES}\"")
      list(APPEND SC_FLAGS1 "\"--defines=${SC_DEFINES1}\"")

      list(APPEND SC_FLAGS "--output=${SC_OUTPUT}")

      set(SC_OUTPUT1 "${SC_OUTPUT}_1")
      list(APPEND SC_FLAGS1 "--output=${SC_OUTPUT1}")
      string(REPLACE ";" " " FULL_COMMAND_LINE1 "${AXSLCC_EXE};${SC_FLAGS1} ...")
      set_source_files_properties(${SC_FILE} DIRECTORY ${CMAKE_BINARY_DIR} PROPERTIES AXSLCC_OUTPUT "${SC_OUTPUT};${SC_OUTPUT1}")
      add_custom_command(
        MAIN_DEPENDENCY ${SC_FILE}
        OUTPUT ${SC_OUTPUT} ${SC_OUTPUT1}
        COMMAND ${AXSLCC_EXE} ${SC_FLAGS}
        COMMAND ${AXSLCC_EXE} ${SC_FLAGS1}
        COMMENT "${SC_COMMENT}"
      )
      list(APPEND compiled_shaders ${SC_OUTPUT} ${SC_OUTPUT1})
    endif()
  endforeach()

  target_sources(${target_name} PRIVATE ${opt_FILES})

  # Set `AX_COMPILED_SHADERS` property on the target to store the list of compiled
  # shaders for this target. This is later used for setting up app's resource copying.
  get_target_property(target_compiled_shaders ${target_name} AX_COMPILED_SHADERS)

  if(NOT target_compiled_shaders)
    set(target_compiled_shaders "")
  endif()

  list(APPEND target_compiled_shaders ${compiled_shaders})
  set_property(TARGET ${target_name} PROPERTY AX_COMPILED_SHADERS ${target_compiled_shaders})

  # folder
  set_target_properties(${target_name} PROPERTIES FOLDER "Shaders")
endfunction()

function(ax_add_shader_dependencies target)
  foreach(shader_tgt IN LISTS ARGN)
    add_dependencies(${target} ${shader_tgt})
    set_property(TARGET ${target} APPEND PROPERTY SHADER_DEPENDS ${shader_tgt})
  endforeach()
endfunction()

function(ax_add_shader_target_for target)
  set(shader_tgt ${target}_shaders)
  cmake_language(CALL ax_add_shader_target ${shader_tgt} ${ARGN})
  ax_add_shader_dependencies(${target} ${shader_tgt})
endfunction()

# for winrt/uwp only
function(ax_target_embed_compiled_shaders target_name rc_output)
  set(multiValueArgs FILES)
  cmake_parse_arguments(opt "" "" "${multiValueArgs}" ${ARGN})
  set(_filters_xml_content "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n")
  string(APPEND _filters_xml_content "<Project ToolsVersion=\"12.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n")
  string(APPEND _filters_xml_content "  <ItemGroup Label=\"axslc\">\n")

  set(_props_xml_content "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n")
  string(APPEND _props_xml_content "<Project DefaultTargets=\"Build\" ToolsVersion=\"12.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n")
  string(APPEND _props_xml_content "  <ItemGroup Label=\"axslc\">\n")

  foreach(compiled_shader ${opt_FILES})
    file(RELATIVE_PATH compiled_shader_rp ${AXSLCC_OUT_DIR} ${compiled_shader})
    file(TO_NATIVE_PATH "Content/axslc/${compiled_shader_rp}" compiled_shader_target_dir)
    file(TO_NATIVE_PATH "${compiled_shader}" compiled_shader_path)
    set(app_all_shaders_filters "${app_all_shaders_filters}  <ItemGroup Label=\"axslc\">\n")
    string(APPEND _props_xml_content "    <None Include=\"${compiled_shader_path}\">\n")
    string(APPEND _props_xml_content "      <Link>${compiled_shader_target_dir}</Link>\n")
    string(APPEND _props_xml_content "      <DeploymentContent Condition=\"'\$(Configuration)|\$(Platform)'=='Debug|x64'\">true</DeploymentContent>\n")
    string(APPEND _props_xml_content "      <DeploymentContent Condition=\"'\$(Configuration)|\$(Platform)'=='Release|x64'\">true</DeploymentContent>\n")
    string(APPEND _props_xml_content "      <DeploymentContent Condition=\"'\$(Configuration)|\$(Platform)'=='MinSizeRel|x64'\">true</DeploymentContent>\n")
    string(APPEND _props_xml_content "      <DeploymentContent Condition=\"'\$(Configuration)|\$(Platform)'=='RelWithDebInfo|x64'\">true</DeploymentContent>\n")
    string(APPEND _props_xml_content "    </None>\n")

    string(APPEND _filters_xml_content "    <None Include=\"${compiled_shader_path}\">\n")
    string(APPEND _filters_xml_content "      <Filter>Content\\axslc</Filter>\n")
    string(APPEND _filters_xml_content "    </None>\n")
  endforeach()

  string(APPEND _props_xml_content "  </ItemGroup>\n</Project>")

  string(APPEND _filters_xml_content "    <Filter Include=\"Content\\axslc\">\n")
  string(APPEND _filters_xml_content "      <UniqueIdentifier>{558D563C-9BE5-4C83-96E9-9C09A63BAF97}</UniqueIdentifier>\n")
  string(APPEND _filters_xml_content "    </Filter>\n")
  string(APPEND _filters_xml_content "  </ItemGroup>\n</Project>")

  set(props_file "${rt_output}/axslc.props")
  write_file("${props_file}" "${_props_xml_content}")

  set(filters_file "${rt_output}/axslc.filters.props")
  write_file("${filters_file}" "${_filters_xml_content}")

  set_target_properties(${target_name} PROPERTIES VS_USER_PROPS "${props_file}" VS_FILTER_PROPS "${filters_file}")
endfunction()

cmake_policy(POP)
