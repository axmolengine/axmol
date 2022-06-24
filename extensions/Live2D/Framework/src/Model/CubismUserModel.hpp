/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "Effect/CubismPose.hpp"
#include "Effect/CubismEyeBlink.hpp"
#include "Effect/CubismBreath.hpp"
#include "Math/CubismModelMatrix.hpp"
#include "Math/CubismTargetPoint.hpp"
#include "Model/CubismMoc.hpp"
#include "Model/CubismModel.hpp"
#include "Motion/CubismMotionManager.hpp"
#include "Motion/CubismExpressionMotion.hpp"
#include "Physics/CubismPhysics.hpp"
#include "Rendering/CubismRenderer.hpp"
#include "Model/CubismModelUserData.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

/**
 * @brief ユーザーが実際に使用するモデル
 *
 * ユーザーが実際に使用するモデルの基底クラス。これを継承してユーザーが実装する。
 */
class CubismUserModel
{
public:
    /**
     * @brief コンストラクタ
     *
     * コンストラクタ。
     */
    CubismUserModel();

    /**
     * @brief デストラクタ
     *
     * デストラクタ。
     */
    virtual ~CubismUserModel();

    /**
     * @brief 初期化状態の取得
     *
     * 初期化されている状態か？
     *
     * @retval  true    初期化されている
     * @retval  false   初期化されていない
     */
    virtual csmBool         IsInitialized();

    /**
     * @brief 初期化状態の設定
     *
     * 初期化状態を設定する。
     *
     * @param[in]   v   初期化状態
     */
    virtual void            IsInitialized(csmBool v);

    /**
     * @brief 更新状態の取得
     *
     * 更新されている状態か？
     *
     * @retval  true    更新されている
     * @retval  false   更新されていない
     */
    virtual csmBool         IsUpdating();

    /**
     * @brief 更新状態の設定
     *
     * 更新状態を設定する。
     *
     * @param[in]   v   更新状態
     */
    virtual void            IsUpdating(csmBool v);

    /**
     * @brief マウスドラッグ情報の設定
     *
     * マウスドラッグの情報を設定する。
     *
     * @param[in]   x   ドラッグしているカーソルのX位置
     * @param[in]   y   ドラッグしているカーソルのY位置
     */
    virtual void            SetDragging(csmFloat32 x, csmFloat32 y);

    /**
     * @brief 加速度情報の設定
     *
     * 加速度の情報を設定する。
     *
     * @param[in]   x   X軸方向の加速度
     * @param[in]   y   Y軸方向の加速度
     * @param[in]   z   Z軸方向の加速度
     */
    virtual void            SetAcceleration(csmFloat32 x, csmFloat32 y, csmFloat32 z);

    /**
     * @brief モデル行列の取得
     *
     * モデル行列を取得する。
     *
     * @return  モデル行列
     */
    CubismModelMatrix*      GetModelMatrix() const;

    /**
     * @brief 不透明度の設定
     *
     * 不透明度を設定する。
     *
     * @param[in]   a   不透明度
     */
    virtual void            SetOpacity(csmFloat32 a);

    /**
     * @brief 不透明度の取得
     *
     * 不透明度を取得する。
     *
     * @return  不透明度
     */
    virtual csmFloat32      GetOpacity();

    /**
     * @brief モデルデータの読み込み
     *
     * モデルデータを読み込む。
     *
     * @param[in]   buffer  moc3ファイルが読み込まれているバッファ
     * @param[in]   size    バッファのサイズ
     */
    virtual void            LoadModel(const csmByte* buffer, csmSizeInt size);

    /**
     * @brief モーションデータの読み込み
     *
     * モーションデータを読み込む。
     *
     * @param[in]   buffer                      motion3.jsonファイルが読み込まれているバッファ
     * @param[in]   size                        バッファのサイズ
     * @param[in]   name                        モーションの名前
     * @param[in]   onFinishedMotionHandler     モーション再生終了時に呼び出されるコールバック関数。NULLの場合、呼び出されない。
     * @return  モーションクラス
     */
    virtual ACubismMotion*  LoadMotion(const csmByte* buffer, csmSizeInt size, const csmChar* name, ACubismMotion::FinishedMotionCallback onFinishedMotionHandler = NULL);

    /**
     * @brief 表情データの読み込み
     *
     * 表情データを読み込む。
     *
     * @param[in]   buffer  expファイルが読み込まれているバッファ
     * @param[in]   size    バッファのサイズ
     * @param[in]   name    表情の名前
     */
    virtual ACubismMotion*   LoadExpression(const csmByte* buffer, csmSizeInt size, const csmChar* name);

