
set(lib_name vlc)

project(${lib_name})

include(AXPlatform)

# -----macro: set vlc sub target-----
macro(set_vlc_sub_target sub_target_name sub_lib_name)
  
  add_library(${sub_target_name} SHARED IMPORTED GLOBAL)

  set_target_properties(${sub_target_name} PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
  )

  set(imp_lib_name "${CMAKE_CURRENT_LIST_DIR}/lib/lib${sub_lib_name}.so")
  set_target_properties(${sub_target_name} PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/lib/lib${sub_lib_name}.so"
    )
endmacro()

# -----sub target 1: libvlc-----
set(target_name VLC::vlc vlc)
set_vlc_sub_target(${target_name})

# -----sub target 2: libvlccore-----
set(target_name VLC::vlccore vlccore)
set_vlc_sub_target(${target_name})
