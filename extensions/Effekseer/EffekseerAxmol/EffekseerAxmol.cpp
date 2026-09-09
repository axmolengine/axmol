#include "EffekseerAxmol.h"
#include "EffekseerRendererAxmol.h"

#include <algorithm>
#include <vector>

namespace efk
{
class EffekseerAxmolLifecycle
{
public:
    static void releaseGraphicsResources(Effect* effect)
    {
        if (effect)
            effect->releaseGraphicsResources();
    }

    static void releaseGraphicsResources(EffectManager* manager)
    {
        if (manager)
            manager->releaseGraphicsResources();
    }
};

namespace
{
std::vector<Effect*> g_effects;
std::vector<EffectManager*> g_managers;
ax::EventListener* g_beforeGfxDropListener = nullptr;
bool g_isBeforeGfxDrop = false;

void CalculateCameraDirectionAndPosition(const Effekseer::Matrix44& matrix, Effekseer::Vector3D& direction, Effekseer::Vector3D& position)
{
    const auto& mat = matrix;
    direction = -Effekseer::Vector3D(matrix.Values[0][2], matrix.Values[1][2], matrix.Values[2][2]);
    auto localPos = Effekseer::Vector3D(-mat.Values[3][0], -mat.Values[3][1], -mat.Values[3][2]);
    auto f = Effekseer::Vector3D(mat.Values[0][2], mat.Values[1][2], mat.Values[2][2]);
    auto r = Effekseer::Vector3D(mat.Values[0][0], mat.Values[1][0], mat.Values[2][0]);
    auto u = Effekseer::Vector3D(mat.Values[0][1], mat.Values[1][1], mat.Values[2][1]);
    position = r * localPos.X + u * localPos.Y + f * localPos.Z;
}

class FileReader : public Effekseer::FileReader
{
public:
    explicit FileReader(std::vector<uint8_t> data) : _data(std::move(data)) {}

    size_t Read(void* buffer, size_t size) override
    {
        const auto readable = std::min<size_t>(size, _data.size() - _position);
        memcpy(buffer, _data.data() + _position, readable);
        _position += readable;
        return readable;
    }

    void Seek(int position) override { _position = std::min<size_t>(std::max(position, 0), _data.size()); }
    int GetPosition() const override { return static_cast<int>(_position); }
    size_t GetLength() const override { return _data.size(); }

private:
    std::vector<uint8_t> _data;
    size_t _position = 0;
};

class FileInterface : public Effekseer::FileInterface
{
public:
    Effekseer::FileReaderRef OpenRead(const EFK_CHAR* path) override
    {
        char utf8[512];
        Effekseer::ConvertUtf16ToUtf8(utf8, sizeof(utf8), path);
        auto data = ax::FileUtils::getInstance()->getDataFromFile(utf8);
        if (data.isNull())
            return nullptr;
        std::vector<uint8_t> bytes(data.getBytes(), data.getBytes() + data.getSize());
        return Effekseer::MakeRefPtr<FileReader>(bytes);
    }

    Effekseer::FileWriterRef OpenWrite(const EFK_CHAR* path) override
    {
        AX_UNUSED_PARAM(path);
        return nullptr;
    }
};

class Setting : public Effekseer::Setting
{
public:
    explicit Setting(EffekseerRenderer::RendererRef renderer)
    {
        _file = Effekseer::MakeRefPtr<FileInterface>();
        SetEffectLoader(Effekseer::Effect::CreateEffectLoader(_file));
        SetTextureLoader(renderer->CreateTextureLoader(_file));
        SetModelLoader(renderer->CreateModelLoader(_file));
        SetMaterialLoader(renderer->CreateMaterialLoader(_file));
        SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>(_file));
        // GPU particle resources are created while Effect::Create parses the
        // effect. The factory must therefore be installed on this setting,
        // not only on the manager used later for simulation.
        SetGpuParticleFactory(renderer->CreateGpuParticleFactory());
    }

private:
    Effekseer::FileInterfaceRef _file;
};

} // namespace

struct EffectResource
{
    Effekseer::EffectRef effect;
    int counter = 0;
};

