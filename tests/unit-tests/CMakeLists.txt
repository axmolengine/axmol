cmake_minimum_required(VERSION 3.20)

set(APP_NAME unit-tests)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(AX_EXT_HINT OFF CACHE BOOL "" FORCE)


# "too large PDB" error often occurs in unit-tests when using default "/Zi"
if (MSVC)
    string(REPLACE "/Zi" "/Z7" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
    string(REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    string(REPLACE "/Zi" "/Z7" CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")
    string(REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
endif()

project(${APP_NAME})

if(NOT DEFINED BUILD_ENGINE_DONE)
    if(XCODE)
        set(CMAKE_XCODE_GENERATE_TOP_LEVEL_PROJECT_ONLY TRUE)
    endif()

    set(_AX_ROOT "$ENV{AX_ROOT}")
    if(NOT (_AX_ROOT STREQUAL ""))
        file(TO_CMAKE_PATH ${_AX_ROOT} _AX_ROOT)
        message(STATUS "Using system env var _AX_ROOT=${_AX_ROOT}")
    else()
        set(_AX_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)
    endif()

    set(CMAKE_MODULE_PATH ${_AX_ROOT}/cmake/Modules/)

    include(AXBuildSet)
    add_subdirectory(${_AX_ROOT}/core ${ENGINE_BINARY_PATH}/axmol/core)
endif()

_1kfetch(sample-assets)
_1klink("${sample-assets_SOURCE_DIR}/unit-tests/Content" "${CMAKE_CURRENT_LIST_DIR}/Content")

set(GAME_SOURCE
    Source/AppDelegate.cpp
    Source/TestUtils.cpp

    Source/core/base/MapTests.cpp
    Source/core/base/UTF8Tests.cpp
    Source/core/base/UtilsTests.cpp
    Source/core/base/ValueTests.cpp
    Source/core/base/VectorTests.cpp

    Source/core/math/FastRNGTests.cpp
    Source/core/math/MathUtilTests.cpp

    Source/core/network/UriTests.cpp

    Source/core/platform/FileUtilsTests.cpp

    Source/core/ui/UIHelperTests.cpp
)


set(GAME_INC_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/Source"
)

set(content_folder
    "${CMAKE_CURRENT_SOURCE_DIR}/Content"
)
if(APPLE)
    ax_mark_multi_resources(common_content_files RES_TO "Resources" FOLDERS ${content_folder})
elseif(WINDOWS)
    ax_mark_multi_resources(common_content_files RES_TO "Content" FOLDERS ${content_folder})
endif()

if(ANDROID)
    list(APPEND GAME_SOURCE
        proj.android/app/jni/main.cpp
    )
elseif(LINUX)
    list(APPEND GAME_SOURCE
         proj.linux/main.cpp
         )
    list(APPEND GAME_SOURCE ${common_content_files})
elseif(WASM)
    list(APPEND GAME_SOURCE
         proj.wasm/main.cpp
         )
    list(APPEND GAME_SOURCE ${common_content_files})
elseif(WINDOWS)
    if(NOT WINRT)
        list(APPEND GAME_SOURCE
             proj.win32/main.cpp
             ${common_content_files}
             )
    else()
        ax_setup_winrt_sources()
    endif()
    list(APPEND GAME_SOURCE ${common_content_files})
elseif(APPLE)
    if(IOS)
        list(APPEND GAME_HEADER
             proj.ios/Source/testsAppDelegate.h
             proj.ios/Source/RootViewController.h
             )

        if (TVOS)
            set(APP_UI_RES
                proj.ios/LaunchScreenBackground.png
                proj.ios/targets/tvos/LaunchScreen.storyboard
                proj.ios/targets/tvos/Images.xcassets
                )
        else()
            set(APP_UI_RES
                proj.ios/LaunchScreenBackground.png
                proj.ios/targets/ios/LaunchScreen.storyboard
                proj.ios/targets/ios/Images.xcassets
                )
        endif()

        list(APPEND GAME_SOURCE
             proj.ios/main.m
             proj.ios/Source/testsAppDelegate.mm
             proj.ios/Source/RootViewController.mm
             proj.ios/Prefix.pch
             ${APP_UI_RES}
             )
    elseif(MACOSX)
        set(APP_UI_RES
            proj.mac/Icon.icns
            proj.mac/Info.plist
            proj.mac/Prefix.pch
            proj.mac/en.lproj/InfoPlist.strings
            )
        list(APPEND GAME_SOURCE
             proj.mac/main.cpp
             ${APP_UI_RES}
             )
    endif()
    list(APPEND GAME_SOURCE ${common_content_files})
endif()


# mark app complie info and libs info
set(all_code_files
    ${GAME_HEADER}
    ${GAME_SOURCE}
)

if(NOT ANDROID)
    add_executable(${APP_NAME} ${all_code_files})
else()
    add_library(${APP_NAME} SHARED ${all_code_files})

    # whole archive for jni
    target_link_libraries(${APP_NAME} -Wl,--whole-archive cpp_android_spec -Wl,--no-whole-archive)

    config_android_shared_libs("org.axmol.lib" "${CMAKE_CURRENT_SOURCE_DIR}/proj.android/app/src")
endif()

target_link_libraries(${APP_NAME} doctest)

target_link_libraries(${APP_NAME} ${_AX_CORE_LIB})

target_include_directories(${APP_NAME} PRIVATE ${GAME_INC_DIRS})

if (AX_ENABLE_EXT_EFFEKSEER)
    target_compile_definitions(${APP_NAME} PRIVATE AX_ENABLE_EXT_EFFEKSEER=1)
endif()

if (AX_ENABLE_EXT_DRAWNODEEX)
    target_compile_definitions(${APP_NAME} PRIVATE AX_ENABLE_EXT_DRAWNODE=1)
endif()

# mark app resources
ax_setup_app_config(${APP_NAME} CONSOLE)

if(APPLE)
    set_target_properties(${APP_NAME} PROPERTIES RESOURCE "${APP_UI_RES}")
    set_xcode_property(${APP_NAME} INSTALL_PATH "\$(LOCAL_APPS_DIR)")
    set_xcode_property(${APP_NAME} PRODUCT_BUNDLE_IDENTIFIER "org.axmol.${APP_NAME}")

    if(MACOSX)
        set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/proj.mac/Info.plist")
    elseif(TVOS)
        set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios/targets/tvos/Info.plist")
        set_xcode_property(${APP_NAME} ASSETCATALOG_COMPILER_APPICON_NAME "Brand Assets")
    elseif(IOS)
        set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios/targets/ios/Info.plist")
        set_xcode_property(${APP_NAME} ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon")
    endif()

    # For code-signing, set the DEVELOPMENT_TEAM:
    #set_xcode_property(${APP_NAME} DEVELOPMENT_TEAM "GRLXXXX2K9")
elseif(WINDOWS AND NOT WINRT)
    if(NOT _AX_USE_PREBUILT)
        ax_sync_target_dlls(${APP_NAME})
    endif()
endif()

if ((NOT APPLE) AND (NOT WINRT))
    ax_get_resource_path(APP_RES_DIR ${APP_NAME})
    ax_sync_target_res(${APP_NAME} LINK_TO ${APP_RES_DIR} FOLDERS ${content_folder} SYM_LINK 1)
    if(WINDOWS)
       set_property(TARGET ${APP_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${content_folder}")
    endif()
elseif(WINRT)
    if(NOT DEFINED BUILD_ENGINE_DONE)
        set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${APP_NAME})
    endif()
endif()

if((NOT IOS) AND (NOT WINRT))
    message("CMake ${APP_NAME} target_precompile_headers")
    target_precompile_headers(${APP_NAME} PRIVATE
      "$<$<COMPILE_LANGUAGE:CXX>:axmol.h>"
    )
endif()

ax_setup_app_props(${APP_NAME})
