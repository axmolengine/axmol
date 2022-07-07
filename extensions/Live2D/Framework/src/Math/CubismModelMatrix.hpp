/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "CubismMatrix44.hpp"
#include "Type/csmMap.hpp"

namespace Live2D { namespace Cubism { namespace Framework {
/**
 * @brief モデル座標設定用の4x4行列
 *
 * モデル座標設定用の4x4行列クラス。
 */
class CubismModelMatrix : public CubismMatrix44
{
public:
    /**
     * @brief コンストラクタ
     *
     * コンストラクタ。
     */
    CubismModelMatrix();

    /**
     * @brief コンストラクタ
     *
     * コンストラクタ。
     *
     * @param[in]   w   横幅
     * @param[in]   h   縦幅
     */
    CubismModelMatrix(csmFloat32 w, csmFloat32 h);

    /**
    * @brief デストラクタ
    *
    * デストラクタ。
    */
    virtual ~CubismModelMatrix();

    /**
     * @brief 横幅を設定
     *
     * 横幅を設定する。
     *
     * @param[in]   w   横幅
     */
    void    SetWidth(csmFloat32 w);

    /**
     * @brief 縦幅を設定
     *
     * 縦幅を設定する。
     *
     * @param[in]   h   縦幅
     */
    void    SetHeight(csmFloat32 h);

    /**
     * @brief 位置を設定
     *
     * 位置を設定する。
     *
     * @param[in]   x   X軸の位置
     * @param[in]   y   Y軸の位置
     */
    void    SetPosition(csmFloat32 x, csmFloat32 y);

    /**
     * @brief 中心位置を設定
     *
     * 中心位置を設定する。
     *
     * @param[in]   x   X軸の中心位置
     * @param[in]   y   Y軸の中心位置
     *
     * @note widthかheightを設定したあとでないと、拡大率が正しく取得できないためずれる。
     */
    void    SetCenterPosition(csmFloat32 x, csmFloat32 y);

    /**
     * @brief 上辺の位置を設定
     *
     * 上辺の位置を設定する。
     *
     * @param[in]   y   上辺のY軸位置
     */
    void    Top(csmFloat32 y);

    /**
     * @brief 下辺の位置を設定
     *
     * 下辺の位置を設定する。
     *
     * @param[in]   y   下辺のY軸位置
     */
    void    Bottom(csmFloat32 y);

    /**
     * @brief 左辺の位置を設定
     *
     * 左辺の位置を設定する。
     *
     * @param[in]   x   左辺のX軸位置
     */
    void    Left(csmFloat32 x);

    /**
     * @brief 右辺の位置を設定
     *
     * 右辺の位置を設定する。
     *
     * @param[in]   x   右辺のX軸位置
     */
    void    Right(csmFloat32 x);

    /**
     * @brief X軸の中心位置を設定
     *
     * X軸の中心位置を設定する。
     *
     * @param[in]   x   X軸の中心位置
     */
    void    CenterX(csmFloat32 x);

    /**
     * @brief X軸の位置を設定
     *
     * X軸の位置を設定する。
     *
     * @param[in]   x   X軸の位置
     */
    void    SetX(csmFloat32 x);

    /**
     * @brief Y軸の中心位置を設定
     *
     * Y軸の中心位置を設定する。
     *
     * @param[in]   y   Y軸の中心位置
     */
    void    CenterY(csmFloat32 y);

    /**
     * @brief Y軸の位置を設定
     *
     * Y軸の位置を設定する。
     *
     * @param[in]   y   Y軸の位置
     */
    void    SetY(csmFloat32 y);

    /**
     * @brief レイアウト情報から位置を設定
     *
     * レイアウト情報から位置を設定する。
     *
     * @param[in]   layout  レイアウト情報
     */
    void    SetupFromLayout(csmMap<csmString, csmFloat32>& layout);

private:
    csmFloat32  _width;         ///< 横幅
    csmFloat32  _height;        ///< 縦幅
};

}}}
