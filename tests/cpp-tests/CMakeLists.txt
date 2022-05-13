cmake_minimum_required(VERSION 3.10)

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

    set(ADXE_ROOT "$ENV{ADXE_ROOT}")
    if(NOT (ADXE_ROOT STREQUAL ""))
        set(ADXE_ROOT_PATH "${ADXE_ROOT}")
        file(TO_CMAKE_PATH ${ADXE_ROOT_PATH} ADXE_ROOT_PATH) # string(REPLACE "\\" "/" ADXE_ROOT_PATH ${ADXE_ROOT_PATH})
        message(STATUS "Using system env var ADXE_ROOT=${ADXE_ROOT}")
    else()
        set(ADXE_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../..)
    endif()

    set(CMAKE_MODULE_PATH ${ADXE_ROOT_PATH}/cmake/Modules/)

    include(CocosBuildSet)
    add_subdirectory(${ADXE_ROOT_PATH}/core ${ENGINE_BINARY_PATH}/adxe/core)
endif()

# record sources, headers, resources...
set(GAME_SOURCE)
set(GAME_HEADER)

set(GAME_RES_FOLDER
    "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
    )
if(APPLE OR VS)
    cocos_mark_multi_resources(cc_common_res RES_TO "Resources" FOLDERS ${GAME_RES_FOLDER})
endif()

if(ANDROID)
    # change APP_NAME to the share library name for Android, it's value depend on AndroidManifest.xml
    set(APP_NAME cpp_tests)
    list(APPEND GAME_HEADER
         Classes/JNITest/JNITest.h
         )
    list(APPEND GAME_SOURCE
         Classes/JNITest/JNITest.cpp
         proj.android/app/jni/main.cpp
         )
elseif(LINUX)
    list(APPEND GAME_SOURCE
         proj.linux/main.cpp
         )
elseif(WINDOWS)
    list(APPEND GAME_HEADER
         proj.win32/main.h
         )
    list(APPEND GAME_SOURCE
         proj.win32/main.cpp
         ${cc_common_res}
         )
