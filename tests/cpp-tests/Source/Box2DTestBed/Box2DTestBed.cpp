/****************************************************************************
 * Copyright (c) 2021 @aismann; Peter Eismann, Germany; dreifrankensoft

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

#include "platform/PlatformConfig.h"
#include "extensions/axmol-ext.h"
#include "ImGui/ImGuiPresenter.h"
#include "axmol.h"

#include "Box2DTestBed.h"
#include "samples/sample.h"
#include "samples/settings.h"
#include "GLView.h"

using namespace ax;
USING_NS_AX_EXT;

enum
{
    kTagParentNode = 1,
};

static Settings s_settings;

enum
{
    kTagBox2DNode,
};

static b2Vec2 tob2Vec2(const Vec2& val)
{
    return b2Vec2{val.x, val.y};
}

static inline int CompareSamples(const void* a, const void* b)
{
    SampleEntry* sa = (SampleEntry*)a;
    SampleEntry* sb = (SampleEntry*)b;

    int result = strcmp(sa->category, sb->category);
    if (result == 0)
    {
        result = strcmp(sa->name, sb->name);
    }

    return result;
}

static void SortTests()
{
    qsort(g_sampleEntries, g_sampleCount, sizeof(SampleEntry), CompareSamples);
}

Box2DTestBedTests::Box2DTestBedTests()
{
    // TODO: determine properly view size
    g_camera.m_width  = 1920;
    g_camera.m_height = 1080;
    g_camera.m_zoom   = 150;
    g_camera.m_center = b2Vec2_zero;

    ImGuiPresenter::getInstance()->setViewResolution(g_camera.m_width, g_camera.m_height);

    SortTests();

    for (int idx = 0; idx < g_sampleCount; ++idx)
    {
        addTestCase(g_sampleEntries[idx].name, [idx]() { return Box2DTestBed::create(idx); });
    }
}

//------------------------------------------------------------------
//
// Box2dTestBed
//
//------------------------------------------------------------------

Box2DTestBed::Box2DTestBed() {}

Box2DTestBed::~Box2DTestBed()
{
    //_eventDispatcher->removeEventListener(_touchListener);
    _eventDispatcher->removeEventListener(_keyboardListener);
    _eventDispatcher->removeEventListener(_mouseListener);
}

Box2DTestBed* Box2DTestBed::create(int index)
{
    auto layer = new Box2DTestBed();
    layer->initWithEntryIndex(index);
    layer->autorelease();
    return layer;
}

bool Box2DTestBed::initWithEntryIndex(int index)
{
    if (!TestCase::init())
    {
        return false;
    }
    auto director      = Director::getInstance();
    Vec2 visibleOrigin = director->getVisibleOrigin();
    Size visibleSize   = director->getVisibleSize();

    m_entryIndex = index;

    m_entry  = g_sampleEntries + index;
    m_sample = m_entry->createFcn(s_settings);

    // init physics
    this->initPhysics();

    auto label = Label::createWithTTF(m_entry->name, "fonts/arial.ttf", 28);
    addChild(label, 1);
    label->setPosition(visibleOrigin.x + visibleSize.width / 2, visibleOrigin.y + visibleSize.height - 50);

    // Adds touch event listener
    // _touchListener = EventListenerTouchOneByOne::create();
    // _touchListener->setSwallowTouches(true);
    // _touchListener->onTouchBegan = AX_CALLBACK_2(Box2DTestBed::onTouchBegan, this);
    // _touchListener->onTouchMoved = AX_CALLBACK_2(Box2DTestBed::onTouchMoved, this);
    // _touchListener->onTouchEnded = AX_CALLBACK_2(Box2DTestBed::onTouchEnded, this);
    // _eventDispatcher->addEventListenerWithFixedPriority(_touchListener, 10);

    // Adds Keyboard event listener
    _keyboardListener                = EventListenerKeyboard::create();
    _keyboardListener->onKeyPressed  = AX_CALLBACK_2(Box2DTestBed::onKeyPressed, this);
    _keyboardListener->onKeyReleased = AX_CALLBACK_2(Box2DTestBed::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithFixedPriority(_keyboardListener, 11);

    _mouseListener                = EventListenerMouse::create();
    _mouseListener->onMouseMove   = AX_CALLBACK_1(Box2DTestBed::onMouseMove, this);
    _mouseListener->onMouseUp     = AX_CALLBACK_1(Box2DTestBed::onMouseUp, this);
    _mouseListener->onMouseDown   = AX_CALLBACK_1(Box2DTestBed::onMouseDown, this);
    _mouseListener->onMouseScroll = AX_CALLBACK_1(Box2DTestBed::onMouseScroll, this);
    _eventDispatcher->addEventListenerWithFixedPriority(_mouseListener, 12);

    scheduleUpdate();

    return true;
}

void Box2DTestBed::onKeyPressed(EventKeyboard::KeyCode code, Event* event)
{
    // AXLOGD("onKeyPressed, keycode: {}", static_cast<int>(code));
    // m_sample->Keyboard((static_cast<int>(code) - 59));  // its a bad hack!
}

void Box2DTestBed::onKeyReleased(EventKeyboard::KeyCode code, Event* event)
{
    AXLOGD("onKeyPressed, keycode: {}", static_cast<int>(code));
    // m_sample->KeyboardUp((static_cast<int>(code) - 59));  // its a bad hack!
    m_sample->Keyboard((static_cast<int>(code) - 59));
}

void Box2DTestBed::onMouseDown(Event* event)
{
    EventMouse* e = static_cast<EventMouse*>(event);

    auto location = e->getLocation() - _debugDraw->getWorldOffset();
    b2Vec2 pos    = {location.x / _debugDraw->getPTMRatio(), location.y / _debugDraw->getPTMRatio()};

    int mods = 0;
#if defined(_WIN32)
    if (GetAsyncKeyState(VK_SHIFT) & 0x80)
        mods |= GLFW_MOD_SHIFT;
    if (GetAsyncKeyState(VK_CONTROL) & 0x80)
        mods |= GLFW_MOD_CONTROL;
    if (GetAsyncKeyState(VK_MENU) & 0x80)
        mods |= GLFW_MOD_ALT;
#endif
    _draging         = true;
    _mouseDownPos    = pos;
    _dragingStartPos = _debugDraw->getPosition();

    m_sample->MouseDown(pos, static_cast<int>(e->getMouseButton()), mods);
}

void Box2DTestBed::onMouseUp(Event* event)
{
    const auto ratio = _debugDraw->getPTMRatio();
    _draging      = false;
    EventMouse* e = static_cast<EventMouse*>(event);
    auto location = e->getLocation() - _debugDraw->getWorldOffset();
    b2Vec2 pos       = {location.x / ratio, location.y / ratio};
    m_sample->MouseUp(pos, static_cast<int>(e->getMouseButton()));
}

void Box2DTestBed::onMouseMove(Event* event)
{
    const auto ratio = _debugDraw->getPTMRatio();
    EventMouse* e = static_cast<EventMouse*>(event);

    auto location = e->getLocation() - _debugDraw->getWorldOffset();
    b2Vec2 pos{location.x / ratio, location.y / ratio};
    m_sample->MouseMove(pos);

    if (e->getMouseButton() == EventMouse::MouseButton::BUTTON_RIGHT)
    {
        auto diff = b2Sub(pos, _mouseDownPos);
        _debugDraw->setPosition(_dragingStartPos.x + diff.x, _dragingStartPos.y + diff.y);
    }
}

void Box2DTestBed::onMouseScroll(Event* event)
{
    EventMouse* e = (EventMouse*)event;
    _debugDraw->setPTMRatio(_debugDraw->getPTMRatio() - e->getScrollY());
}

void Box2DTestBed::onEnter()
{
    Scene::onEnter();
    ImGuiPresenter::getInstance()->addFont(FileUtils::getInstance()->fullPathForFilename("fonts/arial.ttf"));
    ImGuiPresenter::getInstance()->addRenderLoop("#bv3t", AX_CALLBACK_0(Box2DTestBed::onDrawImGui, this), this);
}
void Box2DTestBed::onExit()
{
    ImGuiPresenter::getInstance()->removeRenderLoop("#bv3t");
    Scene::onExit();
}

void Box2DTestBed::update(float dt)
{
    // Debug draw
    _debugDraw->clear();
    m_sample->Step(s_settings);
}

void Box2DTestBed::initPhysics()
{
    _debugDraw =
        utils::createInstance<Box2DTestDebugDrawNode>(&Box2DTestDebugDrawNode::initWithWorld, m_sample->m_worldId);
    _debugDraw->setAutoDraw(false);
    addChild(_debugDraw);

    auto& b2dw      = _debugDraw->getB2DebugDraw();
    b2dw.drawShapes = true;
    b2dw.drawJoints = true;
    b2dw.drawAABBs  = false;

    _debugDraw->setWorldOffset({250, 70});
    _debugDraw->setPTMRatio(3.0f);

    s_settings.hertz = 60;
}

void Box2DTestBed::RestartSample()
{
    getTestSuite()->restartCurrTest();
}

void Box2DTestBed::onDrawImGui()
{
    int maxWorkers = enki::GetNumHardwareThreads();

    float menuWidth       = 180.0f;
    auto cursorPos        = ImGui::GetCursorScreenPos();
    ImVec2 toolWindowPos  = {cursorPos.x + g_camera.m_width - menuWidth - 80, cursorPos.y - 80};
    ImVec2 toolWindowSize = {menuWidth, g_camera.m_height - 200.0f};
    if (g_draw.m_showUI)
    {
        ImGui::SetNextWindowPos(toolWindowPos, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(toolWindowSize, ImGuiCond_FirstUseEver);
        ImGui::Begin("Tools", &g_draw.m_showUI,
                     /*ImGuiWindowFlags_NoMove | */ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

        if (ImGui::BeginTabBar("ControlTabs", ImGuiTabBarFlags_None))
        {
            if (ImGui::BeginTabItem("Controls"))
            {
                ImGui::PushItemWidth(100.0f);
                ImGui::SliderInt("Sub-steps", &s_settings.subStepCount, 1, 50);
                ImGui::SliderFloat("Hertz", &s_settings.hertz, 5.0f, 120.0f, "%.0f hz");

                if (ImGui::SliderInt("Workers", &s_settings.workerCount, 1, maxWorkers))
                {
                    s_settings.workerCount = b2ClampInt(s_settings.workerCount, 1, maxWorkers);
                    RestartSample();
                }
                ImGui::PopItemWidth();

                ImGui::Separator();

                ImGui::Checkbox("Sleep", &s_settings.enableSleep);
                ImGui::Checkbox("Warm Starting", &s_settings.enableWarmStarting);
                ImGui::Checkbox("Continuous", &s_settings.enableContinuous);

                ImGui::Separator();

                ImGui::Checkbox("Shapes", &s_settings.drawShapes);
                ImGui::Checkbox("Joints", &s_settings.drawJoints);
                ImGui::Checkbox("Joint Extras", &s_settings.drawJointExtras);
                ImGui::Checkbox("AABBs", &s_settings.drawAABBs);
                ImGui::Checkbox("Contact Points", &s_settings.drawContactPoints);
                ImGui::Checkbox("Contact Normals", &s_settings.drawContactNormals);
                ImGui::Checkbox("Contact Impulses", &s_settings.drawContactImpulses);
                ImGui::Checkbox("Friction Impulses", &s_settings.drawFrictionImpulses);
                ImGui::Checkbox("Center of Masses", &s_settings.drawMass);
                ImGui::Checkbox("Graph Colors", &s_settings.drawGraphColors);
                ImGui::Checkbox("Counters", &s_settings.drawCounters);
                ImGui::Checkbox("Profile", &s_settings.drawProfile);

                ImVec2 button_sz = ImVec2(-1, 0);
                if (ImGui::Button("Pause (P)", button_sz))
                {
                    s_settings.pause = !s_settings.pause;
                }

                if (ImGui::Button("Single Step (O)", button_sz))
                {
                    s_settings.singleStep = !s_settings.singleStep;
                }

                if (ImGui::Button("Dump Mem Stats", button_sz))
                {
                    b2World_DumpMemoryStats(m_sample->m_worldId);
                }

                if (ImGui::Button("Reset Profile", button_sz))
                {
                    m_sample->ResetProfile();
                }

                if (ImGui::Button("Restart (R)", button_sz))
                {
                    RestartSample();
                }

                if (ImGui::Button("Quit", button_sz))
                {
                    glfwSetWindowShouldClose(g_mainWindow, GL_TRUE);
                }

                ImGui::EndTabItem();
            }

            ImGuiTreeNodeFlags leafNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
            leafNodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

            ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

            if (ImGui::BeginTabItem("Tests"))
            {
                int categoryIndex    = 0;
                const char* category = g_sampleEntries[categoryIndex].category;
                int i                = 0;
                while (i < g_sampleCount)
                {
                    bool categorySelected = strcmp(category, g_sampleEntries[s_settings.sampleIndex].category) == 0;
                    ImGuiTreeNodeFlags nodeSelectionFlags = categorySelected ? ImGuiTreeNodeFlags_Selected : 0;
                    bool nodeOpen                         = ImGui::TreeNodeEx(category, nodeFlags | nodeSelectionFlags);

                    if (nodeOpen)
                    {
                        while (i < g_sampleCount && strcmp(category, g_sampleEntries[i].category) == 0)
                        {
                            ImGuiTreeNodeFlags selectionFlags = 0;
                            if (s_settings.sampleIndex == i)
                            {
                                selectionFlags = ImGuiTreeNodeFlags_Selected;
                            }
                            ImGui::TreeNodeEx((void*)(intptr_t)i, leafNodeFlags | selectionFlags, "%s",
                                              g_sampleEntries[i].name);
                            if (ImGui::IsItemClicked())
                            {
                                getTestSuite()->enterTest(i);
                            }
                            ++i;
                        }
                        ImGui::TreePop();
                    }
                    else
                    {
                        while (i < g_sampleCount && strcmp(category, g_sampleEntries[i].category) == 0)
                        {
                            ++i;
                        }
                    }

                    if (i < g_sampleCount)
                    {
                        category      = g_sampleEntries[i].category;
                        categoryIndex = i;
                    }
                }
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        ImGui::End();

        m_sample->UpdateUI();
    }

    // if (g_draw.m_showUI)
    {
        char buffer[128];
        snprintf(buffer, 128, "%.1f ms - step %d - camera (%g, %g, %g)", 1000.0f * _director->getDeltaTime(),
                 m_sample->m_stepCount, g_camera.m_center.x, g_camera.m_center.y, g_camera.m_zoom);
        // snprintf( buffer, 128, "%.1f ms", 1000.0f * frameTime );

        ImGui::SetNextWindowPos(ImVec2{cursorPos.x + 92, cursorPos.y + g_camera.m_height - 235},
                                ImGuiCond_FirstUseEver);
        ImGui::Begin("Overlay", nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize |
                         ImGuiWindowFlags_NoScrollbar);
        // ImGui::SetCursorPos(ImVec2(5.0f, g_camera.m_height - 20.0f));
        ImGui::TextColored(ImColor(153, 230, 153, 255), "%s", buffer);
        ImGui::End();
    }
}
