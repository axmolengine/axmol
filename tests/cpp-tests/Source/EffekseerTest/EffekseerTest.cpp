/****************************************************************************
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

#include "EffekseerTest.h"
#include "../MeshRendererTest/DrawNode3D.h"
#include "axmol/ui/Button.h"

using namespace ax;

// clang-format off
static const TestEffect _effects[] = {
    {"Homing_Laser01.efk",        "Homing Laser01",         1.0f, true, Vec3(0.0f, 30.0f, 0.0f)},
    {"Laser01.efkefc",            "Laser01",                1.0f},
    {"Laser02.efkefc",            "Laser02",                1.0f},
    {"Laser03.efkefc",            "Laser03",                1.0f},
    {"Laser04.efkefc",            "Laser04",                1.0f},
    {"TriggerLaser.efkefc",       "Trigger Laser",          1.0f},
    {"Simple_Distortion.efkefc",  "Simple Distortion",      1.0f},
};
// clang-format on

//------------------------------------------------------------------
//
// EffekseerTests
//
//------------------------------------------------------------------

EffekseerTests::EffekseerTests()
{
    auto fu      = FileUtils::getInstance();
    _searchPaths = fu->getSearchPaths();
    fu->addSearchPath("Effekseer", true);

    ADD_TEST_CASE(EffekseerTest);
}

EffekseerTests::~EffekseerTests()
{
    FileUtils::getInstance()->setSearchPaths(_searchPaths);
}

//------------------------------------------------------------------
//
// EffekseerTest
//
//------------------------------------------------------------------

EffekseerTest::EffekseerTest() {}

EffekseerTest::~EffekseerTest()
{
    if (_emitter)
    {
        _emitter->stop();
        _emitter->removeFromParent();
        _emitter->release();
        _emitter = nullptr;
    }

    if (_manager)
    {
        _manager->release();
        _manager = nullptr;
    }
}

std::string EffekseerTest::title() const
{
    return "EffekseerTest";
}

bool EffekseerTest::init()
{
    if (!TestCase::init())
        return false;

    constexpr float hPadding = 300;

    auto visibleSize = _director->getVisibleSize();

    // 3D camera (perspective) for Effekseer effects
    _3dCamera = Camera::create(CameraMode::Perspective);
    _3dCamera->setCameraFlag(CameraFlag::USER1);
    _3dCamera->setPosition3D(Vec3(0, 10, 24));
    _3dCamera->lookAt(Vec3(0, 0, 0), Vec3(0, 1, 0));
    _3dCamera->setBackgroundBrush(CameraBackgroundBrush::createColorBrush(Color(Color32(0, 0, 0)), 1.0f));
    addChild(_3dCamera);

    // 3D grid
    auto grid                   = DrawNode3D::create();
    constexpr int gridLineCount = 20;
    constexpr float gridStep    = 2.0f;
    constexpr float gridExtent  = gridLineCount * gridStep;
    const Color gridColor(0.25f, 0.25f, 0.25f, 0.55f);
    const Color axisXColor(0.75f, 0.20f, 0.20f, 0.85f);
    const Color axisZColor(0.20f, 0.35f, 0.85f, 0.85f);

    for (int i = -gridLineCount; i <= gridLineCount; ++i)
    {
        const float p = i * gridStep;
        grid->drawLine(Vec3(-gridExtent, 0, p), Vec3(gridExtent, 0, p), i == 0 ? axisXColor : gridColor);
        grid->drawLine(Vec3(p, 0, -gridExtent), Vec3(p, 0, gridExtent), i == 0 ? axisZColor : gridColor);
    }
    grid->setCameraMask(static_cast<unsigned short>(CameraFlag::USER1));
    addChild(grid, -10);

    // Create effect manager
    _manager = efk::EffectManager::create(visibleSize);
    _manager->setIsDistortionEnabled(true);

    // Label
    _label = Label::createWithTTF("", "fonts/Marker Felt.ttf", 18);
    _label->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 60));
    addChild(_label, 10);

    // Prev button
    auto prevBtn = ui::Button::create();
    prevBtn->setTitleText("<< Prev");
    prevBtn->setTitleFontSize(20);
    prevBtn->setPosition(Vec2(visibleSize.width - hPadding, 30));
    prevBtn->addClickEventListener([this](auto*) { switchEffect(-1); });
    addChild(prevBtn, 10);

    // Next button
    auto nextBtn = ui::Button::create();
    nextBtn->setTitleText("Next >>");
    nextBtn->setTitleFontSize(20);
    nextBtn->setPosition(Vec2(hPadding, 30));
    nextBtn->addClickEventListener([this](auto*) { switchEffect(1); });
    addChild(nextBtn, 10);

    // Keyboard: left/right arrow
    auto kb          = KeyboardEventListener::create();
    kb->onKeyPressed = [this](KeyboardEvent* event) {
        auto code = event->getKeyCode();
        if (code == KeyboardEvent::KeyCode::KEY_LEFT_ARROW)
            switchEffect(-1);
        if (code == KeyboardEvent::KeyCode::KEY_RIGHT_ARROW)
            switchEffect(1);
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(kb, this);

    // Pointer: drag to orbit camera
    auto pointerListener           = PointerEventListener::create();
    pointerListener->onPointerDown = AX_CALLBACK_1(EffekseerTest::onPointerDown, this);
    pointerListener->onPointerMove = AX_CALLBACK_1(EffekseerTest::onPointerMove, this);
    pointerListener->onPointerUp   = AX_CALLBACK_1(EffekseerTest::onPointerUp, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(pointerListener, this);

    scheduleUpdate();

    // Play first effect
    _currentIndex = 0;
    playCurrentEffect();

    return true;
}

void EffekseerTest::switchEffect(int direction)
{
    constexpr auto EFFECT_COUNT = std::size(_effects);
    _currentIndex               = (_currentIndex + direction + EFFECT_COUNT) % EFFECT_COUNT;
    playCurrentEffect();
}

bool EffekseerTest::onPointerDown(ax::PointerEvent* event)
{
    return true;
}

void EffekseerTest::onPointerMove(ax::PointerEvent* event)
{
    if (!event->isCaptured())
        return;

    auto delta = event->getWorldPoint() - event->getPrevWorldPoint();
    _angle -= MathUtil::radians(delta.x);
    _3dCamera->setPosition3D(Vec3(24.0f * sinf(_angle), 10.0f, 24.0f * cosf(_angle)));
    _3dCamera->lookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
}

void EffekseerTest::onPointerUp(ax::PointerEvent* event) {}

void EffekseerTest::playCurrentEffect()
{
    // Stop old emitter
    if (_emitter)
    {
        _emitter->stop();
        _emitter->removeFromParent();
        _emitter->release();
        _emitter = nullptr;
    }

    const auto& entry = _effects[_currentIndex];

    auto effect = efk::Effect::create(entry.filename, entry.magnification);
    if (effect)
    {
        _emitter = efk::EffectEmitter::create(_manager);
        _emitter->setEffect(effect);
        _emitter->setPlayOnEnter(true);
        _emitter->setCameraMask((int)CameraFlag::USER1);
        _emitter->setIsLooping(true);
        _emitter->retain();
        addChild(_emitter, 0);

        if (entry.hasTargetPosition)
            _emitter->setTargetPosition(entry.targetPosition);
    }

    updateLabel();
}

void EffekseerTest::updateLabel()
{
    if (_label)
    {
        _label->setString(
            fmt::format("[{}/{}] {}", _currentIndex + 1, std::size(_effects), _effects[_currentIndex].displayName));
    }
}

void EffekseerTest::update(float delta)
{
    if (_manager)
        _manager->update(delta);
}

void EffekseerTest::visit(const ax::SceneRenderState& state, const ax::Mat4& parentTransform, uint32_t parentFlags)
{
    if (_manager)
        _manager->begin(state, _globalZOrder);
    Scene::visit(state, parentTransform, parentFlags);
    if (_manager)
        _manager->end(state, _globalZOrder);
}
