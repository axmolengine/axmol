cmake_minimum_required(VERSION 3.6)

set(LUA_SRC_PATH ${CMAKE_CURRENT_LIST_DIR}/lua-${LUA_VERSION}/src)

set(lib_name plainlua)
set(target_name ${lib_name})

project(${lib_name})

include(../../cmake/CocosExternalConfig.cmake)

set ( LUA_IDSIZE 128 CACHE STRING "The maximum size for the description of the source." )
configure_file ( ${LUA_SRC_PATH}/luaconf.h.in ${LUA_SRC_PATH}/luaconf.h )

aux_source_directory(${LUA_SRC_PATH} LUA_CORE)
list(REMOVE_ITEM LUA_CORE ${LUA_SRC_PATH}/lua.c ${LUA_SRC_PATH}/luac.c)

add_library(${target_name} ${LUA_CORE})

target_compile_definitions(${target_name} 
    PRIVATE _CRT_SECURE_NO_WARNINGS 
    PRIVATE _WINSOCK_DEPRECATED_NO_WARNINGS
    )

if(${LUA_VERSION} VERSION_GREATER_EQUAL "5.4.0") # !important: traditional lua bindings solution tolua++ not support yet
  target_compile_definitions(${target_name} PUBLIC LUA_COMPAT_APIINTCASTS=1)
elseif(${LUA_VERSION} VERSION_GREATER_EQUAL "5.3.0")
  target_compile_definitions(${target_name} 
    PUBLIC LUA_COMPAT_5_1=1
    PUBLIC LUA_COMPAT_5_2=1
    )
elseif(${LUA_VERSION} VERSION_GREATER_EQUAL "5.2.0")
  target_compile_definitions(${target_name} PUBLIC LUA_COMPAT_ALL=1)
endif()

if(BUILD_SHARED_LIBS)
    target_compile_definitions(${target_name} PUBLIC LUA_BUILD_AS_DLL=1)
endif()

set_target_properties(${target_name} PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${LUA_SRC_PATH}"
)

set_target_properties(${target_name}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    FOLDER "External"
)

if(WIN32 OR MACOSX)
  # tool:plainluac compiling bytecode
  set(target_name plainluac)
  add_executable(${target_name} ${LUA_CORE} ${LUA_SRC_PATH}/luac.c)
  if(${LUA_VERSION} VERSION_GREATER_EQUAL "5.4.0") # !important: traditional lua bindings solution tolua++ not support yet
    target_compile_definitions(${target_name} PUBLIC LUA_COMPAT_APIINTCASTS=1)
  elseif(${LUA_VERSION} VERSION_GREATER_EQUAL "5.3.0")
    target_compile_definitions(${target_name} 
      PUBLIC LUA_COMPAT_5_1=1
      PUBLIC LUA_COMPAT_5_2=1
      )
  elseif(${LUA_VERSION} VERSION_GREATER_EQUAL "5.2.0")
    target_compile_definitions(${target_name} PUBLIC LUA_COMPAT_ALL=1)
  endif()

  set_target_properties(${target_name}
      PROPERTIES
      ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
      FOLDER "Tools"
  )
endif()
