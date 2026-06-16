/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <float.h>
#include "CubismFramework.hpp"
#include "Type/csmVector.hpp"
#include "Type/csmRectF.hpp"
#include "Math/CubismVector2.hpp"
#include "Math/CubismMatrix44.hpp"
#include "Model/CubismModel.hpp"

//------------ LIVE2D NAMESPACE ------------
namespace Live2D { namespace Cubism { namespace Framework { namespace Rendering {

///< ファイルスコープの変数宣言
#ifndef CSM_MASK_FILESCOPE
#define CSM_MASK_FILESCOPE
namespace {
const csmInt32 ColorChannelCount = 4;   // 実験時に1チャンネルの場合は1、RGBだけの場合は3、アルファも含める場合は4
const csmInt32 ClippingMaskMaxCountOnDefault = 36;  // 通常のフレームバッファ1枚あたりのマスク最大数
const csmInt32 ClippingMaskMaxCountOnMultiRenderTexture = 32;   // フレームバッファが2枚以上ある場合のフレームバッファ1枚あたりのマスク最大数
}
#endif

template <class T_ClippingContext, class T_RenderTarget>
class CubismClippingManager
{
public:
    /**
     * @brief    コンストラクタ
     */
    CubismClippingManager();

    /**
     * @brief    デストラクタ
     */
    virtual ~CubismClippingManager();

    /**
     * @brief    マネージャの初期化処理<br>
     *           クリッピングマスクを使うDrawableオブジェクトの登録を行う
     *
     * @param[in]   model           ->  モデルのインスタンス
     * @param[in]   maskBufferCount ->  バッファの生成数
     * @param[in]   drawableObjectType ->  処理するオブジェクトタイプ
     */
    void Initialize(CubismModel& model, const csmInt32 maskBufferCount, CubismRenderer::DrawableObjectType drawableObjectType);

    /**
     * @brief   既にマスクを作っているかを確認。<br>
     *          作っているようであれば該当するクリッピングマスクのインスタンスを返す。<br>
     *          作っていなければNULLを返す
     *
     * @param[in]   drawableMasks    ->  描画オブジェクトをマスクする描画オブジェクトのリスト
     * @param[in]   drawableMaskCounts ->  描画オブジェクトをマスクする描画オブジェクトの数
     * @return          該当するクリッピングマスクが存在すればインスタンスを返し、なければNULLを返す。
     */
    T_ClippingContext* FindSameClip(const csmInt32* drawableMasks, csmInt32 drawableMaskCounts) const;

    /**
     * @brief   Drawableの高精細マスク処理用の行列を計算する
     *
     * @param[in]   model         ->  モデルのインスタンス
     * @param[in]   isRightHanded ->  処理が右手系であるか
     * @param[in]   drawableObjectType ->  処理するオブジェクトタイプ
     * @param[in]   mvp ->  MVP行列
     */
    void SetupMatrixForHighPrecision(CubismModel& model, csmBool isRightHanded, CubismRenderer::DrawableObjectType drawableObjectType, const CubismMatrix44& mvp = CubismMatrix44());

    /**
     * @brief   マスク作成・描画用の行列を作成する。
     *
     * @param[in]   isRightHanded        ->  座標を右手系として扱うかを指定
     * @param[in]   layoutBoundsOnTex01  ->  マスクを収める領域
     * @param[in]   scaleX               ->  描画オブジェクトの伸縮率
     * @param[in]   scaleY               ->  描画オブジェクトの伸縮率
     */
    void CreateMatrixForMask(csmBool isRightHanded, csmRectF* layoutBoundsOnTex01, csmFloat32 scaleX, csmFloat32 scaleY);

    /**
     * @brief   クリッピングコンテキストを配置するレイアウト。<br>
     *           ひとつのレンダーテクスチャを極力いっぱいに使ってマスクをレイアウトする。<br>
     *           マスクグループの数が4以下ならRGBA各チャンネルに１つずつマスクを配置し、5以上6以下ならRGBAを2,2,1,1と配置する。
     *
     * @param[in]   usingClipCount  ->  配置するクリッピングコンテキストの数
     */
    void SetupLayoutBounds(csmInt32 usingClipCount) const;

