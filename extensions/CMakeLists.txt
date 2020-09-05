
# default value for cocos2dx extensions modules to Build
# currently 7 extensions
# extensions dependicies: COCOSTUDIO may depend on spine & dragonBones if they are present in buildset

option(BUILD_EXTENSION_ASSETMANAGER "Build extension asset-manager" ON)
option(BUILD_EXTENSION_GUI "Build extension GUI" ON)
option(BUILD_EXTENSION_PARTICLE3D "Build extension Particle3D" ON)
option(BUILD_EXTENSION_PHYSICS_NODE "Build extension physics-nodes" ON)

option(BUILD_EXTENSION_SPINE "Build extension spine" ON)
option(BUILD_EXTENSION_DRAGONBONES "Build extension DragonBones" ON)

option(BUILD_EXTENSION_COCOSTUDIO "Build extension cocostudio" ON)

option(BUILD_EXTENSION_FAIRYGUI "Build extension FairyGUI" ON)

option(BUILD_EXTENSION_IMGUIEXT "Build extension ImGuiEXT" OFF)

function(setup_cocos_extension_config target_name)
  if(ANDROID)
    target_link_libraries(${target_name} INTERFACE cocos2d)
    target_include_directories(${target_name} PRIVATE $<TARGET_PROPERTY:cocos2d,INTERFACE_INCLUDE_DIRECTORIES>)
    # target_compile_definitions(${target_name} PRIVATE CP_USE_CGTYPES=0)
    # target_compile_definitions(${target_name} PRIVATE CP_USE_DOUBLES=0)
  else()
    target_link_libraries(${target_name} cocos2d)
  endif()
  
  set_target_properties(${target_name}
      PROPERTIES
      ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
      FOLDER "Extensions"
  )

  set(CC_EXTENSION_LIBS "${target_name};${CC_EXTENSION_LIBS}" CACHE INTERNAL "extensions for auto link to target application")
endfunction()

if(BUILD_EXTENSION_ASSETMANAGER)
  add_subdirectory(assets-manager)
endif()

if(BUILD_EXTENSION_GUI)
  add_subdirectory(GUI)
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
  add_subdirectory(dragonbones)
endif()

if(BUILD_EXTENSION_COCOSTUDIO)
  add_subdirectory(cocostudio)
  if(BUILD_EXTENSION_DRAGONBONES)
    target_compile_definitions(cocostudio PRIVATE CC_BUILD_WITH_DRANGBONES=1)
    target_compile_definitions(cocostudio PUBLIC CC_BUILD_WITH_SPINE=1)
  endif()
endif()

if(BUILD_EXTENSION_FAIRYGUI)
  add_subdirectory(fairygui)
endif()

if(BUILD_EXTENSION_IMGUIEXT)
  add_subdirectory(ImGuiEXT)
endif()

message(STATUS "CC_EXTENSION_LIBS:${CC_EXTENSION_LIBS}")
