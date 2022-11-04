cmake_minimum_required(VERSION 3.0.0)
project(LLGI)

file(GLOB files *.h *.cpp)
file(GLOB files_pc PC/*.h PC/*.cpp)

list(APPEND files ${files_pc})

if(WIN32)
  file(GLOB files_win Win/*.h Win/*.cpp)
  list(APPEND files ${files_win})
elseif(APPLE)
  file(GLOB files_mac Mac/*.h Mac/*.cpp Mac/*.mm)
  list(APPEND files ${files_mac})
else()
  file(GLOB files_linux Linux/*.h Linux/*.cpp)
  list(APPEND files ${files_linux})
endif()

if(MSVC)
  file(GLOB files_dx12 DX12/*.h DX12/*.cpp)
  list(APPEND files ${files_dx12})
endif()

if(BUILD_VULKAN)
  file(GLOB files_vulkan Vulkan/*.h Vulkan/*.cpp)
  list(APPEND files ${files_vulkan})
  add_definitions(-DENABLE_VULKAN)
endif()

if(APPLE)
  file(GLOB files_metal Metal/*.h Metal/*.cpp Metal/*.mm)
  list(APPEND files ${files_metal})
endif()

foreach(FILE ${files})
  # Get the directory of the source file
  get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)

  # Remove common directory prefix to make the group
  string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")

  # Make sure we are using windows slashes
  string(REPLACE "/" "\\" GROUP "${GROUP}")

  if("${GROUP}" STREQUAL "")
    set(GROUP "Base")
  endif()

  # Group into "Source Files" and "Header Files"
  if("${FILE}" MATCHES ".*\\.cpp")
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

add_library(LLGI STATIC ${files})

file(GLOB LOCAL_HEADERS *.h)
set_target_properties(LLGI PROPERTIES PUBLIC_HEADER "${LOCAL_HEADERS}")
target_compile_features(LLGI PUBLIC cxx_std_14)

if(BUILD_VULKAN)
  find_package(Vulkan REQUIRED)
  target_include_directories(LLGI PRIVATE ${Vulkan_INCLUDE_DIRS})
  target_link_libraries(LLGI PRIVATE ${Vulkan_LIBRARIES})
  if(BUILD_VULKAN_COMPILER)
    target_include_directories(LLGI PRIVATE ${LLGI_THIRDPARTY_INCLUDES})
    if(USE_THIRDPARTY_DIRECTORY)
      add_dependencies(LLGI EP_glslang)
    endif()
  endif()
endif()

if(WIN32)

elseif(APPLE)

else()
  set_property(TARGET LLGI PROPERTY POSITION_INDEPENDENT_CODE TRUE)
endif()

if(MSVC)
  target_compile_options(LLGI PRIVATE /W4 /WX /wd"4100")
else()
  target_compile_options(LLGI PRIVATE -Wall -Werror)
endif()

# --------------------
# Install

install(
  TARGETS LLGI
  EXPORT LLGI-export
  INCLUDES
  DESTINATION include/LLGI
  PUBLIC_HEADER DESTINATION include/LLGI
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib)

install(
  EXPORT LLGI-export
  FILE LLGI-config.cmake
  DESTINATION lib/cmake
  EXPORT_LINK_INTERFACE_LIBRARIES)

clang_format(LLGI)
