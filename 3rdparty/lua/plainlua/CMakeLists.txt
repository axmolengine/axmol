cmake_minimum_required(VERSION 3.22)

set(_LUA_VERSION "5.4.8")
set(lib_name plainlua)

set(LUA_SRC_PATH ${CMAKE_CURRENT_LIST_DIR})

project(${lib_name})

aux_source_directory(${LUA_SRC_PATH} LUA_CORE)
list(REMOVE_ITEM LUA_CORE ${LUA_SRC_PATH}/lua.c ${LUA_SRC_PATH}/luac.c)

set(LUA_BUILD_AS_DLL ${BUILD_SHARED_LIBS})

# generate additional exports .def file
set(_export_symbols "; Export additional symbols for build luac.exe")
string(APPEND _export_symbols "\nEXPORTS")

if(${_LUA_VERSION} VERSION_GREATER_EQUAL "5.4.0")
  string(APPEND _export_symbols "\n    luaG_getfuncline")

  if(${_LUA_VERSION} VERSION_GREATER_EQUAL "5.5.0")
    string(APPEND _export_symbols "\n    luaH_getstr")
    string(APPEND _export_symbols "\n    luaH_set")
    string(APPEND _export_symbols "\n    luaH_new")
  endif()
elseif(${_LUA_VERSION} VERSION_GREATER_EQUAL "5.3.0")
  string(APPEND _export_symbols "\n")
elseif(${_LUA_VERSION} VERSION_GREATER_EQUAL "5.1.0")
  string(APPEND _export_symbols "\n    luaU_header")

  if(${_LUA_VERSION} VERSION_LESS "5.2.0")
    string(APPEND _export_symbols "\n    luaM_realloc_")
    string(APPEND _export_symbols "\n    luaM_toobig")
    string(APPEND _export_symbols "\n    luaD_growstack")
    string(APPEND _export_symbols "\n    luaS_newlstr")
    string(APPEND _export_symbols "\n    luaU_print")
    string(APPEND _export_symbols "\n    luaF_newproto")
  endif()
else()
  message(FATAL_ERROR "Unsupported Lua version: ${_LUA_VERSION}. Please use Lua 5.1.0 or later.")
endif()

write_file("${LUA_SRC_PATH}/plainlua.def" "${_export_symbols}")

if(WIN32)
  set(LUA_BUILD_AS_DLL TRUE)
  add_library(${lib_name} SHARED ${LUA_CORE} ${LUA_SRC_PATH}/plainlua.def)
else()
  if(BUILD_SHARED_LIBS AND NOT WINRT)
    set(LUA_BUILD_AS_DLL FALSE)
    add_library(${lib_name} SHARED ${LUA_CORE} ${LUA_SRC_PATH}/plainlua.def)
  else()
    add_library(${lib_name} ${LUA_CORE})
  endif()
endif()

target_compile_definitions(${lib_name}
  PRIVATE _CRT_SECURE_NO_WARNINGS
  PRIVATE _WINSOCK_DEPRECATED_NO_WARNINGS
)

if(IOS)
  target_compile_definitions(${lib_name} PUBLIC LUA_USE_IOS=1)
endif()

if(${_LUA_VERSION} VERSION_GREATER_EQUAL "5.4.0") # !important: traditional lua bindings solution tolua++ not support yet
  target_compile_definitions(${lib_name}
    PUBLIC LUA_COMPAT_APIINTCASTS=1
    PUBLIC LUA_COMPAT_MATHLIB=1
  )
elseif(${_LUA_VERSION} VERSION_GREATER_EQUAL "5.3.0")
  target_compile_definitions(${lib_name}
    PUBLIC LUA_COMPAT_5_1=1
    PUBLIC LUA_COMPAT_5_2=1
  )
elseif(${_LUA_VERSION} VERSION_GREATER_EQUAL "5.2.0")
  target_compile_definitions(${lib_name}
    PUBLIC LUA_COMPAT_ALL=1
  )
endif()

if(LUA_BUILD_AS_DLL)
  target_compile_definitions(${lib_name} PUBLIC LUA_BUILD_AS_DLL=1)
endif()

set_target_properties(${lib_name} PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${LUA_SRC_PATH}"
)

set_target_properties(${lib_name}
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
  FOLDER "3rdparty"
)

if(WIN32 OR MACOSX)
  function(add_lua_bin target_name src_files)
    add_executable(${target_name} ${src_files})

    target_compile_definitions(${target_name}
      PRIVATE _CRT_SECURE_NO_WARNINGS
      PRIVATE _WINSOCK_DEPRECATED_NO_WARNINGS
    )

    target_link_libraries(${target_name} ${lib_name})

    set_target_properties(${target_name}
      PROPERTIES
      ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
      FOLDER "Tools"
    )
  endfunction()

  # tool:luac the lua bytecode compiler
  add_lua_bin(luac "${LUA_SRC_PATH}/luac.c;${LUA_SRC_PATH}/ldump.c;${LUA_SRC_PATH}/lopcodes.c")

  # tool:lua the lua interpreter
  add_lua_bin(lua "${LUA_SRC_PATH}/lua.c")
elseif(LINUX)
  target_compile_definitions(${lib_name} PUBLIC LUA_USE_LINUX=1)
endif()
