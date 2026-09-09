cmake_policy(PUSH)
cmake_policy(SET CMP0057 NEW)

# #############################################################
# #  enable shader lang by shader compiler: axslcc
macro(axslcc_option variable value)
  if(NOT DEFINED ${variable})
    set(${variable} ${value} CACHE STATIC "" FORCE)
  endif()
endmacro()

axslcc_option(AXSLCC_SOURCE_FILE_EXTENSIONS ".hlsl")
axslcc_option(AXSLCC_OUT_DIR ${CMAKE_BINARY_DIR}/runtime/axslc)
axslcc_option(AXSLCC_FIND_PROG_ROOT "")
axslcc_option(AXSLCC_FLAGS "-S")

find_program(AXSLCC_EXE NAMES axslcc
  PATHS ${AXSLCC_FIND_PROG_ROOT}
)

if(NOT AXSLCC_EXE)
  message(STATUS, "axslcc not found.")
  message(FATAL_ERROR "Please run setup.ps1 again to download axslcc, and run CMake again.")
endif()

message(STATUS "AXSLCC_OUT_DIR=${AXSLCC_OUT_DIR}")
message(STATUS "AXSLCC_FIND_PROG_ROOT=${AXSLCC_FIND_PROG_ROOT}")

# PROPERTY: include direcotries (optional)
define_property(SOURCE PROPERTY AXSLCC_INCLUDE_DIRS
  BRIEF_DOCS "Compiled shader include directories"
  FULL_DOCS "Compiled shader include directories, seperated with comma")

# PROPERTY: defines (optional)
define_property(SOURCE PROPERTY AXSLCC_DEFINES
  BRIEF_DOCS "Compiled shader defines"
  FULL_DOCS "Compiled shader defines, seperated with comma")

# PROPERTY: variant defines (optional) multi-compile variants
define_property(SOURCE PROPERTY AXSLCC_VARIANT_DEFINES
  BRIEF_DOCS "Compiled shader variant defines"
  FULL_DOCS "Additional defines per output variant. cmake-list separated by semicolons, each element is comma-separated defines for one variant. Outputs: base, base_1, base_2, ...")

# PROPERTY: axslcc output (optional)
define_property(SOURCE PROPERTY AXSLCC_OUTPUT
  BRIEF_DOCS "The compiled sources shader output path list"
  FULL_DOCS "The compiled shaders output list, seperated with comma")

# PROPERTY: per-shader axslcc targets override (optional)
# If set, overrides the backend-derived target list (e.g. gl-430;gles-310;d3d11)
# for a specific shader source file.
define_property(SOURCE PROPERTY AXSLCC_TARGETS
  BRIEF_DOCS "Per-shader axslcc target override"
  FULL_DOCS "Semicolon-separated axslcc target specs (e.g. gl-430;gles-310;d3d11) overriding the default target list for this shader")

define_property(TARGET PROPERTY SHADER_DEPENDS
  BRIEF_DOCS "The shader depends of normal target"
  FULL_DOCS "The shader depends of normal target, seperated with comma")

# Helper: parse comma-separated preprocessor defines into -D-prefixed cmake list
function(axslcc_parse_defines define_string out_var)
  set(result "")
  if(NOT(define_string STREQUAL "NOTFOUND") AND NOT(define_string STREQUAL ""))
    string(REPLACE "," ";" def_list "${define_string}")
    foreach(defv ${def_list})
      list(APPEND result "-D${defv}")
    endforeach()
  endif()
  set(${out_var} ${result} PARENT_SCOPE)
endfunction()

# Find shader sources in specified directory
# syntax: ax_find_shaders(dir shader_sources [RECURSE])
function(ax_find_shaders dir varName)
  set(options RECURSE)
  cmake_parse_arguments(opt "${options}" "" "" ${ARGN})

  set(SC_FILTERS "")

  foreach(fileext ${AXSLCC_SOURCE_FILE_EXTENSIONS})
    list(APPEND SC_FILTERS "${dir}/*${fileext}")
  endforeach()

  if(opt_RECURSE)
    file(GLOB_RECURSE out_files ${SC_FILTERS})
  else()
    file(GLOB out_files ${SC_FILTERS})
  endif()

  set(${varName} ${out_files} PARENT_SCOPE)
endfunction()

