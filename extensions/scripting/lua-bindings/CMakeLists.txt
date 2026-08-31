set(ax_root ${_AX_ROOT})

set(lua_bindings_runtime_headers
  runtime/ComponentLua.h
  runtime/LuaStack.h
  runtime/LuaEngine.h
  runtime/lua_module_register.h
  runtime/LuaBridge.h
  runtime/Lua-BindingsExport.h
  runtime/AxluaLoader.h
  runtime/axlua_runtime.h
  runtime/axlua_sol_config.h
  runtime/axlua_adapter.h
  runtime/axlua_conversions.h
  runtime/LuaValue.h
  runtime/LuaCallFunc.h
)

set(lua_bindings_runtime_files
  runtime/LuaBridge.cpp
  runtime/LuaEngine.cpp
  runtime/LuaStack.cpp
  runtime/LuaValue.cpp
  runtime/AxluaLoader.cpp
  runtime/axlua_runtime.cpp
  runtime/axlua_adapter.cpp
  runtime/axlua_conversions.cpp
  runtime/lua_module_register.cpp
  runtime/ComponentLua.cpp
  runtime/LuaCallFunc.cpp
)

set(lua_bindings_adapter_headers
  adapters/3d/axlua_3d_adapter.h
  adapters/extension/axlua_extension_adapter.h
  adapters/spine/LuaSkeletonAnimation.h
  adapters/network/axlua_network_adapter.h
  adapters/network/lua_xml_http_request.h
  adapters/network/lua_downloader.h
  adapters/navmesh/axlua_navmesh_adapter.h
  adapters/navmesh/axlua_navmesh_conversions.h
  adapters/base/axlua_base_adapter.h
  adapters/physics2d/axlua_physics2d_adapter.h
  adapters/physics3d/axlua_physics3d_adapter.h
  adapters/sceneext/lua-sceneext-conversions.h
  adapters/sceneext/CustomGUIReader.h
  adapters/ui/axlua_ui_adapter.hpp
  adapters/fairygui/axlua_fairygui_adapter.hpp
  adapters/fairygui/axlua_fairygui_conversions.h
)

set(lua_bindings_adapter_files
  adapters/base/axlua_base_adapter.cpp
  adapters/physics2d/axlua_physics2d_adapter.cpp
  adapters/3d/axlua_3d_adapter.cpp
  adapters/sceneext/CustomGUIReader.cpp
  adapters/sceneext/axlua_sceneext_adapter.cpp
  adapters/sceneext/lua-sceneext-conversions.cpp
  adapters/sceneio/axlua_sceneio_adapter.cpp
  adapters/extension/axlua_extension_adapter.cpp
  adapters/network/axlua_network_adapter.cpp
  adapters/network/lua_xml_http_request.cpp
  adapters/network/lua_downloader.cpp
  adapters/network/lua_websocket.cpp
  adapters/spine/axlua_spine_adapter.cpp
  adapters/spine/LuaSkeletonAnimation.cpp
  adapters/ui/axlua_ui_adapter.cpp
  adapters/physics3d/axlua_physics3d_adapter.cpp
  adapters/navmesh/axlua_navmesh_conversions.cpp
  adapters/navmesh/axlua_navmesh_adapter.cpp
  adapters/fairygui/axlua_fairygui_adapter.cpp
  adapters/fairygui/axlua_fairygui_conversions.cpp
)

set(lua_bindings_generated_headers
  generated/axlua_audioengine_gen.h
  generated/axlua_3d_gen.h
  generated/axlua_spine_gen.h
  generated/axlua_sceneext_gen.h
  generated/axlua_sceneio_gen.h
  generated/axlua_base_gen.h
  generated/axlua_navmesh_gen.h
  generated/axlua_physics3d_gen.h
  generated/axlua_ui_gen.h
  generated/axlua_physics2d_gen.h
  generated/axlua_extension_gen.h
  generated/axlua_rhi_gen.h
  generated/axlua_fairygui_gen.h
)

set(lua_bindings_generated_files
  generated/axlua_3d_gen.cpp
  generated/axlua_base_gen.cpp
  generated/axlua_extension_gen.cpp
  generated/axlua_physics2d_gen.cpp
  generated/axlua_spine_gen.cpp
  generated/axlua_sceneext_gen.cpp
  generated/axlua_sceneio_gen.cpp
  generated/axlua_ui_gen.cpp
  generated/axlua_audioengine_gen.cpp
  generated/axlua_physics3d_gen.cpp
  generated/axlua_navmesh_gen.cpp
  generated/axlua_rhi_gen.cpp
  generated/axlua_fairygui_gen.cpp
)

