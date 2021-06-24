#include "platform/CCPlatformConfig.h"
#include "ImGuiEXTTest.h"

#include "ImGuiEXT/CCImGuiEXT.h"

USING_NS_CC;
USING_NS_CC_EXT;

#if defined(CC_PLATFORM_PC)

static bool show_test_window = true;
static bool show_another_window = true;
static ImVec4 clear_color = ImColor(114, 144, 154);

ImGuiEXTTests::ImGuiEXTTests()
{
    // Resize (expand) window 
    static Size resourceSize(1280, 720);
    auto director = Director::getInstance();
    GLViewImpl* view = (GLViewImpl*)Director::getInstance()->getOpenGLView();
    view->setWindowed(resourceSize.width, resourceSize.height);

    ADD_TEST_CASE(ImGuiEXTTest);
}

void ImGuiEXTTest::onEnter()
{
    TestCase::onEnter();

    ImGuiEXT::getInstance()->addFont(FileUtils::getInstance()->fullPathForFilename("fonts/arial.ttf"));
    ImGuiEXT::getInstance()->addRenderLoop("#test", CC_CALLBACK_0(ImGuiEXTTest::onDrawImGui, this), this);
}

void ImGuiEXTTest::onExit()
{
    ImGuiEXT::getInstance()->removeRenderLoop("#test");
    ImGuiEXT::getInstance()->clearFonts();

    ImGuiEXT::destroyInstance();

    static Size resourceSize(960, 640);
    GLViewImpl* view = (GLViewImpl*)Director::getInstance()->getOpenGLView();
    view->setWindowed(resourceSize.width, resourceSize.height);

    TestCase::onExit();
}

void ImGuiEXTTest::onDrawImGui()
{
    // 1. Show a simple window
    // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
    {
        static float f = 0.0f;
        ImGui::Text("Hello, world!");
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
        ImGui::ColorEdit3("clear color", (float*)&clear_color);
        if (ImGui::Button("Test Window")) show_test_window ^= 1;
        if (ImGui::Button("Another Window")) show_another_window ^= 1;
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
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
