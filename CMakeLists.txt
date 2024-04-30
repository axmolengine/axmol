#/****************************************************************************
# Copyright (c) 2013 cocos2d-x.org
# Copyright (c) 2014 martell malone
# Copyright (c) 2015-2017 Chukong Technologies Inc.
# Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).
#
# https://axmolengine.github.io/
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
# ****************************************************************************/

# build engine library and all tests

cmake_minimum_required(VERSION 3.22)

project(axmol)

if(XCODE)
    set(CMAKE_XCODE_GENERATE_TOP_LEVEL_PROJECT_ONLY TRUE)
endif()

# ax root path
set(_AX_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_MODULE_PATH ${_AX_ROOT}/cmake/Modules/)

# prevent in-source-build
include(PreventInSourceBuilds)

# works before build libaxmol
include(AXBuildSet)

option(AX_BUILD_TESTS "Build cpp & lua tests" ON)
# option(AX_BUILD_TOOLS "Build tools" ON)

add_subdirectory(${_AX_ROOT}/core ${ENGINE_BINARY_PATH}/axmol/core)

# prevent tests project to build "axmol/core" again
set(BUILD_ENGINE_DONE ON)

if(AX_BUILD_TESTS)

    message(STATUS "CMAKE_BINARY_DIR=${CMAKE_BINARY_DIR}")

    macro(add_test_target target_name dir)
        add_subdirectory(${dir} ${CMAKE_BINARY_DIR}/tests/${target_name})
        set_target_properties(${target_name} PROPERTIES FOLDER "Apps")
    endmacro()

	# add cpp tests default
    add_test_target(cpp-tests ${_AX_ROOT}/tests/cpp-tests)

    if(LINUX OR MACOSX OR WINDOWS)
        add_test_target(unit-tests ${_AX_ROOT}/tests/unit-tests)
    endif()

	# add fairygui tests when fairygui extension is enabled
    if(AX_ENABLE_EXT_FAIRYGUI)
        add_test_target(fairygui-tests ${_AX_ROOT}/tests/fairygui-tests)
    endif()

    if(AX_ENABLE_EXT_LIVE2D)
        add_test_target(live2d-tests ${_AX_ROOT}/tests/live2d-tests)
    endif()

    if(AX_ENABLE_EXT_LUA)
        add_test_target(lua-tests ${_AX_ROOT}/tests/lua-tests)
    endif(AX_ENABLE_EXT_LUA)

endif()
