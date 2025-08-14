if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/axmol")
  set(_AX_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/axmol")
  set(_is_axmol_embed TRUE)
  message(STATUS "Building isolated project: ${APP_NAME}")
else()
  set(_AX_ROOT "$ENV{AX_ROOT}")

  if(NOT(_AX_ROOT STREQUAL ""))
    file(TO_CMAKE_PATH ${_AX_ROOT} _AX_ROOT)
    message(STATUS "Using system env var _AX_ROOT=${_AX_ROOT}")
  else()
    message(FATAL_ERROR "Please run setup.ps1 add system env var 'AX_ROOT' to specific the engine root")
  endif()
endif()

list(APPEND CMAKE_MODULE_PATH
  ${_AX_ROOT}/cmake/Modules/
)

include(AXBuildSet)

if(NOT _is_axmol_embed)
  if((WIN32 OR LINUX) AND DEFINED AX_PREBUILT_DIR AND IS_DIRECTORY ${_AX_ROOT}/${AX_PREBUILT_DIR})
    set(_AX_USE_PREBUILT TRUE)
  endif()
endif()

if(NOT _AX_USE_PREBUILT)
  add_subdirectory(${_AX_ROOT}/axmol ${ENGINE_BINARY_PATH}/axmol)
endif()
