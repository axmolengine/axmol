/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmol.dev/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "controller.h"
#include <functional>
#include <chrono>
#include "BaseTest.h"
#include "tests.h"

using namespace ax;

#define LOG_INDENTATION "  "
#define LOG_TAG         "[TestController]"

static void initCrashCatch();
static void disableCrashCatch();

class RootTests : public TestList
{
public:
    RootTests()
    {
#if defined(AX_ENABLE_EXT_EFFEKSEER)
#    pragma message("The optional extension Effekseer is enabled.")
        addTest("Effekseer", []() { return new EffekseerTests(); });
#endif
//        addTest("Node: Scene3D", [](){return new Scene3DTests(); });
#if defined(AX_PLATFORM_PC) || (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID) || defined(__EMSCRIPTEN__)
        addTest("ImGui", []() { return new ImGuiTests(); });
#endif
        addTest("Texture2D", []() { return new Texture2DTests(); });
        addTest("ActionManager", []() { return new ActionManagerTests(); });
        addTest("Actions - Basic", []() { return new ActionsTests(); });
        addTest("Actions - Ease", []() { return new ActionsEaseTests(); });
        addTest("Actions - Progress", []() { return new ActionsProgressTests(); });
        addTest("Audio - NewAudioEngine", []() { return new AudioEngineTests(); });

        addTest("Box2D - Basic", []() { return new Box2DTests(); });
#if defined(AX_PLATFORM_PC) || defined(__EMSCRIPTEN__)
        addTest("Box2D - TestBed", []() { return new Box2DTestBedTests(); });
#endif
        addTest("Chipmunk2D - Basic", []() { return new ChipmunkTests(); });
#if defined(AX_PLATFORM_PC) || defined(__EMSCRIPTEN__)
        addTest("Chipmunk2D - TestBed", []() { return new ChipmunkTestBedTests(); });
#endif
        addTest("Bugs", []() { return new BugsTests(); });
        addTest("Click and Move", []() { return new ClickAndMoveTest(); });
        addTest("Configuration", []() { return new ConfigurationTests(); });
        addTest("Console", []() { return new ConsoleTests(); });
#if !defined(__EMSCRIPTEN__)
        addTest("Curl", []() { return new CurlTests(); });
#endif
        addTest("Current Language", []() { return new CurrentLanguageTests(); });
        addTest("Network Test", []() { return new NetworkTests(); });
        addTest("EventDispatcher", []() { return new EventDispatcherTests(); });
        addTest("Effects - Advanced", []() { return new EffectAdvanceTests(); });
        addTest("Effects - Basic", []() { return new EffectTests(); });
        addTest("Extensions", []() { return new ExtensionsTests(); });
        addTest("Fonts", []() { return new FontTests(); });
        addTest("Interval", []() { return new IntervalTests(); });
#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
        addTest("JNIHelper", []() { return new JNITests(); });
#endif
        addTest("Material System", []() { return new MaterialSystemTest(); });
        addTest("Navigation Mesh", []() { return new NavMeshTests(); });
        addTest("Node: BillBoard Test", []() { return new BillBoardTests(); });
        addTest("Node: Camera3D Test", []() { return new Camera3DTests(); });
        addTest("Node: Clipping", []() { return new ClippingNodeTests(); });
        addTest("Node: Draw (WIP)", []() { return new DrawNodeTests(); });
        addTest("Node: Label - New API", []() { return new NewLabelTests(); });
        addTest("Node: Layer", []() { return new LayerTests(); });
        addTest("Node: Light", []() { return new LightTests(); });
        addTest("Node: Menu", []() { return new MenuTests(); });
        addTest("Node: MotionStreak", []() { return new MotionStreakTests(); });
        addTest("Node: Node", []() { return new CocosNodeTests(); });
        addTest("Node: Parallax", []() { return new ParallaxTests(); });
        addTest("Node: Particles", []() { return new ParticleTests(); });
        addTest("Node: Particle3D (PU)", []() { return new Particle3DTests(); });
#if defined(AX_ENABLE_PHYSICS)
        addTest("Node: Physics", []() { return new PhysicsTests(); });
#endif
        addTest("Node: Physics3D", []() { return new Physics3DTests(); });
        addTest("Node: RenderTexture", []() { return new RenderTextureTests(); });
        addTest("Node: Scene", []() { return new SceneTests(); });
        addTest("Node: Spine", []() { return new SpineTests(); });
        addTest("Node: Sprite", []() { return new SpriteTests(); });
        addTest("Node: MeshRenderer", []() { return new MeshRendererTests(); });
        addTest("Node: SpritePolygon", []() { return new SpritePolygonTest(); });
        addTest("Node: Terrain", []() { return new TerrainTests(); });
        addTest("Node: FastTileMap", []() { return new FastTileMapTests(); });
        addTest("Node: Text Input", []() { return new TextInputTests(); });
        addTest("Node: UI", []() { return new UITests(); });
        addTest("Mouse", []() { return new MouseTests(); });
        addTest("MultiTouch", []() { return new MultiTouchTests(); });
        addTest("Renderer", []() { return new NewRendererTests(); });
        addTest("ReleasePool", []() { return new ReleasePoolTests(); });
        addTest("Rotate World", []() { return new RotateWorldTests(); });
        addTest("Scheduler", []() { return new SchedulerTests(); });
        addTest("Shader - Basic", []() { return new ShaderTests(); });
        addTest("Shader - Sprite", []() { return new Shader2Tests(); });
        addTest("TextureCache", []() { return new TextureCacheTests(); });
        addTest("TexturePacker Encryption", []() { return new TextureAtlasEncryptionTests(); });
        addTest("Touches", []() { return new TouchesTests(); });
        addTest("Transitions", []() { return new TransitionsTests(); });
        addTest("Unzip Test", []() { return new ZipTests(); });
        addTest("URL Open Test", []() { return new OpenURLTests(); });
        addTest("UserDefault", []() { return new UserDefaultTests(); });
#if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS || AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
        addTest("Vibrate", []() { return new VibrateTests(); });
#endif
        //       addTest("Zwoptex Deprecrated, will be removed in release axmol-2.2.0 (see #1602)", []() { return new
        //       ZwoptexTests(); });
        addTest("SpriteFrameCache", []() { return new SpriteFrameCacheTests(); });  // TODO
#if (AX_TARGET_PLATFORM == AX_PLATFORM_MAC || AX_TARGET_PLATFORM == AX_PLATFORM_WIN32 || \
     AX_TARGET_PLATFORM == AX_PLATFORM_LINUX)
        addTest("Window Test", []() { return new WindowTests(); });  // TODO wrong effect
#endif
    }
};

