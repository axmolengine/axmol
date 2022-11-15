
project(EffekseerForCocos2d-x)

if (APPLE)

add_library(EffekseerForCocos2d-x
    STATIC
    EffekseerForCocos2d-x.cpp
    EffekseerForCocos2d-x.h
    Metal/Effekseer+Metal.mm
    GL/Effekseer+GL.cpp)

else()

add_library(EffekseerForCocos2d-x
    STATIC
    EffekseerForCocos2d-x.cpp
    EffekseerForCocos2d-x.h
    GL/Effekseer+GL.cpp)

endif()

target_include_directories(${PROJECT_NAME} 
    PUBLIC ../Effekseer/
    PUBLIC ../EffekseerRendererCommon/
    PUBLIC ../
    PUBLIC ./
    PRIVATE ../3rdParty/LLGI/src/)

if (APPLE)
    target_include_directories(${PROJECT_NAME} 
        PUBLIC ../EffekseerRendererMetal/)

    target_link_libraries(${PROJECT_NAME} PRIVATE EffekseerRendererMetal)

    add_dependencies(${PROJECT_NAME} EffekseerRendererMetal)

    # add_dependencies(${PROJECT_NAME} EffekseerRendererGL)
else()
    target_include_directories(${PROJECT_NAME} 
        PUBLIC ../EffekseerRendererGL/)

    target_link_libraries(${PROJECT_NAME} PRIVATE EffekseerRendererGL)

    add_dependencies(${PROJECT_NAME} EffekseerRendererGL)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE Effekseer)

add_dependencies(${PROJECT_NAME} Effekseer)

# axmol spec
if(ANDROID)
    target_link_libraries(${PROJECT_NAME} INTERFACE ${_AX_CORE_LIB})
    target_include_directories(${PROJECT_NAME} PRIVATE $<TARGET_PROPERTY:${_AX_CORE_LIB},INTERFACE_INCLUDE_DIRECTORIES>)
else()
    target_link_libraries(${PROJECT_NAME} PUBLIC ${_AX_CORE_LIB})
endif()

set(_AX_EXTENSION_LIBS "${PROJECT_NAME};${_AX_EXTENSION_LIBS}" CACHE INTERNAL "extensions for auto link to target application")
