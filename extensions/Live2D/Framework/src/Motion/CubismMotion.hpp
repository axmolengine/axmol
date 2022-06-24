/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "ACubismMotion.hpp"
#include "Type/CubismBasicType.hpp"
#include "Type/csmVector.hpp"
#include "Id/CubismId.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

class CubismMotionQueueEntry;
struct CubismMotionData;

/**
 * @brief モーションクラス
 *
 * モーションのクラス。
 */
class CubismMotion : public ACubismMotion
{
public:
    /**
     * @brief インスタンスの生成
     *
     * インスタンスを作成する。
     *
     * @param[in]   buffer                      motion3.jsonが読み込まれているバッファ
     * @param[in]   size                        バッファのサイズ
     * @param[in]   onFinishedMotionHandler     モーション再生終了時に呼び出されるコールバック関数。NULLの場合、呼び出されない。
     * @return  作成されたインスタンス
     */
    static CubismMotion* Create(const csmByte* buffer, csmSizeInt size, FinishedMotionCallback onFinishedMotionHandler = NULL);

    /**
    * @brief モデルのパラメータの更新の実行
    *
    * モデルのパラメータ更新を実行する。
    *
    * @param[in]   model               対象のモデル
    * @param[in]   userTimeSeconds         現在の時刻[秒]
    * @param[in]   fadeWeight          モーションの重み
    * @param[in]   motionQueueEntry    CubismMotionQueueManagerで管理されているモーション
    */
    virtual void        DoUpdateParameters(CubismModel* model, csmFloat32 userTimeSeconds, csmFloat32 fadeWeight, CubismMotionQueueEntry* motionQueueEntry);

    /**
     * @brirf ループ情報の設定
     *
     * ループ情報を設定する。
     *
     * @param[in]   loop    ループ情報
     */
    void                IsLoop(csmBool loop);

    /**
     * @brief ループ情報の取得
     *
     * モーションがループするかどうか？
     *
     * @retval  true    ループする
     * @retval  false   ループしない
     */
    csmBool             IsLoop() const;

    /**
     * @brief ループ時のフェードイン情報の設定
     *
     * ループ時のフェードイン情報を設定する。
     *
     * @param[in]   loopFadeIn  ループ時のフェードイン情報
     */
    void                IsLoopFadeIn(csmBool loopFadeIn);

    /**
     * @brief ループ時のフェードイン情報の取得
     *
     * ループ時にフェードインするかどうか？
     *
     * @retval  true    する
     * @retval  false   しない
     */
    csmBool             IsLoopFadeIn() const;

    /**
     * @brief モーションの長さの取得
     *
     * モーションの長さを取得する。
     *
     * @return  モーションの長さ[秒]
     */
    virtual csmFloat32  GetDuration();

    /**
     * @brief モーションのループ時の長さの取得
     *
     * モーションのループ時の長さを取得する。
     *
     * @return  モーションのループ時の長さ[秒]
     */
    virtual csmFloat32  GetLoopDuration();

    /**
     * @brief パラメータに対するフェードインの時間の設定
     *
     * パラメータに対するフェードインの時間を設定する。
     *
     * @param[in]   parameterId     パラメータID
     * @param[in]   value           フェードインにかかる時間[秒]
     */
    void        SetParameterFadeInTime(CubismIdHandle parameterId, csmFloat32 value);

    /**
    * @brief パラメータに対するフェードアウトの時間の設定
    *
    * パラメータに対するフェードアウトの時間を設定する。
    *
    * @param[in]   parameterId     パラメータID
    * @param[in]   value           フェードアウトにかかる時間[秒]
    */
    void        SetParameterFadeOutTime(CubismIdHandle parameterId, csmFloat32 value);

    /**
    * @brief パラメータに対するフェードインの時間の取得
    *
    * パラメータに対するフェードインの時間を取得する。
    *
    * @param[in]   parameterId     パラメータID
    * @return   フェードインにかかる時間[秒]
    */
    csmFloat32    GetParameterFadeInTime(CubismIdHandle parameterId) const;

    /**
    * @brief パラメータに対するフェードアウトの時間の取得
    *
    * パラメータに対するフェードアウトの時間を取得する。
    *
    * @param[in]   parameterId     パラメータID
    * @return   フェードアウトにかかる時間[秒]
    */
    csmFloat32    GetParameterFadeOutTime(CubismIdHandle parameterId) const;

