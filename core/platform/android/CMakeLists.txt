# need to whole-archive this lib
set(target_name cpp_android_spec)

project(${target_name})

set(${target_name}_src
  Device-android.cpp
  javaactivity-android.cpp
  jni/JniHelper.cpp
  jni/TouchesJni.cpp
  jni/Java_dev_axmol_lib_AxmolEngine.cpp
  jni/Java_dev_axmol_lib_AxmolRenderer.cpp
  jni/Java_dev_axmol_lib_AxmolAccelerometer.cpp
  jni/Java_dev_axmol_lib_BitmapHelper.cpp
)

add_library(${target_name} STATIC
  ${${target_name}_src}
)

target_link_libraries(${target_name} ${_AX_CORE_LIB})

target_include_directories(${target_name}
  PRIVATE ..
  PRIVATE ../..
  PRIVATE ../../../3rdparty
  PRIVATE ../../../3rdparty/yasio
  PRIVATE ../../../3rdparty/glad/include
  PRIVATE ../../../3rdparty/robin-map/include
  PRIVATE ../../../3rdparty/jni.hpp/include
  PRIVATE ../../../3rdparty/fmt/include
)
