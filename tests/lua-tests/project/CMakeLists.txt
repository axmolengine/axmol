cmake_minimum_required(VERSION 3.10)

set(APP_NAME lua-tests)

project(${APP_NAME})

set(_AX_TOP_LEVEL_APP FALSE)
if(NOT DEFINED BUILD_ENGINE_DONE)
    set(_AX_TOP_LEVEL_APP TRUE)
    if(XCODE)
        set(CMAKE_XCODE_GENERATE_TOP_LEVEL_PROJECT_ONLY TRUE)
    endif()

    set(ADXE_ROOT "$ENV{ADXE_ROOT}")
    if(NOT (ADXE_ROOT STREQUAL ""))
        set(ADXE_ROOT_PATH "${ADXE_ROOT}")
        file(TO_CMAKE_PATH ${ADXE_ROOT_PATH} ADXE_ROOT_PATH) # string(REPLACE "\\" "/" ADXE_ROOT_PATH ${ADXE_ROOT_PATH})        
        message(STATUS "Using system env var ADXE_ROOT=${ADXE_ROOT}")
    else()
        message(FATAL_ERROR "Please run setup.py add system env var 'ADXE_ROOT' to specific the engine root")
    endif()
    set(CMAKE_MODULE_PATH ${ADXE_ROOT_PATH}/cmake/Modules/)

    include(CocosBuildSet)
    set(AX_ENABLE_EXT_LUA ON)

    set(_AX_USE_PREBUILT FALSE)
    if (WIN32 AND DEFINED AX_PREBUILT_DIR AND IS_DIRECTORY ${ADXE_ROOT_PATH}/${AX_PREBUILT_DIR})
        set(_AX_USE_PREBUILT TRUE)
    endif()

    if (NOT _AX_USE_PREBUILT)
        add_subdirectory(${ADXE_ROOT_PATH}/core ${ENGINE_BINARY_PATH}/adxe/core)
    endif()

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
    set(cc_common_res ${res_res} ${res_src})
    if (NOT _AX_TOP_LEVEL_APP)
        cocos_mark_multi_resources(res_script RES_TO "Resources/src/cocos" FOLDERS ${res_script_folders})
        set(cc_common_res ${cc_common_res} ${res_script})
    endif()
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

if (NOT _AX_USE_PREBUILT)
    target_link_libraries(${APP_NAME} ${ADXE_LUA_LIB})
endif()

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
    if(NOT _AX_USE_PREBUILT)
        cocos_copy_target_dll(${APP_NAME})
        cocos_copy_lua_dlls(${APP_NAME})
    endif()
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
    if(NOT DEFINED BUILD_ENGINE_DONE)
        set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${APP_NAME})
    endif()
endif()

# The optional thirdparties(not dependent by engine)
if (AX_WITH_YAML_CPP)
    target_include_directories(${APP_NAME} PRIVATE ${ADXE_ROOT_PATH}/thirdparty/yaml-cpp/include)
    target_link_libraries(${APP_NAME} yaml-cpp)
endif()

