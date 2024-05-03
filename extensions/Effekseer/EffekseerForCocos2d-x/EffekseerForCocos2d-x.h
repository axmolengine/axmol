
#pragma once

#include "cocos2d.h"
#include <Effekseer.h>
#include <EffekseerRendererCommon/EffekseerRenderer.Renderer.h>

//#ifdef AX_USE_METAL
//#include <EffekseerRendererMetal.h>
//#else
//#include <EffekseerRendererGL.h>
//#endif

namespace efk
{
class EffectManager;
class InternalManager;

/**
	@brief
	\~English	Effect resource class
	\~Japanese	エフェクトのリソースクラス
	@note
	\~English
	It is released automatically with autorelease after version1.41

	\~Japanese
	1.41以降、autoreleaseにより自動的に解放されます。
*/
class Effect : public cocos2d::Object
{
private:
	::Effekseer::EffectRef effect = nullptr;
	InternalManager* internalManager_ = nullptr;

public:
	static Effect* create(const std::string& filename, float maginification = 1.0f);

	Effect(InternalManager* internalManager = nullptr);

	virtual ~Effect();

	/**
		@brief
		\~English	Get the pointer of Effekseer::Effect
		\~Japanese	Effekseer::Effectのポインタを取得する。
		@return
		\~English　	The pointer of Effekseer::Effect
		\~Japanese	Effekseer::Effectのポインタ
	*/
	::Effekseer::EffectRef getInternalPtr() { return effect; }
};

/**
	@brief
	\~English	Effect emitter class
	\~Japanese	エフェクトのエミッタークラス
	@note
	\~English
	It is released automatically with autorelease after version1.41


	\~Japanese
	1.41以降、autoreleaseにより自動的に解放されます。
*/
class EffectEmitter : public cocos2d::Node
{
	friend class EffectManager;

private:
	bool playOnEnter = false;
	bool removeOnStop = true;
	bool isLooping = false;

	//! whether this effect is played once at least
	bool isPlayedAtLeastOnce = false;

	cocos2d::Vec3 targetPosition_;
	float speed_ = 1.0f;
	cocos2d::Color4B color_ = cocos2d::Color4B(255, 255, 255, 255);
	std::array<float, 4> dynamicInputs_;

	EffectManager* manager = nullptr;
	Effect* effect_ = nullptr;
	::Effekseer::Handle handle = -1;

	// cocos2d::CallbackCommand renderCommand;
	
	void beforeRender(EffekseerRenderer::RendererRef, Effekseer::RefPtr<EffekseerRenderer::CommandList>);
    void afterRender(EffekseerRenderer::RendererRef, Effekseer::RefPtr<EffekseerRenderer::CommandList>);

public:
	/**
		@brief
		\~English	Create the instance of emitter class.
		\~Japanese	エミッタークラスのインスタンスを生成する。

		@return
		\~English	Instance
		\~Japanese	インスタンス
	*/
	static EffectEmitter* create(EffectManager* manager);

	/**
	@brief
	\~English	Create the instance of emitter class.
	\~Japanese	エミッタークラスのインスタンスを生成する。

	@param	filename
	\~English	An effect files's path
	\~Japanese	エフェクトファイルのパス

	@param	magnification
	\~English	A maginification rate. Effects are loaded by enlarge with specified value
	\~Japanese	拡大率、指定された値でエフェクトが拡大されて読み込まれる

	@return
	\~English	Instance
	\~Japanese	インスタンス
	*/
	static EffectEmitter* create(EffectManager* manager, const std::string& filename, float maginification = 1.0f);

	/**
		@brief
		\~English	Constructor
		\~Japanese	コンストラクタ
	*/
	EffectEmitter(EffectManager* manager);

	/**
		@brief
		\~English	Destructor
		\~Japanese	デストラクタ
	*/
	virtual ~EffectEmitter();

	/**
		@brief
		\~English	Get an effect's resource
		\~Japanese	エフェクトのリソースを取得する。
		@return
		\~English	An effect's resource
		\~Japanese	エフェクトのリソース
	*/
	Effect* getEffect();

	/**
		@brief
		\~English	Set an effect's resource
		\~Japanese	エフェクトのリソースを設定する。
		@param	effect
		\~English	An effect's resource
		\~Japanese	エフェクトのリソース
	*/
	void setEffect(Effect* effect);

