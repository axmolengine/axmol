
# need to whole-archive this lib
set(target_name lua_android_spec)

project(${target_name})

set(${target_name}_src
  ../manual/platform/android/LuaJavaBridge.cpp
  ../manual/platform/android/jni/LuaBridge.cpp
)

add_library(${target_name} STATIC
  ${${target_name}_src}
)

get_target_property(lua_header ${AX_LUA_ENGINE} INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(tolua_header tolua INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(${target_name}
  PRIVATE ../..
  PRIVATE ../manual
  PRIVATE ../manual/platform/android
  PRIVATE ${lua_header}
  PRIVATE ${tolua_header}
  PRIVATE ../../../../axmol
  PRIVATE ../../../../3rdparty
  PRIVATE ../../../../3rdparty/fmt/include
  PRIVATE ../../../../3rdparty/robin-map/include
  PRIVATE ../../../../3rdparty/jni.hpp/include
  PRIVATE ../../../../3rdparty/yasio
)

