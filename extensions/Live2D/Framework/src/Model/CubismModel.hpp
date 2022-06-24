/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "CubismFramework.hpp"
#include "Type/csmMap.hpp"
#include "Type/csmVector.hpp"
#include "Rendering/CubismRenderer.hpp"
#include "Id/CubismId.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

class CubismMoc;

/**
 * @brief モデル
 *
 * Mocデータから生成されるモデルのクラス。
 */
class CubismModel
{
    friend class CubismMoc;
public:
    /**
     * @brief モデルのパラメータの更新
     *
     * モデルのパラメータを更新する。
     */
    void    Update() const;

    /**
     * @brief Pixel単位でキャンバスの幅の取得
     *
     * キャンバスの幅を取得する。
     *
     * @return キャンバスの幅(pixel)
     */
    csmFloat32  GetCanvasWidthPixel() const;

    /**
     * @brief Pixel単位でキャンバスの高さの取得
     *
     * キャンバスの高さを取得する。
     *
     * @return キャンバスの高さ(pixel)
     */
    csmFloat32  GetCanvasHeightPixel() const;

    /**
     * @brief PixelsPerUnitの取得
     *
     * PixelsPerUnitを取得する。
     *
     * @return PixelsPerUnit
     */
    csmFloat32  GetPixelsPerUnit() const;

    /**
     * @brief Unit単位でキャンバスの幅の取得
     *
     * キャンバスの幅を取得する。
     *
     * @return キャンバスの幅(Unit)
     */
    csmFloat32  GetCanvasWidth() const;

    /**
     * @brief Unit単位でキャンバスの高さの取得
     *
     * キャンバスの高さを取得する。
     *
     * @return キャンバスの高さ(Unit)
     */
    csmFloat32  GetCanvasHeight() const;

    /**
     * @brief パーツのインデックスの取得
     *
     * パーツのインデックスを取得する。
     *
     * @param[in]   partId  パーツのID
     * @return  パーツのインデックス
     */
    csmInt32    GetPartIndex(CubismIdHandle partId);

    /**
     * @brief パーツの個数の取得
     *
     * パーツの個数を取得する。
     *
     * @return パーツの個数
     */
    csmInt32    GetPartCount() const;

    /**
     * @brief パーツの不透明度の設定
     *
     * パーツの不透明度を設定する。
     *
     * @param[in]   partId  パーツのID
     * @param[in]   opacity 不透明度
     */
    void        SetPartOpacity(CubismIdHandle partId, csmFloat32 opacity);

    /**
     * @brief パーツの不透明度の設定
     *
     * パーツの不透明度を設定する。
     *
     * @param[in]   partIndex   パーツのインデックス
     * @param[in]   opacity     パーツの不透明度
     */
    void        SetPartOpacity(csmInt32 partIndex, csmFloat32 opacity);

    /**
     * @brief パーツの不透明度の取得
     *
     * パーツの不透明度を取得する。
     *
     * @param[in]   partId  パーツのID
     * @return  パーツの不透明度
     */
    csmFloat32  GetPartOpacity(CubismIdHandle partId);

    /**
     * @brief パーツの不透明度の取得
     *
     * パーツの不透明度を取得する。
     *
     * @param[in]   partIndex   パーツのインデックス
     * @return  パーツの不透明度
     */
    csmFloat32  GetPartOpacity(csmInt32 partIndex);

    /**
     * @brief パラメータのインデックスの取得
     *
     * パラメータのインデックスを取得する。
     *
     * @param[in]   parameterId パラメータID
     * @return  パラメータのインデックス
     */
    csmInt32    GetParameterIndex(CubismIdHandle parameterId);

    /**
     * @brief パラメータの個数の取得
     *
     * パラメータの個数を取得する。
     *
     * @return パラメータの個数
     */
    csmInt32    GetParameterCount() const;

    /**
     * @brief パラメータの最大値の取得
     *
     * パラメータの最大値を取得する。
     *
     * @param[in]   parameterIndex  パラメータのインデックス
     * @return パラメータの最大値
     */
    csmFloat32  GetParameterMaximumValue(csmUint32 parameterIndex) const;

    /**
     * @brief パラメータの最小値の取得
     *
     * パラメータの最小値を取得する。
     *
     * @param[in]   parameterIndex  パラメータのインデックス
     * @return パラメータの最小値
     */
    csmFloat32  GetParameterMinimumValue(csmUint32 parameterIndex) const;

    /**
     * @brief パラメータのデフォルト値の取得
     *
     * パラメータのデフォルト値を取得する。
     *
     * @param[in]   parameterIndex  パラメータのインデックス
     * @return  パラメータのデフォルト値
     */
    csmFloat32  GetParameterDefaultValue(csmUint32 parameterIndex) const;

