cmake_minimum_required(VERSION 3.10)

set(LIB_NAME Live2D)

# Force static library.
add_library(${LIB_NAME} STATIC)

add_subdirectory(Framework/src)

# Add include path.
target_include_directories(${LIB_NAME}
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Framework/src
)

# Deprecated functions
# The following expressions are written for compatibility
# and will be removed in a future release.

# Add core include.
target_include_directories(${LIB_NAME}
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/include
)
# Add definitions.
# target_compile_definitions(${LIB_NAME}
#   PRIVATE
#     ${FRAMEWORK_DEFINITIOINS}
# )

if(NOT IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/Core)
  message(AUTHOR_WARNING "Downloading liblive2d-core-dev ...")
  file(DOWNLOAD https://github.com/axmol1/archive/releases/download/v1.0.0/Live2D-4.4-Core.zip ${CMAKE_CURRENT_LIST_DIR}/Core.zip)
  file(ARCHIVE_EXTRACT INPUT ${CMAKE_CURRENT_LIST_DIR}/Core.zip DESTINATION ${CMAKE_CURRENT_LIST_DIR}/)
endif()

add_subdirectory(Core)

if (ANDROID)
    target_link_libraries(${LIB_NAME} INTERFACE Live2DCubismCore)
else()
    target_link_libraries(${LIB_NAME} Live2DCubismCore)
endif()

setup_ax_extension_config(${LIB_NAME})

unset(LIB_NAME)
