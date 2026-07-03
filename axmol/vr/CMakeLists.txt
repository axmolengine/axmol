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
    list(APPEND _AX_VR_HEADER vr/OpenXRContext.h vr/VRSceneCompositor.h)
    list(APPEND _AX_VR_SRC vr/OpenXRContext.cpp vr/VRSceneCompositor.cpp)
    if (APPLE)
      set_source_files_properties(vr/OpenXRContext.cpp vr/VRSceneCompositor.cpp PROPERTIES LANGUAGE OBJCXX)
    endif()
  endif()
endif()
