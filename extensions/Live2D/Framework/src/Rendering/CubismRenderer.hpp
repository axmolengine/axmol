/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "CubismFramework.hpp"
#include "Math/CubismMatrix44.hpp"
#include "Type/csmVector.hpp"
#include "Type/csmRectF.hpp"

namespace Live2D {namespace Cubism {namespace Framework {
class CubismModel;
}}}

//------------ LIVE2D NAMESPACE ------------
namespace Live2D { namespace Cubism { namespace Framework { namespace Rendering {

/**
 * @brief   モデル描画を処理するレンダラ
 *           サブクラスに環境依存の描画命令を記述する
 */
class CubismRenderer
{
public:

    /**
     * @brief 描画オブジェクトのタイプ
    */
    enum DrawableObjectType
    {
        DrawableObjectType_Drawable = 0,
        DrawableObjectType_Offscreen = 2
    };

    /**
     * @brief カラーブレンディングのモード
     */
    enum CubismBlendMode
    {
        CubismBlendMode_Normal = 0,          ///< 通常
        CubismBlendMode_Additive = 1,        ///< 加算
        CubismBlendMode_Multiplicative = 2,  ///< 乗算
        CubismBlendMode_Mask = 3,            ///< マスク
        CubismBlendMode_Copy = 4,            ///< コピー
    };

    /**
    * @brief テクスチャの色をRGBAで扱うための構造体
    */
    struct CubismTextureColor
    {
        /**
         * @brief   コンストラクタ
         */
        CubismTextureColor()
            : R(1.0f)
            , G(1.0f)
            , B(1.0f)
            , A(1.0f)
        {
        }

        /**
         * @brief   コンストラクタ
         */
        CubismTextureColor(csmFloat32 r, csmFloat32 g, csmFloat32 b, csmFloat32 a)
            : R(r)
            , G(g)
            , B(b)
            , A(a)
        {
        }

        /**
         * @brief   デストラクタ
         */
        virtual ~CubismTextureColor()
        {
        };

        csmFloat32 R;   ///< 赤チャンネル
        csmFloat32 G;   ///< 緑チャンネル
        csmFloat32 B;   ///< 青チャンネル
        csmFloat32 A;   ///< αチャンネル

    };

    /**
     * @brief   レンダラのインスタンスを生成して取得する
     *
     * @param[in] width -> モデルを描画したバッファの幅
     * @param[in] height -> モデルを描画したバッファの高さ
     *
     * @retrun  レンダラのインスタンス
     */
    static CubismRenderer* Create(csmUint32 width, csmUint32 height);

    /**
     * @brief   レンダラのインスタンスを解放する
     */
    static void Delete(CubismRenderer* renderer);

    /**
     * @brief   レンダラが保持する静的なリソースを解放する
     */
    static void StaticRelease();

    /**
     * @brief   レンダラの初期化処理を実行する<br>
     *           引数に渡したモデルからレンダラの初期化処理に必要な情報を取り出すことができる
     *
     * @param[in]  model -> モデルのインスタンス
     */
    virtual void Initialize(Framework::CubismModel* model);

    /**
     * @brief   レンダラの初期化処理を実行する<br>
     *           引数に渡したモデルからレンダラの初期化処理に必要な情報を取り出すことができる
     *
     * @param[in]  model -> モデルのインスタンス
     * @param[in]  maskBufferCount -> バッファの生成数
     */
    virtual void Initialize(Framework::CubismModel* model, csmInt32 maskBufferCount);

    /**
     * @brief   モデルを描画する
     *
     */
    void DrawModel();

    /**
     * @brief   オフスクリーンのサイズを設定
     *
     * @param[in] width -> モデルを描画したバッファの幅
     * @param[in] height -> モデルを描画したバッファの高さ
     */
    void SetRenderTargetSize(csmUint32 width, csmUint32 height);

