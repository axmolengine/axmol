/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "CubismFramework.hpp"
#include "Math/CubismMatrix44.hpp"

namespace Live2D {namespace Cubism {namespace Framework {
class CubismModel;
}}}

//------------ LIVE2D NAMESPACE ------------
namespace Live2D { namespace Cubism { namespace Framework { namespace Rendering {

/**
 * @brief   モデル描画を処理するレンダラ<br>
 *           サブクラスに環境依存の描画命令を記述する
 *
 */
class CubismRenderer
{
public:

    /**
     * @brief カラーブレンディングのモード
     */
    enum CubismBlendMode
    {
        CubismBlendMode_Normal = 0,          ///< 通常

        CubismBlendMode_Additive = 1,        ///< 加算

        CubismBlendMode_Multiplicative = 2,  ///< 乗算
    };  // CubismBlendMode

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
            , A(1.0f) {};

        /**
         * @brief   デストラクタ
         */
        virtual ~CubismTextureColor() {};

        csmFloat32 R;   ///< 赤チャンネル
        csmFloat32 G;   ///< 緑チャンネル
        csmFloat32 B;   ///< 青チャンネル
        csmFloat32 A;   ///< αチャンネル

    };  // CubismTextureColor

    /**
     * @brief   レンダラのインスタンスを生成して取得する
     *
     * @retrun  レンダラのインスタンス
     */
    static CubismRenderer* Create();

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
     * @brief   モデルを描画する
     *
     */
    void DrawModel();

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
     */
    CubismRenderer();

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
     * @brief   描画オブジェクト（アートメッシュ）を描画する。<br>
     *           ポリゴンメッシュとテクスチャ番号をセットで渡す。
     *
     * @param[in]   textureNo            ->  描画するテクスチャ番号
     * @param[in]   indexCount           ->  描画オブジェクトのインデックス値
     * @param[in]   vertexCount          ->  ポリゴンメッシュの頂点数
     * @param[in]   indexArray           ->  ポリゴンメッシュ頂点のインデックス配列
     * @param[in]   vertexArray          ->  ポリゴンメッシュの頂点配列
     * @param[in]   uvArray              ->  uv配列
     * @param[in]   opacity              ->  不透明度
     * @param[in]   colorBlendMode       ->  カラーブレンディングのタイプ
     * @param[in]   invertedMask          ->  マスク使用時のマスクの反転使用
     *
     */
    virtual void DrawMesh(csmInt32 textureNo, csmInt32 indexCount, csmInt32 vertexCount
                          , csmUint16* indexArray, csmFloat32* vertexArray, csmFloat32* uvArray
                          , csmFloat32 opacity, CubismBlendMode colorBlendMode, csmBool invertedMask) = 0;

    /**
     * @brief   モデル描画直前のレンダラのステートを保持する
     */
    virtual void SaveProfile() = 0;


    /**
     * @brief   モデル描画直前のレンダラのステートを復帰させる
     */
    virtual void RestoreProfile() = 0;

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

}}}}

//------------ LIVE2D NAMESPACE ------------
