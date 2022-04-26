cmake_minimum_required(VERSION 3.10)

set(APP_NAME lua-tests)

project(${APP_NAME})

if(NOT DEFINED BUILD_ENGINE_DONE)
    if(XCODE)
        set(CMAKE_XCODE_GENERATE_TOP_LEVEL_PROJECT_ONLY TRUE)
    endif()

    set(ADXE_ROOT "$ENV{ADXE_ROOT}")
    if(NOT (ADXE_ROOT STREQUAL ""))
        set(ADXE_ROOT_PATH "${ADXE_ROOT}")
        file(TO_CMAKE_PATH ${ADXE_ROOT_PATH} ADXE_ROOT_PATH) # string(REPLACE "\\" "/" ADXE_ROOT_PATH ${ADXE_ROOT_PATH})        
        message(STATUS "Using system env var ADXE_ROOT=${ADXE_ROOT}")
    else()
        set(ADXE_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../..)
    endif()
    set(CMAKE_MODULE_PATH ${ADXE_ROOT_PATH}/cmake/Modules/)

    include(CocosBuildSet)
    set(AX_ENABLE_EXT_LUA ON)
    add_subdirectory(${ADXE_ROOT_PATH}/core ${ENGINE_BINARY_PATH}/adxe/core)
endif()

set(GAME_HEADER
    Classes/AppDelegate.h
    Classes/lua_test_bindings.h
    Classes/lua_assetsmanager_test_sample.h
    )
set(GAME_SOURCE
    Classes/AppDelegate.cpp
    Classes/lua_assetsmanager_test_sample.cpp
    Classes/lua_test_bindings.cpp
    )

set(res_res_folders
    "${CMAKE_CURRENT_SOURCE_DIR}/../res"
    "${ADXE_ROOT_PATH}/tests/cpp-tests/Resources"
    )
set(res_src_folders
    "${CMAKE_CURRENT_SOURCE_DIR}/../src"
    )
set(res_script_folders
    "${ADXE_ROOT_PATH}/extensions/scripting/lua-bindings/script"
    )
if(APPLE OR VS)
    cocos_mark_multi_resources(res_res RES_TO "Resources/res" FOLDERS ${res_res_folders})
    cocos_mark_multi_resources(res_src RES_TO "Resources/src" FOLDERS ${res_src_folders})
    cocos_mark_multi_resources(res_script RES_TO "Resources/src/cocos" FOLDERS ${res_script_folders})
    set(cc_common_res ${res_res} ${res_src} ${res_script})
endif()

if(ANDROID)
    # change APP_NAME to the share library name for Android, it's value depend on AndroidManifest.xml
    set(APP_NAME lua_tests)
    list(APPEND GAME_SOURCE proj.android/app/jni/main.cpp)
elseif(LINUX)
    list(APPEND GAME_SOURCE proj.linux/main.cpp)
elseif(WINDOWS)
    list(APPEND GAME_HEADER
         proj.win32/main.h
         proj.win32/resource.h
         )
    list(APPEND GAME_SOURCE proj.win32/main.cpp ${cc_common_res})
elseif(APPLE)
    if(IOS)
        list(APPEND GAME_HEADER
             proj.ios_mac/ios/AppController.h
             proj.ios_mac/ios/LuaObjectCBridgeTest.h
             proj.ios_mac/ios/RootViewController.h
             )
        set(APP_UI_RES
            proj.ios_mac/ios/LaunchScreen.storyboard
            proj.ios_mac/ios/LaunchScreenBackground.png
            proj.ios_mac/ios/Images.xcassets
            )
        list(APPEND GAME_SOURCE
             proj.ios_mac/ios/main.m
             proj.ios_mac/ios/LuaObjectCBridgeTest.mm
             proj.ios_mac/ios/AppController.mm
             proj.ios_mac/ios/RootViewController.mm
             ${APP_UI_RES}
             )
    elseif(MACOSX)
        list(APPEND GAME_HEADER
             proj.ios_mac/mac/LuaObjectCBridgeTest.h
             )
        set(APP_UI_RES
            proj.ios_mac/mac/Icon.icns
            proj.ios_mac/mac/Info.plist
            )
        list(APPEND GAME_SOURCE
             proj.ios_mac/mac/main.cpp
             proj.ios_mac/mac/LuaObjectCBridgeTest.mm
             ${APP_UI_RES}
             )
    endif()
    list(APPEND GAME_SOURCE ${cc_common_res})
