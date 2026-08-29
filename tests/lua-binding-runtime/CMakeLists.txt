cmake_minimum_required(VERSION 3.20)

project(lua-binding-runtime LANGUAGES CXX)

add_executable(lua-binding-runtime Source/main.cpp)
target_link_libraries(lua-binding-runtime PRIVATE ${_AX_LUA_LIB} ${_AX_CORE_LIB})
target_include_directories(lua-binding-runtime PRIVATE
    ${_AX_ROOT}/extensions/scripting/lua-bindings
    ${_AX_ROOT}/3rdparty/lua
    ${_AX_ROOT}/3rdparty/lua/plainlua)
target_compile_definitions(lua-binding-runtime PRIVATE
    SOL_NO_CHECK_NUMBER_PRECISION=1
    AXMOL_LUA_BINDINGS=1)

if(NOT IOS AND NOT WINRT)
    target_precompile_headers(lua-binding-runtime PRIVATE
        "$<$<COMPILE_LANGUAGE:CXX>:axmol/axmol.h>")
endif()