    /**
     * @brief   マスクされるdrawableの描画オブジェクト群全体を囲む矩形(モデル座標系)を計算する
     *
     * @param[in]   model            ->  モデルのインスタンス
     * @param[in]   clippingContext  ->  クリッピングマスクのコンテキスト
     * @param[in]   drawableObjectType ->  処理するオブジェクトタイプ
     */
    void CalcClippedTotalBounds(CubismModel& model, T_ClippingContext* clippingContext, CubismRenderer::DrawableObjectType drawableObjectType);

    /**
     * @brief   offscreenに紐づいているpartの子のdrawableを格納先に格納する
     *
     * @param[in]   model                    ->  モデルのインスタンス
     * @param[in]   offscreenIndex           ->  オフスクリーンのインデックス
     * @param[in]   childDrawableIndexList   ->  対象になったdrawableの格納先
     */
    void CollectOffscreenChildDrawableIndexList(CubismModel& model, csmInt32 offscreenIndex, csmVector<csmInt32>& childDrawableIndexList);

    /**
     * @brief   partの子のdrawableを格納先に格納する
     *
     * @param[in]   model                    ->  モデルのインスタンス
     * @param[in]   partIndex                ->  パーツのインデックス
     * @param[in]   childDrawableIndexList   ->  対象になったdrawableの格納先
     */
    void CollectPartChildDrawableIndexList(CubismModel& model, csmInt32 partIndex, csmVector<csmInt32>& childDrawableIndexList);

    /**
     * @brief   画面描画に使用するクリッピングマスクのリストを取得する
     *
     * @return  画面描画に使用するクリッピングマスクのリスト
     */
    csmVector<T_ClippingContext*>* GetClippingContextListForDraw();

        /**
     * @brief   オフスクリーン描画に使用するクリッピングマスクのリストを取得する
     *
     * @return  オフスクリーン描画に使用するクリッピングマスクのリスト
     */
    csmVector<T_ClippingContext*>* GetClippingContextListForOffscreen();
    /**
     *@brief  クリッピングマスクバッファのサイズを取得する
     *
     *@return クリッピングマスクバッファのサイズ
     */
    CubismVector2 GetClippingMaskBufferSize() const;

    /**
     * このバッファのレンダーテクスチャの枚数を取得する。
     *
     * @return このバッファのレンダーテクスチャの枚数
     */
    csmInt32 GetRenderTextureCount();

    /**
     * @brief   カラーチャンネル(RGBA)のフラグを取得する
     *
     * @param[in]   channelIndex   ->   カラーチャンネル(RGBA)の番号(0:R , 1:G , 2:B, 3:A)
     */
    CubismRenderer::CubismTextureColor* GetChannelFlagAsColor(csmInt32 channelIndex);

    /**
     *@brief  クリッピングマスクバッファのサイズを設定する
     *
     *@param  size -> クリッピングマスクバッファのサイズ
     *
     */
    void SetClippingMaskBufferSize(csmFloat32 width, csmFloat32 height);

protected:
    T_RenderTarget* _currentMaskBuffer; /// オフスクリーンサーフェイスのアドレス
    csmVector<csmBool> _clearedMaskBufferFlags; /// マスクのクリアフラグの配列

    csmVector<CubismRenderer::CubismTextureColor*> _channelColors;
    csmVector<T_ClippingContext*> _clippingContextListForMask;   ///< マスク用クリッピングコンテキストのリスト
    csmVector<T_ClippingContext*> _clippingContextListForDraw;   ///< 描画用クリッピングコンテキストのリスト
    csmVector<T_ClippingContext*> _clippingContextListForOffscreen;   ///< オフスクリーン用クリッピングコンテキストのリスト
    CubismVector2 _clippingMaskBufferSize;       ///< クリッピングマスクのバッファサイズ（初期値:256）
    csmInt32 _renderTextureCount;           ///< 生成するレンダーテクスチャの枚数

    CubismMatrix44 _tmpMatrix;              ///< マスク計算用の行列
    CubismMatrix44 _tmpMatrixForMask;       ///< マスク計算用の行列
    CubismMatrix44 _tmpMatrixForDraw;       ///< マスク計算用の行列
    csmRectF _tmpBoundsOnModel;       ///< マスク配置計算用の矩形
};

#include "CubismClippingManager.tpp"
}}}}

//------------ LIVE2D NAMESPACE ------------
