/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "CubismFramework.hpp"
#include "CubismCommandBuffer_Axmol.hpp"
#include "Type/csmVector.hpp"
#include "Type/csmRectF.hpp"
#include "Type/csmMap.hpp"
#include <float.h>

#include "axmol/rhi/RenderTarget.h"

//------------ LIVE2D NAMESPACE ------------
namespace Live2D { namespace Cubism { namespace Framework { namespace Rendering {


/**
 * @brief  オフスクリーン描画用構造体
 */
class CubismOffscreenFrame_Axmol
{
public:

    CubismOffscreenFrame_Axmol();

    /**
     * @brief   指定の描画ターゲットに向けて描画開始
     *
     */
    void BeginDraw(CubismCommandBuffer_Axmol* commandBuffer, ax::rhi::Texture* colorBufferOnFinishDrawing);

    /**
     * @brief   描画終了
     *
     */
    void EndDraw(CubismCommandBuffer_Axmol* commandBuffer);

    /**
     * @brief   レンダリングターゲットのクリア
     *           呼ぶ場合はBeginDrawの後で呼ぶこと
     * @param   r   赤(0.0~1.0)
     * @param   g   緑(0.0~1.0)
     * @param   b   青(0.0~1.0)
     * @param   a   α(0.0~1.0)
     */
    void Clear(CubismCommandBuffer_Axmol* commandBuffer, float r, float g, float b, float a);

    /**
     *  @brief  CubismOffscreenFrame作成
     *  @param  displayBufferWidth     作成するバッファ幅
     *  @param  displayBufferHeight    作成するバッファ高さ
     *  @param  colorBuffer            0以外の場合、ピクセル格納領域としてcolorBufferを使用する
     */
    csmBool CreateOffscreenFrame(csmUint32 displayBufferWidth,
                                 csmUint32 displayBufferHeight,
                                 ax::RenderTexture* renderTexture = NULL,
                                 const ax::Color& clearColorHint = ax::Color{});

    /**
     * @brief   CubismOffscreenFrameの削除
     */
    void DestroyOffscreenFrame();

    /**
     * @brief   カラーバッファメンバーへのアクセッサ
     */
    ax::Texture2D* GetColorBuffer() const;

    /**
     * @brief   バッファ幅取得
     */
    csmUint32 GetBufferWidth() const;

    /**
     * @brief   バッファ高さ取得
     */
    csmUint32 GetBufferHeight() const;

    csmRectF GetViewPortSize() const;

    /**
     * @brief   現在有効かどうか
     */
    csmBool IsValid() const;

private:
    ax::RenderTexture*      _renderTexture;         ///< レンダリングターゲットとしてのアドレス
    ax::rhi::Texture*          _colorBuffer;
    csmBool _isInheritedRenderTexture;

    ax::rhi::Texture*      _previousColorBuffer; ///< 旧フレームバッファ

    csmUint32   _bufferWidth;           ///< Create時に指定された幅
    csmUint32   _bufferHeight;          ///< Create時に指定された高さ

    csmRectF _viewportSize;
};


}}}}

//------------ LIVE2D NAMESPACE ------------
