if(AX_ENABLE_OPENXR)
  set(_AX_OPENXR_HEADER
    platform/openxr/OpenXRContext.h
    platform/openxr/OpenXRVulkanInterop.h
  )
  set(_AX_OPENXR_SRC
    platform/openxr/OpenXRContext.cpp
    platform/openxr/OpenXRVulkanInterop.cpp
  )
  if (APPLE)
    set_source_files_properties(platform/openxr/OpenXRContext.cpp PROPERTIES LANGUAGE OBJCXX)
  endif()
endif()
