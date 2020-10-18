
# default value for cocos2dx extensions modules to Build
# currently 7 extensions
# extensions dependicies: COCOSTUDIO may depend on spine & dragonBones if they are present in buildset

option(BUILD_EXTENSION_GUI "Build extension GUI" ON)

option(BUILD_EXTENSION_ASSETMANAGER "Build extension asset-manager" ON)
option(BUILD_EXTENSION_PARTICLE3D "Build extension Particle3D" ON)
option(BUILD_EXTENSION_PHYSICS_NODE "Build extension physics-nodes" ON)

option(BUILD_EXTENSION_SPINE "Build extension spine" ON)
option(BUILD_EXTENSION_DRAGONBONES "Build extension DragonBones" ON)

option(BUILD_EXTENSION_COCOSTUDIO "Build extension cocostudio" ON)

option(BUILD_EXTENSION_FAIRYGUI "Build extension FairyGUI" ON)

if(WINDOWS OR MACOSX OR LINUX)
  option(BUILD_EXTENSION_IMGUIEXT "Build extension ImGuiEXT" ON)
else()
  set(BUILD_EXTENSION_IMGUIEXT OFF)
endif()

if(BUILD_SHARED_LIBS)
  cmake_policy(SET CMP0079 NEW)
endif()

function(setup_cocos_extension_config target_name)
    set(options DNTLINK opt_DNTLINK)
    cmake_parse_arguments(opt "" "${options}" ""
                          "" ${ARGN} )

  if(ANDROID)
    target_link_libraries(${target_name} INTERFACE ${EGNX_CORE_LIB})
    target_include_directories(${target_name} PRIVATE $<TARGET_PROPERTY:${EGNX_CORE_LIB},INTERFACE_INCLUDE_DIRECTORIES>)
    # target_compile_definitions(${target_name} PRIVATE CP_USE_CGTYPES=0)
    # target_compile_definitions(${target_name} PRIVATE CP_USE_DOUBLES=0)
  else()
    target_link_libraries(${target_name} ${EGNX_CORE_LIB})
  endif()

  if(BUILD_SHARED_LIBS)
    target_compile_definitions(${target_name} PRIVATE _USREXDLL=1)
  endif()

  set_target_properties(${target_name}
      PROPERTIES
      ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
      FOLDER "Extensions"
  )

  if(NOT opt_DNTLINK)
      message(STATUS "linking ${target_name}...")
      set(CC_EXTENSION_LIBS "${target_name};${CC_EXTENSION_LIBS}" CACHE INTERNAL "extensions for auto link to target application")
  endif()
  
  if(XCODE OR VS)
    cocos_mark_code_files("${target_name}")
  endif()

endfunction()

if(BUILD_EXTENSION_GUI)
  add_subdirectory(GUI)
endif()

if(BUILD_EXTENSION_ASSETMANAGER)
  add_subdirectory(assets-manager)
endif()

if(BUILD_EXTENSION_PARTICLE3D)
  add_subdirectory(Particle3D)
endif()

if(BUILD_EXTENSION_PHYSICS_NODE)
  add_subdirectory(physics-nodes)
endif()

if(BUILD_EXTENSION_SPINE)
  add_subdirectory(spine)
endif()

if(BUILD_EXTENSION_DRAGONBONES)
  add_subdirectory(DragonBones)
endif()

if(BUILD_EXTENSION_FAIRYGUI)
  add_subdirectory(fairygui)
  if(BUILD_SHARED_LIBS)
    target_link_libraries(fairygui spine)
  else()
    target_include_directories(fairygui PUBLIC ${CMAKE_CURRENT_LIST_DIR}/spine/runtime/include)
  endif()
endif()

if(BUILD_EXTENSION_IMGUIEXT)
  add_subdirectory(ImGuiEXT)
endif()

if(BUILD_EXTENSION_COCOSTUDIO)
  add_subdirectory(libeditor)
  if(BUILD_EXTENSION_SPINE)
    target_compile_definitions(libeditor PUBLIC CC_BUILD_WITH_SPINE=1)
    if(BUILD_SHARED_LIBS)
      target_link_libraries(libeditor spine)
    else()
      target_include_directories(libeditor PUBLIC ${CMAKE_CURRENT_LIST_DIR}/spine/runtime/include)
    endif()
  endif()
  add_subdirectory(libccs30)
  if(BUILD_SHARED_LIBS)
    target_link_libraries(libccs30 libeditor particle3d)
  endif()
  if(BUILD_EXTENSION_DRAGONBONES)
    target_compile_definitions(libccs30 PRIVATE CC_BUILD_WITH_DRANGBONES=1)
    if(BUILD_SHARED_LIBS)
      target_link_libraries(libccs30 DragonBones)
    endif()
  endif()
endif()

if(BUILD_LUA_LIBS) # TODO: rename to BUILD_EXTENSION_LUA
  add_subdirectory(scripting/lua-bindings)
endif()

message(STATUS "CC_EXTENSION_LIBS:${CC_EXTENSION_LIBS}")