class InternalManager : public Effekseer::ReferenceObject
{
public:
    Effekseer::EffectRef loadEffect(EffekseerRenderer::RendererRef renderer, const EFK_CHAR* path, float magnification)
    {
        auto it = _pathToEffect.find(path);
        if (it != _pathToEffect.end())
        {
            it->second.counter++;
            return it->second.effect;
        }

        auto setting = Effekseer::MakeRefPtr<Setting>(renderer);
        EffectResource resource;
        resource.effect = Effekseer::Effect::Create(setting.DownCast<Effekseer::Setting>(), path, magnification);
        resource.counter = 1;
        if (!resource.effect)
            return nullptr;

        _effectToPath[resource.effect] = path;
        _pathToEffect[path] = resource;
        return resource.effect;
    }

    void unloadEffect(Effekseer::EffectRef effect)
    {
        auto pit = _effectToPath.find(effect);
        if (pit == _effectToPath.end())
            return;

        auto eit = _pathToEffect.find(pit->second);
        if (eit != _pathToEffect.end() && --eit->second.counter == 0)
        {
            _pathToEffect.erase(eit);
            _effectToPath.erase(pit);
        }
    }

    void clearEffects()
    {
        _pathToEffect.clear();
        _effectToPath.clear();
    }

private:
    std::map<std::u16string, EffectResource> _pathToEffect;
    std::map<Effekseer::EffectRef, std::u16string> _effectToPath;
};

namespace
{
InternalManager*& getInternalManagerSingleton()
{
    static InternalManager* manager = nullptr;
    return manager;
}

void clearInternalManagerSingletonGraphicsResources()
{
    auto*& manager = getInternalManagerSingleton();
    if (!manager)
        return;

    manager->clearEffects();
}

void removeEffect(Effect* effect)
{
    auto it = std::find(g_effects.begin(), g_effects.end(), effect);
    if (it != g_effects.end())
        g_effects.erase(it);
}

void removeManager(EffectManager* manager)
{
    auto it = std::find(g_managers.begin(), g_managers.end(), manager);
    if (it != g_managers.end())
        g_managers.erase(it);
}

void releaseEffekseerGraphicsResources()
{
    g_isBeforeGfxDrop = true;

    auto managers = g_managers;
    for (auto* manager : managers)
        EffekseerAxmolLifecycle::releaseGraphicsResources(manager);

    auto effects = g_effects;
    for (auto* effect : effects)
        EffekseerAxmolLifecycle::releaseGraphicsResources(effect);

    clearInternalManagerSingletonGraphicsResources();
}

void ensureBeforeGfxDropListener()
{
    if (g_beforeGfxDropListener || g_isBeforeGfxDrop)
        return;

    auto eventDispatcher = ax::Director::getInstance()->getEventDispatcher();
    g_beforeGfxDropListener = eventDispatcher->addCustomEventListener(ax::Director::EVENT_BEFORE_GFX_DROP,
                                                                      [eventDispatcher](ax::CustomEvent*) {
        releaseEffekseerGraphicsResources();
        if (g_beforeGfxDropListener)
        {
            eventDispatcher->removeEventListener(g_beforeGfxDropListener);
            g_beforeGfxDropListener = nullptr;
        }
    });
}

InternalManager* acquireInternalManager()
{
    ensureBeforeGfxDropListener();

    auto*& manager = getInternalManagerSingleton();
    if (!manager)
    {
        manager = new InternalManager();
        manager->AddRef();
    }
    manager->AddRef();
    return manager;
}
} // namespace

Effect* Effect::create(const std::string& filename, float magnification)
{
    auto app = ax::Director::getInstance()->getRunningScene();
    AX_UNUSED_PARAM(app);
    EFK_CHAR path[512];
    Effekseer::ConvertUtf8ToUtf16(path, sizeof(path) / sizeof(path[0]), filename.c_str());

    auto internalManager = acquireInternalManager();
    auto renderer = EffekseerRendererAxmol::Renderer::Create(1);
    auto effect = internalManager->loadEffect(renderer, path, magnification);
    if (!effect)
    {
        ES_SAFE_RELEASE(internalManager);
        return nullptr;
    }

    auto ret = new Effect(internalManager);
    ret->_effect = effect;
    ret->autorelease();
    ES_SAFE_RELEASE(internalManager);
    return ret;
}