    /**
     * @brief ポーズデータの読み込み
     *
     * ポーズデータを読み込む。
     *
     * @param[in]   buffer  pose3.jsonが読み込まれているバッファ
     * @param[in]   size    バッファのサイズ
     */
    virtual void            LoadPose(const csmByte* buffer, csmSizeInt size);

    /**
     * @brief 物理演算データの読み込み
     *
     * 物理演算データを読み込む。
     *
     * @param[in]   buffer  physics3.jsonが読み込まれているバッファ
     * @param[in]   size    バッファのサイズ
     */
    virtual void            LoadPhysics(const csmByte* buffer, csmSizeInt size);

    /**
    * @brief モデルに付属するユーザーデータを読み込む
    *
    * ユーザーデータを読み込む。
    *
    * @param[in]   buffer  userdata3.jsonが読み込まれているバッファ
    * @param[in]   size    バッファのサイズ
    */
    virtual void            LoadUserData(const csmByte* buffer, csmSizeInt size);

    /**
     * @brief あたり判定の取得
     *
     * 指定した位置にDrawableがヒットしているかどうかを取得する。
     *
     * @param[in]   drawableId  検証したいDrawableのID
     * @param[in]   pointX      X位置
     * @param[in]   pointY      Y位置
     * @retval  true    ヒットしている
     * @retval  false   ヒットしていない
     */
    virtual csmBool         IsHit(CubismIdHandle drawableId, csmFloat32 pointX, csmFloat32 pointY);

    /**
     * @brief モデルの取得
     *
     * モデルを取得する。
     *
     * @return  モデル
     */
    CubismModel*            GetModel() const;

    /**
     * @brief レンダラの取得
     *
     * レンダラを取得する。
     *
     * @return レンダラ
     */
    template<class T> T*    GetRenderer() { return dynamic_cast<T*>(_renderer); }

    /**
     *  @brief  レンダラの生成
     *
     *  レンダラを生成して初期化を実行する。
     *
     */
    void CreateRenderer();

    /**
     *  @brief  レンダラの解放
     *
     *  レンダラを解放する。
     *
     */
    void DeleteRenderer();

    /**
    * @brief  イベント発火時の標準処理
    *
    * Eventが再生処理時にあった場合の処理をする。
    * 継承で上書きすることを想定している。
    * 上書きしない場合はログ出力をする。
    *
    * @param[in]    eventValue    発火したイベントの文字列データ
    */
    virtual void   MotionEventFired(const csmString& eventValue);

    /**
    * @brief  イベント用のCallback
    *
    * CubismMotionQueueManagerにイベント用に登録するためのCallback。
    * CubismUserModelの継承先のEventFiredを呼ぶ。
    *
    * @param[in]    caller              発火したイベントを管理していたモーションマネージャー、比較用
    * @param[in]    eventValue          発火したイベントの文字列データ
    * @param[in]    customData          CubismUserModelを継承したインスタンスを想定
    */
    static void   CubismDefaultMotionEventCallback(const CubismMotionQueueManager* caller, const csmString& eventValue, void* customData);
protected:
    CubismMoc*              _moc;                       ///< Mocデータ
    CubismModel*            _model;                     ///< Modelインスタンス

    CubismMotionManager*    _motionManager;             ///< モーション管理
    CubismMotionManager*    _expressionManager;         ///< 表情管理
    CubismEyeBlink*         _eyeBlink;                  ///< 自動まばたき
    CubismBreath*           _breath;                    ///< 呼吸
    CubismModelMatrix*      _modelMatrix;               ///< モデル行列
    CubismPose*             _pose;                      ///< ポーズ管理
    CubismTargetPoint*      _dragManager;               ///< マウスドラッグ
    CubismPhysics*          _physics;                   ///< 物理演算
    CubismModelUserData*    _modelUserData;             ///< ユーザデータ

    csmBool     _initialized;                   ///< 初期化されたかどうか
    csmBool     _updating;                      ///< 更新されたかどうか
    csmFloat32  _opacity;                       ///< 不透明度
    csmBool     _lipSync;                       ///< リップシンクするかどうか
    csmFloat32  _lastLipSyncValue;              ///< 最後のリップシンクの制御値
    csmFloat32  _dragX;                         ///< マウスドラッグのX位置
    csmFloat32  _dragY;                         ///< マウスドラッグのY位置
    csmFloat32  _accelerationX;                 ///< X軸方向の加速度
    csmFloat32  _accelerationY;                 ///< Y軸方向の加速度
    csmFloat32  _accelerationZ;                 ///< Z軸方向の加速度
    csmBool     _debugMode;                     ///< デバッグモードかどうか

private:
    Rendering::CubismRenderer* _renderer;       ///< レンダラ
};

}}}
