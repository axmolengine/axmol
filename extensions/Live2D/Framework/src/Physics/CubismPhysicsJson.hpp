/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "CubismJsonHolder.hpp"
#include "Utils/CubismJson.hpp"
#include "Math/CubismVector2.hpp"
#include "Id/CubismId.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

/**
 * @brief physics3.jsonのコンテナ
 *
 * physics3.jsonのコンテナ。
 */
class CubismPhysicsJson : public CubismJsonHolder
{
public:
    /**
     * @brief コンストラクタ
     *
     * コンストラクタ。
     *
     * @param[in]   buffer  physics3.jsonが読み込まれているバッファ
     * @param[in]   size    バッファのサイズ
     */
    CubismPhysicsJson(const csmByte* buffer, csmSizeInt size);

    /**
     * @brief デストラクタ
     *
     * デストラクタ。
     */
    virtual ~CubismPhysicsJson();

    /**
     * @brief 重力の取得
     *
     * 重力を取得する。
     *
     * @return 重力
     */
    CubismVector2 GetGravity() const;

    /**
     * @brief 風の取得
     *
     * 風を取得する。
     *
     * @return 風
     */
    CubismVector2 GetWind() const;

    /**
     * @brief 物理演算設定FPSの取得
     *
     * 物理演算の想定FPSを取得する。
     * physics3.jsonにFPS情報が存在しない場合、0.0fを返す。
     *
     * @return 物理演算設定FPS
     */
    csmFloat32 GetFps() const;

    /**
     * @brief 物理点の管理の個数の取得
     *
     * 物理点の管理の個数を取得する。
     *
     * @return 物理点の管理の個数
     */
    csmInt32 GetSubRigCount() const;

    /**
     * @brief 入力の総合計の取得
     *
     * 入力の総合計を取得する。
     *
     * @return 入力の総合計
     */
    csmInt32 GetTotalInputCount() const;

    /**
     * @brief 出力の総合計の取得
     *
     * 出力の総合計を取得する。
     *
     * @return 出力の総合計
     */
    csmInt32 GetTotalOutputCount() const;

    /**
     * @brief 物理点の個数の取得
     *
     * 物理点の個数を取得する。
     *
     * @return 物理点の個数
     */
    csmInt32 GetVertexCount() const;

    /**
     * @brief 正規化された位置の最小値の取得
     *
     * 正規化された位置の最小値を取得する。
     *
     * @param[in]   physicsSettingIndex   物理演算の設定のインデックス
     * @return 正規化された位置の最小値
     */
    csmFloat32 GetNormalizationPositionMinimumValue(csmInt32 physicsSettingIndex) const;

    /**
    * @brief 正規化された位置の最大値の取得
    *
    * 正規化された位置の最大値を取得する。
    *
    * @param[in]   physicsSettingIndex   物理演算の設定のインデックス
    * @return 正規化された位置の最大値
    */
    csmFloat32 GetNormalizationPositionMaximumValue(csmInt32 physicsSettingIndex) const;

    /**
    * @brief 正規化された位置のデフォルト値の取得
    *
    * 正規化された位置のデフォルト値を取得する。
    *
    * @param[in]   physicsSettingIndex   物理演算の設定のインデックス
    * @return 正規化された位置のデフォルト値
    */
    csmFloat32 GetNormalizationPositionDefaultValue(csmInt32 physicsSettingIndex) const;

    /**
    * @brief 正規化された角度の最小値の取得
    *
    * 正規化された角度の最小値を取得する。
    *
    * @param[in]   physicsSettingIndex   物理演算の設定のインデックス
    * @return 正規化された角度の最小値
    */
    csmFloat32 GetNormalizationAngleMinimumValue(csmInt32 physicsSettingIndex) const;

    /**
    * @brief 正規化された角度の最大値の取得
    *
    * 正規化された角度の最大値を取得する。
    *
    * @param[in]   physicsSettingIndex   物理演算の設定のインデックス
    * @return 正規化された角度の最大値
    */
    csmFloat32 GetNormalizationAngleMaximumValue(csmInt32 physicsSettingIndex) const;

    /**
    * @brief 正規化された角度のデフォルト値の取得
    *
    * 正規化された角度のデフォルト値を取得する。
    *
    * @param[in]   physicsSettingIndex   物理演算の設定のインデックス
    * @return 正規化された角度のデフォルト値
    */
    csmFloat32 GetNormalizationAngleDefaultValue(csmInt32 physicsSettingIndex) const;

    /**
     * @brief 入力の個数の取得
     *
     * 入力の個数を取得する。
     *
     * @param[in]   physicsSettingIndex   物理演算の設定のインデックス
     * @return 入力の個数
     */
    csmInt32 GetInputCount(csmInt32 physicsSettingIndex) const;

    /**
    * @brief 入力の重みの取得
    *
    * 入力の重みを取得する。
    *
    * @param[in]   physicsSettingIndex      物理演算の設定のインデックス
    * @param[in]   inputIndex               入力のインデックス
    * @return 入力の重み
    */
    csmFloat32 GetInputWeight(csmInt32 physicsSettingIndex, csmInt32 inputIndex) const;

    /**
    * @brief 入力の反転の取得
    *
    * 入力の反転を取得する。
    *
    * @param[in]   physicsSettingIndex      物理演算の設定のインデックス
    * @param[in]   inputIndex               入力のインデックス
    * @return 入力の反転
    */
    csmBool GetInputReflect(csmInt32 physicsSettingIndex, csmInt32 inputIndex) const;