    /**
     * @brief   Model-View-Projection 行列をセットする<br>
     *           配列は複製されるので元の配列は外で破棄して良い
     *
     * @param[in]   matrix4x4   ->   Model-View-Projection 行列
     */
    void SetMvpMatrix(CubismMatrix44* matrix4x4);

    /**
     * @brief   Model-View-Projection 行列を取得する
     *
     * @return  Model-View-Projection 行列
     */
    CubismMatrix44 GetMvpMatrix() const;

    /**
     * @brief       モデルの色をセットする。<br>
     *               各色0.0f～1.0fの間で指定する(1.0fが標準の状態）。
     *
     * @param[in]   red    ->   赤チャンネルの値
     * @param[in]   green  ->   緑チャンネルの値
     * @param[in]   blue   ->   青チャンネルの値
     * @param[in]   alpha  ->   αチャンネルの値
     *
     */
    void SetModelColor(csmFloat32 red, csmFloat32 green, csmFloat32 blue, csmFloat32 alpha);

    /**
     * @brief       モデルの色を取得する。<br>
     *               各色0.0f～1.0fの間で指定する(1.0fが標準の状態）。
     *
     * @return      RGBAのカラー情報
     */
    CubismTextureColor GetModelColor() const;

    /**
     * @brief       透明度を考慮したモデルの色を計算する。
     *
     * @param[in]   opacity    ->   透明度
     *
     * @return      RGBAのカラー情報
     */
    CubismTextureColor GetModelColorWithOpacity(csmFloat32 opacity) const;

    /**
     * @brief  乗算済みαの有効・無効をセットする。<br>
     *          有効にするならtrue, 無効にするならfalseをセットする。
     */
    void IsPremultipliedAlpha(csmBool enable);

    /**
     * @brief  乗算済みαの有効・無効を取得する。
     *
     * @retval  true    ->  乗算済みα有効
     * @retval  false   ->  乗算済みα無効
     */
    csmBool IsPremultipliedAlpha() const;

    /**
     * @brief  カリング（片面描画）の有効・無効をセットする。<br>
     *          有効にするならtrue, 無効にするならfalseをセットする。
     */
    void IsCulling(csmBool culling);

    /**
     * @brief  カリング（片面描画）の有効・無効を取得する。
     *
     * @retval  true    ->  カリング有効
     * @retval  false   ->  カリング無効
     */
    csmBool IsCulling() const;

    /**
     * @brief   テクスチャの異方性フィルタリングのパラメータをセットする<br>
     *           パラメータ値の影響度はレンダラの実装に依存する
     *
     * @param[in]   anisotropy   ->   パラメータの値
     */
    void SetAnisotropy(csmFloat32 anisotropy);

    /**
     * @brief   テクスチャの異方性フィルタリングのパラメータをセットする
     *
     * @return  異方性フィルタリングのパラメータ
     */
    csmFloat32 GetAnisotropy() const;

    /**
     * @brief   レンダリングするモデルを取得する。
     *
     * @return  レンダリングするモデルのポインタ
     */
    CubismModel* GetModel() const;

    /**
     * @brief   マスク描画の方式を変更する。
     *           falseの場合、マスクを1枚のテクスチャに分割してレンダリングする（デフォルトはこちら）。
     *           高速だが、マスク個数の上限が36に限定され、質も荒くなる。
     *           trueの場合、パーツ描画の前にその都度必要なマスクを描き直す。
     *           レンダリング品質は高いが描画処理負荷は増す。
     */
    void UseHighPrecisionMask(csmBool high);

    /**
     * @brief   マスク描画の方式を取得する。
     */
    csmBool IsUsingHighPrecisionMask();

protected:
    /**
     * @brief   コンストラクタ
     *
     * @param[in] width -> モデルを描画したバッファの幅
     * @param[in] height -> モデルを描画したバッファの高さ
     */
    CubismRenderer(csmUint32 width, csmUint32 height);

    /**
     * @brief   デストラクタ
     */
    virtual ~CubismRenderer();

    /**
     * @brief   モデル描画の実装
     *
     */
    virtual void DoDrawModel() = 0;