if (_AX_USE_PREBUILT) # support windows only

    option(AX_ENABLE_MSEDGE_WEBVIEW2 "Enable msedge webview2" TRUE)

    if(NOT ("${CMAKE_GENERATOR_PLATFORM}" STREQUAL "Win32"))
        set(WIN64 TRUE)
        set(ARCH_ALIAS "x64")
    else()
       set(WIN32 TRUE)
       set(ARCH_ALIAS "x86")
    endif()

    if (NOT AX_USE_SHARED_PREBUILT)
        target_compile_definitions(${APP_NAME}
            PRIVATE CC_STATIC=1
	    PRIVATE _USRLUASTATIC=1
        )
    endif()

    target_include_directories(${APP_NAME}
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/lua
        PRIVATE ${ADXE_ROOT_PATH}/extensions/scripting/lua-bindings/manual
        PRIVATE ${ADXE_ROOT_PATH}
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty
        PRIVATE ${ADXE_ROOT_PATH}/extensions
        PRIVATE ${ADXE_ROOT_PATH}/core
        PRIVATE ${ADXE_ROOT_PATH}/core/platform
        PRIVATE ${ADXE_ROOT_PATH}/core/base
        PRIVATE ${ADXE_ROOT_PATH}/core/audio
        PRIVATE ${ADXE_ROOT_PATH}/core/platform/win32
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/fmt/include
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/robin-map/include
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/freetype/include
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/glfw/include/GLFW
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/box2d/include
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/chipmunk/include
        PRIVATE ${ADXE_ROOT_PATH}/${AX_PREBUILT_DIR}/engine/thirdparty/freetype/include
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/recast/..
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/bullet/.
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/webp/src/webp
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/pugixml
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/xxhash/.
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/lz4/.
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/clipper/.
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/ConvertUTF/.
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/poly2tri/..
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/astc/..
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/openal/include
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/ogg/include
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/glad/include
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/glfw/include
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/png
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/unzip/.
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/llhttp/include
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/lua/plainlua
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/lua/tolua/.
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/lua/lua-cjson/.
        PRIVATE ${ADXE_ROOT_PATH}/extensions/cocostudio
        PRIVATE ${ADXE_ROOT_PATH}/extensions/spine/runtime/include
        PRIVATE ${ADXE_ROOT_PATH}/extensions/fairygui
        PRIVATE ${ADXE_ROOT_PATH}/extensions/GUI
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/zlib/include
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/jpeg/include
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/openssl/include
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/curl/include
    )
    
    target_link_directories(${APP_NAME}
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/openssl/prebuilt/windows/${ARCH_ALIAS}
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/zlib/prebuilt/windows/${ARCH_ALIAS}
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/jpeg/prebuilt/windows/${ARCH_ALIAS}
        PRIVATE ${ADXE_ROOT_PATH}/thirdparty/curl/prebuilt/windows/${ARCH_ALIAS}
        PRIVATE ${ADXE_ROOT_PATH}/${AX_PREBUILT_DIR}/lib/Debug
    )
    
    # Linking OS libs
    target_link_libraries(${APP_NAME} winmm Version)
    
    # Linking engine and thirdparty libs
    target_link_libraries(${APP_NAME}
        adxelua.lib
        spine.lib
        particle3d.lib
        assets-manager.lib
        cocostudio.lib
        DragonBones.lib
        lua-cjson.lib
        tolua.lib
        plainlua.lib
        adxe.lib
        box2d.lib
        chipmunk.lib
        freetype.lib
        zlib.lib
        recast.lib
        bullet.lib
        jpeg-static.lib
        libcrypto.lib
        libssl.lib
        webp.lib
        pugixml.lib
        xxhash.lib
        lz4.lib
        clipper.lib
        ConvertUTF.lib
        poly2tri.lib
        astc.lib
        libcurl_imp.lib
        OpenAL32.lib
        ogg.lib
        glad.lib
        glfw.lib
        png.lib
        unzip.lib
        llhttp.lib
        physics-nodes.lib
    )
    
    # Copy dlls to app bin dir
        # copy thirdparty dlls to target bin dir
    # copy_thirdparty_dlls(${APP_NAME} $<TARGET_FILE_DIR:${APP_NAME}>)
    set(ssl_dll_suffix "")
    if(WIN64)
        set(ssl_dll_suffix "-${ARCH_ALIAS}")
    endif()
    if(NOT CMAKE_GENERATOR STREQUAL "Ninja")
        set(THIRD_PARTY_ARCH "\$\(Configuration\)/")
    endif()
    add_custom_command(TARGET ${APP_NAME} POST_BUILD
       COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${ADXE_ROOT_PATH}/thirdparty/openssl/prebuilt/windows/${ARCH_ALIAS}/libcrypto-3${ssl_dll_suffix}.dll"
        "${ADXE_ROOT_PATH}/thirdparty/openssl/prebuilt/windows/${ARCH_ALIAS}/libssl-3${ssl_dll_suffix}.dll"
        "${ADXE_ROOT_PATH}/thirdparty/curl/prebuilt/windows/${ARCH_ALIAS}/libcurl.dll"
        "${ADXE_ROOT_PATH}/thirdparty/zlib/prebuilt/windows/${ARCH_ALIAS}/zlib1.dll"
        "${ADXE_ROOT_PATH}/${AX_PREBUILT_DIR}/bin/${THIRD_PARTY_ARCH}OpenAL32.dll"
        "${ADXE_ROOT_PATH}/${AX_PREBUILT_DIR}/bin/${THIRD_PARTY_ARCH}plainlua.dll"
         $<TARGET_FILE_DIR:${APP_NAME}>)

    # Copy windows angle binaries
    if (AX_USE_COMPAT_GL)
        add_custom_command(TARGET ${APP_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${ADXE_ROOT_PATH}/thirdparty/angle/prebuilt/${ARCH_ALIAS}/libGLESv2.dll
            ${ADXE_ROOT_PATH}/thirdparty/angle/prebuilt/${ARCH_ALIAS}/libEGL.dll
            ${ADXE_ROOT_PATH}/thirdparty/angle/prebuilt/${ARCH_ALIAS}/d3dcompiler_47.dll
            $<TARGET_FILE_DIR:${APP_NAME}>
        )
    endif()
    
    if (AX_ENABLE_MSEDGE_WEBVIEW2)
        if(CMAKE_GENERATOR STREQUAL "Ninja")
            target_link_libraries(${APP_NAME} ${ADXE_ROOT_PATH}/build/packages/Microsoft.Web.WebView2/build/native/${ARCH_ALIAS}/WebView2Loader.dll.lib)
            target_include_directories(${APP_NAME} ${ADXE_ROOT_PATH}/build/packages/Microsoft.Web.WebView2/build/native/include)
            add_custom_command(TARGET ${cocos_target} POST_BUILD
               COMMAND ${CMAKE_COMMAND} -E copy_if_different
               "${ADXE_ROOT_PATH}/build/packages/Microsoft.Web.WebView2/build/native/${ARCH_ALIAS}/WebView2Loader.dll"
                $<TARGET_FILE_DIR:${APP_NAME}>
            )
        else()
            target_link_libraries(${APP_NAME} ${ADXE_ROOT_PATH}/build/packages/Microsoft.Web.WebView2/build/native/Microsoft.Web.WebView2.targets)
        endif()
    endif()
endif()