TestController::TestController() : _stopAutoTest(true), _isRunInBackground(false), _testSuite(nullptr)
{
    _rootTestList = new RootTests;
    _rootTestList->runThisTest();
    _director = Director::getInstance();

    _touchListener               = EventListenerTouchOneByOne::create();
    _touchListener->onTouchBegan = AX_CALLBACK_2(TestController::blockTouchBegan, this);
    _touchListener->setSwallowTouches(true);

    _director->getEventDispatcher()->addEventListenerWithFixedPriority(_touchListener, -200);
}

TestController::~TestController()
{
    _director->getEventDispatcher()->removeEventListener(_touchListener);

    _rootTestList->release();
    _rootTestList = nullptr;
}

void TestController::startAutoTest()
{
    if (_stopAutoTest)
    {
        _stopAutoTest   = false;
        _logIndentation = "";

        _autoTestRunner = Node::create();
        _director->setNotificationNode(_autoTestRunner);

        auto action = ActionCoroutine::create(std::bind(&TestController::traverseTestList, this, _rootTestList));

        _autoTestRunner->runAction(action);
    }
}

void TestController::stopAutoTest()
{
    _stopAutoTest = true;
}

Coroutine TestController::traverseTestList(TestList* testList)
{
     if (testList != _rootTestList)
     {
         _logIndentation += LOG_INDENTATION;
     }

     co_yield DelayTime::create(0.5);  
     AXLOGD("{}{}Begin traverse TestList:{}", LOG_TAG, _logIndentation, testList->getTestName());

     auto scheduler = _director->getScheduler();
     int testIndex  = 0;
     for (auto&& callback : testList->_testCallbacks)
     {
         if (_stopAutoTest)
             break;
         while (_isRunInBackground)
         {
             AXLOGD("_director is paused");
             co_yield DelayTime::create(0.5);  
         }
         if (callback)
         {
             auto test = callback();
             test->setTestParent(testList);
             test->setTestName(testList->_childTestNames[testIndex++]);

             ActionCoroutine* subAction = nullptr;
             if (test->isTestList())
             {
                 test->runThisTest();

                 subAction =
                     ActionCoroutine::create(std::bind(&TestController::traverseTestList, this, static_cast<TestList*>(test)));
             }
             else
             {
                 subAction =
                     ActionCoroutine::create(std::bind(&TestController::traverseTestSuite, this, static_cast<TestSuite*>(test)));
             }
             co_yield subAction;  // co_yield _autoTestRunner->runAction(subAction);
         }
     }

     if (testList == _rootTestList)
     {
         _stopAutoTest = true;
         if (std::getenv("AXMOL_START_AUTOTEST"))
             utils::killCurrentProcess();
     }
     else
     {
         if (!_stopAutoTest)
         {
             // Backs up one level and release TestList object.
             testList->_parentTest->runThisTest();
             testList->release();
         }

         _logIndentation.erase(_logIndentation.rfind(LOG_INDENTATION));
     }
 }

 Coroutine TestController::traverseTestSuite(TestSuite* testSuite)
 {
     auto scheduler         = _director->getScheduler();
     int testIndex          = 0;
     float testCaseDuration = 0.0f;
     _logIndentation += LOG_INDENTATION;
     AXLOGD("{}{}Begin traverse TestSuite:{}", LOG_TAG, _logIndentation.c_str(), testSuite->getTestName().c_str());

     _logIndentation += LOG_INDENTATION;
     testSuite->_currTestIndex = -1;

     auto logIndentation = _logIndentation;
     for (auto&& callback : testSuite->_testCallbacks)
     {
         auto testName = testSuite->_childTestNames[testIndex++];

         Scene* testScene                 = nullptr;
         TestCase* testCase               = nullptr;
         TransitionScene* transitionScene = nullptr;

         if (_stopAutoTest)
             break;

         while (_isRunInBackground)
         {
             AXLOGD("_director is paused");
             co_yield DelayTime::create(0.5);
         }
         if (_stopAutoTest)
             break;
         AXLOGD("{}{}Run test:{}.", LOG_TAG, logIndentation, testName);

         auto scene = callback();
         if (_stopAutoTest)
             break;

         if (scene)
         {
             transitionScene = dynamic_cast<TransitionScene*>(scene);
             if (transitionScene)
             {
                 testCase         = (TestCase*)transitionScene->getInScene();
                 testCaseDuration = transitionScene->getDuration() + 0.5f;
                 
             }
             else
             {
                 testCase         = (TestCase*)scene;
                 testCaseDuration = testCase->getDuration();
             }
             testSuite->_currTestIndex++;
             testCase->setTestSuite(testSuite);
             testCase->setTestCaseName(testName);
             _director->replaceScene(scene);

             testScene = scene;

             co_yield DelayTime::create(testCaseDuration);
         }

         if (_stopAutoTest)
             break;

         if (transitionScene == nullptr)
         {

             if (!_stopAutoTest)
             {
                 // Check the result of test.
                 checkTest(testCase);
             }
         }
     }

     if (!_stopAutoTest)
     {
         // Backs up one level and release TestSuite object.
         auto parentTest = testSuite->_parentTest;
         parentTest->runThisTest();

         co_yield DelayTime::create(1);
         testSuite->release();
     }

     _logIndentation.erase(_logIndentation.rfind(LOG_INDENTATION));
     _logIndentation.erase(_logIndentation.rfind(LOG_INDENTATION));
 }

