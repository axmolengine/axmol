#include "platform/PlatformConfig.h"
#include "ImGuiTest.h"

#if defined(AX_PLATFORM_PC) || (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID) || defined(__EMSCRIPTEN__)

#include "ImGui/ImGuiPresenter.h"
#if !defined(__ANDROID__)
#   include "SDFGen/SDFGen.h"
#endif

using namespace ax;
USING_NS_AX_EXT;


static bool show_test_window    = true;
static bool show_another_window = true;
static ImVec4 clear_color       = ImColor(114, 144, 154);

ImGuiTests::ImGuiTests()
{
    ImGuiPresenter::getInstance()->setViewResolution(1280, 720);

    ADD_TEST_CASE(ImGuiTest);
}

void ImGuiTest::onEnter()
{
    TestCase::onEnter();
#    if !defined(__ANDROID__)
    SDFGen::getInstance()->open();
#endif

    auto cache = SpriteFrameCache::getInstance();
    cache->addSpriteFramesWithFile("animations/grossini.plist");

    ImGuiPresenter::getInstance()->addFont(FileUtils::getInstance()->fullPathForFilename("fonts/arial.ttf"));
    ImGuiPresenter::getInstance()->enableDPIScale();
    ImGuiPresenter::getInstance()->addRenderLoop("#test", AX_CALLBACK_0(ImGuiTest::onDrawImGui, this), this);
}

void ImGuiTest::onExit()
{
    ImGuiPresenter::getInstance()->removeRenderLoop("#test");
    ImGuiPresenter::getInstance()->clearFonts();

#    if !defined(__ANDROID__)
    SDFGen::getInstance()->close();
#    endif
    ImGuiPresenter::destroyInstance();

    SpriteFrameCache::getInstance()->removeUnusedSpriteFrames();

    TestCase::onExit();
}

void ImGuiTest::onDrawImGui()
{
    // 1. Show a simple window
    // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
    {
        static float f = 0.0f;
        ImGui::Text("Hello, world!");
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
        ImGui::ColorEdit3("clear color", (float*)&clear_color);
        if (ImGui::Button("Test Window"))
            show_test_window ^= 1;
        if (ImGui::Button("Another Window"))
            show_another_window ^= 1;
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);

        auto spriteFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName("grossini_dance_05.png");
        ImGuiPresenter::getInstance()->image(spriteFrame, ImVec2(256, 256), false);
    }

    // 2. Show another simple window, this time using an explicit Begin/End pair
    if (show_another_window)
    {
        ImGui::SetNextWindowSize(ImVec2(170, 80), ImGuiCond_FirstUseEver);
        ImGui::Begin("Another Window", &show_another_window);

        ImGui::Text("Hello");
        ImGui::End();
    }

    // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
    if (show_test_window)
    {
        ImGui::ShowDemoWindow();
    }
}

#endif
