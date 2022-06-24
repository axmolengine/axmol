/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "CubismFramework.hpp"
#include "Id/CubismId.hpp"
#include "Type/csmVector.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

class CubismMotionQueueManager;
class CubismMotionQueueEntry;
class CubismModel;

/**
 * @brief モーションの抽象基底クラス
 *
 * モーションの抽象基底クラス。MotionQueueManagerによってモーションの再生を管理する。
 */
class ACubismMotion
{
public:
    /// モーション再生終了コールバック関数定義
    typedef void (*FinishedMotionCallback)(ACubismMotion* self);
    /**
     * @brief インスタンスの破棄
     *
     * インスタンスを破棄する。
     *
     * @param[in]   motion  破棄対象のACubismMotion
     */
    static void Delete(ACubismMotion* motion);

    /**
     * @brief コンストラクタ
     *
     * コンストラクタ。
     */
    ACubismMotion();

    /**
     * @brief モデルのパラメータ更新
     *
     * モデルのパラメータを更新する。
     *
     * @param[in]   model               対象のモデル
     * @param[in]   motionQueueEntry    CubismMotionQueueManagerで管理されているモーション
     * @param[in]   userTimeSeconds     デルタ時間の積算値[秒]
     */
    void UpdateParameters(CubismModel* model, CubismMotionQueueEntry* motionQueueEntry, csmFloat32 userTimeSeconds);

    /**
     * @brief フェードイン
     *
     * フェードインの時間を設定する。
     *
     * @param[in]   fadeInSeconds      フェードインにかかる時間[秒]
     */
    void SetFadeInTime(csmFloat32 fadeInSeconds);

    /**
     * @brief フェードアウト
     *
     * フェードアウトの時間を設定する。
     *
     * @param[in]   fadeOutSeconds     フェードアウトにかかる時間[秒]
     */
    void SetFadeOutTime(csmFloat32 fadeOutSeconds);

    /**
     * @brief フェードアウトにかかる時間の取得
     *
     * フェードアウトにかかる時間を取得する。
     *
     * @return フェードアウトにかかる時間[秒]
     */
    csmFloat32 GetFadeOutTime() const;

    /**
     * @brief フェードインにかかる時間の取得
     *
     * フェードインにかかる時間を取得する。
     *
     * @return フェードインにかかる時間[秒]
     */
    csmFloat32 GetFadeInTime() const;

    /**
     * @brief モーション適用の重みの設定
     *
     * モーション適用の重みを設定する。
     *
     * @param[in]   weight      重み(0.0 - 1.0)
     */
    void SetWeight(csmFloat32 weight);

    /**
     * @brief モーション適用の重みの取得
     *
     * モーション適用の重みを取得する。
     *
     * @return 重み(0.0 - 1.0)
     */
    csmFloat32 GetWeight() const;

    /**
     * @brief モーションの長さの取得
     *
     * モーションの長さを取得する。
     *
     * @return モーションの長さ[秒]
     *
     * @note ループのときは「-1」。
     *       ループではない場合は、オーバーライドする。
     *       正の値の時は取得される時間で終了する。
     *       「-1」のときは外部から停止命令が無い限り終わらない処理となる。
     */
    virtual csmFloat32 GetDuration();

    /**
     * @brief モーションのループ1回分の長さの取得
     *
     * モーションのループ1回分の長さを取得する。
     *
     * @return モーションのループ1回分の長さ[秒]
     *
     * @note ループしない場合は GetDuration()と同じ値を返す。
     *       ループ一回分の長さが定義できない場合（プログラム的に動き続けるサブクラスなど）の場合は「-1」を返す
     */
    virtual csmFloat32 GetLoopDuration();


    /**
     * @brief モーション再生の開始時刻の設定
     *
     * モーション再生の開始時刻を設定する。
     *
     * @param[in]   offsetSeconds    モーション再生の開始時刻[秒]
     */
    void SetOffsetTime(csmFloat32 offsetSeconds);

    /**
    * @brief モデルのパラメータ更新
    *
    * イベント発火のチェック。
    * 入力する時間は呼ばれるモーションタイミングを０とした秒数で行う。
    *
    * @param[in]   beforeCheckTimeSeconds   前回のイベントチェック時間[秒]
    * @param[in]   motionTimeSeconds        今回の再生時間[秒]
    */
    virtual const csmVector<const csmString*>& GetFiredEvent(csmFloat32 beforeCheckTimeSeconds,
                                                                   csmFloat32 motionTimeSeconds);


    /**
     * @brief モーション再生終了コールバックの登録
     *
     * モーション再生終了コールバックを登録する。
     * IsFinishedフラグを設定するタイミングで呼び出される。
     * 以下の状態の際には呼び出されない:
     *   1. 再生中のモーションが「ループ」として設定されているとき
     *   2. コールバックにNULLが登録されているとき
     *
     * @param[in]   onFinishedMotionHandler     モーション再生終了コールバック関数
     */
    void SetFinishedMotionHandler(FinishedMotionCallback onFinishedMotionHandler);

    /**
     * @brief モーション再生終了コールバックの取得
     *
     * モーション再生終了コールバックを取得する。
     *
     * @return  登録されているモーション再生終了コールバック関数。NULLのとき、関数は何も登録されていない。
     */
    FinishedMotionCallback GetFinishedMotionHandler();

    /**
    * @brief        透明度のカーブが存在するかどうかを確認する
    *
    * @retval       true  -> キーが存在する
    * @retval       false -> キーが存在しない
    */
    virtual csmBool IsExistOpacity() const;

    /**
    * @brief 透明度のカーブのインデックスを返す
    *
    *
    * @return  success：透明度のカーブのインデックス
    */
    virtual csmInt32 GetOpacityIndex() const;

    /**
    * @brief 透明度のIdを返す
    *
    *
    * @return  success：透明度のId
    */
    virtual CubismIdHandle GetOpacityId(csmInt32 index);

    /**
    * @brief 指定時間の透明度の値を返す
    *
    * @param[in]   motionTimeSeconds        現在の再生時間[秒]
    *
    * @return  success：モーションの当該時間におけるOpacityの値
    */
    virtual csmFloat32 GetOpacityValue(csmFloat32 motionTimeSeconds) const;

private:
    // Prevention of copy Constructor
    ACubismMotion(const ACubismMotion&);
    ACubismMotion& operator=(const ACubismMotion&);

protected:
    /**
     * @brief デストラクタ
     *
     * デストラクタ。
     */
    virtual ~ACubismMotion();

    /**
     * @brief モデルのパラメータの更新の実行
     *
     * モデルのパラメータ更新を実行する。
     *
     * @param[in]   model               対象のモデル
     * @param[in]   userTimeSeconds     デルタ時間の積算値[秒]
     * @param[in]   weight              モーションの重み
     * @param[in]   motionQueueEntry    CubismMotionQueueManagerで管理されているモーション
     */
    virtual void DoUpdateParameters(CubismModel* model, csmFloat32 userTimeSeconds, csmFloat32 weight, CubismMotionQueueEntry* motionQueueEntry) = 0;

    csmFloat32    _fadeInSeconds;        ///< フェードインにかかる時間[秒]
    csmFloat32    _fadeOutSeconds;       ///< フェードアウトにかかる時間[秒]
    csmFloat32    _weight;               ///< モーションの重み
    csmFloat32    _offsetSeconds;        ///< モーション再生の開始時刻[秒]

    csmVector<const csmString*>    _firedEventValues;

    // モーション再生終了コールバック関数
    FinishedMotionCallback _onFinishedMotion;
};

}}}