Effect::Effect(InternalManager* internalManager) : _internalManager(internalManager)
{
    ES_SAFE_ADDREF(_internalManager);
    g_effects.push_back(this);
    ensureBeforeGfxDropListener();
}

Effect::~Effect()
{
    releaseGraphicsResources();
    removeEffect(this);
    ES_SAFE_RELEASE(_internalManager);
}

void Effect::releaseGraphicsResources()
{
    if (_internalManager && _effect)
        _internalManager->unloadEffect(_effect);
    _effect = nullptr;
}

EffectEmitter* EffectEmitter::create(EffectManager* manager)
{
    auto ret = new EffectEmitter(manager);
    ret->autorelease();
    return ret;
}

EffectEmitter* EffectEmitter::create(EffectManager* manager, const std::string& filename, float magnification)
{
    auto ret = create(manager);
    ret->setEffect(Effect::create(filename, magnification));
    ret->setPlayOnEnter(true);
    return ret;
}

EffectEmitter::EffectEmitter(EffectManager* manager) : _manager(manager)
{
    AX_SAFE_RETAIN(_manager);
    _dynamicInputs.fill(0.0f);
}

EffectEmitter::~EffectEmitter()
{
    AX_SAFE_RELEASE(_effect);
    AX_SAFE_RELEASE(_manager);
}

Effect* EffectEmitter::getEffect() { return _effect; }

void EffectEmitter::setEffect(Effect* effect)
{
    AX_SAFE_RETAIN(effect);
    AX_SAFE_RELEASE(_effect);
    _effect = effect;
}

void EffectEmitter::play() { play(0); }

void EffectEmitter::play(int32_t startTime)
{
    if (!_manager || !_effect)
        return;
    _handle = _manager->play(_effect, 0, 0, 0, startTime);
    _manager->setMatrix(_handle, getNodeToWorldTransform());
    _playedAtLeastOnce = true;
    setTargetPosition(_targetPosition);
    setColor(_color);
    setSpeed(_speed);
    for (size_t i = 0; i < _dynamicInputs.size(); i++)
        setDynamicInput(static_cast<int32_t>(i), _dynamicInputs[i]);
}

void EffectEmitter::setColor(ax::Color32 color)
{
    _color = color;
    auto mgr = _manager ? _manager->getInternalManager() : nullptr;
    if (mgr)
        mgr->SetAllColor(_handle, Effekseer::Color(color.r, color.g, color.b, color.a));
}

void EffectEmitter::setSpeed(float speed)
{
    _speed = speed;
    auto mgr = _manager ? _manager->getInternalManager() : nullptr;
    if (mgr)
        mgr->SetSpeed(_handle, speed);
}

void EffectEmitter::setTargetPosition(ax::Vec3 position)
{
    _targetPosition = position;
    auto mgr = _manager ? _manager->getInternalManager() : nullptr;
    if (mgr)
        mgr->SetTargetLocation(_handle, position.x, position.y, position.z);
}

float EffectEmitter::getDynamicInput(int32_t index) { return _dynamicInputs.at(index); }

void EffectEmitter::setDynamicInput(int32_t index, float value)
{
    _dynamicInputs.at(index) = value;
    auto mgr = _manager ? _manager->getInternalManager() : nullptr;
    if (mgr)
        mgr->SetDynamicInput(_handle, index, value);
}

bool EffectEmitter::isPlaying() { return _manager && _manager->getInternalManager() && _manager->getInternalManager()->Exists(_handle); }
void EffectEmitter::stop() { if (_manager && _manager->getInternalManager()) _manager->getInternalManager()->StopEffect(_handle); }
void EffectEmitter::stopRoot() { if (_manager && _manager->getInternalManager()) _manager->getInternalManager()->StopRoot(_handle); }

void EffectEmitter::onEnter()
{
    ax::Node::onEnter();
    if (_playOnEnter)
        play();
    scheduleUpdate();
}

void EffectEmitter::onExit()
{
    auto mgr = _manager ? _manager->getInternalManager() : nullptr;
    if (mgr && mgr->Exists(_handle))
        mgr->StopEffect(_handle);
    ax::Node::onExit();
}