    /**
     * @brief 自動エフェクトがかかっているパラメータIDリストの設定
     *
     * 自動エフェクトがかかっているパラメータIDリストを設定する。
     *
     * @param[in]   eyeBlinkParameterIds    自動まばたきがかかっているパラメータIDのリスト
     * @param[in]   lipSyncParameterIds     リップシンクがかかっているパラメータIDのリスト
     */
    void SetEffectIds(const csmVector<CubismIdHandle>& eyeBlinkParameterIds, const csmVector<CubismIdHandle>& lipSyncParameterIds);

    /**
    * @brief モデルのパラメータ更新
    *
    * イベント発火のチェック。
    * 入力する時間は呼ばれるモーションタイミングを０とした秒数で行う。
    *
    * @param[in]   beforeCheckTimeSeconds   前回のイベントチェック時間[秒]
    * @param[in]   motionTimeSeconds        今回の再生時間[秒]
    */
    virtual const csmVector<const csmString*>& GetFiredEvent(csmFloat32 beforeCheckTimeSeconds, csmFloat32 motionTimeSeconds);

    /**
    * @brief        透明度のカーブが存在するかどうかを確認する
    *
    * @retval       true  -> キーが存在する
    * @retval       false -> キーが存在しない
    */
    csmBool IsExistOpacity() const;

    /**
    * @brief 透明度のカーブのインデックスを返す
    *
    *
    * @return  success：透明度のカーブのインデックス
    */
    csmInt32 GetOpacityIndex() const;

    /**
    * @brief 透明度のIdを返す
    *
    *
    * @return  success：透明度のId
    */
    CubismIdHandle GetOpacityId(csmInt32 index);

    /**
    * @brief 指定時間の透明度の値を返す
    *
    * @param[in]   motionTimeSeconds        現在の再生時間[秒]
    *
    * @return  success：モーションの当該時間におけるOpacityの値
    */
    csmFloat32 GetOpacityValue(csmFloat32 motionTimeSeconds) const;

private:
    /**
     * @brief コンストラクタ
     *
     * コンストラクタ。
     */
    CubismMotion();

    /**
     * @brief デストラクタ
     *
     * デストラクタ。
     */
    virtual ~CubismMotion();

    // Prevention of copy Constructor
    CubismMotion(const CubismMotion&);
    CubismMotion& operator=(const CubismMotion&);

    /**
     * @brief motion3.jsonのパース
     *
     * motion3.jsonをパースする。
     *
     * @param[in]   motionJson  motion3.jsonが読み込まれているバッファ
     * @param[in]   size        バッファのサイズ
     */
    void Parse(const csmByte* motionJson, const csmSizeInt size);

    csmFloat32      _sourceFrameRate;                   ///< ロードしたファイルのFPS。記述が無ければデフォルト値15fpsとなる
    csmFloat32      _loopDurationSeconds;               ///< mtnファイルで定義される一連のモーションの長さ
    csmBool         _isLoop;                            ///< ループするか?
    csmBool         _isLoopFadeIn;                      ///< ループ時にフェードインが有効かどうかのフラグ。初期値では有効。
    csmFloat32      _lastWeight;                        ///< 最後に設定された重み

    CubismMotionData*    _motionData;                   ///< 実際のモーションデータ本体

    csmVector<CubismIdHandle>  _eyeBlinkParameterIds;   ///< 自動まばたきを適用するパラメータIDハンドルのリスト。  モデル（モデルセッティング）とパラメータを対応付ける。
    csmVector<CubismIdHandle>  _lipSyncParameterIds;    ///< リップシンクを適用するパラメータIDハンドルのリスト。  モデル（モデルセッティング）とパラメータを対応付ける。

    CubismIdHandle _modelCurveIdEyeBlink;               ///< モデルが持つ自動まばたき用パラメータIDのハンドル。  モデルとモーションを対応付ける。
    CubismIdHandle _modelCurveIdLipSync;                ///< モデルが持つリップシンク用パラメータIDのハンドル。  モデルとモーションを対応付ける。
};

}}}

//--------- LIVE2D NAMESPACE ------------
