set(target_name plainlua)

get_filename_component(LUA_SRC_PATH ../../../3rdparty/lua ABSOLUTE)
message(STATUS "LUA_SRC_PATH=${LUA_SRC_PATH}")

file(GLOB LUA_SRC_FILES 
    "${LUA_SRC_PATH}/*.c")
    
list(REMOVE_ITEM LUA_SRC_FILES "${LUA_SRC_PATH}/lua.c")
list(REMOVE_ITEM LUA_SRC_FILES "${LUA_SRC_PATH}/onelua.c")

add_library(${target_name} ${LUA_SRC_FILES})

# fix default lua_newuserdata can't return aligned pointer properly
# MSVC max_align_t=double, lua already handle properly
if (NOT MSVC)
    # see also: https://github.com/llvm/llvm-project/blob/master/clang/lib/Headers/__stddef_max_align_t.h
    if(APPLE)
        target_compile_definitions (${target_name} PUBLIC "LUAI_USER_ALIGNMENT_T=long double")
    else()
        target_compile_definitions (${target_name} PUBLIC LUAI_USER_ALIGNMENT_T=max_align_t)
    endif()
endif()

if(MSVC AND BUILD_SHARED_LIBS)
    target_compile_definitions(${target_name} 
        PUBLIC LUA_BUILD_AS_DLL=1
        # PUBLIC LUA_CORE=1
        )
endif()

target_include_directories(${target_name} 
    INTERFACE ${LUA_SRC_PATH}
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
    )
