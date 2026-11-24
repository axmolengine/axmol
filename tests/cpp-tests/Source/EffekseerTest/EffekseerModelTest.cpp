/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.
 SPDX-License-Identifier: MIT
 ****************************************************************************/
#include "EffekseerTest.h"
#include "axmol/renderer/ProgramManager.h"
#include "axmol/rhi/GraphicsCore.h"

namespace
{
using namespace ax;
using ModelMaterial = Effekseer::RendererMaterialType;

struct ModelCase
{
    const char* name;
    ModelMaterial material;
    bool advanced = false;
    bool isolation = false;
    bool wireframe = false;
};

class EffekseerModelTest final : public EffekseerViewTest
{
public:
    static Scene* create(ModelCase sample)
    {
        auto test = new EffekseerModelTest();
        test->_sample = sample;
        if (!test->init())
        {
            test->release();
            return nullptr;
        }
        test->autorelease();
        return test;
    }

    bool init() override
    {
        if (!EffekseerViewTest::init())
            return false;
        auto size = _director->getVisibleSize();
        _manager.reset(efk::EffectManager::create(size), tlx::adopt_object);
        _mesh = efk::Effect::create("Simple_Model_UV.efkefc");
        _status = Label::createWithSystemFont("", "Arial", 16);
        _status->setPosition({size.width / 2, size.height * 0.18f});
        addChild(_status, 5);
        if (!_manager || !_mesh || !_mesh->getInternalPtr()->GetModel(0))
        {
            report(false, "Missing model renderer or packaged Simple_Model_UV/Model/Arrow");
            return true;
        }
        const bool distortion = _sample.material == ModelMaterial::BackDistortion;
        if (distortion)
            _resources = efk::Effect::create("Simple_Distortion.efkefc"); // Cloud01 distortion map
        else if (_sample.advanced)
            _resources = efk::Effect::create("Laser01.efkefc"); // separate color, alpha and blend maps
        else
            _resources = _mesh;
        if (!_resources || (distortion && !_resources->getInternalPtr()->GetDistortionImage(0)) ||
            (!distortion && !_resources->getInternalPtr()->GetColorImage(0)) ||
            (_sample.advanced && !distortion && (!_resources->getInternalPtr()->GetColorImage(1) ||
                                                 !_resources->getInternalPtr()->GetColorImage(2))))
        {
            _resources.reset();
            report(false, "Missing packaged model test textures");
            return true;
        }
        auto renderer = _manager->getInternalRenderer();
        if (_sample.wireframe)
            renderer->SetRenderMode(Effekseer::RenderMode::Wireframe);
        renderer->SetLightDirection({0.7f, 0.3f, 0.6f});
        renderer->SetLightColor({255, 238, 205, 255});
        renderer->SetLightAmbientColor({38, 38, 46, 255});
        if (distortion && !installCheckerboard())
        {
            _resources.reset();
            report(false, "Cannot create distortion background");
            return true;
        }
        if (_sample.isolation)
        {
            using Policy = ProgramManager::LoadPolicy;
            auto pm = ProgramManager::getInstance();
            // Intentional errors, only when this regression case is opened. On GL
            // mismatched varyings fail at link time; other RHIs link during PSO creation.
            _loadChecks = !pm->loadProgram("custom/missing_effekseer_model_vs", "custom/model_ps",
                                           VertexLayoutKind::Invalid, Policy::Optional);
            if (rhi::GraphicsCore::isOpenGL())
                _loadChecks &= !pm->loadProgram("custom/model_vs", "custom/model_ps_1",
                                                VertexLayoutKind::Invalid, Policy::Optional);
            auto sprite = efk::EffectEmitter::create(_manager.get(), "Homing_Laser01.efk");
            sprite->setIsLooping(true);
            sprite->setCameraMask(static_cast<unsigned short>(CameraFlag::USER1));
            sprite->setPosition3D(Vec3(0, 1, 0));
            sprite->setTargetPosition(Vec3(0, 6, 0));
            addChild(sprite);
        }
        // Scene::draw submits the models; change the scene's mask without its UI children.
        setCameraMask(getCameraMask() | static_cast<unsigned short>(CameraFlag::USER1), false);
        scheduleUpdate();
        return true;
    }
    std::string title() const override { return _sample.name; }
    std::string subtitle() const override
    {
        if (_sample.wireframe) return "Drag to orbit: both arrows should show only mesh edges";
        if (_sample.isolation) return "Missing/bad shaders and no background: arrows and laser should survive";
        if (_sample.material == ModelMaterial::BackDistortion) return "Drag to orbit: look for two displaced patches in the checkerboard";
        return _sample.advanced ? "Drag to orbit two masked arrows with different colors and UVs"
                                : "Drag to orbit the warm near arrow and cool far arrow";
    }
    void update(float delta) override
    {
        if (_manager && _manager->getInternalManager())
            _manager->update(delta);
    }
    void visit(const SceneRenderState& state, const Mat4& transform, uint32_t flags) override
    {
        const bool effectPass = _manager && state.getCamera() == getEffectCamera();
        if (effectPass)
            _manager->begin(state, _globalZOrder);
        Scene::visit(state, transform, flags);
        if (effectPass)
            _manager->end(state, _globalZOrder);
    }
    void draw(const SceneRenderState& state, const Mat4& transform, uint32_t flags) override
    {
        if (state.getCamera() != getEffectCamera() || !_manager || !_manager->getInternalManager() ||
            !_mesh || !_resources || !_mesh->getInternalPtr() ||
            !_resources->getInternalPtr() || !_mesh->getInternalPtr()->GetModel(0))
            return;
        auto renderer = _manager->getInternalRenderer();
        renderer->BeginRendering();
        bool ok = _loadChecks;
        if (_sample.isolation)
        {
            // No capture callback and no external background. Only distortion must skip.
            ok &= submit(ModelMaterial::BackDistortion, false) == 0;
        }
        ok &= submit(_sample.material, _sample.advanced) == 2;
        renderer->EndRendering();
        state.getRenderer()->addDrawnBatches(renderer->GetDrawCallCount());
        state.getRenderer()->addDrawnVertices(renderer->GetDrawVertexCount());
        renderer->ResetDrawCallCount();
        renderer->ResetDrawVertexCount();
        report(ok, ok ? "2 model draws submitted" : "Unexpected program load or model draw count");
        Scene::draw(state, transform, flags);
    }

private:
    // Supply the same known image as the on-screen background and as the
    // distortion source. This exercises the shader independently of whether
    // the window framebuffer can be copied. Simple_Distortion tests live capture.
    bool installCheckerboard()
    {
        constexpr int columns = 12, rows = 8, tile = 8;
        constexpr int w = columns * tile, h = rows * tile;
        const Color32 colors[] = {{24, 48, 88, 255}, {185, 146, 86, 255}};
        Effekseer::CustomVector<uint8_t> rgba(w * h * 4);
        for (int y = 0; y < h; ++y)
            for (int x = 0; x < w; ++x)
            {
                const auto& c = colors[(x / tile + y / tile) % 2];
                const auto i = (y * w + x) * 4;
                rgba[i] = c.r; rgba[i+1] = c.g; rgba[i+2] = c.b; rgba[i+3] = 255;
            }
        auto renderer = _manager->getInternalRenderer();
        Effekseer::Backend::TextureParameter desc;
        desc.Size = {w, h, 1};
        auto texture = renderer->GetGraphicsDevice()->CreateTexture(desc, rgba);
        if (!texture)
            return false;
        renderer->SetBackground(texture);
        // Keep the backdrop aligned to screen UVs as the perspective camera orbits.
        // Draw it before the world grid, in the same camera pass as the models.
        auto* camera = getEffectCamera();
        constexpr float distance = 100.0f;
        const auto& projection = camera->getProjectionMatrix();
        const Vec2 halfSize(distance / projection.m[0], distance / projection.m[5]);
        const Vec2 cell(2 * halfSize.x / columns, 2 * halfSize.y / rows);
        auto board = DrawNode::create();
        for (int y = 0; y < rows; ++y)
            for (int x = 0; x < columns; ++x)
            {
                const Vec2 corner(x * cell.x, y * cell.y);
                board->drawSolidRect(corner, corner + cell, Color(colors[(x + y) % 2]));
            }
        board->setPosition3D(Vec3(-halfSize.x, -halfSize.y, -distance));
        board->setCameraMask(static_cast<unsigned short>(CameraFlag::USER1));
        camera->setLocalZOrder(-20);
        camera->addChild(board);
        return true;
    }
    int submit(ModelMaterial material, bool advanced)
    {
        using namespace Effekseer;
        auto renderer = _manager->getInternalRenderer();
        auto modelRenderer = _manager->getInternalManager()->GetModelRenderer();
        if (!modelRenderer)
            return 0;
        NodeRendererBasicParameter basic;
        basic.MaterialType = material;
        basic.TextureIndexes[0] = 0;
        basic.DistortionIntensity = 3.0f;
        if (advanced)
        {
            basic.TextureIndexes[2] = material == ModelMaterial::BackDistortion ? 0 : 1; // alpha
            basic.TextureIndexes[4] = material == ModelMaterial::BackDistortion ? 0 : 2; // blend
            basic.TextureBlendType = 1;
            basic.TextureFilters[2] = TextureFilterType::Linear;
            basic.TextureWraps[2] = TextureWrapType::Clamp;
        }
        NodeRendererDepthParameter depth;
        ModelRenderer::NodeParameter node{};
        node.EffectPointer = _resources->getInternalPtr().Get();
        node.IsExternalMode = true;
        node.ExternalModel = _mesh->getInternalPtr()->GetModel(0);
        node.BasicParameterPtr = &basic;
        node.DepthParameterPtr = &depth;
        node.IsRightHand = true;
        node.Magnification = 1.0f;
        node.Billboard = BillboardType::Fixed;
        node.Culling = CullingType::Double;
        node.ZTest = true;

        auto before = renderer->GetDrawCallCount();
        modelRenderer->BeginRendering(node, 2, nullptr);
        for (int i = 0; i < 2; ++i)
        {
            ModelRenderer::InstanceParameter instance{.Direction = SIMD::Vec3f(0, 1, 0)};
            instance.UV = instance.AlphaUV = instance.BlendUV = instance.BlendAlphaUV =
                instance.UVDistortionUV = instance.BlendUVDistortionUV = RectF(0, 0, 1, 1);
            if (advanced)
                instance.AlphaUV = RectF(i * 0.15f, 0.2f, 0.6f, 0.6f);
            instance.SRTMatrix43 = SIMD::Mat43f::SRT(
                SIMD::Vec3f(4, 4, 4), SIMD::Mat43f::RotationXYZ(0.2f, i ? -0.55f : 0.55f, i ? -0.6f : 0.6f),
                i ? SIMD::Vec3f(5, 3, -3) : SIMD::Vec3f(-5, 3, 3));
            instance.AllColor = i ? Effekseer::Color(110,255,255,255) : Effekseer::Color(255,170,100,255);
            modelRenderer->Rendering(node, instance, nullptr);
        }
        modelRenderer->EndRendering(node, nullptr);
        return renderer->GetDrawCallCount() - before;
    }
    void report(bool ok, const char* text)
    {
        auto message = fmt::format("{}: {}", ok ? "OK" : "FAIL", text);
        if (_status->getString() == message)
            return;
        _status->setString(message);
        if (ok)
            AXLOGI("{}: {}", _sample.name, message);
        else
            AXLOGE("{}: {}", _sample.name, message);
    }

    ModelCase _sample{};
    RefPtr<efk::EffectManager> _manager;
    RefPtr<efk::Effect> _mesh, _resources;
    Label* _status = nullptr;
    bool _loadChecks = true;
};
} // namespace

void addEffekseerModelTests(TestSuite& suite)
{
    constexpr ModelCase cases[] = {
        {"Model Unlit", ModelMaterial::Default},
        {"Model Lit", ModelMaterial::Lighting},
        {"Model BackDistortion", ModelMaterial::BackDistortion},
        {"Advanced Model Unlit", ModelMaterial::Default, true},
        {"Advanced Model Lit", ModelMaterial::Lighting, true},
        {"Advanced Model BackDistortion", ModelMaterial::BackDistortion, true},
        {"Model Wireframe", ModelMaterial::Default, false, false, true},
        {"Model failure isolation", ModelMaterial::Default, false, true},
    };
    for (auto sample : cases)
        suite.addTestCase(sample.name, [sample] { return EffekseerModelTest::create(sample); });
}
