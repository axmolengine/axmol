cmake_policy(PUSH)
cmake_policy(SET CMP0057 NEW)

##############################################################
##  enable shader lang by shader compiler: glslcc
macro(glslcc_option variable value)
    if(NOT DEFINED ${variable})
        set(${variable} ${value})
    endif()
endmacro()

glslcc_option(GLSLCC_FRAG_SOURCE_FILE_EXTENSIONS .frag;.fsh)
glslcc_option(GLSLCC_VERT_SOURCE_FILE_EXTENSIONS .vert;.vsh)
glslcc_option(GLSLCC_OUT_DIR ${CMAKE_BINARY_DIR}/runtime/axslc)
glslcc_option(GLSLCC_OUT_SUFFIX "")
glslcc_option(GLSLCC_FLAT_UBOS TRUE)
glslcc_option(GLSLCC_FIND_PROG_ROOT "")

find_program(GLSLCC_EXE NAMES glslcc
    PATHS ${GLSLCC_FIND_PROG_ROOT}
)

if (NOT GLSLCC_EXE)
    message("glslcc not found.")
    message(FATAL_ERROR "Please run setup.ps1 again to download glslcc, and run CMake again.")
endif()

message(STATUS "GLSLCC_FRAG_SOURCE_FILE_EXTENSIONS=${GLSLCC_FRAG_SOURCE_FILE_EXTENSIONS}")
message(STATUS "GLSLCC_VERT_SOURCE_FILE_EXTENSIONS=${GLSLCC_VERT_SOURCE_FILE_EXTENSIONS}")
message(STATUS "GLSLCC_OUT_SUFFIX=${GLSLCC_OUT_SUFFIX}")
message(STATUS "GLSLCC_FLAT_UBOS=${GLSLCC_FLAT_UBOS}")

# PROPERTY: include direcotries (optional)
define_property(SOURCE PROPERTY GLSLCC_INCLUDE_DIRS 
                BRIEF_DOCS "Compiled shader include directories"
                FULL_DOCS "Compiled shader include directories, seperated with comma")

# PROPERTY: defines (optional)
define_property(SOURCE PROPERTY GLSLCC_DEFINES 
                BRIEF_DOCS "Compiled shader defines"
                FULL_DOCS "Compiled shader defines, seperated with comma")

# Find shader sources in specified directory
# syntax: ax_find_shaders(dir shader_sources [RECURSE])
# examples:
#   - ax_find_shaders("${CMAKE_CURRENT_LIST_DIR}/core/renderer/shaders" runtime_shader_sources)
#   - ax_find_shaders("${CMAKE_CURRENT_LIST_DIR}/Source" custom_shader_sources RECURSE)
function (ax_find_shaders dir varName)
    set(options RECURSE)
    cmake_parse_arguments(opt "${options}" "" "" ${ARGN})

    set(SC_FILTERS "")
    foreach(fileext ${GLSLCC_FRAG_SOURCE_FILE_EXTENSIONS})
        list(APPEND SC_FILTERS "${dir}/*${fileext}")
    endforeach()
    foreach(fileext ${GLSLCC_VERT_SOURCE_FILE_EXTENSIONS})
        list(APPEND SC_FILTERS "${dir}/*${fileext}")
    endforeach()
    if (opt_RECURSE)
        file(GLOB_RECURSE out_files ${SC_FILTERS})
    else()
        file(GLOB out_files ${SC_FILTERS})
    endif()
    set(${varName} ${out_files} PARENT_SCOPE)
endfunction()

