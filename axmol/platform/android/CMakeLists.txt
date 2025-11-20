# need to whole-archive this lib
set(target_name cpp_android_spec)

project(${target_name})

set(${target_name}_src
  Device-android.cpp
  javaactivity-android.cpp
  jni/JniHelper.cpp
  jni/AxmolEngineJni.cpp
  jni/AxmolPlayerJni.cpp
  jni/AxmolAccelerometerJni.cpp
  jni/BitmapHelperJni.cpp
)

add_library(${target_name} STATIC
  ${${target_name}_src}
)

if(NOT BUILD_SHARED_LIBS)
  target_link_libraries(${target_name} ${_AX_CORE_LIB})
endif()

target_include_directories(${target_name}
  PRIVATE ../../..
  PRIVATE ../../../3rdparty
  PRIVATE ../../../3rdparty/yasio
  PRIVATE ../../../3rdparty/glad/include
  PRIVATE ../../../3rdparty/robin-map/include
  PRIVATE ../../../3rdparty/jni.hpp/include
  PRIVATE ../../../3rdparty/fmt/include
)
