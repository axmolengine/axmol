
set(lib_name vlc)

project(${lib_name})

include(AXPlatform)

if (NOT (IS_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/${PLATFORM_NAME}"))
  if (NOT EXISTS libvlc-dev-${PLATFORM_NAME}.zip)
    message(STATUS "Downloading libvlc-dev-${PLATFORM_NAME}.zip ...")
    file(DOWNLOAD https://github.com/halx99/build-vlc/releases/download/v3.0.18/libvlc-dev-${PLATFORM_NAME}.zip ${CMAKE_CURRENT_LIST_DIR}/libvlc-dev-${PLATFORM_NAME}.zip)
  endif()
  file(ARCHIVE_EXTRACT INPUT ${CMAKE_CURRENT_LIST_DIR}/libvlc-dev-${PLATFORM_NAME}.zip DESTINATION ${CMAKE_CURRENT_LIST_DIR}/)
  file(RENAME ${CMAKE_CURRENT_LIST_DIR}/libvlc-dev-${PLATFORM_NAME} ${CMAKE_CURRENT_LIST_DIR}/${PLATFORM_NAME})
endif()

# -----macro: set vlc sub target-----
macro(set_vlc_sub_target sub_target_name sub_lib_name)
  add_library(${sub_target_name} SHARED IMPORTED GLOBAL)

  set_target_properties(${sub_target_name} PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_NAME}/include"
  )

  if (LINUX)
    set(imp_lib_name "${CMAKE_CURRENT_LIST_DIR}/${PLATFORM_NAME}/lib/lib${sub_lib_name}.so")
    set_target_properties(${sub_target_name} PROPERTIES
      IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${PLATFORM_NAME}/lib/lib${sub_lib_name}.so"
    )
  elseif(WINDOWS)
    set_target_properties(${sub_target_name} PROPERTIES
      IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/${PLATFORM_NAME}/bin/lib${sub_lib_name}.dll"
      IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/${PLATFORM_NAME}/lib/lib${sub_lib_name}.dll.a"
    )
  endif()
endmacro()

# -----sub target 1: libvlc-----
set(target_name VLC::vlc vlc)
set_vlc_sub_target(${target_name})

# -----sub target 2: libvlccore-----
set(target_name VLC::vlccore vlccore)
set_vlc_sub_target(${target_name})
