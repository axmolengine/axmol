#include "platform/CCPlatformConfig.h"
#include "ImGuiTest.h"

#include "ImGui/ImGuiPresenter.h"

USING_NS_AX;
USING_NS_AX_EXT;

#if defined(AX_PLATFORM_PC) || (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)

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

    ImGuiPresenter::getInstance()->addFont(FileUtils::getInstance()->fullPathForFilename("fonts/arial.ttf"));
    ImGuiPresenter::getInstance()->addRenderLoop("#test", AX_CALLBACK_0(ImGuiTest::onDrawImGui, this), this);
}

void ImGuiTest::onExit()
{
    ImGuiPresenter::getInstance()->removeRenderLoop("#test");
    ImGuiPresenter::getInstance()->clearFonts();

    ImGuiPresenter::destroyInstance();

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
