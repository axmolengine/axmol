/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "Model/CubismModel.hpp"
#include "Utils/CubismJson.hpp"

namespace Live2D { namespace Cubism { namespace Framework {
/**
 * @brief パーツの不透明度の設定
 *
 * パーツの不透明度の管理と設定を行う。
 */
class CubismPose
{
public:
    /**
    * @brief パーツにまつわるデータを管理
    *
    * パーツにまつわる諸々のデータを管理する。
    */
    struct PartData
    {
        /**
        * @brief コンストラクタ
        *
        * コンストラクタ。
        */
        PartData();

        /**
        * @brief コピーコンストラクタ
        *
        * コピーコンストラクタ。
        */
        PartData(const PartData& v);

        /**
        * @brief デストラクタ
        *
        * デストラクタ。
        */
        virtual ~PartData();

        /**
        * @brief 代入のオーバーロード
        *
        * 代入のオーバーロード。
        */
        PartData&   operator=(const PartData& v);

        /**
        * @brief 初期化
        *
        * 初期化する。
        *
        * @param[in]   model   初期化に使用するモデル
        */
        void                Initialize(CubismModel* model);

        CubismIdHandle                     PartId;                 ///< パーツID
        csmInt32                            ParameterIndex;         ///< パラメータのインデックス
        csmInt32                            PartIndex;              ///< パーツのインデックス
        csmVector<PartData>                 Link;                   ///< 連動するパラメータ
    };

    /**
     * @brief インスタンスの作成
     *
     * インスタンスを作成する。
     *
     * @param[in]   pose3json    pose3.jsonのデータ
     * @param[in]   size         pose3.jsonのデータのサイズ[Byte]
     * @return                   作成されたインスタンス
     */
    static CubismPose*  Create(const csmByte* pose3json, csmSizeInt size);

    /**
     * @brief インスタンスの破棄
     *
     * インスタンスを破棄する。
     *
     * @param[in]   pose    対象のCubismPose
     */
    static void         Delete(CubismPose* pose);

    /**
     * @brief モデルのパラメータの更新
     *
     * モデルのパラメータを更新する。
     *
     * @param[in]   model              対象のモデル
     * @param[in]   deltaTimeSeconds   デルタ時間[秒]
     */
    void                UpdateParameters(CubismModel* model, csmFloat32 deltaTimeSeconds);

private:
    /**
    * @brief コンストラクタ
    *
    * コンストラクタ。
    */
    CubismPose();

    /**
    * @brief デストラクタ
    *
    * デストラクタ。
    */
    virtual ~CubismPose();

    /**
     * @brief 表示を初期化
     *
     * 表示を初期化する。
     *
     * @param[in]   model   対象のモデル
     *
     * @note 不透明度の初期値が0でないパラメータは、不透明度を1に設定する。
     */
    void                Reset(CubismModel* model);

    /**
     * @brief パーツの不透明度をコピー
     *
     * パーツの不透明度をコピーし、リンクしているパーツへ設定する。
     *
     * @param[in]   model   対象のモデル
     */
    void                CopyPartOpacities(CubismModel* model);

    /**
     * @brief パーツのフェード操作を実行
     *
     * パーツのフェード操作を行う。
     *
     * @param[in]   model               対象のモデル
     * @param[in]   deltaTimeSeconds    デルタ時間[秒]
     * @param[in]   beginIndex          フェード操作を行うパーツグループの先頭インデックス
     * @param[in]   partGroupCount      フェード操作を行うパーツグループの個数
     */
    void                DoFade(CubismModel* model, csmFloat32 deltaTimeSeconds, csmInt32 beginIndex, csmInt32 partGroupCount);

    csmVector<PartData>             _partGroups;                ///< パーツグループ
    csmVector<csmInt32>             _partGroupCounts;           ///< それぞれのパーツグループの個数
    csmFloat32                      _fadeTimeSeconds;           ///< フェード時間[秒]
    CubismModel*                    _lastModel;                 ///< 前回操作したモデル
};

}}}