    /**
     * @brief パラメータの値の取得
     *
     * パラメータの値を取得する。
     *
     * @param[in]   parameterId パラメータID
     * @return  パラメータの値
     */
    csmFloat32  GetParameterValue(CubismIdHandle parameterId);

    /**
     * @brief パラメータの値の取得
     *
     * パラメータの値を取得する。
     *
     * @param[in]   parameterIndex  パラメータのインデックス
     * @return  パラメータの値
     */
    csmFloat32  GetParameterValue(csmInt32 parameterIndex);

    /**
     * @brief パラメータの値の設定
     *
     * パラメータの値を設定する。
     *
     * @param[in]   parameterId パラメータID
     * @param[in]   value       パラメータの値
     * @param[in]   weight      重み
     */
    void        SetParameterValue(CubismIdHandle parameterId, csmFloat32 value, csmFloat32 weight = 1.0f);

    /**
     * @brief パラメータの値の設定
     *
     * パラメータの値を設定する。
     *
     * @param[in]   parameterIndex  パラメータのインデックス
     * @param[in]   value           パラメータの値
     * @param[in]   weight          重み
     */
    void        SetParameterValue(csmInt32 parameterIndex, csmFloat32 value, csmFloat32 weight = 1.0f);

    /**
     * @brief パラメータの値の加算
     *
     * パラメータの値を加算する。
     *
     * @param[in]   parameterId     パラメータID
     * @param[in]   value           加算する値
     * @param[in]   weight          重み
     */
    void        AddParameterValue(CubismIdHandle parameterId, csmFloat32 value, csmFloat32 weight = 1.0f);

    /**
     * @brief パラメータの値の加算
     *
     * パラメータの値を加算する。
     *
     * @param[in]   parameterIndex  パラメータのインデックス
     * @param[in]   value           加算する値
     * @param[in]   weight          重み
     */
    void        AddParameterValue(csmInt32 parameterIndex, csmFloat32 value, csmFloat32 weight = 1.0f);

    /**
     * @brief パラメータの値の乗算
     *
     * パラメータの値を乗算する。
     *
     * @param[in]   parameterId     パラメータID
     * @param[in]   value           乗算する値
     * @param[in]   weight          重み
     */
    void        MultiplyParameterValue(CubismIdHandle parameterId, csmFloat32 value, csmFloat32 weight = 1.0f);

    /**
    * @brief パラメータの値の乗算
    *
    * パラメータの値を乗算する。
    *
    * @param[in]   parameterIndex  パラメータのインデックス
    * @param[in]   value           乗算する値
    * @param[in]   weight          重み
    */
    void        MultiplyParameterValue(csmInt32 parameterIndex, csmFloat32 value, csmFloat32 weight = 1.0f);

    /**
     * @brief Drawableのインデックスの取得
     *
     * Drawableのインデックスを取得する。
     *
     * @param[in]   drawableId  DrawableのID
     * @return  Drawableのインデックス
     */
    csmInt32            GetDrawableIndex(CubismIdHandle drawableId) const;

    /**
     * @brief Drawableの個数の取得
     *
     * Drawableの個数を取得する。
     *
     * @return  Drawableの個数
     */
    csmInt32            GetDrawableCount() const;

    /**
     * @brief DrawableのIDの取得
     *
     * DrawableのIDを取得する。
     *
     * @param[in]   drawableIndex   Drawableのインデックス
     * @return  DrawableのID
     */
    CubismIdHandle      GetDrawableId(csmInt32 drawableIndex) const;

    /**
     * @brief Drawableの描画順リストの取得
     *
     * Drawableの描画順リストを取得する。
     *
     * @return Drawableの描画順リスト
     */
    const csmInt32*     GetDrawableRenderOrders() const;

    /**
     * @brief Drawableのテクスチャインデックスリストの取得
     *
     * Drawableのテクスチャインデックスリストを取得する。
     *
     * @param[in]   drawableIndex   Drawableのインデックス
     * @return  Drawableのテクスチャインデックスリスト
     */
    csmInt32            GetDrawableTextureIndices(csmInt32 drawableIndex) const;

    /**
     * @brief Drawableの頂点インデックスの個数の取得
     *
     * Drawableの頂点インデックスの個数を取得する。
     *
     * @param[in]   drawableIndex   Drawableのインデックス
     * @return  Drawableの頂点インデックスの個数
     */
    csmInt32            GetDrawableVertexIndexCount(csmInt32 drawableIndex) const;