	/**
		@brief
		\~English	Get an internal handle of Effekseer
		\~Japanese	Effekseerの内部のハンドルを取得する。
		@param	effect
		\~English	An internal handle of Effekseer
		\~Japanese	Effekseerの内部のハンドル
	*/
	::Effekseer::Handle getInternalHandle() const;

	/**
		@brief
		\~English	Play the effect.
		\~Japanese	エフェクトを再生する。
	*/
	void play();

	/**
	@brief
	\~English	Play the effect.
	\~Japanese	エフェクトを再生する。
	@param	startFrame
	\~English	A time to play from middle
	\~Japanese	途中から再生するための時間
	*/
	void play(int32_t startTime);

	/**
		@brief
		\~English	Get whether to start playing the effect when adding a node.
		\~Japanese	追加時に再生を開始するかどうか、設定する。
		@return	value
		\~English	Flag
		\~Japanese	フラグ
	*/
	bool getPlayOnEnter();

	/**
		@brief
		\~English	Set whether to start playing the effect when adding a node.
		\~Japanese	追加時に再生を開始するかどうか、設定する。
		@return	value
		\~English	Flag
		\~Japanese	フラグ
	*/
	void setPlayOnEnter(bool value);

	/**
	@brief
		\~English	Get whether to loop play the effect.
		\~Japanese	ループ再生するかどうか、取得する。
		@return	value
		\~English	Flag
		\~Japanese	フラグ
	*/
	bool getIsLooping();

	/**
		@brief
		\~English	Set whether to loop play the effect.
		\~Japanese	ループ再生するかどうか、設定する。
		@return	value
		\~English	Flag
		\~Japanese	フラグ
	*/
	void setIsLooping(bool value);

	/**
		@brief
		\~English	Get whether to discard the node at the end of playback.
		\~Japanese	再生終了時にノードを破棄するかどうか、設定する。
	*/
	bool getRemoveOnStop();

	/**
		@brief
		\~English	Set whether to discard the node at the end of playback.
		\~Japanese	再生終了時にノードを破棄するかどうか、設定する。
	*/
	void setRemoveOnStop(bool value);

	/**
		@brief
		\~English	Set the color which affects whole particles.
		\~Japanese	全てのパーティクルに影響する色を設定する。
	*/
	void setColor(cocos2d::Color4B color);

	/**
	@brief
	\~English	Get the playback speed
	\~Japanese	再生速度を取得する。
	*/
	float getSpeed();

	/**
	@brief
	\~English	Set the playback speed
	\~Japanese	再生速度を設定する。
	*/
	void setSpeed(float speed);

	/**
	@brief
	\~English	Set the position of target to gather
	\~Japanese	エフェクトが集まる位置を設定する。
	*/
	void setTargetPosition(cocos2d::Vec3 position);

	/**
		@brief
		\~English get a dynamic parameter, which changes effect parameters dynamically while playing
		\~Japanese 再生中にエフェクトのパラメーターを変更する動的パラメーターを取得する。
	*/
	float getDynamicInput(int32_t index);

	/**
		@brief
		\~English specfiy a dynamic parameter, which changes effect parameters dynamically while playing
		\~Japanese 再生中にエフェクトのパラメーターを変更する動的パラメーターを設定する。
	*/
	void setDynamicInput(int32_t index, float value);

	/**
		@brief
		\~English	Get whether or not playing the effect.
		\~Japanese	再生中かどうか、取得する。
	*/
	bool isPlaying();

	/**
		@brief
		\~English	Stop the effect.
		\~Japanese	エフェクトを停止する。
	*/
	void stop();

	/**
		@brief
		\~English	Stop only the effect's root.
		\~Japanese	エフェクトのルートのみを停止する。
	*/
	void stopRoot();

	void onEnter() override;

	void onExit() override;

	void update(float delta) override;

	void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& parentTransform, uint32_t parentFlags) override;
};

class EffectManager : public cocos2d::Object
{
	friend class EffectEmitter;

private:
	::Effekseer::ManagerRef manager2d = nullptr;
	::EffekseerRenderer::RendererRef renderer2d = nullptr;
	::EffekseerRenderer::DistortingCallback* distortingCallback = NULL;
    ::Effekseer::RefPtr<::EffekseerRenderer::SingleFrameMemoryPool> memoryPool_ = nullptr;
    ::Effekseer::RefPtr<::EffekseerRenderer::CommandList> commandList_ = nullptr;
    
