/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "ACubismMotion.hpp"
#include "Type/csmVector.hpp"
#include "Model/CubismUserModel.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

class CubismMotion;

/**
 * @brief CubismMotionQueueManagerで再生している各モーションの管理
 *
 * CubismMotionQueueManagerで再生している各モーションの管理クラス。
 */
class CubismMotionQueueEntry
{
    friend class CubismMotionQueueManager;
    friend class ACubismMotion;
    friend class CubismMotion;

public:
    /**
     * @brief コンストラクタ
     *
     * コンストラクタ。
     */
    CubismMotionQueueEntry();

    /**
     * @brief デストラクタ
     *
     * デストラクタ。
     */
    virtual ~CubismMotionQueueEntry();

    /**
     * @brief フェードアウト開始の設定
     *
     * フェードアウトの開始を設定する。
     *
     * @param[in]   fadeOutSeconds     フェードアウトにかかる時間[秒]
     */
    void        SetFadeout(csmFloat32 fadeOutSeconds);

    /**
     * @brief フェードアウトの開始
     *
     * フェードアウトを開始する。
     *
     * @param[in]   fadeOutSeconds     フェードアウトにかかる時間[秒]
     * @param[in]   userTimeSeconds    デルタ時間の積算値[秒]
     */
    void        StartFadeout(csmFloat32 fadeOutSeconds, csmFloat32 userTimeSeconds);

    /**
     * @brief モーションの終了の確認
     *
     * モーションが終了したかどうか。
     *
     * @retval  true    モーションが終了した
     * @retval  false   終了していない
     */
    csmBool     IsFinished() const;

    /**
     * @brief モーションの開始の確認
     *
     * モーションが開始したかどうか。
     *
     * @retval  true    モーションが開始した
     * @retval  false   終了していない
     */
    csmBool     IsStarted() const;

    /**
     * @brief モーションの開始時刻の取得
     *
     * モーションの開始時刻を取得する。
     *
     * @return  モーションの開始時刻[秒]
     */
    csmFloat32    GetStartTime() const;

    /**
     * @brief フェードインの開始時刻の取得
     *
     * フェードインの開始時刻を取得する。
     *
     * @return  フェードインの開始時刻[秒]
     */
    csmFloat32    GetFadeInStartTime() const;

    /**
     * @brief フェードインの終了時刻の取得
     *
     * フェードインの終了時刻を取得する。
     *
     * @return  フェードインの終了時刻[秒]
     */
    csmFloat32    GetEndTime() const;

    /**
     * @brief モーションの開始時刻の設定
     *
     * モーションの開始時刻を設定する。
     *
     * @param[in]   startTime   モーションの開始時刻[秒]
     */
    void        SetStartTime(csmFloat32 startTime);

    /**
     * @brief フェードインの開始時刻の設定
     *
     * フェードインの開始時刻を設定する。
     *
     * @param[in]   startTime   フェードインの開始時刻[秒]
     */
    void        SetFadeInStartTime(csmFloat32 startTime);

    /**
     * @brief フェードインの終了時刻の設定
     *
     * フェードインの終了時刻を設定する。
     *
     * @param[in]   endTime   フェードインの終了時刻[秒]
     */
    void        SetEndTime(csmFloat32 endTime);

    /**
     * @brief モーションの終了の設定
     *
     * モーションの終了を設定する。
     *
     * @param[in]   f   trueならモーションの終了
     */
    void        IsFinished(csmBool f);

    /**
     * @brief モーションの開始の設定
     *
     * モーションの開始を設定する。
     *
     * @param[in]   f   trueならモーションの開始
     */
    void        IsStarted(csmBool f);

    /**
     * @brief モーションの有効性の確認
     *
     * モーションの有効・無効を取得する。
     *
     * @retval  true    モーションは有効
     * @retval  false   モーションは無効
     */
    csmBool     IsAvailable() const;

    /**
     * @brief モーションの有効性の設定
     *
     * モーションの有効・無効を設定する。
     *
     * @param[in]   v   trueならモーションは有効
     */
    void        IsAvailable(csmBool v);

    /**
     * @brief モーションの状態の設定
     *
     * モーションの状態を設定する。
     *
     * @param[in]   timeSeconds    現在時刻[秒]
     * @param[in]   weight  モーションの重み
     */
    void        SetState(csmFloat32 timeSeconds, csmFloat32 weight);

    /**
     * @brief モーションの現在時刻の取得
     *
     * モーションの現在時刻を取得する。
     *
     * @return  モーションの現在時刻[秒]
     */
    csmFloat32  GetStateTime() const;

    /**
     * @brief モーションの重みの取得
     *
     * モーションの重みを取得する。
     *
     * @return  モーションの重み
     */
    csmFloat32  GetStateWeight() const;

    /**
    * @brief 最後にイベントの発火をチェックした時間を取得
    *
    * 最後にイベントの発火をチェックした時間を取得する。
    *
    * @return  最後にイベントの発火をチェックした時間[秒]
    */
    csmFloat32  GetLastCheckEventTime() const;

    /**
    * @brief 最後にイベントをチェックした時間を設定
    *
    * 最後にイベントをチェックした時間を設定する。
    *
    * @param[in]    checkTime   最後にイベントをチェックした時間[秒]
    */
    void        SetLastCheckEventTime(csmFloat32 checkTime);

    /**
    * @brief フェードアウトが開始しているかを取得
    *
    * モーションがフェードアウトが開始しているかを取得する。
    *
    * @return    フェードアウトが開始しているか
    */
    csmBool     IsTriggeredFadeOut();

    /**
    * @brief フェードアウト時間の取得
    *
    * モーションのフェードアウト時間を取得する。
    *
    * @return    フェードアウト開始[秒]
    */
    csmFloat32     GetFadeOutSeconds();

private:
    csmBool         _autoDelete;                    ///< 自動削除
    ACubismMotion*  _motion;                        ///< モーション

    csmBool         _available;                     ///< 有効化フラグ
    csmBool         _finished;                      ///< 終了フラグ
    csmBool         _started;                       ///< 開始フラグ（0.9.00以降）
    csmFloat32      _startTimeSeconds;              ///<  モーション再生開始時刻[秒]
    csmFloat32      _fadeInStartTimeSeconds;        ///<  フェードイン開始時刻（ループの時は初回のみ）[秒]
    csmFloat32      _endTimeSeconds;                ///< 終了予定時刻[秒]
    csmFloat32      _stateTimeSeconds;              ///<  時刻の状態[秒]
    csmFloat32      _stateWeight;                   ///<  重みの状態
    csmFloat32      _lastEventCheckSeconds;         ///<   最終のMotion側のチェックした時間
    csmFloat32      _fadeOutSeconds;
    csmBool         _IsTriggeredFadeOut;

    CubismMotionQueueEntryHandle  _motionQueueEntryHandle;        ///< インスタンスごとに一意の値を持つ識別番号
};

}}}
