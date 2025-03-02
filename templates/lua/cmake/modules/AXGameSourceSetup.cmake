if(APPLE)
    ax_mark_multi_resources(content_files RES_TO "Resources" FOLDERS ${content_folder})
    set(common_content_files ${content_files})
elseif(WINDOWS)
    ax_mark_multi_resources(content_files RES_TO "Content" FOLDERS ${content_folder})
    set(common_content_files ${content_files})
endif()

if(ANDROID)
    # the APP_NAME should match on AndroidManifest.xml
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
        list(APPEND GAME_HEADER
             proj.win32/main.h
             )
        list(APPEND GAME_SOURCE
             proj.win32/main.cpp
             )
    else()
        ax_setup_winrt_sources()
    endif()
    list(APPEND GAME_SOURCE ${common_content_files})
elseif(APPLE)
    if(IOS)
        list(APPEND GAME_HEADER
             proj.ios_mac/ios/AppController.h
             proj.ios_mac/ios/RootViewController.h
             )

        if (TVOS)
            set(APP_UI_RES
                proj.ios_mac/ios/targets/tvos/LaunchScreenBackground.png
                proj.ios_mac/ios/targets/tvos/LaunchScreen.storyboard
                proj.ios_mac/ios/targets/tvos/Images.xcassets
                )
        else()
            set(APP_UI_RES
                proj.ios_mac/ios/targets/ios/LaunchScreenBackground.png
                proj.ios_mac/ios/targets/ios/LaunchScreen.storyboard
                proj.ios_mac/ios/targets/ios/Images.xcassets
                )
        endif()

        list(APPEND GAME_SOURCE
             proj.ios_mac/ios/main.m
             proj.ios_mac/ios/AppController.mm
             proj.ios_mac/ios/RootViewController.mm
             proj.ios_mac/ios/Prefix.pch
             ${APP_UI_RES}
             )
    elseif(MACOSX)
        set(APP_UI_RES
            proj.ios_mac/mac/Icon.icns
            proj.ios_mac/mac/Info.plist
            )
        list(APPEND GAME_SOURCE
             proj.ios_mac/mac/main.cpp
             proj.ios_mac/mac/Prefix.pch
             ${APP_UI_RES}
             )
    endif()
    list(APPEND GAME_SOURCE ${common_content_files})
endif()

# mark app complie info and libs info
set(APP_SOURCES
    ${GAME_HEADER}
    ${GAME_SOURCE}
    )