    /**
     * @brief Drawableの頂点の個数の取得
     *
     * Drawableの頂点の個数を取得する。
     *
     * @param[in]   drawableIndex   Drawableのインデックス
     * @return  Drawableの頂点の個数
     */
    csmInt32            GetDrawableVertexCount(csmInt32 drawableIndex) const;

    /**
     * @brief Drawableの頂点リストの取得
     *
     * Drawableの頂点リストを取得する。
     *
     * @param[in]   drawableIndex   Drawableのインデックス
     * @return  Drawableの頂点リスト
     */
    const csmFloat32*   GetDrawableVertices(csmInt32 drawableIndex) const;

    /**
     * @brief Drawableの頂点インデックスリストの取得
     *
     * Drawableの頂点インデックスリストを取得する。
     *
     * @param[in]   drawableIndex   Drawableのインデックス
     * @return  Drawableの頂点インデックスリスト
     */
    const csmUint16*            GetDrawableVertexIndices(csmInt32 drawableIndex) const;

    /**
     * @brief Drawableの頂点リストの取得
     *
     * Drawableの頂点リストを取得する。
     *
     * @param[in]   drawableIndex   Drawableのインデックス
     * @return  Drawableの頂点リスト
     */
    const Core::csmVector2*     GetDrawableVertexPositions(csmInt32 drawableIndex) const;

    /**
     * @brief Drawableの頂点のUVリストの取得
     *
     * Drawableの頂点のUVリストを取得する。
     *
     * @param[in]   drawableIndex   Drawableのインデックス
     * @return  Drawableの頂点のUVリスト
     */
    const Core::csmVector2*     GetDrawableVertexUvs(csmInt32 drawableIndex) const;

    /**
     * @brief Drawableの不透明度の取得
     *
     * Drawableの不透明度を取得する。
     *
     * @param[in]   drawableIndex   Drawableのインデックス
     * @return  Drawableの不透明度
     */
    csmFloat32                  GetDrawableOpacity(csmInt32 drawableIndex) const;

    /**
     * @brief Drawableのカリング情報の取得
     *
     * Drawableのカリング情報を取得する。
     *
     * @param[in]   drawableIndex   Drawableのインデックス
     * @return  Drawableのカリング情報
     */
    csmInt32                    GetDrawableCulling(csmInt32 drawableIndex) const;

    /**
     * @brief Drawableのブレンドモードの取得
     *
     * Drawableのブレンドモードを取得する。
     *
     * @param[in]   drawableIndex   Drawableのインデックス
     * @return  Drawableのブレンドモード
     */
    Rendering::CubismRenderer::CubismBlendMode   GetDrawableBlendMode(csmInt32 drawableIndex) const;

    /**
    * @brief Drawableのマスクの反転使用の取得
    *
    * Drawableのマスク使用時の反転設定を取得する。
    * マスクを使用しない場合は無視される
    *
    * @param[in]   drawableIndex   Drawableのインデックス
    * @return  Drawableのマスクの反転設定
    */
    csmBool                    GetDrawableInvertedMask(csmInt32 drawableIndex) const;

    /**
    * @brief Drawableの表示情報の取得
    *
    * Drawableの表示情報を取得する。
    *
    * @param[in]   drawableIndex   Drawableのインデックス
    * @return  true    Drawableが表示
    * @retval  false   Drawableが非表示
    */
    csmBool                  GetDrawableDynamicFlagIsVisible(csmInt32 drawableIndex) const;

    /**
    * @brief Drawableの表示状態の変化の取得
    *
    * 直近のCubismModel::Update関数でDrawableの表示状態が変化したかを取得する。
    *
    * @param[in]   drawableIndex   Drawableのインデックス
    * @retval  true    Drawableの表示状態が直近のCubismModel::Update関数で変化した
    * @retval  false   Drawableの表示状態が直近のCubismModel::Update関数で変化していない
    */
    csmBool                  GetDrawableDynamicFlagVisibilityDidChange(csmInt32 drawableIndex) const;

    /**
    * @brief Drawableの不透明度の変化情報の取得
    *
    * 直近のCubismModel::Update関数でDrawableの不透明度が変化したかを取得する。
    *
    * @param[in]   drawableIndex   Drawableのインデックス
    * @retval  true    Drawableの不透明度が直近のCubismModel::Update関数で変化した
    * @retval  false   Drawableの不透明度が直近のCubismModel::Update関数で変化していない
    */
    csmBool                  GetDrawableDynamicFlagOpacityDidChange(csmInt32 drawableIndex) const;