    /**
    * @brief 入力の種類の取得
    *
    * 入力の種類を取得する。
    *
    * @param[in]   physicsSettingIndex      物理演算の設定のインデックス
    * @param[in]   inputIndex               入力のインデックス
    * @return 入力の種類
    */
    const csmChar* GetInputType(csmInt32 physicsSettingIndex, csmInt32 inputIndex) const;

    /**
    * @brief 入力元のIDの取得
    *
    * 入力元のIDを取得する。
    *
    * @param[in]   physicsSettingIndex      物理演算の設定のインデックス
    * @param[in]   inputIndex               入力のインデックス
    * @return 入力元のID
    */
    CubismIdHandle GetInputSourceId(csmInt32 physicsSettingIndex, csmInt32 inputIndex) const;

    /**
    * @brief 出力の個数の取得
    *
    * 出力の個数を取得する。
    *
    * @param[in]   physicsSettingIndex   物理演算の設定のインデックス
    * @return 出力の個数
    */
    csmInt32 GetOutputCount(csmInt32 physicsSettingIndex) const;

    /**
    * @brief 出力の物理点のインデックスの取得
    *
    * 出力の物理点のインデックスを取得する。
    *
    * @param[in]   physicsSettingIndex      物理演算の設定のインデックス
    * @param[in]   outputIndex              出力のインデックス
    * @return 出力の物理点のインデックス
    */
    csmInt32 GetOutputVertexIndex(csmInt32 physicsSettingIndex, csmInt32 outputIndex) const;

    /**
    * @brief 出力の角度のスケールの取得
    *
    * 出力の角度のスケールを取得する。
    *
    * @param[in]   physicsSettingIndex      物理演算の設定のインデックス
    * @param[in]   outputIndex              出力のインデックス
    * @return 出力の角度のスケール
    */
    csmFloat32 GetOutputAngleScale(csmInt32 physicsSettingIndex, csmInt32 outputIndex) const;

    /**
    * @brief 出力の重みの取得
    *
    * 出力の重みを取得する。
    *
    * @param[in]   physicsSettingIndex      物理演算の設定のインデックス
    * @param[in]   outputIndex              出力のインデックス
    * @return 出力の重み
    */
    csmFloat32 GetOutputWeight(csmInt32 physicsSettingIndex, csmInt32 outputIndex) const;

    /**
    * @brief 出力先のIDの取得
    *
    * 出力先のIDを取得する。
    *
    * @param[in]   physicsSettingIndex      物理演算の設定のインデックス
    * @param[in]   outputIndex              出力のインデックス
    * @return 出力先のID
    */
    CubismIdHandle GetOutputsDestinationId(csmInt32 physicsSettingIndex, csmInt32 outputIndex) const;

    /**
    * @brief 出力の種類の取得
    *
    * 出力の種類を取得する。
    *
    * @param[in]   physicsSettingIndex      物理演算の設定のインデックス
    * @param[in]   outputIndex              出力のインデックス
    * @return 出力の種類
    */
    const csmChar* GetOutputType(csmInt32 physicsSettingIndex, csmInt32 outputIndex) const;

    /**
    * @brief 出力の反転の取得
    *
    * 出力の反転を取得する。
    *
    * @param[in]   physicsSettingIndex      物理演算の設定のインデックス
    * @param[in]   outputIndex              出力のインデックス
    * @return 出力の反転
    */
    csmBool GetOutputReflect(csmInt32 physicsSettingIndex, csmInt32 outputIndex) const;

    /**
     * @brief 物理点の個数の取得
     *
     * 物理点の個数を取得する。
     *
     * @param[in]   physicsSettingIndex   物理演算の設定のインデックス
     * @return 物理点の個数
     */
    csmInt32 GetParticleCount(csmInt32 physicsSettingIndex) const;

    /**
     * @brief 物理点の動きやすさの取得
     *
     * 物理点の動きやすさを取得する。
     *
     * @param[in]   physicsSettingIndex     物理演算の設定のインデックス
     * @param[in]   vertexIndex             物理点のインデックス
     * @return 物理点の動きやすさ
     */
    csmFloat32 GetParticleMobility(csmInt32 physicsSettingIndex, csmInt32 vertexIndex) const;

    /**
    * @brief 物理点の遅れの取得
    *
    * 物理点の遅れを取得する。
    *
    * @param[in]   physicsSettingIndex   物理演算の設定のインデックス
    * @param[in]   vertexIndex           物理点のインデックス
    * @return 物理点の遅れ
    */
    csmFloat32 GetParticleDelay(csmInt32 physicsSettingIndex, csmInt32 vertexIndex) const;

    /**
    * @brief 物理点の加速度の取得
    *
    * 物理点の加速度を取得する。
    *
    * @param[in]   physicsSettingIndex   物理演算の設定のインデックス
    * @param[in]   vertexIndex           物理点のインデックス
    * @return 物理点の加速度
    */
    csmFloat32 GetParticleAcceleration(csmInt32 physicsSettingIndex, csmInt32 vertexIndex) const;

    /**
    * @brief 物理点の距離の取得
    *
    * 物理点の距離を取得する。
    *
    * @param[in]   physicsSettingIndex      物理演算の設定のインデックス
    * @param[in]   vertexIndex              物理点のインデックス
    * @return 物理点の距離
    */
    csmFloat32 GetParticleRadius(csmInt32 physicsSettingIndex, csmInt32 vertexIndex) const;

    /**
    * @brief 物理点の位置の取得
    *
    * 物理点の位置を取得する。
    *
    * @param[in]   physicsSettingIndex   物理演算の設定のインデックス
    * @param[in]   vertexIndex           物理点のインデックス
    * @return 物理点の位置
    */
    CubismVector2 GetParticlePosition(csmInt32 physicsSettingIndex, csmInt32 vertexIndex) const;
};

}}}