endif()

set(APP_SRC ${GAME_HEADER} ${GAME_SOURCE})

if(NOT ANDROID)
    add_executable(${APP_NAME} ${APP_SRC})
    target_link_libraries(${APP_NAME} ${ADXE_LUA_LIB})
else()
    add_library(${APP_NAME} SHARED ${APP_SRC})

    # whole-archive for jni
    add_subdirectory(${ADXE_ROOT_PATH}/core/platform/android ${ENGINE_BINARY_PATH}/core/cpp-android)
    target_link_libraries(${APP_NAME} -Wl,--whole-archive cpp_android_spec -Wl,--no-whole-archive)

    add_subdirectory(${ADXE_ROOT_PATH}/extensions/scripting/lua-bindings/proj.android ${ENGINE_BINARY_PATH}/core/lua-android)
    if(${CMAKE_ANDROID_ARCH_ABI} STREQUAL "x86")
        #add `-Wl,--no-warn-shared-textrel` to fix warning "shared library text segment is not shareable clang"
        target_link_libraries(${APP_NAME} -Wl,--whole-archive lua_android_spec -Wl,--no-whole-archive -Wl,--no-warn-shared-textrel)
    else()
        target_link_libraries(${APP_NAME} -Wl,--whole-archive lua_android_spec -Wl,--no-whole-archive)
    endif()

    config_android_shared_libs("org.cocos2dx.lib" "${CMAKE_CURRENT_SOURCE_DIR}/proj.android/app/src")
endif()

target_link_libraries(${APP_NAME} ${ADXE_LUA_LIB})

target_include_directories(${APP_NAME} PRIVATE Classes)

if(WIN64)
    target_link_options(${APP_NAME} PRIVATE "/STACK:4194304")
endif()

# mark app resources
setup_cocos_app_config(${APP_NAME})
if(APPLE)
    set_target_properties(${APP_NAME} PROPERTIES RESOURCE "${APP_UI_RES}")
    set_xcode_property(${APP_NAME} INSTALL_PATH "\$(LOCAL_APPS_DIR)")
    set_xcode_property(${APP_NAME} PRODUCT_BUNDLE_IDENTIFIER "org.adxe.lua-tests")

    if(MACOSX)
        set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios_mac/mac/Info.plist")
        set_target_properties(${APP_NAME} PROPERTIES
            LINK_FLAGS "-pagezero_size 10000 -image_base 100000000"
        )
    elseif(IOS)
        set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios_mac/ios/Info.plist")
        set_xcode_property(${APP_NAME} ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon-${APP_NAME}")
    endif()

    # For code-signing, set the DEVELOPMENT_TEAM:
    #set_xcode_property(${APP_NAME} DEVELOPMENT_TEAM "GRLXXXX2K9")
elseif(WINDOWS)
    cocos_copy_target_dll(${APP_NAME})
    cocos_copy_lua_dlls(${APP_NAME})
endif()

if((WINDOWS AND (CMAKE_GENERATOR STREQUAL "Ninja")) OR LINUX)
    cocos_get_resource_path(APP_RES_DIR ${APP_NAME})
    cocos_copy_target_res(${APP_NAME} LINK_TO ${APP_RES_DIR}/res FOLDERS ${res_res_folders})
    cocos_copy_lua_scripts(${APP_NAME} ${res_src_folders} ${APP_RES_DIR}/src )
    cocos_copy_lua_scripts(${APP_NAME} ${res_script_folders} ${APP_RES_DIR}/src/cocos)
elseif(WINDOWS)
    set(my_res_folder "${CMAKE_CURRENT_SOURCE_DIR}/..")
    cocos_copy_lua_scripts(${APP_NAME} ${res_script_folders} "${my_res_folder}/src/cocos")
    cocos_copy_target_res(${APP_NAME} LINK_TO "${my_res_folder}/res" FOLDERS "${ADXE_ROOT_PATH}/tests/cpp-tests/Resources")
    set_property(TARGET ${APP_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${my_res_folder}")
endif()
