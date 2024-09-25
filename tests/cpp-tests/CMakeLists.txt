cmake_minimum_required(VERSION 3.20)

set(APP_NAME cpp-tests)


# "too large PDB" error often occurs in cpp-tests when using default "/Zi"
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
_1klink("${sample-assets_SOURCE_DIR}/cpp-tests/Content" "${CMAKE_CURRENT_LIST_DIR}/Content")

# record sources, headers, resources...
file(GLOB GAME_SOURCE Source/*.cpp)
file(GLOB GAME_HEADER Source/*.h)

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
    list(APPEND GAME_HEADER
         Source/JNITest/JNITest.h
         )
    list(APPEND GAME_SOURCE
         Source/JNITest/JNITest.cpp
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

list(APPEND GAME_HEADER
     Source/ActionsEaseTest/ActionsEaseTest.h
     Source/ParallaxTest/ParallaxTest.h
     Source/testBasic.h
     Source/CurlTest/CurlTest.h
     Source/ConfigurationTest/ConfigurationTest.h
     Source/CurrentLanguageTest/CurrentLanguageTest.h
     Source/AppDelegate.h
     Source/LabelTest/LabelTestNew.h
    #  Source/LabelTest/LabelTest.h
     Source/Particle3DTest/Particle3DTest.h
     Source/VibrateTest/VibrateTest.h
     Source/ClippingNodeTest/ClippingNodeTest.h
     Source/UserDefaultTest/UserDefaultTest.h
     Source/ImGuiTest/ImGuiTest.h
     Source/tests.h
     Source/DataVisitorTest/DataVisitorTest.h
     Source/NewAudioEngineTest/NewAudioEngineTest.h
     Source/BugsTest/Bug-422.h
     Source/BugsTest/Bug-DrawNode.h
     Source/BugsTest/BugsTest.h
     Source/BugsTest/Bug-12847.h
     Source/BugsTest/Bug-899.h
     Source/BugsTest/Bug-1159.h
     Source/BugsTest/Bug-350.h
     Source/BugsTest/Bug-15594.h
     Source/BugsTest/Bug-914.h
     Source/BugsTest/Bug-624.h
     Source/BugsTest/Bug-458/QuestionContainerSprite.h
     Source/BugsTest/Bug-458/Bug-458.h
     Source/BugsTest/Bug-886.h
     Source/BugsTest/Bug-Child.h
     Source/BugsTest/Bug-15776.h
     Source/BugsTest/Bug-1174.h
     Source/ActionsTest/ActionsTest.h
     Source/TouchesTest/TouchesTest.h
     Source/TouchesTest/Ball.h
     Source/TouchesTest/Paddle.h
     Source/ActionManagerTest/ActionManagerTest.h
     Source/ClickAndMoveTest/ClickAndMoveTest.h
     Source/MaterialSystemTest/MaterialSystemTest.h
     Source/IntervalTest/IntervalTest.h
     Source/TileMapTest/TileMapTest2.h
     # Source/TileMapTest/TileMapTest.h
     Source/NewEventDispatcherTest/NewEventDispatcherTest.h
     Source/ActionsProgressTest/ActionsProgressTest.h
     Source/RotateWorldTest/RotateWorldTest.h
     Source/Camera3DTest/Camera3DTest.h
     Source/TextInputTest/TextInputTest.h
     Source/Texture2dTest/Texture2dTest.h
     Source/TerrainTest/TerrainTest.h
     Source/controller.h
     Source/TransitionsTest/TransitionsTest.h
     Source/TextureCacheTest/TextureCacheTest.h
     Source/MotionStreakTest/MotionStreakTest.h
     Source/ExtensionsTest/AssetsManagerExTest/AssetsManagerExTest.h
     Source/ExtensionsTest/JSONDefaultTest/JSONDefaultTest.h
     Source/ExtensionsTest/ExtensionsTest.h
     Source/ExtensionsTest/TableViewTest/CustomTableViewCell.h
     Source/ExtensionsTest/TableViewTest/TableViewTestScene.h
     Source/MeshRendererTest/MeshRendererTest.h
     Source/MeshRendererTest/DrawNode3D.h
     Source/DrawNodeTest/DrawNodeTest.h
     Source/BaseTest.h
     Source/SceneTest/SceneTest.h
     Source/ReleasePoolTest/ReleasePoolTest.h
     Source/InputTest/MouseTest.h
     Source/SpineTest/SpineTest.h
    #  Source/Scene3DTest/Scene3DTest.h
     Source/ParticleTest/ParticleTest.h
     Source/EffectsTest/EffectsTest.h
     Source/UITest/UITest.h
     Source/UITest/CocoStudioGUITest/UIScrollViewTest/UIScrollViewTest.h
     Source/UITest/CocoStudioGUITest/CocosGUIScene.h
     Source/UITest/CocoStudioGUITest/UITabControlTest/UITabControlTest.h
     Source/UITest/CocoStudioGUITest/UITextFieldTest/UITextFieldTest.h
     Source/UITest/CocoStudioGUITest/UITextBMFontTest/UITextBMFontTest.h
     Source/UITest/CocoStudioGUITest/UIImageViewTest/UIImageViewTest.h
     Source/UITest/CocoStudioGUITest/UISliderTest/UISliderTest.h
     Source/UITest/CocoStudioGUITest/UILoadingBarTest/UILoadingBarTest.h
     Source/UITest/CocoStudioGUITest/UIEditBoxTest.h
     Source/UITest/CocoStudioGUITest/UIRichTextTest/UIRichTextTest.h
     Source/UITest/CocoStudioGUITest/UIScene.h
     Source/UITest/CocoStudioGUITest/UIPageViewTest/UIPageViewTest.h
     Source/UITest/CocoStudioGUITest/UIButtonTest/UIButtonTest.h
     Source/UITest/CocoStudioGUITest/UIScale9SpriteTest.h
     Source/UITest/CocoStudioGUITest/UIWidgetAddNodeTest/UIWidgetAddNodeTest.h
     Source/UITest/CocoStudioGUITest/UIListViewTest/UIListViewTest.h
     Source/UITest/CocoStudioGUITest/UISceneManager.h
     Source/UITest/CocoStudioGUITest/UIRadioButtonTest/UIRadioButtonTest.h
     Source/UITest/CocoStudioGUITest/UILayoutTest/UILayoutTest.h
     Source/UITest/CocoStudioGUITest/UITextAtlasTest/UITextAtlasTest.h
     Source/UITest/CocoStudioGUITest/UICheckBoxTest/UICheckBoxTest.h
     Source/UITest/CocoStudioGUITest/UIFocusTest/UIFocusTest.h
     Source/UITest/CocoStudioGUITest/UITextTest/UITextTest.h
     Source/BillBoardTest/BillBoardTest.h
     Source/SpriteFrameCacheTest/SpriteFrameCacheTest.h
     Source/EffectsAdvancedTest/EffectsAdvancedTest.h
     Source/SchedulerTest/SchedulerTest.h
     Source/MultiTouchTest/MultiTouchTest.h
     Source/testResource.h
     Source/ShaderTest/ShaderTest.h
     Source/ShaderTest/ShaderTest2.h
     Source/NewRendererTest/NewRendererTest.h
     Source/SpritePolygonTest/SpritePolygonTest.h
     Source/TexturePackerEncryptionTest/TextureAtlasEncryptionTest.h
     Source/NodeTest/NodeTest.h
     Source/RenderTextureTest/RenderTextureTest.h
     Source/LayerTest/LayerTest.h
     Source/SpriteTest/SpriteTest.h
     Source/FontTest/FontTest.h
     Source/LightTest/LightTest.h
     Source/VisibleRect.h
     Source/NetworkTest/NetworkTest.h
     Source/NetworkTest/DownloaderTest/DownloaderTest.h
     Source/NetworkTest/HttpClientTest/HttpClientTest.h
     Source/NetworkTest/WebSocketTest/WebSocketTest.h
     Source/OpenURLTest/OpenURLTest.h
     Source/ConsoleTest/ConsoleTest.h
     Source/MenuTest/MenuTest.h
     Source/ZipTest/ZipTests.h
     )

list(APPEND GAME_SOURCE
     Source/ActionManagerTest/ActionManagerTest.cpp
     Source/ActionsEaseTest/ActionsEaseTest.cpp
     Source/ActionsProgressTest/ActionsProgressTest.cpp
     Source/ActionsTest/ActionsTest.cpp
     Source/AppDelegate.cpp
     Source/BaseTest.cpp
     Source/BillBoardTest/BillBoardTest.cpp
     Source/BugsTest/Bug-DrawNode.cpp
     Source/BugsTest/Bug-1159.cpp
     Source/BugsTest/Bug-1174.cpp
     Source/BugsTest/Bug-350.cpp
     Source/BugsTest/Bug-422.cpp
     Source/BugsTest/Bug-458/Bug-458.cpp
     Source/BugsTest/Bug-458/QuestionContainerSprite.cpp
     Source/BugsTest/Bug-624.cpp
     Source/BugsTest/Bug-886.cpp
     Source/BugsTest/Bug-899.cpp
     Source/BugsTest/Bug-914.cpp
     Source/BugsTest/Bug-12847.cpp
     Source/BugsTest/Bug-15594.cpp
     Source/BugsTest/Bug-15776.cpp
     Source/BugsTest/Bug-Child.cpp
     Source/BugsTest/BugsTest.cpp
     Source/Camera3DTest/Camera3DTest.cpp
     Source/ClickAndMoveTest/ClickAndMoveTest.cpp
     Source/ClippingNodeTest/ClippingNodeTest.cpp
     Source/ConfigurationTest/ConfigurationTest.cpp
     Source/ConsoleTest/ConsoleTest.cpp
     Source/CurlTest/CurlTest.cpp
     Source/CurrentLanguageTest/CurrentLanguageTest.cpp
     Source/DataVisitorTest/DataVisitorTest.cpp
     Source/NetworkTest/NetworkTest.cpp
     Source/NetworkTest/DownloaderTest/DownloaderTest.cpp
     Source/NetworkTest/HttpClientTest/HttpClientTest.cpp
     Source/NetworkTest/WebSocketTest/WebSocketTest.cpp
     Source/EffectsAdvancedTest/EffectsAdvancedTest.cpp
     Source/EffectsTest/EffectsTest.cpp
     Source/ExtensionsTest/AssetsManagerExTest/AssetsManagerExTest.cpp
     Source/ExtensionsTest/JSONDefaultTest/JSONDefaultTest.cpp
     Source/ExtensionsTest/ExtensionsTest.cpp
     Source/ExtensionsTest/TableViewTest/CustomTableViewCell.cpp
     Source/ExtensionsTest/TableViewTest/TableViewTestScene.cpp
     Source/FontTest/FontTest.cpp
     Source/InputTest/MouseTest.cpp
     Source/IntervalTest/IntervalTest.cpp
    #  Source/LabelTest/LabelTest.cpp
     Source/LabelTest/LabelTestNew.cpp
     Source/LayerTest/LayerTest.cpp
     Source/LightTest/LightTest.cpp
     Source/MaterialSystemTest/MaterialSystemTest.cpp
     Source/MenuTest/MenuTest.cpp
     Source/MotionStreakTest/MotionStreakTest.cpp
     Source/MultiTouchTest/MultiTouchTest.cpp
     Source/NewAudioEngineTest/NewAudioEngineTest.cpp
     Source/NewEventDispatcherTest/NewEventDispatcherTest.cpp
     Source/NewRendererTest/NewRendererTest.cpp
     Source/NodeTest/NodeTest.cpp
     Source/OpenURLTest/OpenURLTest.cpp
     Source/ParallaxTest/ParallaxTest.cpp
     Source/Particle3DTest/Particle3DTest.cpp
     Source/ParticleTest/ParticleTest.cpp
     Source/ReleasePoolTest/ReleasePoolTest.cpp
     Source/RenderTextureTest/RenderTextureTest.cpp
     Source/RotateWorldTest/RotateWorldTest.cpp
     Source/SceneTest/SceneTest.cpp
     Source/SchedulerTest/SchedulerTest.cpp
     Source/ShaderTest/ShaderTest.cpp
     Source/ShaderTest/ShaderTest2.cpp
     Source/SpineTest/SpineTest.cpp
    #  Source/Scene3DTest/Scene3DTest.cpp
     Source/MeshRendererTest/DrawNode3D.cpp
     Source/DrawNodeTest/DrawNodeTest.cpp
     Source/MeshRendererTest/MeshRendererTest.cpp
     Source/SpritePolygonTest/SpritePolygonTest.cpp
     Source/SpriteTest/SpriteTest.cpp
     Source/TerrainTest/TerrainTest.cpp
     Source/TextInputTest/TextInputTest.cpp
     Source/Texture2dTest/Texture2dTest.cpp
     Source/TextureCacheTest/TextureCacheTest.cpp
     Source/TexturePackerEncryptionTest/TextureAtlasEncryptionTest.cpp
     #Source/TileMapTest/TileMapTest.cpp
     Source/TileMapTest/TileMapTest2.cpp
     Source/TouchesTest/Ball.cpp
     Source/TouchesTest/Paddle.cpp
     Source/TouchesTest/TouchesTest.cpp
     Source/TransitionsTest/TransitionsTest.cpp
     Source/UITest/CocoStudioGUITest/CocosGUIScene.cpp
     Source/UITest/CocoStudioGUITest/UIButtonTest/UIButtonTest.cpp
     Source/UITest/CocoStudioGUITest/UICheckBoxTest/UICheckBoxTest.cpp
     Source/UITest/CocoStudioGUITest/UIRadioButtonTest/UIRadioButtonTest.cpp
     Source/UITest/CocoStudioGUITest/UIFocusTest/UIFocusTest.cpp
     Source/UITest/CocoStudioGUITest/UIImageViewTest/UIImageViewTest.cpp
     Source/UITest/CocoStudioGUITest/UILayoutTest/UILayoutTest.cpp
     Source/UITest/CocoStudioGUITest/UIListViewTest/UIListViewTest.cpp
     Source/UITest/CocoStudioGUITest/UILoadingBarTest/UILoadingBarTest.cpp
     Source/UITest/CocoStudioGUITest/UIPageViewTest/UIPageViewTest.cpp
     Source/UITest/CocoStudioGUITest/UIRichTextTest/UIRichTextTest.cpp
     Source/UITest/CocoStudioGUITest/UIScale9SpriteTest.cpp
     Source/UITest/CocoStudioGUITest/UIScene.cpp
     Source/UITest/CocoStudioGUITest/UISceneManager.cpp
     Source/UITest/CocoStudioGUITest/UIScrollViewTest/UIScrollViewTest.cpp
     Source/UITest/CocoStudioGUITest/UISliderTest/UISliderTest.cpp
     Source/UITest/CocoStudioGUITest/UITextAtlasTest/UITextAtlasTest.cpp
     Source/UITest/CocoStudioGUITest/UITextBMFontTest/UITextBMFontTest.cpp
     Source/UITest/CocoStudioGUITest/UITextFieldTest/UITextFieldTest.cpp
     Source/UITest/CocoStudioGUITest/UITextTest/UITextTest.cpp
     Source/UITest/CocoStudioGUITest/UIWidgetAddNodeTest/UIWidgetAddNodeTest.cpp
     Source/UITest/CocoStudioGUITest/UITabControlTest/UITabControlTest.cpp
     Source/UITest/UITest.cpp
     Source/UserDefaultTest/UserDefaultTest.cpp
     Source/ImGuiTest/ImGuiTest.cpp
     Source/VisibleRect.cpp
     Source/VibrateTest/VibrateTest.cpp
     Source/SpriteFrameCacheTest/SpriteFrameCacheTest.cpp
     Source/controller.cpp
     Source/ZipTest/ZipTests.cpp
     )

if(AX_ENABLE_MEDIA)
    list(APPEND GAME_HEADER
             Source/UITest/CocoStudioGUITest/UIVideoPlayerTest/UIVideoPlayerTest.h)
    list(APPEND GAME_SOURCE
             Source/UITest/CocoStudioGUITest/UIVideoPlayerTest/UIVideoPlayerTest.cpp)
endif()

if(ANDROID OR IOS OR LINUX OR (WINDOWS AND AX_ENABLE_MSEDGE_WEBVIEW2))
    list(APPEND GAME_HEADER
         Source/UITest/CocoStudioGUITest/UIWebViewTest/UIWebViewTest.h
         )
    list(APPEND GAME_SOURCE
         Source/UITest/CocoStudioGUITest/UIWebViewTest/UIWebViewTest.cpp
         )
endif()

if(WINDOWS OR MACOSX OR LINUX)
    list(APPEND GAME_HEADER
         Source/WindowTest/WindowTest.h
         Source/BugsTest/Bug-14327.h
         )
    list(APPEND GAME_SOURCE
         Source/WindowTest/WindowTest.cpp
         Source/BugsTest/Bug-14327.cpp
         )
endif()

list(APPEND GAME_HEADER
   Source/Box2DTest/Box2dTest.h
   )

list(APPEND GAME_SOURCE
   Source/Box2DTest/Box2dTest.cpp
)

if((WINDOWS OR MACOSX OR LINUX OR WASM) AND (NOT WINRT))
    list(APPEND GAME_HEADER
        Source/Box2DTestBed/tests/test.h
        Source/Box2DTestBed/tests/settings.h
        Source/Box2DTestBed/Box2DTestBed.h
     )

   list(APPEND GAME_SOURCE
    Source/Box2DTestBed/Box2DTestBed.cpp
    Source/Box2DTestBed/test.cpp
    Source/Box2DTestBed/tests/add_pair.cpp
    Source/Box2DTestBed/tests/apply_force.cpp
    Source/Box2DTestBed/tests/body_types.cpp
    Source/Box2DTestBed/tests/box_stack.cpp
    Source/Box2DTestBed/tests/breakable.cpp
    Source/Box2DTestBed/tests/bridge.cpp
    Source/Box2DTestBed/tests/bullet_test.cpp
    Source/Box2DTestBed/tests/cantilever.cpp
    Source/Box2DTestBed/tests/car.cpp
    Source/Box2DTestBed/tests/chain.cpp
    Source/Box2DTestBed/tests/chain_problem.cpp
    Source/Box2DTestBed/tests/character_collision.cpp
    Source/Box2DTestBed/tests/circle_stack.cpp
    Source/Box2DTestBed/tests/collision_filtering.cpp
    Source/Box2DTestBed/tests/collision_processing.cpp
    Source/Box2DTestBed/tests/compound_shapes.cpp
    Source/Box2DTestBed/tests/confined.cpp
    Source/Box2DTestBed/tests/continuous_test.cpp
    Source/Box2DTestBed/tests/convex_hull.cpp
    Source/Box2DTestBed/tests/conveyor_belt.cpp
    Source/Box2DTestBed/tests/distance_joint.cpp
    Source/Box2DTestBed/tests/distance_test.cpp
    Source/Box2DTestBed/tests/dominos.cpp
    Source/Box2DTestBed/tests/dump_loader.cpp
    Source/Box2DTestBed/tests/dynamic_tree.cpp
    Source/Box2DTestBed/tests/edge_shapes.cpp
    Source/Box2DTestBed/tests/edge_test.cpp
    Source/Box2DTestBed/tests/friction.cpp
    Source/Box2DTestBed/tests/gear_joint.cpp
    Source/Box2DTestBed/tests/heavy1.cpp
    Source/Box2DTestBed/tests/heavy2.cpp
    Source/Box2DTestBed/tests/mobile_balanced.cpp
    Source/Box2DTestBed/tests/mobile_unbalanced.cpp
    Source/Box2DTestBed/tests/motor_joint.cpp
    Source/Box2DTestBed/tests/pinball.cpp
    Source/Box2DTestBed/tests/platformer.cpp
    Source/Box2DTestBed/tests/polygon_collision.cpp
    Source/Box2DTestBed/tests/polygon_shapes.cpp
    Source/Box2DTestBed/tests/prismatic_joint.cpp
    Source/Box2DTestBed/tests/pulley_joint.cpp
    Source/Box2DTestBed/tests/pyramid.cpp
    Source/Box2DTestBed/tests/ray_cast.cpp
    Source/Box2DTestBed/tests/restitution.cpp
    Source/Box2DTestBed/tests/revolute_joint.cpp
    Source/Box2DTestBed/tests/rope.cpp
    Source/Box2DTestBed/tests/sensor.cpp
    Source/Box2DTestBed/tests/shape_cast.cpp
    Source/Box2DTestBed/tests/shape_editing.cpp
    Source/Box2DTestBed/tests/skier.cpp
    Source/Box2DTestBed/tests/slider_crank_1.cpp
    Source/Box2DTestBed/tests/slider_crank_2.cpp
    Source/Box2DTestBed/tests/theo_jansen.cpp
    Source/Box2DTestBed/tests/tiles.cpp
    Source/Box2DTestBed/tests/time_of_impact.cpp
    Source/Box2DTestBed/tests/tumbler.cpp
    Source/Box2DTestBed/tests/web.cpp
    Source/Box2DTestBed/tests/wheel_joint.cpp
    Source/Box2DTestBed/tests/wrecking_ball.cpp
     )
endif()

list(APPEND GAME_HEADER
     Source/Physics3DTest/Physics3DTest.h
     Source/NavMeshTest/NavMeshTest.h
     )
list(APPEND GAME_SOURCE
     Source/Physics3DTest/Physics3DTest.cpp
     Source/NavMeshTest/NavMeshTest.cpp
     )
list(APPEND GAME_HEADER
     Source/PhysicsTest/PhysicsTest.h
     Source/ChipmunkTest/ChipmunkTest.h
	 )

list(APPEND GAME_SOURCE
     Source/PhysicsTest/PhysicsTest.cpp
     Source/ChipmunkTest/ChipmunkTest.cpp
	 )

if(WINDOWS OR MACOSX OR LINUX OR WASM)
	list(APPEND GAME_HEADER
	    Source/ChipmunkTestBed/demo/ChipmunkDemo.h
		Source/ChipmunkTestBed/ChipmunkTestBed.h
	)
   set (TESTBED_C_SORUCES
        Source/ChipmunkTestBed/demo/Bench.cpp
        Source/ChipmunkTestBed/demo/Chains.cpp
        Source/ChipmunkTestBed/demo/Convex.cpp
        Source/ChipmunkTestBed/demo/Crane.cpp
        Source/ChipmunkTestBed/demo/Joints.cpp
        Source/ChipmunkTestBed/demo/LogoSmash.cpp
        Source/ChipmunkTestBed/demo/OneWay.cpp
        Source/ChipmunkTestBed/demo/Planet.cpp
        Source/ChipmunkTestBed/demo/Player.cpp
        Source/ChipmunkTestBed/demo/Plink.cpp
        Source/ChipmunkTestBed/demo/Pump.cpp
        Source/ChipmunkTestBed/demo/PyramidStack.cpp
        Source/ChipmunkTestBed/demo/PyramidTopple.cpp
        Source/ChipmunkTestBed/demo/Shatter.cpp
        Source/ChipmunkTestBed/demo/Springies.cpp
        Source/ChipmunkTestBed/demo/Sticky.cpp
        Source/ChipmunkTestBed/demo/Tank.cpp
        Source/ChipmunkTestBed/demo/TheoJansen.cpp
        Source/ChipmunkTestBed/demo/Buoyancy.cpp
        Source/ChipmunkTestBed/demo/ContactGraph.cpp
        Source/ChipmunkTestBed/demo/Example.cpp
        Source/ChipmunkTestBed/demo/Query.cpp
        Source/ChipmunkTestBed/demo/Slice.cpp
        Source/ChipmunkTestBed/demo/Unicycle.cpp
        Source/ChipmunkTestBed/demo/Tumble.cpp)
	list(APPEND GAME_SOURCE
		${TESTBED_C_SORUCES}
		Source/ChipmunkTestBed/ChipmunkTestBed.cpp
	)
endif()

if (AX_ENABLE_EXT_EFFEKSEER)
    list(APPEND GAME_HEADER Source/EffekseerTest/EffekseerTest.h)
    list(APPEND GAME_SOURCE Source/EffekseerTest/EffekseerTest.cpp)
endif()

list(APPEND GAME_SOURCE
        Source/UITest/CocoStudioGUITest/UIEditBoxTest.cpp
        )

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

target_link_libraries(${APP_NAME} ${_AX_CORE_LIB})

target_include_directories(${APP_NAME} PRIVATE ${GAME_INC_DIRS})

if (AX_ENABLE_EXT_EFFEKSEER)
    target_compile_definitions(${APP_NAME} PRIVATE AX_ENABLE_EXT_EFFEKSEER=1)
endif()


# mark app resources
ax_setup_app_config(${APP_NAME})

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
