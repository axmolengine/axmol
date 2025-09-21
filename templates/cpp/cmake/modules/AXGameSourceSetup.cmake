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
      proj.win32/resource.h
    )
    list(APPEND GAME_SOURCE
      proj.win32/main.cpp
      proj.win32/game.rc
    )
  else()
    ax_setup_winrt_sources()
  endif()

  list(APPEND GAME_SOURCE ${common_content_files})
elseif(APPLE)
  if(IOS)
    list(APPEND GAME_HEADER
      proj.ios_mac/ios/GameAppController.h
      proj.ios_mac/ios/GameViewController.h
    )

    if(TVOS)
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
      proj.ios_mac/ios/main.mm
      proj.ios_mac/ios/GameAppController.mm
      proj.ios_mac/ios/GameViewController.mm
      proj.ios_mac/ios/Prefix.pch
      ${APP_UI_RES}
    )
  elseif(MACOSX)
    set(APP_UI_RES
      proj.ios_mac/mac/Icon.icns
    )
    list(APPEND GAME_SOURCE
      proj.ios_mac/mac/main.cpp
      proj.ios_mac/mac/Prefix.pch
      ${APP_UI_RES}
    )
  endif()

  list(APPEND GAME_SOURCE ${common_content_files})
endif()