void EffectEmitter::update(float delta)
{
    auto mgr = _manager ? _manager->getInternalManager() : nullptr;
    if (mgr && !mgr->Exists(_handle))
    {
        if (_isLooping)
            play();
        else if (_removeOnStop && _playedAtLeastOnce)
        {
            removeFromParent();
            return;
        }
    }
    if (_manager)
        _manager->setMatrix(_handle, getNodeToWorldTransform());
    ax::Node::update(delta);
}

void EffectEmitter::draw(const ax::SceneRenderState& state, const ax::Mat4& parentTransform, uint32_t parentFlags)
{
    auto mgr = _manager ? _manager->getInternalManager() : nullptr;
    if (!mgr || !mgr->GetShown(_handle))
        return;

    _manager->markEmitterDrawn();
    auto renderer = _manager->getInternalRenderer();
    static_cast<EffekseerRendererAxmol::Renderer*>(renderer.Get())->SetGlobalZOrder(_globalZOrder);
    renderer->BeginRendering();
    mgr->DrawHandle(_handle);
    renderer->EndRendering();
    state.getRenderer()->addDrawnBatches(renderer->GetDrawCallCount());
    state.getRenderer()->addDrawnVertices(renderer->GetDrawVertexCount());
    renderer->ResetDrawCallCount();
    renderer->ResetDrawVertexCount();

    ax::Node::draw(state, parentTransform, parentFlags);
}

EffectManager* EffectManager::create(ax::Size visibleSize)
{
    auto ret = new EffectManager();
    if (ret->initialize(visibleSize))
    {
        return ret;
    }
    ret->release();
    return nullptr;
}

EffectManager::EffectManager()
{
    g_managers.push_back(this);
    ensureBeforeGfxDropListener();
}

EffectManager::~EffectManager()
{
    releaseGraphicsResources();
    removeManager(this);
    ES_SAFE_RELEASE(_internalManager);
}

void EffectManager::releaseGraphicsResources()
{
    if (_manager)
        _manager->StopAllEffects();

    if (_distortionCallback)
    {
        if (_renderer)
            _renderer->SetDistortingCallback(nullptr);
        delete _distortionCallback;
        _distortionCallback = nullptr;
    }
    if (_renderer)
        static_cast<EffekseerRendererAxmol::Renderer*>(_renderer.Get())->ReleaseCachedCommands();

    _manager = nullptr;
    _renderer = nullptr;
    _lastComputedFrame = ~0u;
    _didDrawEmitterInPass = false;
}

bool EffectManager::initialize(ax::Size visibleSize)
{
    _renderer = EffekseerRendererAxmol::Renderer::Create(4000);
    if (!_renderer)
        return false;

    _manager = Effekseer::Manager::Create(8000);
    _manager->SetSpriteRenderer(_renderer->CreateSpriteRenderer());
    _manager->SetRibbonRenderer(_renderer->CreateRibbonRenderer());
    _manager->SetRingRenderer(_renderer->CreateRingRenderer());
    _manager->SetTrackRenderer(_renderer->CreateTrackRenderer());

    // Register the GPU particle system when the backend supports compute/storage/Texture3D.
    Effekseer::GpuParticleSystem::Settings gpuSettings;
    auto gpuFactory = _renderer->CreateGpuParticleFactory();
    auto gpuSystem = _renderer->CreateGpuParticleSystem(gpuSettings);
    if (gpuFactory && gpuSystem)
    {
        _manager->SetGpuParticleFactory(gpuFactory);
        _manager->SetGpuParticleSystem(gpuSystem);
    }

    _renderer->SetProjectionMatrix(Effekseer::Matrix44().OrthographicRH(visibleSize.width, visibleSize.height, 1.0f, 400.0f));
    _renderer->SetCameraMatrix(Effekseer::Matrix44().LookAtRH(Effekseer::Vector3D(visibleSize.width / 2.0f, visibleSize.height / 2.0f, 200.0f),
                                                              Effekseer::Vector3D(visibleSize.width / 2.0f, visibleSize.height / 2.0f, -200.0f),
                                                              Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));
    _internalManager = acquireInternalManager();
    return true;
}

Effekseer::Handle EffectManager::play(Effect* effect, float x, float y, float z, int32_t startTime)
{
    if (startTime == 0)
        return _manager->Play(effect->getInternalPtr(), x, y, z);
    return _manager->Play(effect->getInternalPtr(), Effekseer::Vector3D(x, y, z), startTime);
}