    /**
     * @brief   モデル描画直前のレンダラのステートを保持する
     */
    virtual void SaveProfile() = 0;

    /**
     * @brief   モデル描画直前のレンダラのステートを復帰させる
     */
    virtual void RestoreProfile() = 0;

    /**
     * @brief   モデル描画直前のオフスクリーン設定
     */
    virtual void BeforeDrawModelRenderTarget() = 0;

    /**
     * @brief   モデル描画後のオフスクリーン設定
     */
    virtual void AfterDrawModelRenderTarget() = 0;

    csmUint32 _modelRenderTargetWidth;
    csmUint32 _modelRenderTargetHeight;

private:
    // コピーコンストラクタを隠す
    CubismRenderer(const CubismRenderer&);
    CubismRenderer& operator=(const CubismRenderer&);

    CubismMatrix44      _mvpMatrix4x4;          ///< Model-View-Projection 行列
    CubismTextureColor  _modelColor;            ///< モデル自体のカラー(RGBA)
    csmBool             _isCulling;             ///< カリングが有効ならtrue
    csmBool             _isPremultipliedAlpha;  ///< 乗算済みαならtrue
    csmFloat32          _anisotropy;            ///< テクスチャの異方性フィルタリングのパラメータ
    CubismModel*        _model;                 ///< レンダリング対象のモデル

    csmBool             _useHighPrecisionMask;  ///< falseの場合、マスクを纏めて描画する trueの場合、マスクはパーツ描画ごとに書き直す
};


/**
 * @brief   クリッピングについての設定を保持するクラス<br>
 *           サブクラスに環境依存のフィールドを保持する
 */
class CubismClippingContext
{
public:
    /**
     * @brief   引数付きコンストラクタ
     *
     */
    CubismClippingContext(const csmInt32* clippingDrawableIndices, csmInt32 clipCount);

    /**
     * @brief   デストラクタ
     */
    ~CubismClippingContext();

    /**
     * @brief   このマスクにクリップされるDrawableを追加する
     *
     * @param[in]   drawableIndex   ->  クリッピング対象に追加するDrawableのインデックス
     */
    void AddClippedDrawable(csmInt32 drawableIndex);

        /**
     * @brief   このマスクにクリップされるOffscreenを追加する
     *
     * @param[in]   drawableIndex   ->  クリッピング対象に追加するOffscreenのインデックス
     */
    void AddClippedOffscreen(csmInt32 offscreenIndex);

    csmBool _isUsing;                                ///< 現在の描画状態でマスクの準備が必要ならtrue
    const csmInt32* _clippingIdList;                 ///< クリッピングマスクのIDリスト
    csmInt32 _clippingIdCount;                       ///< クリッピングマスクの数
    csmInt32 _layoutChannelIndex;                       ///< RGBAのいずれのチャンネルにこのクリップを配置するか(0:R , 1:G , 2:B , 3:A)
    csmRectF* _layoutBounds;                         ///< マスク用チャンネルのどの領域にマスクを入れるか(View座標-1..1, UVは0..1に直す)
    csmRectF* _allClippedDrawRect;                   ///< このクリッピングで、クリッピングされる全ての描画オブジェクトの囲み矩形（毎回更新）
    CubismMatrix44 _matrixForMask;                   ///< マスクの位置計算結果を保持する行列
    CubismMatrix44 _matrixForDraw;                   ///< 描画オブジェクトの位置計算結果を保持する行列
    csmVector<csmInt32>* _clippedDrawableIndexList;  ///< このマスクにクリップされるDrawableのリスト
    csmVector<csmInt32>* _clippedOffscreenIndexList;  ///< このマスクにクリップされるOffscreenのリスト
    csmInt32 _bufferIndex;                           ///< このマスクが割り当てられるレンダーテクスチャ（フレームバッファ）やカラーバッファのインデックス
};

}}}}

//------------ LIVE2D NAMESPACE ------------