elseif(APPLE)
    if(IOS)
        list(APPEND GAME_HEADER
             proj.ios/Classes/testsAppDelegate.h
             proj.ios/Classes/RootViewController.h
             )
        set(APP_UI_RES
            proj.ios/LaunchScreen.storyboard
            proj.ios/LaunchScreenBackground.png
            proj.ios/Images.xcassets
            )
        list(APPEND GAME_SOURCE
             proj.ios/main.m
             proj.ios/Classes/testsAppDelegate.mm
             proj.ios/Classes/RootViewController.mm
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
    list(APPEND GAME_SOURCE ${cc_common_res})
endif()

list(APPEND GAME_HEADER
     Classes/ActionsEaseTest/ActionsEaseTest.h
     Classes/ParallaxTest/ParallaxTest.h
     Classes/testBasic.h
     Classes/ZwoptexTest/ZwoptexTest.h
     Classes/precheader.h
     Classes/CurlTest/CurlTest.h
     Classes/UnitTest/UnitTest.h
#    Classes/UnitTest/RefPtrTest.h
     Classes/ConfigurationTest/ConfigurationTest.h
     Classes/CurrentLanguageTest/CurrentLanguageTest.h
     Classes/AppDelegate.h
     Classes/LabelTest/LabelTestNew.h
    #  Classes/LabelTest/LabelTest.h
     Classes/Particle3DTest/Particle3DTest.h
     Classes/VibrateTest/VibrateTest.h
     Classes/ClippingNodeTest/ClippingNodeTest.h
     Classes/UserDefaultTest/UserDefaultTest.h
     Classes/ImGuiTest/ImGuiTest.h
     Classes/tests.h
     Classes/DataVisitorTest/DataVisitorTest.h
     Classes/NewAudioEngineTest/NewAudioEngineTest.h
     Classes/BugsTest/Bug-422.h
     Classes/BugsTest/Bug-CCDrawNode.h
     Classes/BugsTest/BugsTest.h
     Classes/BugsTest/Bug-12847.h
     Classes/BugsTest/Bug-899.h
     Classes/BugsTest/Bug-1159.h
     Classes/BugsTest/Bug-350.h
     Classes/BugsTest/Bug-15594.h
     Classes/BugsTest/Bug-914.h
     Classes/BugsTest/Bug-624.h
     Classes/BugsTest/Bug-458/QuestionContainerSprite.h
     Classes/BugsTest/Bug-458/Bug-458.h
     Classes/BugsTest/Bug-886.h
     Classes/BugsTest/Bug-Child.h
     Classes/BugsTest/Bug-15776.h
     Classes/BugsTest/Bug-1174.h
     Classes/ActionsTest/ActionsTest.h
     Classes/TouchesTest/TouchesTest.h
     Classes/TouchesTest/Ball.h
     Classes/TouchesTest/Paddle.h
     Classes/ActionManagerTest/ActionManagerTest.h
     Classes/ClickAndMoveTest/ClickAndMoveTest.h
     Classes/MaterialSystemTest/MaterialSystemTest.h
     Classes/IntervalTest/IntervalTest.h
     Classes/TileMapTest/TileMapTest2.h
     # Classes/TileMapTest/TileMapTest.h
     Classes/NewEventDispatcherTest/NewEventDispatcherTest.h
     Classes/ActionsProgressTest/ActionsProgressTest.h
     Classes/RotateWorldTest/RotateWorldTest.h
     Classes/Camera3DTest/Camera3DTest.h
     Classes/TextInputTest/TextInputTest.h
     Classes/Texture2dTest/Texture2dTest.h
     Classes/TerrainTest/TerrainTest.h
     Classes/controller.h
     Classes/TransitionsTest/TransitionsTest.h
     Classes/TextureCacheTest/TextureCacheTest.h
     Classes/MotionStreakTest/MotionStreakTest.h
     Classes/ExtensionsTest/AssetsManagerExTest/AssetsManagerExTest.h
     Classes/ExtensionsTest/ExtensionsTest.h
     Classes/ExtensionsTest/TableViewTest/CustomTableViewCell.h
     Classes/ExtensionsTest/TableViewTest/TableViewTestScene.h
     Classes/Sprite3DTest/Sprite3DTest.h
     Classes/Sprite3DTest/DrawNode3D.h
     Classes/BaseTest.h
     Classes/SceneTest/SceneTest.h
     Classes/ReleasePoolTest/ReleasePoolTest.h
     Classes/InputTest/MouseTest.h
     Classes/SpineTest/SpineTest.h
    #  Classes/Scene3DTest/Scene3DTest.h
     Classes/ParticleTest/ParticleTest.h
     Classes/EffectsTest/EffectsTest.h
     Classes/UITest/UITest.h
     Classes/UITest/CocoStudioGUITest/UIScrollViewTest/UIScrollViewTest.h
     Classes/UITest/CocoStudioGUITest/CocosGUIScene.h
     Classes/UITest/CocoStudioGUITest/UITabControlTest/UITabControlTest.h
     Classes/UITest/CocoStudioGUITest/UITextFieldTest/UITextFieldTest.h
     Classes/UITest/CocoStudioGUITest/UITextBMFontTest/UITextBMFontTest.h
     Classes/UITest/CocoStudioGUITest/UIImageViewTest/UIImageViewTest.h
     Classes/UITest/CocoStudioGUITest/UISliderTest/UISliderTest.h
     Classes/UITest/CocoStudioGUITest/UILoadingBarTest/UILoadingBarTest.h
     Classes/UITest/CocoStudioGUITest/UIEditBoxTest.h
     Classes/UITest/CocoStudioGUITest/UIRichTextTest/UIRichTextTest.h
     Classes/UITest/CocoStudioGUITest/UIScene.h
     Classes/UITest/CocoStudioGUITest/UIPageViewTest/UIPageViewTest.h
     Classes/UITest/CocoStudioGUITest/UIButtonTest/UIButtonTest.h
     Classes/UITest/CocoStudioGUITest/UIScale9SpriteTest.h
     Classes/UITest/CocoStudioGUITest/UIWidgetAddNodeTest/UIWidgetAddNodeTest.h
     Classes/UITest/CocoStudioGUITest/UIListViewTest/UIListViewTest.h
     Classes/UITest/CocoStudioGUITest/UISceneManager.h
     Classes/UITest/CocoStudioGUITest/UIRadioButtonTest/UIRadioButtonTest.h
     Classes/UITest/CocoStudioGUITest/UILayoutTest/UILayoutTest.h
     Classes/UITest/CocoStudioGUITest/UITextAtlasTest/UITextAtlasTest.h
     Classes/UITest/CocoStudioGUITest/UICheckBoxTest/UICheckBoxTest.h
     Classes/UITest/CocoStudioGUITest/UIFocusTest/UIFocusTest.h
     Classes/UITest/CocoStudioGUITest/UITextTest/UITextTest.h
     Classes/DrawPrimitivesTest/DrawPrimitivesTest.h
     Classes/BillBoardTest/BillBoardTest.h
     Classes/SpriteFrameCacheTest/SpriteFrameCacheTest.h
     Classes/EffectsAdvancedTest/EffectsAdvancedTest.h
     Classes/SchedulerTest/SchedulerTest.h
     Classes/MultiTouchTest/MultiTouchTest.h
     Classes/testResource.h
     Classes/ShaderTest/ShaderTest.vsh.h
     Classes/ShaderTest/shaderTest.psh.h
     Classes/ShaderTest/ShaderTest.h
     Classes/ShaderTest/ShaderTest2.h
     Classes/NewRendererTest/NewRendererTest.h
     Classes/SpritePolygonTest/SpritePolygonTest.h
     Classes/TexturePackerEncryptionTest/TextureAtlasEncryptionTest.h
     Classes/NodeTest/NodeTest.h
     Classes/FileUtilsTest/FileUtilsTest.h
     Classes/RenderTextureTest/RenderTextureTest.h
     Classes/LayerTest/LayerTest.h
     Classes/SpriteTest/SpriteTest.h
     Classes/FontTest/FontTest.h
     Classes/LightTest/LightTest.h
     Classes/VisibleRect.h
     Classes/NetworkTest/NetworkTest.h
     Classes/NetworkTest/DownloaderTest/DownloaderTest.h
     Classes/NetworkTest/HttpClientTest/HttpClientTest.h
     Classes/OpenURLTest/OpenURLTest.h
     Classes/ConsoleTest/ConsoleTest.h
     Classes/MenuTest/MenuTest.h
     Classes/ZipTest/ZipTests.h
     )

list(APPEND GAME_SOURCE
     Classes/ActionManagerTest/ActionManagerTest.cpp
     Classes/ActionsEaseTest/ActionsEaseTest.cpp
     Classes/ActionsProgressTest/ActionsProgressTest.cpp
     Classes/ActionsTest/ActionsTest.cpp
     Classes/AppDelegate.cpp
     Classes/BaseTest.cpp
     Classes/BillBoardTest/BillBoardTest.cpp
     Classes/BugsTest/Bug-CCDrawNode.cpp
     Classes/BugsTest/Bug-1159.cpp
     Classes/BugsTest/Bug-1174.cpp
     Classes/BugsTest/Bug-350.cpp
     Classes/BugsTest/Bug-422.cpp
     Classes/BugsTest/Bug-458/Bug-458.cpp
     Classes/BugsTest/Bug-458/QuestionContainerSprite.cpp
     Classes/BugsTest/Bug-624.cpp
     Classes/BugsTest/Bug-886.cpp
     Classes/BugsTest/Bug-899.cpp
     Classes/BugsTest/Bug-914.cpp
     Classes/BugsTest/Bug-12847.cpp
     Classes/BugsTest/Bug-15594.cpp
     Classes/BugsTest/Bug-15776.cpp
     Classes/BugsTest/Bug-Child.cpp
     Classes/BugsTest/BugsTest.cpp
     Classes/Camera3DTest/Camera3DTest.cpp
     Classes/ClickAndMoveTest/ClickAndMoveTest.cpp
     Classes/ClippingNodeTest/ClippingNodeTest.cpp
     Classes/ConfigurationTest/ConfigurationTest.cpp
     Classes/ConsoleTest/ConsoleTest.cpp
     Classes/CurlTest/CurlTest.cpp
     Classes/CurrentLanguageTest/CurrentLanguageTest.cpp
     Classes/DataVisitorTest/DataVisitorTest.cpp
     Classes/NetworkTest/NetworkTest.cpp
     Classes/NetworkTest/DownloaderTest/DownloaderTest.cpp
     Classes/NetworkTest/HttpClientTest/HttpClientTest.cpp
     Classes/DrawPrimitivesTest/DrawPrimitivesTest.cpp
     Classes/EffectsAdvancedTest/EffectsAdvancedTest.cpp
     Classes/EffectsTest/EffectsTest.cpp
     Classes/ExtensionsTest/AssetsManagerExTest/AssetsManagerExTest.cpp
     Classes/ExtensionsTest/ExtensionsTest.cpp
     Classes/ExtensionsTest/TableViewTest/CustomTableViewCell.cpp
     Classes/ExtensionsTest/TableViewTest/TableViewTestScene.cpp
     Classes/FileUtilsTest/FileUtilsTest.cpp
     Classes/FontTest/FontTest.cpp
     Classes/InputTest/MouseTest.cpp
     Classes/IntervalTest/IntervalTest.cpp
    #  Classes/LabelTest/LabelTest.cpp
     Classes/LabelTest/LabelTestNew.cpp
     Classes/LayerTest/LayerTest.cpp
     Classes/LightTest/LightTest.cpp
     Classes/MaterialSystemTest/MaterialSystemTest.cpp
     Classes/MenuTest/MenuTest.cpp
     Classes/MotionStreakTest/MotionStreakTest.cpp
     Classes/MultiTouchTest/MultiTouchTest.cpp
     Classes/NewAudioEngineTest/NewAudioEngineTest.cpp
     Classes/NewEventDispatcherTest/NewEventDispatcherTest.cpp
     Classes/NewRendererTest/NewRendererTest.cpp
     Classes/NodeTest/NodeTest.cpp
     Classes/OpenURLTest/OpenURLTest.cpp
     Classes/ParallaxTest/ParallaxTest.cpp
     Classes/Particle3DTest/Particle3DTest.cpp
     Classes/ParticleTest/ParticleTest.cpp
     Classes/ReleasePoolTest/ReleasePoolTest.cpp
     Classes/RenderTextureTest/RenderTextureTest.cpp
     Classes/RotateWorldTest/RotateWorldTest.cpp
     Classes/SceneTest/SceneTest.cpp
     Classes/SchedulerTest/SchedulerTest.cpp
     Classes/ShaderTest/ShaderTest.cpp
     Classes/ShaderTest/ShaderTest2.cpp
     Classes/SpineTest/SpineTest.cpp
    #  Classes/Scene3DTest/Scene3DTest.cpp
     Classes/Sprite3DTest/DrawNode3D.cpp
     Classes/Sprite3DTest/Sprite3DTest.cpp
     Classes/SpritePolygonTest/SpritePolygonTest.cpp
     Classes/SpriteTest/SpriteTest.cpp
     Classes/TerrainTest/TerrainTest.cpp
     Classes/TextInputTest/TextInputTest.cpp
     Classes/Texture2dTest/Texture2dTest.cpp
     Classes/TextureCacheTest/TextureCacheTest.cpp
     Classes/TexturePackerEncryptionTest/TextureAtlasEncryptionTest.cpp
     #Classes/TileMapTest/TileMapTest.cpp
     Classes/TileMapTest/TileMapTest2.cpp
     Classes/TouchesTest/Ball.cpp
     Classes/TouchesTest/Paddle.cpp
     Classes/TouchesTest/TouchesTest.cpp
     Classes/TransitionsTest/TransitionsTest.cpp
     Classes/UITest/CocoStudioGUITest/CocosGUIScene.cpp
     Classes/UITest/CocoStudioGUITest/UIButtonTest/UIButtonTest.cpp
     Classes/UITest/CocoStudioGUITest/UICheckBoxTest/UICheckBoxTest.cpp
     Classes/UITest/CocoStudioGUITest/UIRadioButtonTest/UIRadioButtonTest.cpp
     Classes/UITest/CocoStudioGUITest/UIFocusTest/UIFocusTest.cpp
     Classes/UITest/CocoStudioGUITest/UIImageViewTest/UIImageViewTest.cpp
     Classes/UITest/CocoStudioGUITest/UILayoutTest/UILayoutTest.cpp
     Classes/UITest/CocoStudioGUITest/UIListViewTest/UIListViewTest.cpp
     Classes/UITest/CocoStudioGUITest/UILoadingBarTest/UILoadingBarTest.cpp
     Classes/UITest/CocoStudioGUITest/UIPageViewTest/UIPageViewTest.cpp
     Classes/UITest/CocoStudioGUITest/UIRichTextTest/UIRichTextTest.cpp
     Classes/UITest/CocoStudioGUITest/UIScale9SpriteTest.cpp
     Classes/UITest/CocoStudioGUITest/UIScene.cpp
     Classes/UITest/CocoStudioGUITest/UISceneManager.cpp
     Classes/UITest/CocoStudioGUITest/UIScrollViewTest/UIScrollViewTest.cpp
     Classes/UITest/CocoStudioGUITest/UISliderTest/UISliderTest.cpp
     Classes/UITest/CocoStudioGUITest/UITextAtlasTest/UITextAtlasTest.cpp
     Classes/UITest/CocoStudioGUITest/UITextBMFontTest/UITextBMFontTest.cpp
     Classes/UITest/CocoStudioGUITest/UITextFieldTest/UITextFieldTest.cpp
     Classes/UITest/CocoStudioGUITest/UITextTest/UITextTest.cpp
     Classes/UITest/CocoStudioGUITest/UIWidgetAddNodeTest/UIWidgetAddNodeTest.cpp
     Classes/UITest/CocoStudioGUITest/UITabControlTest/UITabControlTest.cpp
     Classes/UITest/UITest.cpp
#     Classes/UnitTest/RefPtrTest.cpp
     Classes/UnitTest/UnitTest.cpp
     Classes/UserDefaultTest/UserDefaultTest.cpp
     Classes/ImGuiTest/ImGuiTest.cpp
     Classes/VisibleRect.cpp
     Classes/VibrateTest/VibrateTest.cpp
     Classes/ZwoptexTest/ZwoptexTest.cpp
     Classes/SpriteFrameCacheTest/SpriteFrameCacheTest.cpp
     Classes/controller.cpp
     Classes/ZipTest/ZipTests.cpp
     )

if(ANDROID OR IOS OR WIN32)
    list(APPEND GAME_HEADER
             Classes/UITest/CocoStudioGUITest/UIVideoPlayerTest/UIVideoPlayerTest.h)
    list(APPEND GAME_SOURCE
             Classes/UITest/CocoStudioGUITest/UIVideoPlayerTest/UIVideoPlayerTest.cpp)
endif()

if(ANDROID OR IOS)
    list(APPEND GAME_HEADER
         Classes/UITest/CocoStudioGUITest/UIWebViewTest/UIWebViewTest.h
         )
    list(APPEND GAME_SOURCE
         Classes/UITest/CocoStudioGUITest/UIWebViewTest/UIWebViewTest.cpp
         )
endif()

if(WINDOWS OR MACOSX OR LINUX)
    list(APPEND GAME_HEADER
         Classes/WindowTest/WindowTest.h
         Classes/BugsTest/Bug-14327.h
         )
    list(APPEND GAME_SOURCE
         Classes/WindowTest/WindowTest.cpp
         Classes/BugsTest/Bug-14327.cpp
         )
endif()

if(WINDOWS)
    list(APPEND GAME_HEADER
         Classes/UITest/CocoStudioGUITest/UIWebViewTest/UIWebViewTest.h
         )
    list(APPEND GAME_SOURCE
         Classes/UITest/CocoStudioGUITest/UIWebViewTest/UIWebViewTest.cpp
         )
endif()

list(APPEND GAME_HEADER
   Classes/Box2DTest/Box2dTest.h
   )

list(APPEND GAME_SOURCE
   Classes/Box2DTest/Box2dTest.cpp
)

if(WINDOWS OR MACOSX OR LINUX)
    list(APPEND GAME_HEADER
        Classes/Box2DTestBed/tests/test.h
        Classes/Box2DTestBed/tests/settings.h
        Classes/Box2DTestBed/Box2DTestBed.h
     )

   list(APPEND GAME_SOURCE
    Classes/Box2DTestBed/Box2DTestBed.cpp
    Classes/Box2DTestBed/test.cpp
    Classes/Box2DTestBed/tests/add_pair.cpp
    Classes/Box2DTestBed/tests/apply_force.cpp
    Classes/Box2DTestBed/tests/body_types.cpp
    Classes/Box2DTestBed/tests/box_stack.cpp
    Classes/Box2DTestBed/tests/breakable.cpp
    Classes/Box2DTestBed/tests/bridge.cpp
    Classes/Box2DTestBed/tests/bullet_test.cpp
    Classes/Box2DTestBed/tests/cantilever.cpp
    Classes/Box2DTestBed/tests/car.cpp
    Classes/Box2DTestBed/tests/chain.cpp
    Classes/Box2DTestBed/tests/chain_problem.cpp
    Classes/Box2DTestBed/tests/character_collision.cpp
    Classes/Box2DTestBed/tests/circle_stack.cpp
    Classes/Box2DTestBed/tests/collision_filtering.cpp
    Classes/Box2DTestBed/tests/collision_processing.cpp
    Classes/Box2DTestBed/tests/compound_shapes.cpp
    Classes/Box2DTestBed/tests/confined.cpp
    Classes/Box2DTestBed/tests/continuous_test.cpp
    Classes/Box2DTestBed/tests/convex_hull.cpp
    Classes/Box2DTestBed/tests/conveyor_belt.cpp
    Classes/Box2DTestBed/tests/distance_joint.cpp
    Classes/Box2DTestBed/tests/distance_test.cpp
    Classes/Box2DTestBed/tests/dominos.cpp
    Classes/Box2DTestBed/tests/dump_loader.cpp
    Classes/Box2DTestBed/tests/dynamic_tree.cpp
    Classes/Box2DTestBed/tests/edge_shapes.cpp
    Classes/Box2DTestBed/tests/edge_test.cpp
    Classes/Box2DTestBed/tests/friction.cpp
    Classes/Box2DTestBed/tests/gear_joint.cpp
    Classes/Box2DTestBed/tests/heavy1.cpp
    Classes/Box2DTestBed/tests/heavy2.cpp
    Classes/Box2DTestBed/tests/mobile_balanced.cpp
    Classes/Box2DTestBed/tests/mobile_unbalanced.cpp
    Classes/Box2DTestBed/tests/motor_joint.cpp
    Classes/Box2DTestBed/tests/pinball.cpp
    Classes/Box2DTestBed/tests/platformer.cpp
    Classes/Box2DTestBed/tests/polygon_collision.cpp
    Classes/Box2DTestBed/tests/polygon_shapes.cpp
    Classes/Box2DTestBed/tests/prismatic_joint.cpp
    Classes/Box2DTestBed/tests/pulley_joint.cpp
    Classes/Box2DTestBed/tests/pyramid.cpp
    Classes/Box2DTestBed/tests/ray_cast.cpp
    Classes/Box2DTestBed/tests/restitution.cpp
    Classes/Box2DTestBed/tests/revolute_joint.cpp
    Classes/Box2DTestBed/tests/rope.cpp
    Classes/Box2DTestBed/tests/sensor.cpp
    Classes/Box2DTestBed/tests/shape_cast.cpp
    Classes/Box2DTestBed/tests/shape_editing.cpp
    Classes/Box2DTestBed/tests/skier.cpp
    Classes/Box2DTestBed/tests/slider_crank_1.cpp
    Classes/Box2DTestBed/tests/slider_crank_2.cpp
    Classes/Box2DTestBed/tests/theo_jansen.cpp
    Classes/Box2DTestBed/tests/tiles.cpp
    Classes/Box2DTestBed/tests/time_of_impact.cpp
    Classes/Box2DTestBed/tests/tumbler.cpp
    Classes/Box2DTestBed/tests/web.cpp
    Classes/Box2DTestBed/tests/wheel_joint.cpp
    Classes/Box2DTestBed/tests/wrecking_ball.cpp
     )
endif()

list(APPEND GAME_HEADER
     Classes/Physics3DTest/Physics3DTest.h
     Classes/NavMeshTest/NavMeshTest.h
     )
list(APPEND GAME_SOURCE
     Classes/Physics3DTest/Physics3DTest.cpp
     Classes/NavMeshTest/NavMeshTest.cpp
     )
list(APPEND GAME_HEADER
     Classes/PhysicsTest/PhysicsTest.h
     Classes/ChipmunkTest/ChipmunkTest.h
	 )

list(APPEND GAME_SOURCE
     Classes/PhysicsTest/PhysicsTest.cpp
     Classes/ChipmunkTest/ChipmunkTest.cpp
	 )

if(WINDOWS OR MACOSX OR LINUX)
	list(APPEND GAME_HEADER
	    Classes/ChipmunkTestBed/demo/ChipmunkDemo.h
		Classes/ChipmunkTestBed/ChipmunkTestBed.h
	)
   set (TESTBED_C_SORUCES
        Classes/ChipmunkTestBed/demo/Bench.c
        Classes/ChipmunkTestBed/demo/Chains.c
        Classes/ChipmunkTestBed/demo/Convex.c
        Classes/ChipmunkTestBed/demo/Crane.c
        Classes/ChipmunkTestBed/demo/Joints.c
        Classes/ChipmunkTestBed/demo/LogoSmash.c
        Classes/ChipmunkTestBed/demo/OneWay.c
        Classes/ChipmunkTestBed/demo/Planet.c
        Classes/ChipmunkTestBed/demo/Player.c
        Classes/ChipmunkTestBed/demo/Plink.c
        Classes/ChipmunkTestBed/demo/Pump.c
        Classes/ChipmunkTestBed/demo/PyramidStack.c
        Classes/ChipmunkTestBed/demo/PyramidTopple.c
        Classes/ChipmunkTestBed/demo/Shatter.c
        Classes/ChipmunkTestBed/demo/Springies.c
        Classes/ChipmunkTestBed/demo/Sticky.c
        Classes/ChipmunkTestBed/demo/Tank.c
        Classes/ChipmunkTestBed/demo/TheoJansen.c
        Classes/ChipmunkTestBed/demo/Buoyancy.c
        Classes/ChipmunkTestBed/demo/ContactGraph.c
        Classes/ChipmunkTestBed/demo/Example.c
        Classes/ChipmunkTestBed/demo/Query.c
        Classes/ChipmunkTestBed/demo/Slice.c
        Classes/ChipmunkTestBed/demo/Unicycle.c
        Classes/ChipmunkTestBed/demo/Tumble.c)
    if(WINDOWS)
        set_source_files_properties(${TESTBED_C_SORUCES} PROPERTIES COMPILE_FLAGS "/TP")
    else()
        set_source_files_properties(${TESTBED_C_SORUCES} PROPERTIES LANGUAGE CXX)
    endif()
	list(APPEND GAME_SOURCE
		${TESTBED_C_SORUCES}
		Classes/ChipmunkTestBed/ChipmunkTestBed.cpp
	)
endif()


if(NOT LINUX)
    list(APPEND GAME_SOURCE
         Classes/UITest/CocoStudioGUITest/UIEditBoxTest.cpp
         )
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
    add_subdirectory(${ADXE_ROOT_PATH}/core/platform/android ${ENGINE_BINARY_PATH}/core/platform)
    target_link_libraries(${APP_NAME} -Wl,--whole-archive cpp_android_spec -Wl,--no-whole-archive)

    config_android_shared_libs("org.cocos2dx.lib" "${CMAKE_CURRENT_SOURCE_DIR}/proj.android/app/src")
endif()

target_link_libraries(${APP_NAME} ${ADXE_CORE_LIB})

target_include_directories(${APP_NAME}
        PRIVATE Classes
)

if(WIN64)
    target_link_options(${APP_NAME} PRIVATE "/STACK:4194304")
endif()

# mark app resources
setup_cocos_app_config(${APP_NAME})
if(APPLE)
    set_target_properties(${APP_NAME} PROPERTIES RESOURCE "${APP_UI_RES}")
    set_xcode_property(${APP_NAME} INSTALL_PATH "\$(LOCAL_APPS_DIR)")
    set_xcode_property(${APP_NAME} PRODUCT_BUNDLE_IDENTIFIER "org.adxe.cpp-tests")

    if(MACOSX)
        set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/proj.mac/Info.plist")
    elseif(IOS)
        set_target_properties(${APP_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios/Info.plist")
        set_xcode_property(${APP_NAME} ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon-${APP_NAME}")
    endif()

    # For code-signing, set the DEVELOPMENT_TEAM:
    #set_xcode_property(${APP_NAME} DEVELOPMENT_TEAM "GRLXXXX2K9")
elseif(WINDOWS)
    cocos_copy_target_dll(${APP_NAME})
endif()

if((WINDOWS AND (CMAKE_GENERATOR STREQUAL "Ninja")) OR LINUX)
    cocos_get_resource_path(APP_RES_DIR ${APP_NAME})
    cocos_copy_target_res(${APP_NAME} LINK_TO ${APP_RES_DIR} FOLDERS ${GAME_RES_FOLDER})
elseif(WINDOWS)
    set_property(TARGET ${APP_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${GAME_RES_FOLDER}")
endif()

if(WINDOWS)
    # precompiled header. Compilation time speedup ~4x, but don't know how Ninja to support PCH yet and needs help from someone.
    if(NOT CMAKE_GENERATOR STREQUAL "Ninja") 
        target_sources(${APP_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Classes/precheader.cpp")
        set_target_properties(${APP_NAME} PROPERTIES COMPILE_FLAGS "/Yuprecheader.h /FIprecheader.h")
        set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/Classes/precheader.cpp" PROPERTIES COMPILE_FLAGS "/Ycprecheader.h")
    endif()
endif()
