add_library(Live2DCubismCore STATIC IMPORTED GLOBAL)

_1kfetch(live2d-core PREFIX ${CMAKE_CURRENT_LIST_DIR} FOLDER "Core")

set(live2d_core_dir ${CMAKE_CURRENT_LIST_DIR}/Core)

set(live2d_arch ${ARCH_ALIAS})

if(ARCH_ALIAS STREQUAL "x64")
  set(live2d_arch "x86_64")
else()
  set(live2d_arch ${ARCH_ALIAS})
endif()

if(ANDROID)
  set_target_properties(Live2DCubismCore
    PROPERTIES
    IMPORTED_LOCATION
    ${live2d_core_dir}/lib/android/${ANDROID_ABI}/libLive2DCubismCore.a
  )
elseif(LINUX)
  set_target_properties(Live2DCubismCore
    PROPERTIES
    IMPORTED_LOCATION
    ${live2d_core_dir}/lib/linux/x86_64/libLive2DCubismCore.a
  )
elseif(WINDOWS)
  set(CRT MD)
  # The Live2D prebuilt libraries for MSVC are available only up to version 143, 
  # while Axmol requires 14.39. Compatibility should be ensured.
  set(live2d_msvc 143)

  set_target_properties(Live2DCubismCore
    PROPERTIES
    IMPORTED_LOCATION ${live2d_core_dir}/lib/windows/${live2d_arch}/${live2d_msvc}/Live2DCubismCore_MD.lib
    IMPORTED_LOCATION_DEBUG
    ${live2d_core_dir}/lib/windows/${live2d_arch}/${live2d_msvc}/Live2DCubismCore_${CRT}d.lib
    IMPORTED_LOCATION_RELEASE
    ${live2d_core_dir}/lib/windows/${live2d_arch}/${live2d_msvc}/Live2DCubismCore_MD.lib
  )
elseif(APPLE)
  if(IOS)
    # The variable 'SIMULATOR' is from 1k/ios.cmake
    if(NOT SIMULATOR)
      set_target_properties(Live2DCubismCore
        PROPERTIES
        IMPORTED_LOCATION_DEBUG
        ${live2d_core_dir}/lib/ios/Debug-iphoneos/libLive2DCubismCore.a
        IMPORTED_LOCATION_RELEASE
        ${live2d_core_dir}/lib/ios/Release-iphoneos/libLive2DCubismCore.a
      )
    else()
      set_target_properties(Live2DCubismCore
        PROPERTIES
        IMPORTED_LOCATION_DEBUG
        ${live2d_core_dir}/lib/ios/Debug-iphonesimulator/libLive2DCubismCore.a
        IMPORTED_LOCATION_RELEASE
        ${live2d_core_dir}/lib/ios/Release-iphonesimulator/libLive2DCubismCore.a
      )
    endif()
  elseif(MACOSX)
    set_target_properties(Live2DCubismCore
      PROPERTIES
      IMPORTED_LOCATION
      ${live2d_core_dir}/lib/macos/${live2d_arch}/libLive2DCubismCore.a
    )
  endif()
endif()

# Set core include directory.
set_target_properties(Live2DCubismCore
  PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${live2d_core_dir}/include
)
