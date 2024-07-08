# print cmake debug info
set(CMAKE_DEBUG_TARGET_PROPERTIES
    # INCLUDE_DIRECTORIES
    # COMPILE_DEFINITIONS
    # COMPILE_OPTIONS
    # AUTOUIC_OPTIONS
    # POSITION_INDEPENDENT_CODE
)

# The ``OLD`` behavior for this policy is to set ``OpenGL_GL_PREFERENCE`` to
# ``LEGACY``.  The ``NEW`` behavior for this policy is to set
# ``OpenGL_GL_PREFERENCE`` to ``GLVND``.
# need review those libs: X11 Xi Xrandr Xxf86vm Xinerama Xcursor rt m
cmake_policy(SET CMP0072 NEW)

# some useful variables for every one ax project
set(ENGINE_BINARY_PATH ${PROJECT_BINARY_DIR}/engine)

if(CMAKE_TOOLCHAIN_FILE)
    message(STATUS "Using toolchain file:" ${CMAKE_TOOLCHAIN_FILE})
endif()

# hold the extensions list to auto link to app
set(_AX_EXTENSION_LIBS "" CACHE INTERNAL "extensions for auto link to target application")

# configure android GLSLCC compile output, this is the first include cmake module
if (ANDROID)
    file(TO_NATIVE_PATH "${_AX_ANDROID_PROJECT_DIR}/build/runtime/axslc" _GLSLCC_OUT_DIR)
    set(GLSLCC_OUT_DIR "${_GLSLCC_OUT_DIR}" CACHE STRING "" FORCE)
    message(AUTHOR_WARNING "Set GLSLCC_OUT_DIR to ${GLSLCC_OUT_DIR} for android")
endif()

# import minimal AXGLSLCC.cmake for shader compiler support
# the function: ax_target_compile_shaders avaiable from it
set(GLSLCC_FIND_PROG_ROOT "${_AX_ROOT}/tools/external/glslcc" "$ENV{AX_ROOT}/tools/external/glslcc")
include(AXGLSLCC)

# include helper functions
include(AXBuildHelpers)

# set common compiler options
# add target compile define function
# add target compile options function
include(AXConfigDefine)

# config libraries dependence
include(AXConfigDepend)

message(AUTHOR_WARNING "CMAKE_VERSION:" ${CMAKE_VERSION})
message(STATUS "CMAKE_HOST_SYSTEM_NAME:" ${CMAKE_HOST_SYSTEM_NAME})
message(STATUS "CMAKE_SYSTEM_NAME:" ${CMAKE_SYSTEM_NAME})
message(STATUS "CMAKE_GENERATOR_PLATFORM:" ${CMAKE_GENERATOR_PLATFORM})
message(STATUS "CMAKE_SYSTEM_PROCESSOR:" ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "CMAKE_CXX_COMPILER_ID:" ${CMAKE_CXX_COMPILER_ID})
message(STATUS "FUZZ_MSVC=${FUZZ_MSVC}, FULL_CLANG=${FULL_CLANG}")

message(STATUS "PROJECT_NAME:" ${PROJECT_NAME})
message(STATUS "PROJECT_SOURCE_DIR:" ${PROJECT_SOURCE_DIR})
message(STATUS "_AX_ROOT:" ${_AX_ROOT})
message(STATUS "CMAKE_MODULE_PATH:" ${CMAKE_MODULE_PATH})
# delete binary dir if you hope a full clean re-build
message(STATUS "PROJECT_BINARY_DIR:" ${PROJECT_BINARY_DIR})
message(STATUS "ENGINE_BINARY_PATH:" ${ENGINE_BINARY_PATH})
message(STATUS "ARCH_ALIAS:" ${ARCH_ALIAS})

_1k_deprecated_32bit(axmol 2.2.0)