# ax_add_shader_target: compile HLSL shader sources to binary SC chunks
# usage:
# - ax_add_shader_target(shader_target FILES source_files BUILTIN)
# - ax_add_shader_target(shader_target FILES source_files)
function(ax_add_shader_target target_name)
  set(options BUILTIN CVAR)
  set(oneValueArgs PATH)
  set(multiValueArgs FILES)
  cmake_parse_arguments(opt "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT TARGET ${target_name})
    message(STATUS "Add shader build target: ${target_name}, BUILTIN:${opt_BUILTIN}")
    add_custom_target(${target_name})
  endif()

  if(opt_PATH)
    if(NOT opt_FILES)
      set(opt_FILES "")
    endif()
    ax_find_shaders(${opt_PATH} opt_FILES)
  endif()

  # Build target list based on enabled RHIs
  set(TARGET_LIST "")
  if(AX_ENABLE_GL)
    if(AX_GLES_PROFILE)
      if(AX_GLES_PROFILE GREATER_EQUAL 300)
        list(APPEND TARGET_LIST "gles")
      else()
        list(APPEND TARGET_LIST "glsl-100")
      endif()
    else()
      list(APPEND TARGET_LIST "gl")
    endif()
  endif()
  if(AX_ENABLE_D3D11)
    list(APPEND TARGET_LIST "d3d11")
  endif()
  if(AX_ENABLE_D3D12)
    list(APPEND TARGET_LIST "d3d12")
  endif()
  if(AX_ENABLE_MTL)
    list(APPEND TARGET_LIST "mtl")
  endif()
  if(AX_ENABLE_VK)
    list(APPEND TARGET_LIST "vk")
  endif()
  if(NOT TARGET_LIST)
    message(FATAL_ERROR "No shader targets enabled. Enable at least one RHI backend.")
  endif()

  set(compiled_shaders)

  foreach(SC_FILE ${opt_FILES})
    get_filename_component(FILE_EXT ${SC_FILE} LAST_EXT)
    get_filename_component(FILE_NAME ${SC_FILE} NAME_WE)
    string(TOLOWER "${FILE_EXT}" FILE_EXT)

    set(SC_DEFINES "")
    set(SC_FLAGS "-a")

    separate_arguments(AXSLCC_FLAGS_LIST NATIVE_COMMAND "${AXSLCC_FLAGS}")
    list(APPEND SC_FLAGS ${AXSLCC_FLAGS_LIST})

    # Per-shader target override, otherwise use the backend-derived target list.
    set(SC_TARGET_LIST ${TARGET_LIST})
    get_source_file_property(SOURCE_SC_TARGETS ${SC_FILE} AXSLCC_TARGETS)
    if(NOT SOURCE_SC_TARGETS STREQUAL "NOTFOUND")
      set(SC_TARGET_LIST ${SOURCE_SC_TARGETS})
    endif()

    foreach(TARG ${SC_TARGET_LIST})
      list(APPEND SC_FLAGS "-t" "${TARG}")
    endforeach()

    if(IOS AND AX_ENABLE_MTL)
      list(APPEND SC_FLAGS "--msl-ios")
    endif()

    # defines
    get_source_file_property(SOURCE_SC_DEFINES ${SC_FILE} AXSLCC_DEFINES)
    axslcc_parse_defines("${SOURCE_SC_DEFINES}" PARSED_DEFINES)
    list(APPEND SC_FLAGS ${PARSED_DEFINES})

    # includes
    get_source_file_property(INC_DIRS ${SC_FILE} AXSLCC_INCLUDE_DIRS)
    if(INC_DIRS STREQUAL "NOTFOUND")
      set(INC_DIRS "")
    endif()
    list(APPEND INC_DIRS "${_AX_ROOT}/axmol/renderer/shaders")
    foreach(incdir ${INC_DIRS})
      list(APPEND SC_FLAGS "-I${incdir}")
    endforeach()

    if(opt_CVAR)
      list(APPEND SC_FLAGS "--cvar" "shader_rt_${FILE_NAME}")
    endif()

    # output
    set(OUT_DIR ${AXSLCC_OUT_DIR})
    if(NOT opt_BUILTIN)
      set(OUT_DIR "${OUT_DIR}/custom")
    endif()
    if(NOT(IS_DIRECTORY ${OUT_DIR}))
      file(MAKE_DIRECTORY ${OUT_DIR})
    endif()

    set(SC_OUTPUT "${OUT_DIR}/${FILE_NAME}")
    file(TO_CMAKE_PATH "${SC_OUTPUT}" SC_OUTPUT)
    set(SC_COMMENT "[${AX_RENDER_API}] Compiling shader ${SC_FILE} to ${SC_OUTPUT} ...")

    get_source_file_property(SOURCE_SC_VARIANTS ${SC_FILE} AXSLCC_VARIANT_DEFINES)

    if(SOURCE_SC_VARIANTS STREQUAL "NOTFOUND")
      set_source_files_properties(${SC_FILE} DIRECTORY ${CMAKE_BINARY_DIR} PROPERTIES AXSLCC_OUTPUT ${SC_OUTPUT})
      add_custom_command(
        MAIN_DEPENDENCY ${SC_FILE}
        OUTPUT ${SC_OUTPUT}
        COMMAND ${AXSLCC_EXE} ${SC_FLAGS} "-o" "${SC_OUTPUT}" "${SC_FILE}"
        COMMENT "${SC_COMMENT}"
        VERBATIM
      )
      list(APPEND compiled_shaders ${SC_OUTPUT})
    else()
      set(SC_OUTPUTS "${SC_OUTPUT}")
      set(SC_CMD_LINES COMMAND ${AXSLCC_EXE} ${SC_FLAGS} "-o" "${SC_OUTPUT}" "${SC_FILE}")
      set(vidx 0)
      foreach(variant_defs ${SOURCE_SC_VARIANTS})
        math(EXPR vidx "${vidx} + 1")
        set(SC_VOUT "${SC_OUTPUT}_${vidx}")
        list(APPEND SC_OUTPUTS "${SC_VOUT}")
        axslcc_parse_defines("${variant_defs}" VARIANT_DEFINES)
        list(APPEND SC_CMD_LINES COMMAND ${AXSLCC_EXE} ${SC_FLAGS} ${VARIANT_DEFINES} "-o" "${SC_VOUT}" "${SC_FILE}")
      endforeach()
      add_custom_command(
        MAIN_DEPENDENCY ${SC_FILE}
        OUTPUT ${SC_OUTPUTS}
        ${SC_CMD_LINES}
        COMMENT "${SC_COMMENT}"
        VERBATIM
      )
      list(APPEND compiled_shaders ${SC_OUTPUTS})
    endif()
  endforeach()

  target_sources(${target_name} PRIVATE ${opt_FILES})

  get_target_property(target_compiled_shaders ${target_name} AX_COMPILED_SHADERS)
  if(NOT target_compiled_shaders)
    set(target_compiled_shaders "")
  endif()
  list(APPEND target_compiled_shaders ${compiled_shaders})
  set_property(TARGET ${target_name} PROPERTY AX_COMPILED_SHADERS ${target_compiled_shaders})

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
