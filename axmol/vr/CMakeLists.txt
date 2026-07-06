if(AX_ENABLE_VR)
  set(_AX_VR_HEADER
    vr/VRBase.h
    vr/VRGenericHeadTracker.h
    vr/VRDistortion.h
    vr/VRPreviewSceneCompositor.h
    vr/VRDistortionMesh.h
  )
  set(_AX_VR_SRC
    vr/VRDistortion.cpp
    vr/VRDistortionMesh.cpp
    vr/VRPreviewSceneCompositor.cpp
    vr/VRGenericHeadTracker.cpp
  )
  if(AX_ENABLE_OPENXR)
    list(APPEND _AX_VR_HEADER vr/VRSceneCompositor.h vr/OpenXRDriver.h vr/OpenXRVulkanInterop.h)
    list(APPEND _AX_VR_SRC vr/VRSceneCompositor.cpp vr/OpenXRDriver.cpp vr/OpenXRVulkanInterop.cpp)
    if (APPLE)
      set_source_files_properties(vr/VRSceneCompositor.cpp vr/OpenXRDriver.cpp PROPERTIES LANGUAGE OBJCXX)
    endif()
  endif()
endif()
