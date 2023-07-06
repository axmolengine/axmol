if (__AXGLSLCC_CMAKE__)
    return()
endif()
set(__AXGLSLCC_CMAKE__ TRUE)

cmake_policy(PUSH)
cmake_policy(SET CMP0057 NEW)

##############################################################
##  enable shader lang by shader compiler: glslcc
find_program(GLSLCC_EXE NAMES glslcc
    PATHS ${_AX_ROOT}/tools/external/glslcc
)

if (NOT GLSLCC_EXE)
    message("glslcc not found.")
    message(FATAL_ERROR "Please run setup.ps1 again to download glslcc, and run CMake again.")
endif()

set(GLSLCC_FRAG_SOURCE_FILE_EXTENSIONS .frag;.fsh)
set(GLSLCC_VERT_SOURCE_FILE_EXTENSIONS .vert;.vsh)

# This function allow make shader files (.frag, .vert) compiled with glslcc
# usage:
#   - ax_target_compile_shaders(axmol FILES filepathList): output compiled shader to ${CMAKE_BINARY_DIR}/runtime/shaders/${SC_LANG}/xxx_fs.bin
#   - ax_target_compile_shaders(axmol FILES filepathList CUSTOM): output compiled shader to ${CMAKE_BINARY_DIR}/runtime/shaders/${SC_LANG}/xxx_fs.bin
#   - ax_target_compile_shaders(axmol FILES filepathList CVAR): the shader will compiled to c hex header for embed include by C/C++ use
# Use global variable to control shader file extension:
#   - GLSLCC_FRAG_SOURCE_FILE_EXTENSIONS: default is .frag;.fsh
#   - GLSLCC_VERT_SOURCE_FILE_EXTENSIONS: default is .vert;.vsh
# 
# PROPERTY: output directory (optional)
define_property(SOURCE PROPERTY GLSLCC_OUTPUT_DIRECTORY 
                BRIEF_DOCS "Compiled shader output directory"
                FULL_DOCS "Compiled shader output directory")
# PROPERTY: include direcotries (optional)
define_property(SOURCE PROPERTY GLSLCC_INCLUDE_DIRS 
                BRIEF_DOCS "Compiled shader include directories"
                FULL_DOCS "Compiled shader include directories, seperated with comma")
function (ax_target_compile_shaders target_name)
    set(options RUNTIME CVAR)
    set(multiValueArgs FILES)
    cmake_parse_arguments(opt "${options}" "" "${multiValueArgs}" ${ARGN})

    foreach(SC_FILE ${opt_FILES})
        get_filename_component(FILE_EXT ${SC_FILE} LAST_EXT)
        get_filename_component(FILE_NAME ${SC_FILE} NAME_WE)
        string(TOLOWER "${FILE_EXT}" FILE_EXT)

        # silent when compile shader success
        set(SC_FLAGS "--silent" "--err-format=msvc")
        
        # shader lang
        if(opt_RUNTIME)
            set(SHADER_CATALOG "runtime")
        else()
            set(SHADER_CATALOG "custom")
        endif()
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
            list(APPEND SC_FLAGS  "--lang=msl" "--defines=METAL")
        endif()

        # includes
        get_source_file_property(INC_DIRS ${SC_FILE} GLSLCC_INCLUDE_DIRS)
        if (INC_DIRS STREQUAL "NOTFOUND")
            set(INC_DIRS "")
        endif()
        list(APPEND INC_DIRS "${_AX_ROOT}/core/renderer/shaders")
        list(APPEND SC_FLAGS "--include-dirs=${INC_DIRS}")

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
        get_source_file_property(OUT_DIR ${SC_FILE} GLSLCC_OUTPUT_DIRECTORY)
        if (OUT_DIR STREQUAL "NOTFOUND")
            set(OUT_DIR "${CMAKE_BINARY_DIR}/shaders/${SHADER_CATALOG}")
        endif()
        file(MAKE_DIRECTORY ${OUT_DIR})
        if (NOT opt_CVAR)
            list(APPEND SC_FLAGS "--output=${OUT_DIR}/${FILE_NAME}.bin" )

            # glscc will auto insert ${FILE_NAME}_vs.bin or ${FILE_NAME}_fs.bin
            # so we set OUTPUT to match with it, otherwise will cause cause incremental build to work incorrectly.
            set(SC_OUTPUT "${OUT_DIR}/${FILE_NAME}_${TYPE}.bin")
        else()
            set(SC_OUTPUT "${OUT_DIR}/${FILE_NAME}_${TYPE}.bin.h")
            list(APPEND SC_FLAGS "${OUT_FILE}" "--cvar=shader_rt_${FILE_NAME}" "--output=${SC_OUTPUT}")
        endif()

        set(SC_COMMENT "Compiling ${SHADER_CATALOG} shader ${SC_FILE} for ${OUT_LANG}${SC_PROFILE} to ${SC_OUTPUT} ...")
        # string(REPLACE ";" " " FULL_COMMAND_LINE "${GLSLCC_EXE};${SC_FLAGS} ...")
        add_custom_command(
                MAIN_DEPENDENCY ${SC_FILE} OUTPUT ${SC_OUTPUT} COMMAND ${GLSLCC_EXE} ${SC_FLAGS}
                COMMENT "${SC_COMMENT}"
            )
    endforeach()
    target_sources(${target_name} PRIVATE ${opt_FILES})
endfunction()
cmake_policy(POP)
