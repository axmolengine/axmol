/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "CubismFramework.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

/**
 * @brief モーションカーブの種類
 *
 * モーションカーブの種類。
 */
enum CubismMotionCurveTarget
{
    CubismMotionCurveTarget_Model,          ///< モデルに対して
    CubismMotionCurveTarget_Parameter,      ///< パラメータに対して
    CubismMotionCurveTarget_PartOpacity     ///< パーツの不透明度に対して
};


/**
 * @brief モーションカーブのセグメントの種類
 *
 * モーションカーブのセグメントの種類。
 */
enum CubismMotionSegmentType
{
    CubismMotionSegmentType_Linear = 0,         ///< リニア
    CubismMotionSegmentType_Bezier = 1,         ///< ベジェ曲線
    CubismMotionSegmentType_Stepped = 2,        ///< ステップ
    CubismMotionSegmentType_InverseStepped = 3  ///< インバースステップ
};

/**
 * @brief モーションカーブの制御点
 *
 * モーションカーブの制御点。
 */
struct CubismMotionPoint
{
    CubismMotionPoint()
        : Time(0.0f)
        , Value(0.0f)
    { }

    csmFloat32 Time;         ///< 時間[秒]
    csmFloat32 Value;        ///< 値
};

/**
 * @brief モーションカーブのセグメントの評価関数
 *
 * モーションカーブのセグメントの評価関数。
 *
 * @param[in]   points      モーションカーブの制御点リスト
 * @param[in]   time        評価する時間[秒]
 */
typedef csmFloat32 (*csmMotionSegmentEvaluationFunction)(const CubismMotionPoint* points, const csmFloat32 time);


/**
 * @brief モーションカーブのセグメント
 *
 * モーションカーブのセグメント。
 */
struct CubismMotionSegment
{
    /**
     * @brief コンストラクタ
     *
     * コンストラクタ。
     */
    CubismMotionSegment()
        : Evaluate(NULL)
        , BasePointIndex(0)
        , SegmentType(0)
    { }

    csmMotionSegmentEvaluationFunction Evaluate;            ///< 使用する評価関数
    csmInt32 BasePointIndex;                                ///< 最初のセグメントへのインデックス
    csmInt32 SegmentType;                                   ///< セグメントの種類
};

/**
 * @brief モーションカーブ
 *
 * モーションカーブ。
 */
struct CubismMotionCurve
{
    CubismMotionCurve()
        : Type(CubismMotionCurveTarget_Model)
        , SegmentCount(0)
        , BaseSegmentIndex(0)
        , FadeInTime(0.0f)
        , FadeOutTime(0.0f)
    { }

    CubismMotionCurveTarget Type;               ///< カーブの種類
    CubismIdHandle Id;                               ///< カーブのID
    csmInt32 SegmentCount;                      ///< セグメントの個数
    csmInt32 BaseSegmentIndex;                  ///< 最初のセグメントのインデックス
    csmFloat32 FadeInTime;                      ///< フェードインにかかる時間[秒]
    csmFloat32 FadeOutTime;                     ///< フェードアウトにかかる時間[秒]
};

/**
* @brief イベント
*
* イベント。
*/
struct CubismMotionEvent
{
    CubismMotionEvent()
        : FireTime(0.0f)
    { }

    csmFloat32  FireTime;
    csmString   Value;
};

/**
 * @brief モーションデータ
 *
 * モーションデータ。
 */
struct CubismMotionData
{
    CubismMotionData()
        : Duration(0.0f)
        , Loop(0)
        , CurveCount(0)
        , EventCount(0)
        , Fps(0.0f)
    { }

    csmFloat32 Duration;                                ///< モーションの長さ[秒]
    csmInt16 Loop;                                  ///< ループするかどうか
    csmInt16 CurveCount;                            ///< カーブの個数
    csmInt32 EventCount;                         ///< UserDataの個数
    csmFloat32 Fps;                              ///< フレームレート
    csmVector<CubismMotionCurve> Curves;                ///< カーブのリスト
    csmVector<CubismMotionSegment> Segments;            ///< セグメントのリスト
    csmVector<CubismMotionPoint> Points;                ///< ポイントのリスト
    csmVector<CubismMotionEvent> Events;          ///< イベントのリスト
};

}}}
