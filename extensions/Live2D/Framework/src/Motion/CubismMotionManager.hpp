/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "Model/CubismModel.hpp"
#include "ACubismMotion.hpp"
#include "CubismMotionQueueManager.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

/**
 * @brief モーションの管理
 *
 * モーションの管理を行うクラス。
 */
class CubismMotionManager : public CubismMotionQueueManager
{
public:
    /**
     * @brief コンストラクタ
     *
     * コンストラクタ。
     */
    CubismMotionManager();

    /**
     * @brief デストラクタ
     *
     * デストラクタ。
     */
    virtual ~CubismMotionManager();

    /**
     * @brief 再生中のモーションの優先度の取得
     *
     * 再生中のモーションの優先度の取得する。
     *
     * @return  モーションの優先度
     */
    csmInt32 GetCurrentPriority() const;

    /**
     * @brief 予約中のモーションの優先度の取得
     *
     * 予約中のモーションの優先度を取得する。
     *
     * @return  モーションの優先度
     */
    csmInt32 GetReservePriority() const;

    /**
     * @brief 予約中のモーションの優先度の設定
     *
     * 予約中のモーションの優先度を設定する。
     *
     * @param[in]   val     優先度
     */
    void SetReservePriority(csmInt32 val);

    /**
     * @brief 優先度を設定してモーションの開始
     *
     * 優先度を設定してモーションを開始する。
     *
     * @param[in]   motion          モーション
     * @param[in]   autoDelete      再生が狩猟したモーションのインスタンスを削除するならtrue
     * @param[in]   priority        優先度
     * @return                      開始したモーションの識別番号を返す。個別のモーションが終了したか否かを判定するIsFinished()の引数で使用する。開始できない時は「-1」
     */
    CubismMotionQueueEntryHandle StartMotionPriority(ACubismMotion* motion, csmBool autoDelete, csmInt32 priority);

    /**
     * @brief モーションの更新
     *
     * モーションを更新して、モデルにパラメータ値を反映する。
     *
     * @param[in]   model   対象のモデル
     * @param[in]   deltaTimeSeconds    デルタ時間[秒]
     * @param[in][out]   opacity 透明度の値（Nullable）
     * @retval  true    更新されている
     * @retval  false   更新されていない
     */
    csmBool UpdateMotion(CubismModel* model, csmFloat32 deltaTimeSeconds, csmFloat32* opacity = NULL);

    /**
     * @brief モーションの予約
     *
     * モーションを予約する。
     *
     * @param[in]   priority    優先度
     * @retval  true    予約できた
     * @retval  false   予約できなかった
     */
    csmBool ReserveMotion(csmInt32 priority);

private:
    csmInt32 _currentPriority;                  ///<  現在再生中のモーションの優先度
    csmInt32 _reservePriority;                  ///<  再生予定のモーションの優先度。再生中は0になる。モーションファイルを別スレッドで読み込むときの機能。
};

}}}
