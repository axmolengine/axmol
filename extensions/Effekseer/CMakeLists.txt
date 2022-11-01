
# The project's name
project(Effekseer)

# Check cmake version
cmake_minimum_required(VERSION 3.1)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(FilterFolder)

# C++ version

set(USE_INTERNAL_LOADER OFF)

option(USE_OPENGLES2 "Use OpenGL ES2" OFF)
option(USE_OPENGLES3 "Use OpenGL ES3" OFF)
option(USE_OPENGL3 "Use OpenGL 3" OFF)

if(_CMAKE_OSX_SYSROOT_PATH MATCHES "/iPhone(OS|Simulator)" OR ANDROID)
    set(USE_OPENGLES2 ON)
endif()


if(APPLE)
    set(BUILD_METAL ON)
endif()

if (USE_INTERNAL_LOADER)
	add_definitions(-D__EFFEKSEER_RENDERER_INTERNAL_LOADER__)
endif()

if (USE_OPENGLES2)
	add_definitions(-D__EFFEKSEER_RENDERER_GLES2__)
endif()

if (USE_OPENGLES3)
	add_definitions(-D__EFFEKSEER_RENDERER_GLES3__)
endif()

if (USE_OPENGL3)
	add_definitions(-D__EFFEKSEER_RENDERER_GL3__)
endif()

# Substitute
function(clang_format target)
endfunction()

# a function to generate filters by folders
function(FilterFolder srcs)
    foreach(FILE ${srcs}) 
        # Get the directory of the source file
        get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)
    
        # Remove common directory prefix to make the group
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")
    
        # Make sure we are using windows slashes
        string(REPLACE "/" "\\" GROUP "${GROUP}")
    
        # Group into "Source Files" and "Header Files"
        if ("${FILE}" MATCHES ".*\\.cpp")
           set(GROUP "${GROUP}")
        elseif("${FILE}" MATCHES ".*\\.c")
           set(GROUP "${GROUP}")
        elseif("${FILE}" MATCHES ".*\\.cxx")
           set(GROUP "${GROUP}")
        elseif("${FILE}" MATCHES ".*\\.h")
           set(GROUP "${GROUP}")
        elseif("${FILE}" MATCHES ".*\\.mm")
           set(GROUP "${GROUP}")
        endif()
    
        source_group("${GROUP}" FILES "${FILE}")
    endforeach()
endfunction(FilterFolder)

function(WarnError target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /we4267 /we4456 /we4458 /we4715)
    endif()
endfunction()

add_subdirectory(Effekseer)

if (APPLE)
    option(USE_INTERNAL_COMMAND_BUFFER "Use Internal Command Buffer" OFF)
    option(USE_INTERNAL_RENDER_PASS "Use Internal Render Pass" OFF)
    set(USE_INTERNAL_COMMAND_BUFFER OFF)
    set(USE_INTERNAL_RENDER_PASS OFF)

    add_subdirectory(EffekseerRendererMetal)

    set(BUILD_TEST_TEMP ${BUILD_TEST})
    set(BUILD_TEST OFF)
    add_subdirectory(3rdParty/LLGI)
    set(BUILD_TEST ${BUILD_TEST_TEMP})

    set_target_properties (EffekseerRendererMetal PROPERTIES FOLDER Extensions/Effekseer)
    set_target_properties (LLGI PROPERTIES FOLDER Extensions/Effekseer)
endif()

if (NOT APPLE)
    add_subdirectory(EffekseerRendererGL)
    set_target_properties (EffekseerRendererGL PROPERTIES FOLDER Extensions/Effekseer)
endif()

add_subdirectory(EffekseerForCocos2d-x)

set_target_properties (EffekseerForCocos2d-x PROPERTIES FOLDER Extensions/Effekseer)
set_target_properties (Effekseer PROPERTIES FOLDER Extensions/Effekseer)
