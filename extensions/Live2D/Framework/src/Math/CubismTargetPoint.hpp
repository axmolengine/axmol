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
 * @brief 顔の向きの制御機能
 *
 * 顔の向きの制御機能を提供するクラス。
 */
class CubismTargetPoint
{
public:
    /**
     * @brief コンストラクタ
     *
     * コンストラクタ。
     */
    CubismTargetPoint();

    /**
     * @brief デストラクタ
     *
     * デストラクタ。
     */
    virtual ~CubismTargetPoint();

    /**
     * @brief 更新処理
     *
     * 更新処理を行う。
     *
     * @param[in]   deltaTimeSeconds   デルタ時間[秒]
     */
    void        Update(csmFloat32 deltaTimeSeconds);

    /**
     * @brief X軸の顔の向きの値を取得
     *
     * X軸の顔の向きの値を取得する。
     *
     * @return X軸の顔の向きの値(-1.0 - 1.0)
     */
    csmFloat32  GetX() const;

    /**
    * @brief Y軸の顔の向きの値を取得
    *
    * Y軸の顔の向きの値を取得する。
    *
    * @return Y軸の顔の向きの値(-1.0 - 1.0)
    */
    csmFloat32  GetY() const;

    /**
     * @brief 顔の向きの目標値を設定
     *
     * 顔の向きの目標値を設定する。
     *
     * @param[in]   x   X軸の顔の向きの値(-1.0 - 1.0)
     * @param[in]   y   Y軸の顔の向きの値(-1.0 - 1.0)
     */
    void        Set(csmFloat32 x, csmFloat32 y);

private:
    csmFloat32  _faceTargetX;       ///< 顔の向きのX目標値(この値に近づいていく)
    csmFloat32  _faceTargetY;       ///< 顔の向きのY目標値(この値に近づいていく)
    csmFloat32  _faceX;             ///< 顔の向きX(-1.0 - 1.0)
    csmFloat32  _faceY;             ///< 顔の向きY(-1.0 - 1.0)
    csmFloat32  _faceVX;            ///< 顔の向きの変化速度X
    csmFloat32  _faceVY;            ///< 顔の向きの変化速度Y
    csmFloat32  _lastTimeSeconds;   ///< 最後の実行時間[秒]
    csmFloat32  _userTimeSeconds;   ///< デルタ時間の積算値[秒]

};

}}}
