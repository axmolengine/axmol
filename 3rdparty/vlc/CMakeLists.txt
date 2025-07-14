
set(lib_name vlc)

project(${lib_name})

if(WIN32)
  set(vlc_suffix win)
elseif(LINUX)
  set(vlc_suffix linux)
else()
  message(FATAL_ERROR "axmol not built vlc for current platform: ${CMAKE_SYSTEM_NAME}")
endif()

_1kfetch("https://github.com/halx99/build-vlc/releases/download/v3.0.18/libvlc-dev-${vlc_suffix}.zip" NAME vlc)

# -----macro: set vlc sub target-----
macro(set_vlc_sub_target sub_target_name sub_lib_name)
  add_library(${sub_target_name} SHARED IMPORTED GLOBAL)

  set_target_properties(${sub_target_name} PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${vlc_SOURCE_DIR}/include"
  )

  if(LINUX)
    set(imp_lib_name "${vlc_SOURCE_DIR}/lib/lib${sub_lib_name}.so")
    set_target_properties(${sub_target_name} PROPERTIES
      IMPORTED_LOCATION "${vlc_SOURCE_DIR}/lib/lib${sub_lib_name}.so"
    )
  elseif(WINDOWS)
    set_target_properties(${sub_target_name} PROPERTIES
      IMPORTED_LOCATION "${vlc_SOURCE_DIR}/bin/lib${sub_lib_name}.dll"
      IMPORTED_IMPLIB "${vlc_SOURCE_DIR}/lib/lib${sub_lib_name}.dll.a"
    )
  endif()
endmacro()

# -----sub target 1: libvlc-----
set(target_name VLC::vlc vlc)
set_vlc_sub_target(${target_name})

# -----sub target 2: libvlccore-----
set(target_name VLC::vlccore vlccore)
set_vlc_sub_target(${target_name})