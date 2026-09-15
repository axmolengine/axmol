# /****************************************************************************
# Copyright (c) 2013-2014 cocos2d-x.org
# Copyright (c) 2019-present Simdsoft Limited.
#
# https://axmol.dev/
#
# SPDX-License-Identifier: MIT
# ****************************************************************************/

cmake_minimum_required(VERSION 3.22...4.1)

set(APP_NAME Dummy)

project(${APP_NAME})

if(XCODE)
  set(CMAKE_XCODE_GENERATE_TOP_LEVEL_PROJECT_ONLY TRUE)
endif()

set(_is_axmol_embed FALSE)
set(_AX_USE_PREBUILT FALSE)

set(CMAKE_MODULE_PATH
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules
)

# NOTE: The order of the cmake module "include(AXGame...)" statements matters
include(AXGameEngineOptions)
include(AXGameEngineSetup)

# The common cross-platforms source files and header files
file(GLOB_RECURSE GAME_HEADER
  Source/*.h Source/*.hpp
)

file(GLOB_RECURSE GAME_SOURCE
  Source/*.cpp Source/*.c
)

set(GAME_INC_DIRS
  "${CMAKE_CURRENT_SOURCE_DIR}/Source"
)

set(content_folder
  "${CMAKE_CURRENT_SOURCE_DIR}/Content"
)

if(APPLE)
  ax_mark_multi_resources(common_content_files RES_TO "Resources" FOLDERS ${content_folder})
elseif(WINDOWS)
  ax_mark_multi_resources(common_content_files RES_TO "Content" FOLDERS ${content_folder})
endif()

include(AXGameSourceSetup)

# mark app complie info and libs info
set(APP_SOURCES
  ${GAME_HEADER}
  ${GAME_SOURCE}
)

include(AXGameTargetSetup)

# mark app resources, resource will be copy auto after mark
ax_setup_app_config(${APP_NAME})

# Add any libraries you need to link to the project after this point

# Default Platform-specific setup
include(AXGamePlatformSetup)

# Make sure the AXGameFinalSetup is included at the end of this file
include(AXGameFinalSetup)
