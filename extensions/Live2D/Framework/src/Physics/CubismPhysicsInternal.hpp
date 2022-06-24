/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */


#pragma once

#include "Model/CubismModel.hpp"
#include "Math/CubismVector2.hpp"
#include "Id/CubismId.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

/**
 * @brief 物理演算の適用先の種類
 *
 * 物理演算の適用先の種類。
 */
enum CubismPhysicsTargetType
{
    CubismPhysicsTargetType_Parameter,          ///< パラメータに対して適用
};

/**
 * @brief 物理演算の入力の種類
 *
 * 物理演算の入力の種類。
 */
enum CubismPhysicsSource
{
    CubismPhysicsSource_X,          ///< X軸の位置から
    CubismPhysicsSource_Y,          ///< Y軸の位置から
    CubismPhysicsSource_Angle,      ///< 角度から
};

/**
 * @brief 物理演算で使用する外部の力
 *
 * 物理演算で使用する外部の力。
 */
struct PhysicsJsonEffectiveForces
{
    CubismVector2 Gravity;          ///< 重力
    CubismVector2 Wind;             ///< 風
};

/**
 * @brief 物理演算のパラメータ情報
 *
 * 物理演算のパラメータ情報。
 */
struct CubismPhysicsParameter
{
    CubismIdHandle Id;                           ///< パラメータID
    CubismPhysicsTargetType TargetType;     ///< 適用先の種類
};

/**
 * @brief 物理演算の正規化情報
 *
 * 物理演算の正規化情報。
 */
struct CubismPhysicsNormalization
{
    csmFloat32 Minimum;             ///< 最大値
    csmFloat32 Maximum;             ///< 最小値
    csmFloat32 Default;             ///< デフォルト値
};

/**
 * @brief 物理演算の演算に使用する物理点の情報
 *
 * 物理演算の演算に使用する物理点の情報。
 */
struct CubismPhysicsParticle
{
    CubismVector2 InitialPosition;          ///< 初期位置
    csmFloat32 Mobility;                    ///< 動きやすさ
    csmFloat32 Delay;                       ///< 遅れ
    csmFloat32 Acceleration;                ///< 加速度
    csmFloat32 Radius;                      ///< 距離
    CubismVector2 Position;                 ///< 現在の位置
    CubismVector2 LastPosition;             ///< 最後の位置
    CubismVector2 LastGravity;              ///< 最後の重力
    CubismVector2 Force;                    ///< 現在かかっている力
    CubismVector2 Velocity;                 ///< 現在の速度
};

/**
 * @brief 物理演算の物理点の管理
 *
 * 物理演算の物理点の管理。
 */
struct CubismPhysicsSubRig
{
    csmInt32 InputCount;                                        ///< 入力の個数
    csmInt32 OutputCount;                                       ///< 出力の個数
    csmInt32 ParticleCount;                                     ///< 物理点の個数
    csmInt32 BaseInputIndex;                                    ///< 入力の最初のインデックス
    csmInt32 BaseOutputIndex;                                   ///< 出力の最初のインデックス
    csmInt32 BaseParticleIndex;                                 ///< 物理点の最初のインデックス
    CubismPhysicsNormalization NormalizationPosition;           ///< 正規化された位置
    CubismPhysicsNormalization NormalizationAngle;              ///< 正規化された角度
};

/**
 * @brief 正規化されたパラメータの取得関数の宣言
 *
 * 正規化されたパラメータの取得関数の宣言。
 *
 * @param[out]      targetTranslation       演算結果の移動値
 * @param[out]      targetAngle             演算結果の角度
 * @param[in]       value                   パラメータの値
 * @param[in]       parameterMinimumValue   パラメータの最小値
 * @param[in]       parameterMaximumValue   パラメータの最大値
 * @param[in]       parameterDefaultValue   パラメータのデフォルト値
 * @param[in]       normalizationPosition   正規化された位置
 * @param[in]       normalizationAngle      正規化された角度
 * @param[in]       isInverted              値が反転されているか？
 * @param[in]       weight                  重み
 */
