


if(BUILD_SHARED_LIBS)
  cmake_policy(SET CMP0079 NEW)
endif()

function(setup_ax_extension_config target_name)
    set(options DNTLINK opt_DNTLINK)
    cmake_parse_arguments(opt "" "${options}" ""
                          "" ${ARGN} )

    target_link_libraries(${target_name} ${_AX_CORE_LIB})

  if(BUILD_SHARED_LIBS)
    target_compile_definitions(${target_name} PRIVATE _USREXDLL=1)
  endif()

  set_target_properties(${target_name}
      PROPERTIES
      ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
      FOLDER "Extensions"
  )

  if (WINRT)
    set_property(TARGET ${target_name} PROPERTY VS_PROJECT_IMPORT ${CMAKE_BINARY_DIR}/packages/Microsoft.Windows.CppWinRT/build/native/Microsoft.Windows.CppWinRT.props)
  endif()

  if(NOT opt_DNTLINK)
      set(_AX_EXTENSION_LIBS "${target_name};${_AX_EXTENSION_LIBS}" CACHE INTERNAL "extensions for auto link to target application")
  endif()
  
  if(XCODE OR VS)
    ax_mark_code_files("${target_name}")
  endif()

endfunction()

if(AX_ENABLE_EXT_GUI)
  add_subdirectory(GUI)
endif()

if(AX_ENABLE_EXT_ASSETMANAGER)
  add_subdirectory(assets-manager)
endif()

if(AX_ENABLE_EXT_PARTICLE3D)
  add_subdirectory(Particle3D)
endif()

if(AX_ENABLE_EXT_PHYSICS_NODE)
  add_subdirectory(physics-nodes)
endif()

if(AX_ENABLE_EXT_SPINE)
  add_subdirectory(spine)
endif()

if(AX_ENABLE_EXT_DRAGONBONES)
  add_subdirectory(DragonBones)
endif()

if(AX_ENABLE_EXT_FAIRYGUI)
  add_subdirectory(fairygui)
  if(BUILD_SHARED_LIBS)
    target_link_libraries(fairygui spine)
  else()
    target_include_directories(fairygui PUBLIC ${CMAKE_CURRENT_LIST_DIR}/spine/runtime/include)
  endif()
endif()

if(AX_ENABLE_EXT_IMGUI)
  add_subdirectory(ImGui)
  option(AX_ENABLE_EXT_INSPECTOR "Enable extension Inspector" ON)
  if(AX_ENABLE_EXT_INSPECTOR)
      add_subdirectory(Inspector)
      target_include_directories(Inspector PUBLIC ${CMAKE_CURRENT_LIST_DIR}/ImGui)
  endif()
  if (AX_ENABLE_EXT_SDFGEN)
      add_subdirectory(SDFGen)
      target_include_directories(SDFGen PUBLIC ${CMAKE_CURRENT_LIST_DIR}/ImGui)
  endif()
endif()

if(AX_ENABLE_EXT_COCOSTUDIO)
  add_subdirectory(cocostudio)
  if(AX_ENABLE_EXT_SPINE)
    target_compile_definitions(cocostudio PRIVATE AX_ENABLE_EXT_SPINE=1)
    if(BUILD_SHARED_LIBS)
      target_link_libraries(cocostudio spine particle3d)
    else()
      target_include_directories(cocostudio PUBLIC ${CMAKE_CURRENT_LIST_DIR}/spine/runtime/include)
    endif()
  endif()
  if(AX_ENABLE_EXT_DRAGONBONES)
    target_compile_definitions(cocostudio PRIVATE AX_ENABLE_EXT_DRAGONBONES=1)
    if(BUILD_SHARED_LIBS)
      target_link_libraries(cocostudio DragonBones)
    endif()
  endif()
endif()

if(AX_ENABLE_EXT_LIVE2D)
  add_subdirectory(Live2D)
endif()

if (AX_ENABLE_EXT_EFFEKSEER)
  add_subdirectory(Effekseer)
endif()

if(AX_ENABLE_EXT_LUA)
  add_subdirectory(scripting/lua-bindings)
endif()

if(AX_ENABLE_EXT_JSONDEFAULT)
  add_subdirectory(JSONDefault)
endif()

message(STATUS "Enabled ${_AX_CORE_LIB} extensions:${_AX_EXTENSION_LIBS}")
