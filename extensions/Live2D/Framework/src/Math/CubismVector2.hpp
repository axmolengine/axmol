/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "CubismFramework.hpp"
#include "math.h"

#ifndef NULL
#   define  NULL  0
#endif

namespace Live2D { namespace Cubism { namespace Framework {

/**
 * @brief 2次元ベクトル型
 *
 * 2次元ベクトル型の機能を提供する。
 */
struct CubismVector2
{
    csmFloat32 X;           ///< X軸の値
    csmFloat32 Y;           ///< Y軸の値

    /**
     * @brief コンストラクタ
     *
     * コンストラクタ。
     */
    CubismVector2(): X(0.0f), Y(0.0f)
    {}

    /**
     * @brief デストラクタ
     *
     * デストラクタ。
     */
    CubismVector2(csmFloat32 x, csmFloat32 y) : X(x), Y(y)
    {}

    /**
     * @brief ベクトルの加算
     *
     * ベクトルを加算する。
     * a + b = result
     *
     * @param[in]   a   値a
     * @oaram[in]   b   値b
     * @return  a + bの結果
     */
    friend CubismVector2 operator+(const CubismVector2& a, const CubismVector2& b);

    /**
     * @brief ベクトルの減算
     *
     * ベクトルを減算する。
     * a - b = result
     *
     * @param[in]   a   値a
     * @param[in]   b   値b
     * @return  a - bの結果
     */
    friend CubismVector2 operator-(const CubismVector2& a, const CubismVector2& b);

    /**
     * @brief ベクトルの乗算（ベクトル値とスカラー値）
     *
     * ベクトルを乗算する。（ベクトル値とスカラー値）
     * vector * scalar = result
     *
     * @param[in]   vector  値vector（ベクトル値）
     * @param[in]   scalar  値scalar（スカラー値）
     * @return  vector * scalarの結果（ベクトル値）
     */
    friend CubismVector2 operator*(const CubismVector2& vector, const csmFloat32 scalar);

    /**
     * @brief ベクトルの乗算（スカラー値とベクトル値）
     *
     * ベクトルを乗算する。（スカラー値とベクトル値）
     * scalar * vector = result
     *
     * @param[in]   scalar  値scalar（スカラー値）
     * @param[in]   vector  値vector（ベクトル値）
     * @return  scalar * vectorの結果（ベクトル値）
     */
    friend CubismVector2 operator*(const csmFloat32 scalar, const CubismVector2& vector);

    /**
     * @brief ベクトルの除算（ベクトル値とスカラー値）
     *
     * ベクトルを除算する。（ベクトル値とスカラー値）
     * vector / scalar = result
     *
     * @param[in]   vector  値vector（ベクトル値）
     * @param[in]   scalar  値scalar（スカラー値）
     * @return  vector / scalarの結果（ベクトル値）
     */
    friend CubismVector2 operator/(const CubismVector2& vector, const csmFloat32 scalar);

    /**
     * @brief 加算
     *
     * 加算する。
     *
     * @param[in]   rhs     加算する値
     * @return  結果
     */
    const CubismVector2& operator+=(const CubismVector2& rhs);

    /**
    * @brief 減算
    *
    * 減算する。
    *
    * @param[in]   rhs     減算する値
    * @return  結果
    */
    const CubismVector2& operator-=(const CubismVector2& rhs);

    /**
    * @brief 乗算（ベクトル値）
    *
    * 乗算する。（ベクトル値）
    *
    * @param[in]   rhs     乗算する値（ベクトル値）
    * @return  結果
    */
    const CubismVector2& operator*=(const CubismVector2& rhs);

    /**
    * @brief 除算（ベクトル値）
    *
    * 除算する。（ベクトル値）
    *
    * @param[in]   rhs     除算する値（ベクトル値）
    * @return  結果
    */
    const CubismVector2& operator/=(const CubismVector2& rhs);

    /**
    * @brief 乗算（スカラー値）
    *
    * 乗算する。（スカラー値）
    *
    * @param[in]   scalar     乗算する値（スカラー値）
    * @return  結果
    */
    const CubismVector2& operator*=(const csmFloat32 scalar);

    /**
    * @brief 除算（スカラー値）
    *
    * 除算する。（スカラー値）
    *
    * @param[in]   scalar     除算する値（スカラー値）
    * @return  結果
    */
    const CubismVector2& operator/=(const csmFloat32 scalar);

    /**
     * @brief 等しさの確認（等しいか？）
     *
     * 値が等しいか？
     *
     * @param[in]   rhs     確認する値
     * @retval  true    値は等しい
     * @retval  false   値は等しくない
     */
    csmBool operator==(const CubismVector2& rhs) const;

    /**
     * @brief 等しさの確認（等しくないか？）
     *
     * 値が等しくないか？
     *
     * @param[in]   rhs     確認する値
     * @retval  true    値は等しくない
     * @retval  false   値は等しい
     */
    csmBool operator!=(const CubismVector2& rhs) const;

    /**
     * @brief 正規化の適用
     *
     * 正規化する。
     */
    void Normalize();

    /**
     * @brief ベクトルの長さの取得
     *
     * ベクトルの長さを取得する。
     *
     * @return  ベクトルの長さ
     */
    csmFloat32 GetLength() const;

    /**
     * @brief ベクトルの距離の取得
     *
     * ベクトルの距離を取得する。
     *
     * @param[in]   a   点
     * @return ベクトルの距離
     */
    csmFloat32 GetDistanceWith(CubismVector2 a) const;

    /**
     * @brief ドット積の計算
     *
     * ドット積を計算する。
     *
     * @param[in]   a   値
     * @return 結果
     */
    csmFloat32 Dot(const CubismVector2& a) const;
};

// Utility functions for csmVector2.
CubismVector2 operator+(const CubismVector2& a, const CubismVector2& b);
CubismVector2 operator-(const CubismVector2& a, const CubismVector2& b);
CubismVector2 operator*(const CubismVector2& vector, const csmFloat32 scalar);
CubismVector2 operator*(const csmFloat32 scalar, const CubismVector2& vector);
CubismVector2 operator/(const CubismVector2& vector, const csmFloat32 scalar);

}}}

//------------------------- LIVE2D NAMESPACE ------------
