target_sources(${LIB_NAME}
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismCdiJson.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismCdiJson.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismDefaultParameterId.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismDefaultParameterId.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismFramework.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismFramework.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismFrameworkConfig.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismModelSettingJson.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismModelSettingJson.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismJsonHolder.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ICubismAllocator.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ICubismModelSetting.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Live2DCubismCore.hpp
)

# Add sub directories.
add_subdirectory(Effect)
add_subdirectory(Id)
add_subdirectory(Math)
add_subdirectory(Model)
add_subdirectory(Motion)
add_subdirectory(Physics)
add_subdirectory(Rendering)
add_subdirectory(Type)
add_subdirectory(Utils)