    /**
    * @brief DrawableのDrawOrderの変化情報の取得
    *
    * 直近のCubismModel::Update関数でDrawableのDrawOrderが変化したかを取得する。
    * DrawOrderはArtMesh上で指定する0から1000の情報
    *
    * @param[in]   drawableIndex   Drawableのインデックス
    * @retval  true    Drawableの不透明度が直近のCubismModel::Update関数で変化した
    * @retval  false   Drawableの不透明度が直近のCubismModel::Update関数で変化していない
    */
    csmBool                  GetDrawableDynamicFlagDrawOrderDidChange(csmInt32 drawableIndex) const;

    /**
    * @brief Drawableの描画順序の変化情報の取得
    *
    * 直近のCubismModel::Update関数でDrawableの描画の順序が変化したかを取得する。
    *
    * @param[in]   drawableIndex   Drawableのインデックス
    * @retval  true    Drawableの描画の順序が直近のCubismModel::Update関数で変化した
    * @retval  false   Drawableの描画の順序が直近のCubismModel::Update関数で変化していない
    */
    csmBool                  GetDrawableDynamicFlagRenderOrderDidChange(csmInt32 drawableIndex) const;

    /**
    * @brief DrawableのVertexPositionsの変化情報の取得
    *
    * 直近のCubismModel::Update関数でDrawableの頂点情報が変化したかを取得する。
    *
    * @param[in]   drawableIndex   Drawableのインデックス
    * @retval  true    Drawableの頂点情報が直近のCubismModel::Update関数で変化した
    * @retval  false   Drawableの頂点情報が直近のCubismModel::Update関数で変化していない
    */
    csmBool                  GetDrawableDynamicFlagVertexPositionsDidChange(csmInt32 drawableIndex) const;

    /**
     * @brief Drawableのクリッピングマスクリストの取得
     *
     * Drawableのクリッピングマスクリストを取得する。
     *
     * @return  Drawableのクリッピングマスクリスト
     */
    const csmInt32**            GetDrawableMasks() const;

    /**
     * @brief Drawableのクリッピングマスクの個数リストの取得
     *
     * Drawableのクリッピングマスクの個数リストを取得する。
     *
     * @return  Drawableのクリッピングマスクの個数リスト
     */
    const csmInt32*             GetDrawableMaskCounts() const;

    /**
     * @brief クリッピングマスクの使用状態
     *
     * クリッピングマスクを使用しているかどうか？
     *
     * @retval  true    クリッピングマスクを使用している
     * @retval  false   クリッピングマスクを使用していない
     */
    csmBool     IsUsingMasking() const;

    /**
     * @brief 保存されたパラメータの読み込み
     *
     * 保存されたパラメータを読み込む
     */
    void    LoadParameters();

    /**
     * @brief パラメータの保存
     *
     * パラメータを保存する。
     */
    void    SaveParameters();

    Core::csmModel*     GetModel() const;

private:
    /**
     * @brief コンストラクタ
     *
     * コンストラクタ。
     *
     * @param[in]   model   csmModelへのポインタ
     */
    CubismModel(Core::csmModel* model);

    /**
     * @brief デストラクタ
     *
     * デストラクタ。
     */
    virtual ~CubismModel();

    //Prevention of copy Constructor
    CubismModel(const CubismModel&);
    CubismModel& operator=(const CubismModel&);

    /**
     * @brief 初期化
     *
     * 初期化する。
     */
    void Initialize();

    csmMap<csmInt32, csmFloat32>        _notExistPartOpacities;             ///< 存在していないパーツの不透明度のリスト
    csmMap<CubismIdHandle, csmInt32>   _notExistPartId;                    ///< 存在していないパーツIDのリスト

    csmMap<csmInt32, csmFloat32>        _notExistParameterValues;           ///< 存在していないパラメータの値のリスト
    csmMap<CubismIdHandle, csmInt32>   _notExistParameterId;               ///< 存在していないパラメータIDのリスト

    csmVector<csmFloat32>   _savedParameters;                   ///< 保存されたパラメータ

    Core::csmModel*     _model;                                 ///< モデル

    csmFloat32*         _parameterValues;                       ///< パラメータの値のリスト
    const csmFloat32*   _parameterMaximumValues;                ///< パラメータの最大値のリスト
    const csmFloat32*   _parameterMinimumValues;                ///< パラメータの最小値のリスト

    csmFloat32*         _partOpacities;                         ///< パーツの不透明度のリスト

    csmVector<CubismIdHandle> _parameterIds;
    csmVector<CubismIdHandle> _partIds;
    csmVector<CubismIdHandle> _drawableIds;
};

}}}