bool TestController::checkTest(TestCase* testCase)
{
    if (testCase)
    {
        switch (testCase->getTestType())
        {
        case TestCase::Type::UNIT:
        {
            if (testCase && testCase->getExpectedOutput() != testCase->getActualOutput())
            {
                AXLOGE("{} {} test fail", LOG_TAG, testCase->getTestCaseName());
            }
            else
            {
                AXLOGE("{} {} test pass", LOG_TAG, testCase->getTestCaseName());
            }
            break;
        }
        case TestCase::Type::ROBUSTNESS:
        {
            break;
        }
        case TestCase::Type::MANUAL:
        {
            break;
        }
        default:
            break;
        }
    }

    return true;
}

void TestController::handleCrash()
{
    disableCrashCatch();

    AXLOGE("{}Catch an crash event", LOG_TAG);

    if (!_stopAutoTest)
    {
        stopAutoTest();
    }
}

void TestController::onEnterBackground()
{
    _isRunInBackground = true;
}

void TestController::onEnterForeground()
{
    _isRunInBackground = false;
}

static TestController* s_testController = nullptr;

TestController* TestController::getInstance()
{
    if (s_testController == nullptr)
    {
        s_testController = new TestController;

        initCrashCatch();
    }

    return s_testController;
}

void TestController::destroyInstance()
{
    if (s_testController)
    {
        s_testController->stopAutoTest();
        delete s_testController;
        s_testController = nullptr;
    }

    disableCrashCatch();
}

bool TestController::blockTouchBegan(Touch* touch, Event* event)
{
    return !_stopAutoTest;
}

//==================================================================================================
#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
#    include <windows.h>

static long __stdcall windowExceptionFilter(_EXCEPTION_POINTERS* excp)
{
    if (s_testController)
    {
        s_testController->handleCrash();
    }

    return EXCEPTION_EXECUTE_HANDLER;
}

static void initCrashCatch()
{
    SetUnhandledExceptionFilter(windowExceptionFilter);
}
static void disableCrashCatch()
{
    SetUnhandledExceptionFilter(UnhandledExceptionFilter);
}

#elif AX_TARGET_PLATFORM == AX_PLATFORM_MAC || AX_TARGET_PLATFORM == AX_PLATFORM_IOS || \
    AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID

#    if AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
static int s_fatal_signals[] = {
    SIGILL, SIGABRT, SIGBUS, SIGFPE, SIGSEGV, SIGSTKFLT, SIGPIPE,
};
#    else
static int s_fatal_signals[] = {
    SIGABRT, SIGBUS, SIGFPE, SIGILL, SIGSEGV, SIGTRAP, SIGTERM, SIGKILL,
};
#    endif

static void signalHandler(int sig)
{
    if (s_testController)
    {
        s_testController->handleCrash();
    }
}

static void initCrashCatch()
{
    for (auto&& sig : s_fatal_signals)
    {
        signal(sig, signalHandler);
    }
}

static void disableCrashCatch()
{
    for (auto&& sig : s_fatal_signals)
    {
        signal(sig, SIG_DFL);
    }
}

#else

static void initCrashCatch() {}

static void disableCrashCatch() {}

#endif