typedef void (*NormalizedPhysicsParameterValueGetter)(
    CubismVector2* targetTranslation,
    csmFloat32* targetAngle,
    csmFloat32 value,
    csmFloat32 parameterMinimumValue,
    csmFloat32 parameterMaximumValue,
    csmFloat32 parameterDefaultValue,
    CubismPhysicsNormalization* normalizationPosition,
    CubismPhysicsNormalization* normalizationAngle,
    csmInt32 isInverted,
    csmFloat32 weight
);

/**
 * @brief 物理演算の値の取得関数の宣言
 *
 * 物理演算の値の取得関数の宣言。
 *
 * @param[in]       translation     移動値
 * @param[in]       particles       物理点のリスト
 * @param[in]       isInverted      値が反転されているか？
 * @param[in]       parentGravity   重力
 * @return  値
 */
typedef csmFloat32 (*PhysicsValueGetter)(
    CubismVector2 translation,
    CubismPhysicsParticle* particles,
    csmInt32 particleIndex,
    csmInt32 isInverted,
    CubismVector2 parentGravity
);

/**
 * @brief 物理演算のスケールの取得関数の宣言
 *
 * 物理演算のスケールの取得関数の宣言。
 *
 * @param[in]   translationScale    移動値のスケール
 * @param[in]   angleScale          角度のスケール
 * @return  スケール値
 */
typedef csmFloat32 (*PhysicsScaleGetter)(CubismVector2 translationScale, csmFloat32 angleScale);

/**
 * @brief 物理演算の入力情報
 *
 * 物理演算の入力情報。
 */
struct CubismPhysicsInput
{
    CubismPhysicsParameter Source;                  ///< 入力元のパラメータ
    csmInt32 SourceParameterIndex;                  ///< 入力元のパラメータのインデックス
    csmFloat32 Weight;                              ///< 重み
    csmInt16 Type;                                  ///< 入力の種類
    csmInt16 Reflect;                               ///< 値が反転されているかどうか
    NormalizedPhysicsParameterValueGetter GetNormalizedParameterValue;          ///< 正規化されたパラメータ値の取得関数
};

/**
 * @brief 物理演算の出力情報
 *
 * 物理演算の出力情報。
 */
struct CubismPhysicsOutput
{
    CubismPhysicsParameter Destination;         ///< 出力先のパラメータ
    csmInt32 DestinationParameterIndex;         ///< 出力先のパラメータのインデックス
    csmInt32 VertexIndex;                       ///< 振り子のインデックス
    CubismVector2 TranslationScale;             ///< 移動値のスケール
    csmFloat32 AngleScale;                      ///< 角度のスケール
    csmFloat32 Weight;                          /// 重み
    CubismPhysicsSource Type;                   ///< 出力の種類
    csmInt16 Reflect;                           ///< 値が反転されているかどうか
    csmFloat32 ValueBelowMinimum;               ///< 最小値を下回った時の値
    csmFloat32 ValueExceededMaximum;            ///< 最大値をこえた時の値
    PhysicsValueGetter GetValue;                ///< 物理演算の値の取得関数
    PhysicsScaleGetter GetScale;                ///< 物理演算のスケール値の取得関数
};

/**
 * @brief 物理演算のデータ
 *
 * 物理演算のデータ。
 */
struct CubismPhysicsRig
{
    csmInt32 SubRigCount;                           ///< 物理演算の物理点の個数
    csmVector<CubismPhysicsSubRig> Settings;        ///< 物理演算の物理点の管理のリスト
    csmVector<CubismPhysicsInput> Inputs;           ///< 物理演算の入力のリスト
    csmVector<CubismPhysicsOutput> Outputs;         ///< 物理演算の出力のリスト
    csmVector<CubismPhysicsParticle> Particles;     ///< 物理演算の物理点のリスト
    CubismVector2 Gravity;                          ///< 重力
    CubismVector2 Wind;                             ///< 風
};

}}}
