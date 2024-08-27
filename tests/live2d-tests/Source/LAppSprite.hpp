/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "Rendering/axmol/CubismRenderer_Cocos2dx.hpp"
#include "Rendering/axmol/CubismCommandBuffer_Cocos2dx.hpp"
#include "cocos2d.h"

using namespace ax;

/**
* @brief スプライトを実装するクラス。
*
* テクスチャID、Rectの管理。
*
*/
class LAppSprite
{
public:
    /**
    * @brief Rect 構造体。
    */
    struct Rect
    {
    public:
        float left;     ///< 左辺
        float right;    ///< 右辺
        float up;       ///< 上辺
        float down;     ///< 下辺
    };

    /**
    * @brief コンストラクタ
    *
    * @param[in]       x            x座標
    * @param[in]       y            y座標
    * @param[in]       width        横幅
    * @param[in]       height       高さ
    * @param[in]       textureId    テクスチャID
    * @param[in]       programId    シェーダID
    */
    LAppSprite(backend::Program* program);

    /**
    * @brief デストラクタ
    */
    ~LAppSprite();

    /**
    * @brief テクスチャIDを指定して描画する
    *
    */
    void RenderImmidiate(Csm::Rendering::CubismCommandBuffer_Cocos2dx* commandBuffer, backend::TextureBackend* texture, float uvVertex[8]) const;

    /**
     * @brief 色設定
     *
     * @param[in]       r (0.0~1.0)
     * @param[in]       g (0.0~1.0)
     * @param[in]       b (0.0~1.0)
     * @param[in]       a (0.0~1.0)
     */
    void SetColor(float r, float g, float b, float a);

private:

    float _spriteColor[4];  ///< 表示カラー

    ax::backend::Program* _program;
};

