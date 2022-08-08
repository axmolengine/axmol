# need to whole-archive this lib
set(target_name cpp_android_spec)

project(${target_name})

set(${target_name}_src
  CCDevice-android.cpp
  javaactivity-android.cpp
  jni/JniHelper.cpp
  jni/TouchesJni.cpp
  jni/Java_org_axys1_lib_AxysAccelerometer.cpp
  jni/Java_org_axys1_lib_AxysBitmap.cpp
  jni/Java_org_axys1_lib_AxysHelper.cpp
  jni/Java_org_axys1_lib_AxysRenderer.cpp
)

add_library(${target_name} STATIC
  ${${target_name}_src}
)

target_include_directories(${target_name}
  PRIVATE ..
  PRIVATE ../..
  PRIVATE ../../../thirdparty
  PRIVATE ../../../thirdparty/robin-map/include
  PRIVATE ../../../thirdparty/jni.hpp/include
)  
