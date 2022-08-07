# print cmake debug info
set(CMAKE_DEBUG_TARGET_PROPERTIES
    # INCLUDE_DIRECTORIES
    # COMPILE_DEFINITIONS
    # COMPILE_OPTIONS
    # AUTOUIC_OPTIONS
    # POSITION_INDEPENDENT_CODE
)

# some useful variables for every one cocos project
set(ENGINE_BINARY_PATH ${PROJECT_BINARY_DIR}/engine)

if(CMAKE_TOOLCHAIN_FILE)
    message(STATUS "using toolchain file:" ${CMAKE_TOOLCHAIN_FILE})
endif()

find_program(PYTHON_COMMAND NAMES python3 python2 python)
find_program(COCOS_COMMAND NAME axis
    PATHS ${AXYS_ROOT_PATH}/tools/axis-console/bin $ENV{AXIS_CONSOLE_ROOT})

message(STATUS "PROJECT_NAME:" ${PROJECT_NAME})
message(STATUS "PROJECT_SOURCE_DIR:" ${PROJECT_SOURCE_DIR})
message(STATUS "AXYS_ROOT_PATH:" ${AXYS_ROOT_PATH})
message(STATUS "CMAKE_MODULE_PATH:" ${CMAKE_MODULE_PATH})
# delete binary dir if you hope a full clean re-build
message(STATUS "PROJECT_BINARY_DIR:" ${PROJECT_BINARY_DIR})
message(STATUS "ENGINE_BINARY_PATH:" ${ENGINE_BINARY_PATH})
message(STATUS "PYTHON_PATH:"  ${PYTHON_COMMAND})
message(STATUS "COCOS_COMMAND_PATH:"  ${COCOS_COMMAND})
message(STATUS "HOST_SYSTEM:" ${CMAKE_HOST_SYSTEM_NAME})
# the default behavior of build module
option(AX_ENABLE_EXT_LUA "Build lua libraries" OFF)

# hold the extensions list to auto link to app
set(_AX_EXTENSION_LIBS "" CACHE INTERNAL "extensions for auto link to target application")

# include helper functions
include(AxisBuildHelpers)

# set common compiler options
# add target compile define function
# add target compile options function
include(AxisConfigDefine)

# config libraries dependence
include(AxisConfigDepend)

if(COCOS_COMMAND)
    get_filename_component(axis_console_dir ${COCOS_COMMAND} DIRECTORY)
    set(AXIS_LUAJIT_ROOT ${axis_console_dir}/../plugins/plugin_luacompile/bin)
    message(STATUS "AXIS_LUAJIT_ROOT:" ${AXIS_LUAJIT_ROOT})
    if("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Windows")
        find_program(LUAJIT32_COMMAND NAMES luajit-win32 PATHS ${AXIS_LUAJIT_ROOT}/32bit NO_SYSTEM_ENVIRONMENT_PATH)
        find_program(LUAJIT64_COMMAND NAMES luajit-win32 PATHS ${AXIS_LUAJIT_ROOT}/64bit NO_SYSTEM_ENVIRONMENT_PATH)
    elseif("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Linux")
        find_program(LUAJIT32_COMMAND NAMES luajit-linux PATHS ${AXIS_LUAJIT_ROOT}/32bit NO_SYSTEM_ENVIRONMENT_PATH)
        find_program(LUAJIT64_COMMAND NAMES luajit-linux PATHS ${AXIS_LUAJIT_ROOT}/64bit NO_SYSTEM_ENVIRONMENT_PATH)
    endif()
endif()


if(WINDOWS OR LINUX)
    message(STATUS "LUAJIT32_COMMAND:" ${LUAJIT32_COMMAND})
    message(STATUS "LUAJIT64_COMMAND:" ${LUAJIT64_COMMAND})
endif()
