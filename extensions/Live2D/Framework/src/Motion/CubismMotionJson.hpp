/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */


#pragma once

#include "CubismJsonHolder.hpp"
#include "Utils/CubismJson.hpp"
#include "Id/CubismId.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

/**
* @brief ベジェカーブの解釈方法のフラグタイプ
*/
enum EvaluationOptionFlag
{
    EvaluationOptionFlag_AreBeziersRistricted = 0,  ///< ベジェハンドルの規制状態
};

/**
 * @brief motion3.jsonのコンテナ。
 *
 * motion3.jsonのコンテナ。
 */
class CubismMotionJson : public CubismJsonHolder
{
public:
    /**
     * @brief コンストラクタ
     *
     * コンストラクタ。
     *
     * @param[in]   buffer  motion3.jsonが読み込まれているバッファ
     * @param[in]   size    バッファのサイズ
     */
    CubismMotionJson(const csmByte* buffer, csmSizeInt size);

    /**
     * @brief デストラクタ
     *
     * デストラクタ。
     */
    virtual ~CubismMotionJson();

    /**
     * @brief モーションの長さの取得
     *
     * モーションの長さを取得する。
     *
     * @return モーションの長さ[秒]
     */
    csmFloat32 GetMotionDuration() const;

    /**
     * @brief モーションのループ情報の取得
     *
     * モーションがループするかどうか？
     *
     * @retval  true    ループする
     * @retval  false   ループしない
     */
    csmBool IsMotionLoop() const;

    /**
     * @brief モーションのベジェカーブの解釈方式のフラグ取得
     *
     * モーション内のベジェカーブハンドルの解釈フラグの状態を取得する。
     *
     * @param[in]   flagType  EvaluationOptionFlagで指定されるフラグタイプ
     *
     * @retval  true    フラグあり
     * @retval  false   フラグなし
     */
    csmBool GetEvaluationOptionFlag(csmInt32 flagType) const;

    /**
     * @brief モーションカーブの個数の取得
     *
     * モーションカーブの個数を取得する。
     *
     * @return モーションカーブの個数
     */
    csmInt32 GetMotionCurveCount() const;

    /**
     * @brief モーションのフレームレートの取得
     *
     * モーションのフレームレートを取得する。
     *
     * @return フレームレート[FPS]
     */
    csmFloat32 GetMotionFps() const;

    /**
     * @brief モーションのセグメントの総合計の取得
     *
     * モーションのセグメントの総合計を取得する。
     *
     * @return モーションのセグメントの総合計
     */
    csmInt32 GetMotionTotalSegmentCount() const;

    /**
     * @brief モーションのカーブの制御点の総合計の取得
     *
     * モーションのカーブの制御点の総合計を取得する。
     *
     * @return モーションのカーブの制御点の総合計
     */
    csmInt32 GetMotionTotalPointCount() const;

    /**
    * @brief モーションのフェードイン時間の存在
    *
    * モーションにフェードイン時間が設定されているかどうか？
    *
    * @retval  true    存在する
    * @retval  false   存在しない
    */
    csmBool IsExistMotionFadeInTime() const;

    /**
    * @brief モーションのフェードアウト時間の存在
    *
    * モーションにフェードアウト時間が設定されているかどうか？
    *
    * @retval  true    存在する
    * @retval  false   存在しない
    */
    csmBool IsExistMotionFadeOutTime() const;

    /**
    * @brief モーションのフェードイン時間の取得
    *
    * モーションのフェードイン時間を取得する。
    *
    * @return  フェードイン時間[秒]
    */
    csmFloat32 GetMotionFadeInTime() const;

    /**
    * @brief モーションのフェードアウト時間の取得
    *
    * モーションのフェードアウト時間を取得する。
    *
    * @return  フェードアウト時間[秒]
    */
    csmFloat32 GetMotionFadeOutTime() const;

    /**
     * @brief モーションのカーブの種類の取得
     *
     * モーションのカーブの種類を取得する。
     *
     * @param[in]   curveIndex   カーブのインデックス
     * @return カーブの種類
     */
    const csmChar* GetMotionCurveTarget(csmInt32 curveIndex) const;

    /**
     * @brief モーションのカーブのIDの取得
     *
     * モーションのカーブのIDを取得する。
     *
     * @param[in]   curveIndex   カーブのインデックス
     * @return カーブのID
     */
    CubismIdHandle GetMotionCurveId(csmInt32 curveIndex) const;

    /**
     * @brief モーションのカーブのフェードイン時間の存在
     *
     * モーションのカーブにフェードイン時間が設定されているかどうか？
     *
     * @param[in]   curveIndex   カーブのインデックス
     * @retval  true    存在する
     * @retval  false   存在しない
     */
    csmBool IsExistMotionCurveFadeInTime(csmInt32 curveIndex) const;

    /**
     * @brief モーションのカーブのフェードアウト時間の存在
     *
     * モーションのカーブにフェードアウト時間が設定されているかどうか？
     *
     * @param[in]   curveIndex   カーブのインデックス
     * @retval  true    存在する
     * @retval  false   存在しない
     */
    csmBool IsExistMotionCurveFadeOutTime(csmInt32 curveIndex) const;

    /**
     * @brief モーションのカーブのフェードイン時間の取得
     *
     * モーションのカーブのフェードイン時間を取得する。
     *
     * @param[in]   curveIndex   カーブのインデックス
     * @return  フェードイン時間[秒]
     */
    csmFloat32 GetMotionCurveFadeInTime(csmInt32 curveIndex) const;

    /**
    * @brief モーションのカーブのフェードアウト時間の取得
    *
    * モーションのカーブのフェードアウト時間を取得する。
    *
    * @param[in]   curveIndex   カーブのインデックス
    * @return  フェードアウト時間[秒]
    */
    csmFloat32 GetMotionCurveFadeOutTime(csmInt32 curveIndex) const;

    /**
     * @brief モーションのカーブのセグメントの個数の取得
     *
     * モーションのカーブのセグメントの個数を取得する。
     *
     * @param[in]   curveIndex   カーブのインデックス
     * @return  モーションのカーブのセグメントの個数
     */
    csmInt32 GetMotionCurveSegmentCount(csmInt32 curveIndex) const;


    /**
     * @brief モーションのカーブのセグメントの値の取得
     *
     * モーションのカーブのセグメントの値を取得する、
     *
     * @param[in]   curveIndex      カーブのインデックス
     * @param[in]   segmentIndex    セグメントのインデックス
     * @return  セグメントの値
     */
    csmFloat32 GetMotionCurveSegment(csmInt32 curveIndex, csmInt32 segmentIndex) const;

    /**
    * @brief イベントの個数の取得
    *
    * イベントの個数の取得する。
    *
    * @return  イベントの個数
    */
    csmInt32 GetEventCount() const;

    /**
    * @brief イベントの総文字数の取得
    *
    * イベントの総文字数の取得する。
    *
    * @return  イベントの総文字数
    */
    csmInt32 GetTotalEventValueSize() const;

    /**
    * @brief イベントの時間の取得
    *
    * イベントの時間の取得する。
    *
    * @param[in]   userDataIndex イベントのインデックス
    * @return  イベントの時間[秒]
    */
    csmFloat32 GetEventTime(csmInt32 userDataIndex) const;

    /**
    * @brief イベントの取得
    *
    * イベントの取得する。
    *
    * @param[in]   userDataIndex    イベントのインデックス
    * @return  イベントの文字列
    */
    const csmChar* GetEventValue(csmInt32 userDataIndex) const;
};

}}}
