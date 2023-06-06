#/****************************************************************************
# Copyright (c) 2013 cocos2d-x.org
# Copyright (c) 2014 martell malone
# Copyright (c) 2015-2017 Chukong Technologies Inc.
# Copyright (c) 2021-2022 Bytedance Inc.
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

cmake_minimum_required(VERSION 3.10)

project(axmol)

if(XCODE)
    set(CMAKE_XCODE_GENERATE_TOP_LEVEL_PROJECT_ONLY TRUE)
endif()

# ax root path
set(_AX_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_MODULE_PATH ${_AX_ROOT_PATH}/cmake/Modules/)

# prevent in-source-build
include(PreventInSourceBuilds)

# works before build libaxmol
include(AXBuildSet)

option(AX_BUILD_TESTS "Build cpp & lua tests" ON)

add_subdirectory(${_AX_ROOT_PATH}/core ${ENGINE_BINARY_PATH}/axmol/core)

# prevent tests project to build "axmol/core" again
set(BUILD_ENGINE_DONE ON)

if(AX_BUILD_TESTS)

    message(STATUS "CMAKE_BINARY_DIR=${CMAKE_BINARY_DIR}")

    macro(add_test_target target_name dir)
        add_subdirectory(${dir} ${CMAKE_BINARY_DIR}/tests/${target_name})
        set_target_properties(${target_name} PROPERTIES FOLDER "Apps")
    endmacro()
    
    # add cpp-template-default into project(axmol) for tmp test
    add_test_target(HelloCpp ${_AX_ROOT_PATH}/templates/cpp-template-default )

	  # add cpp tests default
    add_test_target(cpp_tests ${_AX_ROOT_PATH}/tests/cpp-tests)
    
	  # add fairygui tests when fairygui extension is enabled
    if(AX_ENABLE_EXT_FAIRYGUI)
        add_test_target(fairygui_tests ${_AX_ROOT_PATH}/tests/fairygui-tests)
    endif()
    
    if(AX_ENABLE_EXT_LIVE2D)
        add_test_target(live2d_tests ${_AX_ROOT_PATH}/tests/live2d-tests)
    endif()

    if(AX_ENABLE_EXT_LUA)
        add_test_target(HelloLua ${_AX_ROOT_PATH}/templates/lua-template-default)
        add_test_target(lua_tests ${_AX_ROOT_PATH}/tests/lua-tests)
    endif(AX_ENABLE_EXT_LUA)

endif()

ax_uwp_set_all_targets_deploy_min_version()
