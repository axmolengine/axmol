
# default value for cocos2dx extensions modules to Build
# currently 7 extensions
# extensions dependicies: COCOSTUDIO may depend on spine & dragonBones if they are present in buildset

option(AX_ENABLE_EXT_GUI "Build extension GUI" ON)

option(AX_ENABLE_EXT_ASSETMANAGER "Build extension asset-manager" ON)
option(AX_ENABLE_EXT_PARTICLE3D "Build extension Particle3D" ON)
option(AX_ENABLE_EXT_PHYSICS_NODE "Build extension physics-nodes" ON)

option(AX_ENABLE_EXT_SPINE "Build extension spine" ON)
option(AX_ENABLE_EXT_DRAGONBONES "Build extension DragonBones" ON)

option(AX_ENABLE_EXT_COCOSTUDIO "Build extension cocostudio" ON)

option(AX_ENABLE_EXT_FAIRYGUI "Build extension FairyGUI" ON)

if(WINDOWS OR MACOSX OR LINUX)
  option(AX_ENABLE_EXT_IMGUI "Build extension ImGui" ON)
else()
  set(AX_ENABLE_EXT_IMGUI OFF)
endif()

if(BUILD_SHARED_LIBS)
  cmake_policy(SET CMP0079 NEW)
endif()

function(setup_cocos_extension_config target_name)
    set(options DNTLINK opt_DNTLINK)
    cmake_parse_arguments(opt "" "${options}" ""
                          "" ${ARGN} )

  if(ANDROID)
    target_link_libraries(${target_name} INTERFACE ${ADXE_CORE_LIB})
    target_include_directories(${target_name} PRIVATE $<TARGET_PROPERTY:${ADXE_CORE_LIB},INTERFACE_INCLUDE_DIRECTORIES>)
    # target_compile_definitions(${target_name} PRIVATE CP_USE_CGTYPES=0)
    # target_compile_definitions(${target_name} PRIVATE CP_USE_DOUBLES=0)
  else()
    target_link_libraries(${target_name} ${ADXE_CORE_LIB})
  endif()

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

  if(NOT opt_DNTLINK)
      set(_AX_EXTENSION_LIBS "${target_name};${_AX_EXTENSION_LIBS}" CACHE INTERNAL "extensions for auto link to target application")
  endif()
  
  if(XCODE OR VS)
    cocos_mark_code_files("${target_name}")
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
endif()

if(AX_ENABLE_EXT_COCOSTUDIO)
  add_subdirectory(cocostudio)
  if(AX_ENABLE_EXT_SPINE)
    target_compile_definitions(cocostudio PUBLIC CC_BUILD_WITH_SPINE=1)
    if(BUILD_SHARED_LIBS)
      target_link_libraries(cocostudio spine particle3d)
    else()
      target_include_directories(cocostudio PUBLIC ${CMAKE_CURRENT_LIST_DIR}/spine/runtime/include)
    endif()
  endif()
  if(AX_ENABLE_EXT_DRAGONBONES)
    target_compile_definitions(cocostudio PRIVATE CC_BUILD_WITH_DRANGBONES=1)
    if(BUILD_SHARED_LIBS)
      target_link_libraries(cocostudio DragonBones)
    endif()
  endif()
endif()

if(AX_ENABLE_EXT_LUA)
  add_subdirectory(scripting/lua-bindings)
endif()


message(STATUS "Enabled adxe extensions:${_AX_EXTENSION_LIBS}")