# Large generated modules are emitted as independently compiled shards.  The
# stable *_gen.cpp dispatcher remains in the explicit list above, while this
# pattern picks up the generator-owned *_gen_<n>.cpp implementation files.
file(GLOB lua_bindings_generated_chunks CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/generated/axlua_*_gen_[0-9]*.cpp")
list(APPEND lua_bindings_generated_files ${lua_bindings_generated_chunks})

if(APPLE)
  # CCLuaObjcBridge.mm needed by ios and mac
  set(lua_bindings_platform_headers
    ${lua_bindings_platform_headers}
    platform/ios/LuaObjcBridge.h
  )
  set(lua_bindings_platform_files
    ${lua_bindings_platform_files}
    platform/ios/LuaObjcBridge.mm
  )

  if(IOS)
    set(lua_bindings_adapter_headers
      ${lua_bindings_adapter_headers}
      generated/axlua_webview_gen.h
    )
    set(lua_bindings_adapter_files
      ${lua_bindings_adapter_files}
      generated/axlua_webview_gen.cpp
    )
  endif()

  if(IOS OR MACOSX)
    set(lua_bindings_adapter_headers
      ${lua_bindings_adapter_headers}
      generated/axlua_video_gen.h
    )
    set(lua_bindings_adapter_files
      ${lua_bindings_adapter_files}
      generated/axlua_video_gen.cpp
    )
  endif()
elseif(ANDROID)
  set(lua_bindings_adapter_headers
    ${lua_bindings_adapter_headers}
    generated/axlua_webview_gen.h
    generated/axlua_video_gen.h
  )
  set(lua_bindings_adapter_files
    ${lua_bindings_adapter_files}
    generated/axlua_webview_gen.cpp
    generated/axlua_video_gen.cpp
  )
elseif(WINDOWS)
  if(AX_ENABLE_MFMEDIA)
    set(lua_bindings_adapter_headers
      ${lua_bindings_adapter_headers}
      generated/axlua_video_gen.h
    )
    set(lua_bindings_adapter_files
      ${lua_bindings_adapter_files}
      generated/axlua_video_gen.cpp
    )
  endif()

  if(AX_ENABLE_MSEDGE_WEBVIEW2)
    set(lua_bindings_adapter_headers
      ${lua_bindings_adapter_headers}
      generated/axlua_webview_gen.h
    )
    set(lua_bindings_adapter_files
      ${lua_bindings_adapter_files}
      generated/axlua_webview_gen.cpp
    )
  endif()
elseif(LINUX)
  set(lua_bindings_adapter_files
    ${lua_bindings_adapter_files}
    generated/axlua_webview_gen.cpp)

  if(AX_ENABLE_VLC_MEDIA)
    set(lua_bindings_adapter_files
      ${lua_bindings_adapter_files}
      generated/axlua_video_gen.cpp
    )
  endif()
endif()

set(lua_bindings_files
  ${lua_bindings_runtime_files}
  ${lua_bindings_adapter_files}
  ${lua_bindings_platform_files}
  ${lua_bindings_generated_files}
)
set(lua_bindings_headers
  ${lua_bindings_runtime_headers}
  ${lua_bindings_adapter_headers}
  ${lua_bindings_platform_headers}
  ${lua_bindings_generated_headers}
)

set(lua_bindings_files
  ${lua_bindings_files}
  "${ax_root}/3rdparty/yasio/yasio/bindings/lyasio.cpp"
  "${ax_root}/3rdparty/yasio/yasio/bindings/yasio_axlua.cpp")

list(APPEND lua_bindings_files ${lua_bindings_headers})

set(_AX_LUA_LIB axlua CACHE INTERNAL "The axmol lua bindings lib name")

add_library(${_AX_LUA_LIB} STATIC ${lua_bindings_files})

# Preserve the historical numeric conversion behavior.
# This must be a target-wide definition: sol2 may be pulled in by a unity
# source or precompiled header before axlua_runtime.h is parsed.
target_compile_definitions(${_AX_LUA_LIB} PRIVATE
  SOL_NO_CHECK_NUMBER_PRECISION=1
  AXMOL_LUA_BINDINGS=1)

# The new generator/runtime includes the checked-in sol2 fork directly.  Keep
# these paths private to the binding library; application targets only need the
# public Lua headers and generated binding headers.
target_include_directories(${_AX_LUA_LIB}
  PRIVATE ${ax_root}/3rdparty/lua
  PRIVATE ${ax_root}/3rdparty/lua/plainlua
  PRIVATE ${ax_root}/3rdparty/yasio
  PRIVATE ${ax_root}/3rdparty/robin-map/include
  PRIVATE ${ax_root}/3rdparty/fmt/include
)

if(MSVC)
  target_compile_options(${_AX_LUA_LIB} PRIVATE /bigobj)
endif()

ax_config_pred(${_AX_LUA_LIB} AX_ENABLE_EXT_SPINE)
ax_config_pred(${_AX_LUA_LIB} AX_ENABLE_EXT_FAIRYGUI)

get_target_property(lua_ext_depend 3rdparty AX_LUA_DEPEND)
target_link_libraries(${_AX_LUA_LIB} ${lua_ext_depend})

ax_setup_ext_config(${_AX_LUA_LIB} DNTLINK)

target_include_directories(${_AX_LUA_LIB}
  PRIVATE ${ax_root}/3rdparty
  PUBLIC ${ax_root}/3rdparty/lua
  PUBLIC ${ax_root}/extensions/scripting
  PRIVATE platform/android
  PRIVATE platform/android/jni
  PRIVATE ${ax_root}/extensions/spine/runtime/include
  PRIVATE ${ax_root}/extensions/spine/src
  PRIVATE ${ax_root}/extensions/sceneext/src
  PRIVATE ${ax_root}/extensions/sceneio/src
  PRIVATE ${ax_root}/extensions/fairygui/src
  PRIVATE ${ax_root}/extensions/fairygui/src/fairygui
  PRIVATE ${ax_root}/extensions/Particle3D/src
  INTERFACE ${ax_root}/extensions/scripting/lua-bindings
)

if(BUILD_SHARED_LIBS)
  target_compile_definitions(${_AX_LUA_LIB} PRIVATE SCNIO_DLLIMPORT SCNEXT_DLLIMPORT)
endif()

if(WINRT)
    target_compile_options(${_AX_LUA_LIB} PUBLIC /wd4700)
endif()

set_target_properties(${_AX_LUA_LIB}
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  FOLDER "Extensions"
)

if(XCODE OR VS)
  ax_mark_code_files("${_AX_LUA_LIB}")
endif()