# This function allow make shader files (.frag, .vert) compiled with glslcc
# usage:
#   - ax_target_compile_shaders(axmol FILES source_files): output compiled shader to ${CMAKE_BINARY_DIR}/runtime/shaders/${SC_LANG}/xxx_fs
#   - ax_target_compile_shaders(axmol FILES source_files CUSTOM): output compiled shader to ${CMAKE_BINARY_DIR}/runtime/shaders/${SC_LANG}/xxx_fs
#   - ax_target_compile_shaders(axmol FILES source_files CVAR): the shader will compiled to c hex header for embed include by C/C++ use
# Use global variable to control shader file extension:
#   - GLSLCC_FRAG_SOURCE_FILE_EXTENSIONS: default is .frag;.fsh
#   - GLSLCC_VERT_SOURCE_FILE_EXTENSIONS: default is .vert;.vsh
# 
function (ax_target_compile_shaders target_name)
    set(options RUNTIME CVAR)
    set(multiValueArgs FILES)
    cmake_parse_arguments(opt "${options}" "" "${multiValueArgs}" ${ARGN})

    foreach(SC_FILE ${opt_FILES})
        get_filename_component(FILE_EXT ${SC_FILE} LAST_EXT)
        get_filename_component(FILE_NAME ${SC_FILE} NAME_WE)
        string(TOLOWER "${FILE_EXT}" FILE_EXT)

        set(SC_DEFINES "")

        # silent when compile shader success
        set(SC_FLAGS "--silent" "--err-format=msvc")
        
        # shader lang
        set(SC_PROFILE "")
        if(ANDROID OR WINRT OR AX_USE_ANGLE)
            # version 300 es
            set(OUT_LANG "ESSL")
            set(SC_PROFILE "300")
            list(APPEND SC_FLAGS  "--lang=gles" "--profile=${SC_PROFILE}")
        elseif (WIN32 OR LINUX)
            # version 330
            set(OUT_LANG "GLSL")
            set(SC_PROFILE "330")
            list(APPEND SC_FLAGS  "--lang=glsl" "--profile=${SC_PROFILE}")
        elseif (APPLE) 
            set(OUT_LANG "MSL")
            list(APPEND SC_DEFINES "METAL")
        endif()

        # defines
        get_source_file_property(SOURCE_SC_DEFINES ${SC_FILE} GLSLCC_DEFINES)
        if (NOT (SOURCE_SC_DEFINES STREQUAL "NOTFOUND"))
            list(APPEND SC_DEFINES ${SOURCE_SC_DEFINES})
        endif()
        if (SC_DEFINES)
            list(APPEND SC_FLAGS "--defines=${SC_DEFINES}")
        endif()

        # includes
        get_source_file_property(INC_DIRS ${SC_FILE} GLSLCC_INCLUDE_DIRS)
        if (INC_DIRS STREQUAL "NOTFOUND")
            set(INC_DIRS "")
        endif()
        list(APPEND INC_DIRS "${_AX_ROOT}/core/renderer/shaders")
        list(APPEND SC_FLAGS "--include-dirs=${INC_DIRS}")

        # flat-ubos
        if(${GLSLCC_FLAT_UBOS})
            list(APPEND SC_FLAGS "--flatten-ubos")
        endif()

        # input
        if (${FILE_EXT} IN_LIST GLSLCC_FRAG_SOURCE_FILE_EXTENSIONS)
            list(APPEND SC_FLAGS "--frag=${SC_FILE}")
            set(TYPE "fs")
        elseif(${FILE_EXT} IN_LIST GLSLCC_VERT_SOURCE_FILE_EXTENSIONS)
            set(TYPE "vs")
            list(APPEND SC_FLAGS "--vert=${SC_FILE}")
        else()
            message(FATAL_ERROR "Invalid shader source, the file extesion must be one of .frag;.vert")
        endif()

        # output
        set(OUT_DIR ${GLSLCC_OUT_DIR})
        if (NOT (IS_DIRECTORY ${OUT_DIR}))
            file(MAKE_DIRECTORY ${OUT_DIR})
        endif()
        if (NOT opt_CVAR)
            list(APPEND SC_FLAGS "--output=${OUT_DIR}/${FILE_NAME}${GLSLCC_OUT_SUFFIX}" )

            # glscc will auto insert ${FILE_NAME}_vs.bin or ${FILE_NAME}_fs.bin
            # so we set OUTPUT to match with it, otherwise will cause cause incremental build to work incorrectly.
            set(SC_OUTPUT "${OUT_DIR}/${FILE_NAME}_${TYPE}${GLSLCC_OUT_SUFFIX}")
        else()
            set(SC_OUTPUT "${OUT_DIR}/${FILE_NAME}_${TYPE}${GLSLCC_OUT_SUFFIX}.h")
            list(APPEND SC_FLAGS "${OUT_FILE}" "--cvar=shader_rt_${FILE_NAME}" "--output=${SC_OUTPUT}")
        endif()

        set(SC_COMMENT "Compiling shader ${SC_FILE} for ${OUT_LANG}${SC_PROFILE} to ${SC_OUTPUT} ...")
        string(REPLACE ";" " " FULL_COMMAND_LINE "${GLSLCC_EXE};${SC_FLAGS} ...")
        add_custom_command(
                MAIN_DEPENDENCY ${SC_FILE} OUTPUT ${SC_OUTPUT} COMMAND ${GLSLCC_EXE} ${SC_FLAGS}
                COMMENT "${FULL_COMMAND_LINE}"
            )
    endforeach()
    target_sources(${target_name} PRIVATE ${opt_FILES})
endfunction()
cmake_policy(POP)
