/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "Model/CubismModel.hpp"
#include "Type/csmVector.hpp"
#include "Id/CubismId.hpp"
#include "ICubismModelSetting.hpp"


namespace Live2D { namespace Cubism { namespace Framework {

/**
 * @brief 自動まばたき機能
 *
 * 自動まばたき機能を提供する。
 *
 */
class CubismEyeBlink
{
public:
    /**
     * @brief まばたきの状態
     *
     * まばたきの状態を表す列挙型。
     */
    enum EyeState
    {
        EyeState_First = 0,         ///< 初期状態
        EyeState_Interval,          ///< まばたきしていない状態
        EyeState_Closing,           ///< まぶたが閉じていく途中の状態
        EyeState_Closed,            ///< まぶたが閉じている状態
        EyeState_Opening            ///< まぶたが開いていく途中の状態
    };

    /**
     * @brief インスタンスの作成
     *
     * インスタンスを作成する。
     *
     * @param[in]   modelSetting    モデルの設定情報
     * @return 作成されたインスタンス
     * @note 引数がNULLの場合、パラメータIDが設定されていない空のインスタンスを作成する。
     */
    static CubismEyeBlink* Create(ICubismModelSetting* modelSetting = NULL);

    /**
     * @brief インスタンスの破棄
     *
     * インスタンスを破棄する。
     *
     * @param[in]   eyeBlink    対象のCubismEyeBlink
     */
    static void Delete(CubismEyeBlink* eyeBlink);

    /**
     * @brief まばたきの間隔の設定
     *
     * まばたきの間隔を設定する。
     *
     * @param[in]   blinkingInterval     まばたきの間隔の時間[秒]
     */
    void            SetBlinkingInterval(csmFloat32 blinkingInterval);

    /**
     * @brief またばきのモーションの詳細設定
     *
     * まばたきのモーションの詳細設定を行う。
     *
     * @param[in]   closing     まぶたを閉じる動作の所要時間[秒]
     * @param[in]   closed      まぶたを閉じている動作の所要時間[秒]
     * @param[in]   opening     まぶたを開く動作の所要時間[秒]
     */
    void            SetBlinkingSettings(csmFloat32 closing, csmFloat32 closed, csmFloat32 opening);

    /**
     * @brief まばたきさせるパラメータIDのリストの設定
     *
     * まばたきさせるパラメータIDのリストを設定する。
     *
     * @param[in]   parameterIds    パラメータIDのリスト
     */
    void            SetParameterIds(const csmVector<CubismIdHandle>& parameterIds);

    /**
    * @brief まばたきさせるパラメータIDのリストの取得
    *
    * まばたきさせるパラメータIDのリストを取得する。
    *
    * @return   パラメータIDのリスト
    */
    const csmVector<CubismIdHandle>&     GetParameterIds() const;

    /**
     * @brief モデルのパラメータの更新
     *
     * モデルのパラメータを更新する。
     *
     * @param[in]   model   対象のモデル
     * @param[in]   deltaTimeSeconds   デルタ時間[秒]
     */
    void            UpdateParameters(CubismModel* model, csmFloat32 deltaTimeSeconds);

private:

    /**
    * @brief コンストラクタ
    *
    * コンストラクタ。
    *
    * @param[in]   modelSetting    モデルの設定情報
    */
    CubismEyeBlink(ICubismModelSetting* modelSetting);

    /**
    * @brief デストラクタ
    *
    * デストラクタ。
    */
    virtual ~CubismEyeBlink();

    /**
     * @brief 次のまばたきのタイミングの決定
     *
     * 次のまばたきのタイミングを決定する。
     *
     * @return 次のまばたきを行う時刻[秒]
     */
    csmFloat32        DeterminNextBlinkingTiming() const;

    csmInt32                    _blinkingState;                   ///< 現在の状態
    csmVector<CubismIdHandle>   _parameterIds;                    ///< 操作対象のパラメータのIDのリスト
    csmFloat32                  _nextBlinkingTime;                ///< 次のまばたきの時刻[秒]
    csmFloat32                  _stateStartTimeSeconds;           ///< 現在の状態が開始した時刻[秒]
    csmFloat32                  _blinkingIntervalSeconds;         ///< まばたきの間隔[秒]
    csmFloat32                  _closingSeconds;                  ///< まぶたを閉じる動作の所要時間[秒]
    csmFloat32                  _closedSeconds;                   ///< まぶたを閉じている動作の所要時間[秒]
    csmFloat32                  _openingSeconds;                  ///< まぶたを開く動作の所要時間[秒]
    csmFloat32                  _userTimeSeconds;                 ///< デルタ時間の積算値[秒]

};

}}}
