#pragma once

#include "axmol/axmol.h"
#include <Effekseer.h>
#include <EffekseerRendererCommon/EffekseerRenderer.Renderer.h>

namespace efk
{
class EffectManager;
class EffekseerAxmolLifecycle;
class InternalManager;

class Effect : public ax::Object
{
    friend class EffekseerAxmolLifecycle;

public:
    static Effect* create(const std::string& filename, float magnification = 1.0f);
    explicit Effect(InternalManager* internalManager = nullptr);
    ~Effect() override;

    Effekseer::EffectRef getInternalPtr() { return _effect; }

private:
    void releaseGraphicsResources();

    Effekseer::EffectRef _effect;
    InternalManager* _internalManager = nullptr;
};

class EffectEmitter : public ax::Node
{
public:
    static EffectEmitter* create(EffectManager* manager);
    static EffectEmitter* create(EffectManager* manager, const std::string& filename, float magnification = 1.0f);

    explicit EffectEmitter(EffectManager* manager);
    ~EffectEmitter() override;

    Effect* getEffect();
    void setEffect(Effect* effect);
    Effekseer::Handle getInternalHandle() const { return _handle; }

    void play();
    void play(int32_t startTime);
    bool getPlayOnEnter() const { return _playOnEnter; }
    void setPlayOnEnter(bool value) { _playOnEnter = value; }
    bool getIsLooping() const { return _isLooping; }
    void setIsLooping(bool value) { _isLooping = value; }
    bool getRemoveOnStop() const { return _removeOnStop; }
    void setRemoveOnStop(bool value) { _removeOnStop = value; }
    void setColor(ax::Color32 color);
    float getSpeed() const { return _speed; }
    void setSpeed(float speed);
    void setTargetPosition(ax::Vec3 position);
    float getDynamicInput(int32_t index);
    void setDynamicInput(int32_t index, float value);
    bool isPlaying();
    void stop();
    void stopRoot();

    void onEnter() override;
    void onExit() override;
    void update(float delta) override;
    void draw(const ax::SceneRenderState& state, const ax::Mat4& parentTransform, uint32_t parentFlags) override;

private:
    bool _playOnEnter = false;
    bool _removeOnStop = true;
    bool _isLooping = false;
    bool _playedAtLeastOnce = false;
    ax::Vec3 _targetPosition;
    float _speed = 1.0f;
    ax::Color32 _color{255, 255, 255, 255};
    std::array<float, 4> _dynamicInputs{};
    EffectManager* _manager = nullptr;
    Effect* _effect = nullptr;
    Effekseer::Handle _handle = -1;
};

class EffectManager : public ax::Object
{
    friend class EffectEmitter;
    friend class EffekseerAxmolLifecycle;

public:
    static EffectManager* create(ax::Size visibleSize);
    EffectManager();
    ~EffectManager() override;

    void begin(const ax::SceneRenderState& state, float globalZOrder);
    void end(const ax::SceneRenderState& state, float globalZOrder);
    void update(float delta = 1.0f / 60.0f);
    void setIsDistortionEnabled(bool value);
    void setCameraMatrix(const ax::Mat4& mat);
    void setProjectionMatrix(const ax::Mat4& mat);

    Effekseer::ManagerRef getInternalManager() { return _manager; }
    EffekseerRenderer::RendererRef getInternalRenderer() { return _renderer; }
    EffekseerRenderer::DistortingCallback* getDistortionCallback() { return _distortionCallback; }

private:
    bool initialize(ax::Size visibleSize);
    void releaseGraphicsResources();
    Effekseer::Handle play(Effect* effect, float x, float y, float z, int32_t startTime = 0);
    void setMatrix(Effekseer::Handle handle, const ax::Mat4& mat);

    Effekseer::ManagerRef _manager;
    EffekseerRenderer::RendererRef _renderer;
    EffekseerRenderer::DistortingCallback* _distortionCallback = nullptr;
    InternalManager* _internalManager = nullptr;
    float _time = 0.0f;
};

class NetworkServer : public ax::Object
{
public:
    static NetworkServer* create();
    bool makeNetworkServerEnabled(uint16_t port);
    void update() {}
};
} // namespace efk
