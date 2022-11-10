# need to whole-archive this lib
set(target_name cpp_android_spec)

project(${target_name})

set(${target_name}_src
  CCDevice-android.cpp
  javaactivity-android.cpp
  jni/JniHelper.cpp
  jni/TouchesJni.cpp
  jni/Java_org_axmol_lib_AxmolEngine.cpp
  jni/Java_org_axmol_lib_AxmolRenderer.cpp
  jni/Java_org_axmol_lib_AxmolAccelerometer.cpp
  jni/Java_org_axmol_lib_BitmapHelper.cpp
)

add_library(${target_name} STATIC
  ${${target_name}_src}
)

if (AX_USE_GLAD)
  target_compile_definitions(${target_name} PRIVATE AX_USE_GLAD=1)
endif()

target_include_directories(${target_name}
  PRIVATE ..
  PRIVATE ../..
  PRIVATE ../../../thirdparty
  PRIVATE ../../../thirdparty/glad/include
  PRIVATE ../../../thirdparty/robin-map/include
  PRIVATE ../../../thirdparty/jni.hpp/include
)
