/****************************************************************************
 * Copyright (c) 2021 @aismann; Peter Eismann, Germany; dreifrankensoft
 * Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

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

#include "axmol/platform/PlatformConfig.h"
#include "extensions/axmol-ext.h"
#include "ImGui/ImGuiPresenter.h"
#include "implot.h"
#include "axmol/axmol.h"

#include "Box2DTestBed.h"
#include "samples/sample.h"
#include "samples/TaskScheduler.h"
#include "axmol/platform/RenderView.h"

using namespace ax;
USING_NS_AX_EXT;

enum
{
    kTagParentNode = 1,
};

static SampleContext s_context;

SampleCamera& getBox2dTestBedCamera()
{
    return s_context.camera;
}

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
    s_context.camera.width  = g_resourceSize.width;
    s_context.camera.height = g_resourceSize.height;
    s_context.camera.zoom   = 80;
    s_context.camera.center = b2Vec2_zero;

    s_context.window = static_cast<RenderViewImpl*>(Director::getInstance()->getRenderView())->getWindow();

    s_context.Load();

#if defined(AX_PLATFORM_GLFW)
    static_cast<RenderViewImpl*>(Director::getInstance()->getRenderView())
        ->setWindowed(s_context.camera.width, s_context.camera.height);
#endif

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

Box2DTestBed::Box2DTestBed()
{
    m_statsWindowOffset = Vec2(-60, -80);
}

Box2DTestBed::~Box2DTestBed()
{
    _eventDispatcher->removeEventListener(_keyboardListener);
    _eventDispatcher->removeEventListener(_mouseListener);

    AX_SAFE_DELETE(_sampleDrawProxy);
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

    m_entryIndex = s_context.sampleIndex = index;

    _sampleDrawProxy = new SampleDraw{&s_context};
    s_context.draw   = _sampleDrawProxy;

    m_entry  = g_sampleEntries + index;
    m_sample = m_entry->createFcn(&s_context);

    // init physics
    this->initPhysics();

    _sampleDrawProxy->impl = _debugDrawNode;

    auto label = Label::createWithTTF(m_entry->name, "fonts/arial.ttf", 28);
    addChild(label, 1);
    label->setPosition(visibleOrigin.x + visibleSize.width / 2, visibleOrigin.y + visibleSize.height - 50);

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

bool Box2DTestBed::onMouseDown(Event* event)
{
    EventMouse* e = static_cast<EventMouse*>(event);

    auto location = e->getLocation() - _debugDrawNode->getWorldOffset();
    b2Vec2 pos    = {location.x / _debugDrawNode->getPTMRatio(), location.y / _debugDrawNode->getPTMRatio()};

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
    _dragingStartPos = _debugDrawNode->getPosition();

    m_sample->MouseDown(pos, static_cast<int>(e->getMouseButton()), mods);

    return true;
}

bool Box2DTestBed::onMouseUp(Event* event)
{
    const auto ratio = _debugDrawNode->getPTMRatio();
    _draging         = false;
    EventMouse* e    = static_cast<EventMouse*>(event);
    auto location    = e->getLocation() - _debugDrawNode->getWorldOffset();
    b2Vec2 pos       = {location.x / ratio, location.y / ratio};
    m_sample->MouseUp(pos, static_cast<int>(e->getMouseButton()));
    return true;
}

bool Box2DTestBed::onMouseMove(Event* event)
{
    const auto ratio = _debugDrawNode->getPTMRatio();
    EventMouse* e    = static_cast<EventMouse*>(event);

    auto location = e->getLocation() - _debugDrawNode->getWorldOffset();
    b2Vec2 pos{location.x / ratio, location.y / ratio};
    m_sample->MouseMove(pos);

    if (e->getMouseButton() == EventMouse::MouseButton::BUTTON_RIGHT)
    {
        auto diff = b2Sub(pos, _mouseDownPos);
        _debugDrawNode->setPosition(_dragingStartPos.x + diff.x, _dragingStartPos.y + diff.y);
    }
    return true;
}

bool Box2DTestBed::onMouseScroll(Event* event)
{
    EventMouse* e = (EventMouse*)event;
    _debugDrawNode->setPTMRatio(_debugDrawNode->getPTMRatio() - e->getScrollY());
    return true;
}

void Box2DTestBed::onEnter()
{
    Scene::onEnter();
    _director->getRenderView()->setWindowSize(1920, 1080);
    ImGuiPresenter::getInstance()->enableDPIScale();
    ImGuiPresenter::getInstance()->addFont(FileUtils::getInstance()->fullPathForFilename("fonts/arial.ttf"));
    ImGuiPresenter::getInstance()->addRenderLoop("#bv3t", AX_CALLBACK_0(Box2DTestBed::renderSamples, this), this);
    ImPlot::CreateContext();
}
void Box2DTestBed::onExit()
{
    ImPlot::DestroyContext();
    auto presenter = ImGuiPresenter::getInstance();
    if (presenter)
        presenter->removeRenderLoop("#bv3t");
    Scene::onExit();
}

void Box2DTestBed::initPhysics()
{
    _debugDrawNode = new SampleDrawNode(&s_context.debugDraw);
    _debugDrawNode->initWithWorld(m_sample->m_worldId);
    _debugDrawNode->setAutoDraw(false);
    addChild(_debugDrawNode);

    _debugDrawNode->release();

    auto& b2dw      = _debugDrawNode->getB2DebugDraw();
    b2dw.drawShapes = true;
    b2dw.drawJoints = true;
    b2dw.drawBounds = false;

    _debugDrawNode->setWorldOffset({250, 70});
    _debugDrawNode->setPTMRatio(3.0f);

    s_context.hertz = 60;
}

void Box2DTestBed::RestartSample()
{
    getTestSuite()->restartCurrTest();
}

void Box2DTestBed::renderSamples()
{
    _debugDrawNode->clear();

    auto cursorPos = ImGui::GetCursorScreenPos();

    auto& windowSize        = _director->getRenderView()->getWindowSize();
    s_context.camera.width  = static_cast<int>(windowSize.width);
    s_context.camera.height = static_cast<int>(windowSize.height);

    auto viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + 5.0f, viewport->Pos.y + 5.0f));
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x - 10.0f, viewport->Size.y - 10.0f));
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("Overlay", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize |
                     ImGuiWindowFlags_NoScrollbar);
    ImGui::End();

    m_sample->ResetText();

    char buffer[128];
    if (s_context.showUI)
    {
        const SampleEntry& entry = g_sampleEntries[s_context.sampleIndex];
        snprintf(buffer, 128, "%s : %s", entry.category, entry.name);
        m_sample->DrawColoredTextLine(b2_colorYellow, buffer);
    }

    m_sample->Step();

    /// BEGIN UpdateUI
    int maxWorkers  = enki::GetNumHardwareThreads();
    float menuWidth = 180.0f * Device::getPixelRatio();
    if (s_context.showUI)
    {
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::SetNextWindowPos(
            ImVec2(viewport->Pos.x + viewport->Size.x - menuWidth - 10.0f, viewport->Pos.y + 10.0f));
        ImGui::SetNextWindowSize(ImVec2(menuWidth, viewport->Size.y - 20.0f));
        ImGui::Begin("Tools", &s_context.showUI,
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

        if (ImGui::BeginTabBar("ControlTabs", ImGuiTabBarFlags_None))
        {
            if (ImGui::BeginTabItem("Controls"))
            {
                ImGui::PushItemWidth(100.0f);
                ImGui::SliderInt("Sub-steps", &s_context.subStepCount, 1, 32);
                ImGui::SliderFloat("Hertz", &s_context.hertz, 5.0f, 240.0f, "%.0f hz");

                if (ImGui::SliderInt("Workers", &s_context.workerCount, 1, maxWorkers))
                {
                    s_context.workerCount = b2ClampInt(s_context.workerCount, 1, maxWorkers);
                    RestartSample();
                }
                ImGui::PopItemWidth();

                ImGui::Separator();

                ImGui::Checkbox("Sleep", &s_context.enableSleep);
                ImGui::Checkbox("Warm Starting", &s_context.enableWarmStarting);
                ImGui::Checkbox("Continuous", &s_context.enableContinuous);
                ImGui::Checkbox("Contact Recycling", &s_context.enableRecycling);

                ImGui::Separator();

                ImGui::Checkbox("Shapes", &s_context.debugDraw.drawShapes);
                ImGui::Checkbox("Joints", &s_context.debugDraw.drawJoints);
                ImGui::Checkbox("Joint Extras", &s_context.debugDraw.drawJointExtras);
                ImGui::Checkbox("Bounds", &s_context.debugDraw.drawBounds);
                ImGui::Checkbox("Mass", &s_context.debugDraw.drawMass);
                ImGui::Checkbox("Body Names", &s_context.debugDraw.drawBodyNames);
                ImGui::Checkbox("Graph Colors", &s_context.debugDraw.drawGraphColors);
                ImGui::Checkbox("Islands", &s_context.debugDraw.drawIslands);
                ImGui::Checkbox("Counters", &s_context.drawCounters);
                ImGui::Checkbox("Profile", &s_context.drawProfile);
                ImGui::Checkbox("Frame Time", &s_context.frameTime);

                ImGui::Separator();

                {
                    bool changed            = false;
                    const char* drawTypes[] = {"None", "Clip", "AnchorA", "AnchorB", "Average"};
                    int drawType            = int(s_context.debugDraw.contactDrawType);
                    changed = changed || ImGui::Combo("Contact", &drawType, drawTypes, IM_ARRAYSIZE(drawTypes));
                    s_context.debugDraw.contactDrawType = b2ContactDrawType(drawType);
                }

                ImGui::Checkbox("Contact Normals", &s_context.debugDraw.drawContactNormals);
                ImGui::Checkbox("Contact Features", &s_context.debugDraw.drawContactFeatures);
                ImGui::Checkbox("Contact Forces", &s_context.debugDraw.drawContactForces);
                ImGui::Checkbox("Friction Forces", &s_context.debugDraw.drawFrictionForces);

                ImGui::Separator();

                ImGui::PushItemWidth(80.0f);
                ImGui::InputFloat("Joint Scale", &s_context.debugDraw.jointScale);
                ImGui::InputFloat("Force Scale", &s_context.debugDraw.forceScale);
                ImGui::PopItemWidth();

                ImVec2 button_sz = ImVec2(-1, 0);
                if (ImGui::Button("Pause (P)", button_sz))
                {
                    s_context.pause = !s_context.pause;
                }

                if (ImGui::Button("Single Step (O)", button_sz))
                {
                    s_context.singleStep = !s_context.singleStep;
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
                    onBackCallback(this);
                }

                ImGui::EndTabItem();
            }

            ImGuiTreeNodeFlags leafNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
            leafNodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

            ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

            if (ImGui::BeginTabItem("Samples"))
            {
                int categoryIndex    = 0;
                const char* category = g_sampleEntries[categoryIndex].category;
                int i                = 0;
                while (i < g_sampleCount)
                {
                    bool categorySelected = strcmp(category, g_sampleEntries[s_context.sampleIndex].category) == 0;
                    ImGuiTreeNodeFlags nodeSelectionFlags = categorySelected ? ImGuiTreeNodeFlags_Selected : 0;
                    bool nodeOpen                         = ImGui::TreeNodeEx(category, nodeFlags | nodeSelectionFlags);

                    if (nodeOpen)
                    {
                        while (i < g_sampleCount && strcmp(category, g_sampleEntries[i].category) == 0)
                        {
                            ImGuiTreeNodeFlags selectionFlags = 0;
                            if (s_context.sampleIndex == i)
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

        m_sample->UpdateGui();
    }
    /// END UpdateUI

    if (s_context.showUI)
    {
        snprintf(buffer, 128, "%.1f ms - step %d - camera (%g, %g, %g)", 1000.0f * _director->getDeltaTime(),
                 m_sample->m_stepCount, s_context.camera.center.x, s_context.camera.center.y, s_context.camera.zoom);
        ImGui::Begin("Overlay", nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize |
                         ImGuiWindowFlags_NoScrollbar);
        ImGui::SetCursorPos(ImVec2(5.0f, s_context.camera.height - 50.0f));
        ImGui::TextColored(ImColor(153, 230, 153, 255), "%s", buffer);
        ImGui::End();
    }
}