	bool isDistortionEnabled = false;

	bool isDistorted = false;
	float time_ = 0.0f;
	InternalManager* internalManager_ = nullptr;

	cocos2d::CustomCommand distortionCommand;
	cocos2d::CustomCommand beginCommand;
	cocos2d::CustomCommand endCommand;

	::Effekseer::Handle play(Effect* effect, float x, float y, float z);

	::Effekseer::Handle play(Effect* effect, float x, float y, float z, int startTime);

	void setMatrix(::Effekseer::Handle handle, const cocos2d::Mat4& mat);

	void setPotation(::Effekseer::Handle handle, float x, float y, float z);

	void setRotation(::Effekseer::Handle handle, float x, float y, float z);

	void setScale(::Effekseer::Handle handle, float x, float y, float z);

	bool Initialize(cocos2d::Size visibleSize);

    void CreateRenderer(int32_t spriteSize);
	void onDestructor();

public:
	/**
		@brief
		\~English	Create the instance of manager class.
		\~Japanese	マネージャークラスのインスタンスを生成する。

		@return
		\~English	Instance
		\~Japanese	インスタンス
	*/
	static EffectManager* create(cocos2d::Size visibleSize);

	/**
		@brief
		\~English	Constructor
		\~Japanese	コンストラクタ
	*/
	EffectManager();

	/**
		@brief
		\~English	Destructor
		\~Japanese	デストラクタ
	*/
	virtual ~EffectManager();

	/**
		@brief
		\~English	Set whether distortion is enabled.
		\~Japanese	歪みが有功かどうかを設定する。
	*/
	void setIsDistortionEnabled(bool value);

	/**
		@brief
		\~English	Inherit visit and add a process before drawing the layer.
		\~Japanese	visitを継承してレイヤーの描画を行う前に実行する。
	*/
	void begin(cocos2d::Renderer* renderer, float globalZOrder);

	/**
		@brief
		\~English	Inherit visit and add a process after drawing the layer.
		\~Japanese	visitを継承してレイヤーの描画を行った後に実行する。
	*/
	void end(cocos2d::Renderer* renderer, float globalZOrder);

    /**
        @brief
        \~English    This is called within begin. For special behavioural handling of different graphic device
    */
    void newFrame();
    
	/**
		@brief
		\~English	Update the manager every frame.
		\~Japanese	毎フレーム実行する。
		@param delta
		\~English	In seconds.
		\~Japanese	秒

	*/
	void update(float delta = 1.0f / 60.0f);

	/**
		@brief
		\~English	Get the pointer of Effekseer::Manager
		\~Japanese	Effekseer::Managerのポインタを取得する。
		@return
		\~English　	The pointer of Effekseer::Manager
		\~Japanese	Effekseer::Managerのポインタ
	*/
	::Effekseer::ManagerRef getInternalManager() { return manager2d; }

	/**
		@brief
		\~English	Get the pointer of Effekseer::Renderer
		\~Japanese	Effekseer::Rendererのポインタを取得する。
		@return
		\~English　	The pointer of Effekseer::Renderer
		\~Japanese	Effekseer::Rendererのポインタ
	*/
	::EffekseerRenderer::RendererRef getInternalRenderer() { return renderer2d; }

	void setCameraMatrix(const cocos2d::Mat4& mat);
	void setProjectionMatrix(const cocos2d::Mat4& mat);
    
    Effekseer::RefPtr<::EffekseerRenderer::CommandList> getInternalCommandList() { return commandList_; }
};

class NetworkServer : public cocos2d::Object
{
private:
	InternalManager* internalManager_ = nullptr;

public:
	NetworkServer();

	virtual ~NetworkServer();

	/**
		@brief
		\~English	Create the instance of network server class.
		\~Japanese	ネットワークサーバークラスのインスタンスを生成する。
	

		@return
		\~English	Instance
		\~Japanese	インスタンス
	*/
	static NetworkServer* create();

	/**
	@brief
	\~English	make network server enabled to edit effects from remote.
	\~Japanese	リモートからエフェクトを編集するためにサーバーを有効にする。
	*/
	bool makeNetworkServerEnabled(uint16_t port);

	/**
	@brief
	\~English	Update the manager every frame.
	\~Japanese	毎フレーム実行する。
	*/
	void update();
};
} // namespace efk
