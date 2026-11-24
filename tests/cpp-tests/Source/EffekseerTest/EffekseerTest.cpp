/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
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
    {"Simple_Model_UV.efkefc",    "Simple Model UV",        1.0f},

    // GPU particles (require compute/storage/Texture3D backend support)
    {"GpuParticles_sprite_simple.efkefc",    "GPU Sprite Simple",    1.0f},
    {"GpuParticles_sprite_gradient.efkefc",  "GPU Sprite Gradient",  1.0f},
    {"GpuParticles_force_turbulence.efkefc", "GPU Force Turbulence", 1.0f},
    {"GpuParticles_emit_mesh.efkefc",        "GPU Emit Mesh",        1.0f},
    {"GpuParticles_trails_simple.efkefc",    "GPU Trails Simple",    1.0f},
    {"GpuParticles_mesh_simple.efkefc",      "GPU Mesh Simple",      1.0f},
};
// clang-format on

namespace
{
class EffekseerPlainSamplerTest final : public TestCase
{
public:
    CREATE_FUNC(EffekseerPlainSamplerTest);

    std::string title() const override { return "Plain particle wrap and filter"; }
    std::string subtitle() const override
    {
        return "Same texture, UV 0..2. Left: Repeat. Right: Clamp. Top: Nearest. Bottom: Linear.";
    }

    bool init() override
    {
        if (!TestCase::init())
            return false;
        _manager.reset(efk::EffectManager::create(_director->getVisibleSize()), tlx::adopt_object);
        _effect = efk::Effect::create("Simple_Model_UV.efkefc");  // only use the Arrow color texture
        if (!_manager || !_effect || !_effect->getInternalPtr()->GetColorImage(0))
        {
            auto* label = Label::createWithSystemFont("Missing Effekseer renderer or Arrow texture", "Arial", 16);
            label->setPosition(_director->getVisibleSize() * 0.5f);
            addChild(label);
        }
        return true;
    }

    void draw(const SceneRenderState& state, const Mat4&, uint32_t) override
    {
        if (!_manager || !_manager->getInternalManager() || !_effect || !_effect->getInternalPtr())
            return;
        auto renderer = _manager->getInternalRenderer();
        auto sprite   = _manager->getInternalManager()->GetSpriteRenderer();
        auto effect   = _effect->getInternalPtr();
        if (!sprite || !effect->GetColorImage(0))
            return;

        using namespace Effekseer;
        NodeRendererDepthParameter depth;
        NodeRendererBasicParameter material;
        material.TextureIndexes[0] = 0;
        SpriteRenderer::NodeParameter node{};
        node.EffectPointer     = effect.Get();
        node.Billboard         = BillboardType::Fixed;
        node.IsRightHand       = true;
        node.Maginification    = 1.0f;
        node.DepthParameterPtr = &depth;
        node.BasicParameterPtr = &material;

        const auto size      = _director->getVisibleSize();
        const float halfSize = size.height * 0.145f;
        SpriteRenderer::InstanceParameter quad{
            .Positions = {SIMD::Vec2f(-halfSize, -halfSize), SIMD::Vec2f(halfSize, -halfSize),
                          SIMD::Vec2f(-halfSize, halfSize), SIMD::Vec2f(halfSize, halfSize)},
            .Direction = SIMD::Vec3f(0, 0, 1)};
        quad.AllColor = Effekseer::Color(255, 255, 255, 255);
        for (auto& color : quad.Colors)
            color = quad.AllColor;
        quad.UV = RectF(0, 0, 2, 2);

        _manager->begin(state, _globalZOrder);
        renderer->BeginRendering();
        for (int row = 0; row != 2; ++row)
        {
            for (int column = 0; column != 2; ++column)
            {
                material.TextureWraps[0]   = column == 0 ? TextureWrapType::Repeat : TextureWrapType::Clamp;
                material.TextureFilters[0] = row == 0 ? TextureFilterType::Nearest : TextureFilterType::Linear;
                quad.SRTMatrix43           = SIMD::Mat43f::Translation(size.width * (column == 0 ? 0.29f : 0.71f),
                                                                       size.height * (row == 0 ? 0.60f : 0.28f), 0);
                sprite->BeginRendering(node, 1, nullptr);
                sprite->Rendering(node, quad, nullptr);
                sprite->EndRendering(node, nullptr);
            }
        }
        renderer->EndRendering();
        _manager->end(state, _globalZOrder);
        state.getRenderer()->addDrawnBatches(renderer->GetDrawCallCount());
        state.getRenderer()->addDrawnVertices(renderer->GetDrawVertexCount());
        renderer->ResetDrawCallCount();
        renderer->ResetDrawVertexCount();
    }

private:
    RefPtr<efk::EffectManager> _manager;
    RefPtr<efk::Effect> _effect;
};
}  // namespace

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
    addEffekseerModelTests(*this);
    ADD_TEST_CASE(EffekseerPlainSamplerTest);
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

bool EffekseerViewTest::init()
{
    if (!TestCase::init())
        return false;

    constexpr float radius = 24.0f;
    constexpr float height = 10.0f;
    _effectCamera          = Camera::create(CameraMode::Perspective);
    _effectCamera->setCameraFlag(CameraFlag::USER1);
    _effectCamera->setPosition3D(Vec3(0, height, radius));
    _effectCamera->lookAt(Vec3::zero, Vec3(0, 1, 0));
    _effectCamera->setBackgroundBrush(CameraBackgroundBrush::createColorBrush(Color(Color32(0, 0, 0)), 1.0f));
    addChild(_effectCamera);

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

    auto orbit              = PointerEventListener::create();
    orbit->onPointerHitTest = [](PointerEvent* event, Vec3* hit) {
        // Drag on the whole viewport, not just where the pointer ray hits scene geometry.
        if (event->getCamera()->getCameraFlag() != CameraFlag::DEFAULT)
            return false;
        const auto point = event->getWorldPoint();
        *hit             = Vec3(point.x, point.y, 0);
        return true;
    };
    orbit->onPointerDown = [](PointerEvent* event) { return event->isPrimaryPressed(); };
    orbit->onPointerMove = [this](PointerEvent* event) {
        if (!event->isCaptured())
            return;
        // Use pointer displacement on screen so orbit speed is independent of the camera angle.
        _orbitAngle -= MathUtil::radians(event->getPoint().x - event->getPrevPoint().x);
        _effectCamera->setPosition3D(Vec3(radius * sinf(_orbitAngle), height, radius * cosf(_orbitAngle)));
        _effectCamera->lookAt(Vec3::zero, Vec3(0, 1, 0));
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(orbit, this);
    return true;
}

bool EffekseerTest::init()
{
    if (!EffekseerViewTest::init())
        return false;

    auto visibleSize = _director->getVisibleSize();

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
    // Leave the bottom row free for the suite's model test navigation.
    prevBtn->setPosition(Vec2(visibleSize.width * 0.25f, visibleSize.height * 0.22f));
    prevBtn->addClickEventListener([this](auto*) { switchEffect(-1); });
    addChild(prevBtn, 10);

    // Next button
    auto nextBtn = ui::Button::create();
    nextBtn->setTitleText("Next >>");
    nextBtn->setTitleFontSize(20);
    nextBtn->setPosition(Vec2(visibleSize.width * 0.75f, visibleSize.height * 0.22f));
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