void EffectManager::setMatrix(Effekseer::Handle handle, const ax::Mat4& mat)
{
    Effekseer::Matrix43 efkMat;
    const float* p = mat.m;
    memcpy(efkMat.Value[0], p, sizeof(float) * 3);
    p += 4;
    memcpy(efkMat.Value[1], p, sizeof(float) * 3);
    p += 4;
    memcpy(efkMat.Value[2], p, sizeof(float) * 3);
    p += 4;
    memcpy(efkMat.Value[3], p, sizeof(float) * 3);
    _manager->SetMatrix(handle, efkMat);
}

void EffectManager::begin(const ax::SceneRenderState& state, float globalZOrder)
{
    if (!_manager || !_renderer)
        return;

    _didDrawEmitterInPass = false;
    auto renderer = static_cast<EffekseerRendererAxmol::Renderer*>(_renderer.Get());
    renderer->SetGlobalZOrder(globalZOrder);
    renderer->BeginFrame(state.getRenderer());
    setCameraMatrix(state.getViewMatrix());
    setProjectionMatrix(state.getProjectionMatrix());

    // SceneCompositor visits the scene once per camera. Simulation advances
    // once per Director frame, while rendering remains camera-pass specific.
    const auto frame = ax::Director::getInstance()->getTotalFrames();
    if (_lastComputedFrame != frame)
    {
        _manager->Compute();
        _lastComputedFrame = frame;
    }
}

void EffectManager::end(const ax::SceneRenderState& state, float globalZOrder)
{
    AX_UNUSED_PARAM(state);
    if (!_manager || !_renderer)
        return;

    auto renderer = static_cast<EffekseerRendererAxmol::Renderer*>(_renderer.Get());
    renderer->SetGlobalZOrder(globalZOrder);

    // Axmol renders CPU effects per node through Manager::DrawHandle(), which
    // does not invoke Effekseer's global GPU particle render pass. Submit that
    // pass once after all emitter nodes have enqueued their CPU draw commands.
    if (_didDrawEmitterInPass)
    {
        if (auto gpuParticleSystem = _manager->GetGpuParticleSystem())
        {
            Effekseer::GpuParticleSystem::Context context{};
            context.CoordinateReversed = _manager->GetCoordinateSystem() != Effekseer::CoordinateSystem::RH;
            gpuParticleSystem->RenderFrame(context);
        }
    }
}

void EffectManager::update(float delta)
{
    _manager->Update();
    _time += delta;
    _renderer->SetTime(_time);
}

void EffectManager::setIsDistortionEnabled(bool value)
{
    if (value && !_distortionCallback)
    {
        _distortionCallback = EffekseerRendererAxmol::createDistortingCallback();
        _renderer->SetDistortingCallback(_distortionCallback);
    }
    else if (!value && _distortionCallback)
    {
        _renderer->SetDistortingCallback(nullptr);
        delete _distortionCallback;
        _distortionCallback = nullptr;
    }
}

void EffectManager::setCameraMatrix(const ax::Mat4& mat)
{
    Effekseer::Matrix44 efkMat;
    memcpy(efkMat.Values, mat.m, sizeof(float) * 16);
    Effekseer::Vector3D cameraPosition;
    Effekseer::Vector3D cameraFrontDirection;
    CalculateCameraDirectionAndPosition(efkMat, cameraFrontDirection, cameraPosition);
    Effekseer::Manager::LayerParameter layerParam;
    layerParam.ViewerPosition = cameraPosition;
    _manager->SetLayerParameter(0, layerParam);
    _renderer->SetCameraMatrix(efkMat);
}

void EffectManager::setProjectionMatrix(const ax::Mat4& mat)
{
    Effekseer::Matrix44 efkMat;
    memcpy(efkMat.Values, mat.m, sizeof(float) * 16);
    _renderer->SetProjectionMatrix(efkMat);
}

NetworkServer* NetworkServer::create()
{
    auto ret = new NetworkServer();
    ret->autorelease();
    return ret;
}

bool NetworkServer::makeNetworkServerEnabled(uint16_t port)
{
    AX_UNUSED_PARAM(port);
    AXLOGW("Effekseer network server is not supported by EffekseerAxmol yet.");
    return false;
}
} // namespace efk
